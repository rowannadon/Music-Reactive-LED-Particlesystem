#include "Particle.h"

class ParticleContainer {
    public:
        Particle** particles;
        int maxParticles;
        int topParticleIndex;

        ParticleContainer(int);
        void emit(float, float, float, float, float, int, int*, CRGB);
        void kill(int);
        void update();
        void render();
};

ParticleContainer::ParticleContainer(int mp) {
    maxParticles = mp;
    particles = new Particle*[mp];
}

void ParticleContainer::emit(float size, float position, float velocity, float acceleration, float jerk, int lifespan, int* brightnessProfile, CRGB color) {
    if (topParticleIndex < maxParticles) {
        // Serial.print("Emitting particle with data: ");
        // Serial.print(topParticleIndex);
        // Serial.print("\t");
        // Serial.println(acceleration);
        particles[topParticleIndex++] = new Particle(position, size, velocity, acceleration, jerk, lifespan, brightnessProfile, color);
    }
}

void ParticleContainer::kill(int index) {
    topParticleIndex--;
    particles[index] = particles[topParticleIndex];
    particles[topParticleIndex] = NULL;
}

void ParticleContainer::update() {
    for (int i = 0; i < topParticleIndex; i++) {
        if (particles[i]->update())
            kill(i);
    }
    //Serial.println(topParticleIndex);
}

void ParticleContainer::render() {
    for (int i = 0; i < topParticleIndex; i++) {
        particles[i]->render();
        //Serial.print(particles[i]->pos);
        //Serial.print("\t");
    }
    //Serial.println();
}