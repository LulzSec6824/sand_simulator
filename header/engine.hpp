#pragma once
#include "particle.hpp"
#include <vector>

class SandEngine {
  private:
    int width;
    int height;
    int cellSize;

    std::vector<Particle> grid;
    Image screenImage;
    Texture2D screenTexture;

    // Helper functions for safe matrix index management
    int getIndex(int x, int y) const;
    bool isValid(int x, int y) const;
    void updateSandParticle(int x, int y);

  public:
    SandEngine(int screenWidth, int screenHeight, int cellSize);
    ~SandEngine();

    void UpdatePhysics();
    void Render();
    void SpawnElement(int mouseX, int mouseY, ParticleType type, int radius);
};
