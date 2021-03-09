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
}

bool Particle::update() {
    lastPos = pos;
    pos += vel;
    vel += acc;
    acc += jerk;
    life--;

    if (life < 0) {
        return true;
    }

    if (pos < 0) {
        //vel *= -1;
        return true;
    } else if (pos+length > 144) {
        //vel *= -1;
        return true;
    }
    return false;
}

void Particle::render() {
    //Serial.println("rendering");
    drawPixels(constrain(pos, 0, 144), constrain(lastPos, 0, 144), color);
}