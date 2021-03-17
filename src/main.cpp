#include <FreeRTOS.h>
#include <Arduino.h>
#include <FastLED.h>
#include "Utils.h"
#include "ParticleContainer.h"
#include <WiFi.h>
#include <AsyncUDP.h>

#define LED_PIN     18
#define NUM_LEDS    73
#define BRIGHTNESS  255
#define LED_TYPE    WS2812
#define SSID "WolfieNet-IoT"
#define UDP_PORT 2390

void ledTask(void * params);
void udpListenTask(void * params);
void emitterTask(void * params);

CRGB leds[NUM_LEDS];
ParticleContainer p(64);

uint16_t mode = '3';
uint16_t color = 0;
uint16_t onsetStrength = 0;
uint16_t paletteIndex = 0;

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
            if (packet.data()[0] == '0') {
                onsetStrength = packet.data()[1];
                color = packet.data()[2] + random8(20);
                xTaskNotify(emitterTaskHandle, packet.data()[1], eSetValueWithOverwrite);
            } else {
                mode = packet.data()[0];
            }
        });
    }

    xTaskCreate(ledTask, "FastLED task", 30000, NULL, 1, NULL);
    xTaskCreate(emitterTask, "Particle emitter task", 30000, NULL, 1, &emitterTaskHandle);
}

void loop() {
    delay(1000);
}

void emitterTask(void * params) {
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(100);
    while (true) {
        uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
        if (ulNotificationValue > 0) {
            switch (mode) {
                case '1':
                    //Serial.println(energy);
                    p.emit(1, // length
                        1, // position
                        min((log2((float)ulNotificationValue) - 3), 3.0), //  velocity
                        -0.06, //acceleration
                        0, // jerk
                        400, // lifespan
                        NULL, // TODO brightness profile
                        ColorFromPalette(RainbowColors_p, color, 255, LINEARBLEND) ); // color
                    break;
                case '2':
                    p.emit(1, // length
                        36, // position
                        -1, //  velocity
                        -0.03, //acceleration
                        0, // jerk
                        100, // lifespan
                        NULL, // TODO brightness profile
                        ColorFromPalette(RainbowColors_p, ulNotificationValue, 255, LINEARBLEND) ); // color
                    p.emit( 1, // length
                        36, // position
                        1, //  velocity
                        0.03, //acceleration
                        0, // jerk
                        100, // lifespan
                        NULL, // TODO brightness profile
                        ColorFromPalette(RainbowColors_p, onsetStrength, 255, LINEARBLEND) ); // color
                    break;
                case '3':
                    p.emit(3, // length
                        random8(20)+50, // position
                        0, //  velocity
                        -0.03, //acceleration
                        0, // jerk
                        150, // lifespan
                        NULL, // TODO brightness profile
                        ColorFromPalette(RainbowColors_p, random8(), 255, LINEARBLEND) ); // color
                    break;
            }
        }
    
    }
}

void ledTask(void * params) {
    while(true) {
        // update and render particles
        p.update();
        p.render();

        fadeToBlackBy(leds, NUM_LEDS, 150);
        FastLED.show();

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}