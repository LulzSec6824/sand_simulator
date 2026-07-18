#include "engine.hpp"
#include "particle.hpp"
#include "raylib.h"

SandEngine::SandEngine(int screenWidth, int screenHeight, int cellSz) : cellSize(cellSz) {
    width  = screenWidth / cellSize;
    height = screenHeight / cellSize;

    grid.resize(width * height, Particle{AIR, BLACK, false});

    screenImage   = GenImageColor(screenWidth, screenHeight, BLACK);
    screenTexture = LoadTextureFromImage(screenImage);
}

SandEngine::~SandEngine() {
    UnloadImage(screenImage);
    UnloadTexture(screenTexture);
}

int SandEngine::getIndex(int x, int y) const { return y * width + x; }

bool SandEngine::isValid(int x, int y) const { return (x >= 0 && x < width && y >= 0 && y < height); }

void SandEngine::SpawnElement(int mouseX, int mouseY, ParticleType type, int radius) {
    int gridX = mouseX / cellSize;
    int gridY = mouseY / cellSize;

    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            int nx = gridX + dx;
            int ny = gridY + dy;

            if (isValid(nx, ny)) {
                int idx = getIndex(nx, ny);
                if (type == SAND) {
                    unsigned char tone = 180 + GetRandomValue(0, 75);
                    grid[idx]          = Particle{SAND, Color{tone, static_cast<unsigned char>(tone * 0.8f), 50, 255}, false};
                } else if (type == STONE) {
                    grid[idx] = Particle{STONE, GRAY, false};
                } else {
                    grid[idx] = Particle{AIR, BLACK, false};
                }
            }
        }
    }
}

void SandEngine::updateSandParticle(int x, int y) {
    int currentIdx = getIndex(x, y);

    int belowY = y + 1;
    if (belowY >= height)
        return;

    int belowIdx       = getIndex(x, belowY);
    int bottomLeftIdx  = getIndex(x - 1, belowY);
    int bottomRightIdx = getIndex(x + 1, belowY);

    if (grid[belowIdx].type == AIR) {
        grid[belowIdx]            = grid[currentIdx];
        grid[belowIdx].hasUpdated = true;
        grid[currentIdx]          = Particle{AIR, BLACK, false};
    } else {
        bool canLeft  = (x > 0) && (grid[bottomLeftIdx].type == AIR);
        bool canRight = (x < width - 1) && (grid[bottomRightIdx].type == AIR);

        if (canLeft && canRight) {
            int targetIdx              = (GetRandomValue(0, 1) == 0) ? bottomLeftIdx : bottomRightIdx;
            grid[targetIdx]            = grid[currentIdx];
            grid[targetIdx].hasUpdated = true;
            grid[currentIdx]           = Particle{AIR, BLACK, false};
        } else if (canLeft) {
            grid[bottomLeftIdx]            = grid[currentIdx];
            grid[bottomLeftIdx].hasUpdated = true;
            grid[currentIdx]               = Particle{AIR, BLACK, false};
        } else if (canRight) {
            grid[bottomRightIdx]            = grid[currentIdx];
            grid[bottomRightIdx].hasUpdated = true;
            grid[currentIdx]                = Particle{AIR, BLACK, false};
        }
    }
}

void SandEngine::UpdatePhysics() {
    for (auto &particle : grid) {
        particle.hasUpdated = false;
    }

    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            int idx = getIndex(x, y);
            if (grid[idx].type == SAND && !grid[idx].hasUpdated) {
                updateSandParticle(x, y);
            }
        }
    }
}

void SandEngine::Render() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Color color = grid[getIndex(x, y)].color;
            ImageDrawRectangle(&screenImage, x * cellSize, y * cellSize, cellSize, cellSize, color);
        }
    }
    UpdateTexture(screenTexture, screenImage.data);
    DrawTexture(screenTexture, 0, 0, WHITE);
}
