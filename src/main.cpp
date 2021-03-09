#include <FreeRTOS.h>
#include <Arduino.h>
#include <FastLED.h>
#include "Utils.h"
#include "ParticleContainer.h"
#include <WiFi.h>
#include <AsyncUDP.h>

#define LED_PIN     18
#define NUM_LEDS    145
#define BRIGHTNESS  255
#define LED_TYPE    WS2812
#define SSID "WolfieNet-IoT"
#define UDP_PORT 2390

void ledTask(void * params);
void udpListenTask(void * params);
void emitterTask(void * params);

CRGB leds[NUM_LEDS];
ParticleContainer p(32);

char last_beat = 0;
char beat = 0;
int r = 0;
TaskHandle_t emitterTaskHandle;

AsyncUDP udp;
char packetBuffer[255];

void setup() {
    delay(500);
    pinMode(LED_PIN, OUTPUT);
    FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(BRIGHTNESS);
    
    Serial.begin(115200);
    WiFi.begin(SSID);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }

    Serial.println("Connected to the WiFi network");
    Serial.println(WiFi.macAddress());
    Serial.println(WiFi.localIP());

    if(udp.listen(UDP_PORT)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        udp.onPacket([](AsyncUDPPacket packet) {
            Serial.print("UDP Packet Type: ");
            Serial.print(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
            Serial.print(", From: ");
            Serial.print(packet.remoteIP());
            Serial.print(":");
            Serial.print(packet.remotePort());
            Serial.print(", To: ");
            Serial.print(packet.localIP());
            Serial.print(":");
            Serial.print(packet.localPort());
            Serial.print(", Length: ");
            Serial.print(packet.length());
            Serial.print(", Data: ");
            Serial.write(packet.data(), packet.length());
            Serial.println();
            beat = packet.data()[0];
            if (beat == '1')
                xTaskNotify(emitterTaskHandle, packet.data()[1], eSetValueWithOverwrite);
            //reply to the client
            packet.printf("Got %u bytes of data", packet.length());
        });
    }

    xTaskCreate(ledTask, "FastLED task", 30000, NULL, 1, NULL);
    xTaskCreate(emitterTask, "Particle emitter task", 30000, NULL, 1, &emitterTaskHandle);
}

void loop() {
    delay(1500);
}

void emitterTask(void * params) {
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(100);
    while (true) {
        uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
        if (ulNotificationValue > 0) {
            r++;
            //float v = (float)ulNotificationValue / 50.0;
            p.emit(1, 36, 1, 0.01, 0, 200, NULL, ColorFromPalette(RainbowColors_p, r, 255, LINEARBLEND));
            p.emit(1, 36, -1, -0.01, 0, 200, NULL, ColorFromPalette(RainbowColors_p, r, 255, LINEARBLEND));
        }
    
    }
}

void ledTask(void * params) {
    while(true) {
        p.update();
        p.render();
        if (r < 255) {
            r++;
        } else {
            r = 0;
        }

        fadeToBlackBy(leds, NUM_LEDS, 150);
        delay(5);


        FastLED.show();
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}