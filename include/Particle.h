#include <FastLED.h>

class Particle {
    public: 
        float pos;
        float lastPos;
        float length;
        float vel;
        float acc;
        float jerk;
        int life;
        int lifespan;
        int brightness;

        CRGB color;

        Particle(float, float, float, float, float, int, int*, CRGB);
        bool update();
        void render();
};

Particle::Particle(float position, float size, float velocity, float acceleration, float jer, int ls, int *brp, CRGB c) 
{
    pos = position;
    lastPos = position;
    length = size;
    vel = velocity;
    acc = acceleration;
    jerk = jer;
    life = ls;
    lifespan = ls;
    color = c;
    brightness = 255;
}

bool Particle::update() {
    lastPos = pos;
    pos += vel;
    vel += acc;
    acc += jerk;
    life--;
    brightness = max((int)pow(2.7182, (5.545/(double)lifespan)*(double)life), 64);

    if (life < 0) {
        return true;
    }

    if (pos < 0) {
        vel *= -0.6;
        //pos = 0;
        //return true;
    } else if (pos+length > 73) {
        //vel *= -0.6;
        //pos = 73;
        return true;
    }
    return false;
}

void Particle::render() {
    if (vel < 0)
        drawPixels(constrain(pos, 0, 72), constrain(lastPos+length, 0, 72), color, brightness);
    else
        drawPixels(constrain(pos+length, 0, 72), constrain(lastPos, 0, 72), color, brightness);
}