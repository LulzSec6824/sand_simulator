#pragma once

#include "raylib.h"

enum ParticleType { AIR = 0, SAND, WATER, STONE };

struct Particle {
    ParticleType type = AIR;
    Color color       = BLACK;
    bool hasUpdated   = false;
};