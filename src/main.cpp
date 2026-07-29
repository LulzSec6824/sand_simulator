#include "../raylib/src/raylib.h"
#include <vector>

enum ParticleType { AIR = 0, SAND, WATER, STONE };

struct Particle {
    ParticleType type = AIR;
    Color color       = BLACK;
    bool hasUpdated   = false;
};

class SandEngine {
    int width;
    int height;
    int cellSize;
    std::vector<Particle> grid;

    std::vector<Color> pixelBuffer;
    Texture2D screenTexture;

    int getIndex(int x, int y) const { return y * width + x; }
    bool isValid(int x, int y) const { return (x >= 0 && x < width && y >= 0 && y < height); }

    void swapParticles(int idxA, int idxB) {
        Particle temp         = grid[idxA];
        grid[idxA]            = grid[idxB];
        grid[idxA].hasUpdated = true;
        grid[idxB]            = temp;
        grid[idxB].hasUpdated = true;
    }

    void updateSandParticle(int x, int y) {
        int currentIdx = getIndex(x, y);
        int belowY     = y + 1;
        if (belowY >= height)
            return;

        int belowIdx       = getIndex(x, belowY);
        int bottomLeftIdx  = getIndex(x - 1, belowY);
        int bottomRightIdx = getIndex(x + 1, belowY);

        if (grid[belowIdx].type == AIR || grid[belowIdx].type == WATER) {
            swapParticles(currentIdx, belowIdx);
            return;
        }

        bool canLeft  = (x > 0) && (grid[bottomLeftIdx].type == AIR || grid[bottomLeftIdx].type == WATER);
        bool canRight = (x < width - 1) && (grid[bottomRightIdx].type == AIR || grid[bottomRightIdx].type == WATER);

        if (canLeft && canRight) {
            int targetIdx = (GetRandomValue(0, 1) == 0) ? bottomLeftIdx : bottomRightIdx;
            swapParticles(currentIdx, targetIdx);
        } else if (canLeft) {
            swapParticles(currentIdx, bottomLeftIdx);
        } else if (canRight) {
            swapParticles(currentIdx, bottomRightIdx);
        }
    }

    void updateWaterParticle(int x, int y) {
        int currentIdx = getIndex(x, y);
        int belowY     = y + 1;
        if (belowY >= height)
            return;

        int belowIdx       = getIndex(x, belowY);
        int bottomLeftIdx  = getIndex(x - 1, belowY);
        int bottomRightIdx = getIndex(x + 1, belowY);

        if (grid[belowIdx].type == AIR) {
            swapParticles(currentIdx, belowIdx);
            return;
        }

        bool canLeft  = (x > 0) && (grid[bottomLeftIdx].type == AIR);
        bool canRight = (x < width - 1) && (grid[bottomRightIdx].type == AIR);

        if (canLeft && canRight) {
            int targetIdx = (GetRandomValue(0, 1) == 0) ? bottomLeftIdx : bottomRightIdx;
            swapParticles(currentIdx, targetIdx);
            return;
        } else if (canLeft) {
            swapParticles(currentIdx, bottomLeftIdx);
            return;
        } else if (canRight) {
            swapParticles(currentIdx, bottomRightIdx);
            return;
        }

        bool slideLeft  = (x > 0) && (grid[getIndex(x - 1, y)].type == AIR);
        bool slideRight = (x < width - 1) && (grid[getIndex(x + 1, y)].type == AIR);

        if (slideLeft && slideRight) {
            int targetIdx = (GetRandomValue(0, 1) == 0) ? getIndex(x - 1, y) : getIndex(x + 1, y);
            swapParticles(currentIdx, targetIdx);
        } else if (slideLeft) {
            swapParticles(currentIdx, getIndex(x - 1, y));
        } else if (slideRight) {
            swapParticles(currentIdx, getIndex(x + 1, y));
        }
    }

  public:
    SandEngine(int screenWidth, int screenHeight, int cellSize) : cellSize(cellSize) {
        width  = screenWidth / cellSize;
        height = screenHeight / cellSize;
        grid.resize(width * height, Particle{AIR, BLACK, false});

        pixelBuffer.resize(screenWidth * screenHeight, BLACK);

        Image blankImage = GenImageColor(screenWidth, screenHeight, BLACK);
        screenTexture    = LoadTextureFromImage(blankImage);
        UnloadImage(blankImage);
    }

    ~SandEngine() { UnloadTexture(screenTexture); }

    void UpdatePhysics() {
        for (auto &particle : grid) {
            particle.hasUpdated = false;
        }

        bool leftToRight = (GetRandomValue(0, 1) == 0);

        for (int y = height - 1; y >= 0; y--) {
            if (leftToRight) {
                for (int x = 0; x < width; x++) {
                    int idx = getIndex(x, y);
                    if (grid[idx].hasUpdated)
                        continue;

                    if (grid[idx].type == SAND)
                        updateSandParticle(x, y);
                    else if (grid[idx].type == WATER)
                        updateWaterParticle(x, y);
                }
            } else {
                for (int x = width - 1; x >= 0; x--) {
                    int idx = getIndex(x, y);
                    if (grid[idx].hasUpdated)
                        continue;

                    if (grid[idx].type == SAND)
                        updateSandParticle(x, y);
                    else if (grid[idx].type == WATER)
                        updateWaterParticle(x, y);
                }
            }
        }
    }

    void Render() {
        int screenWidth = width * cellSize;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                Color pColor = grid[getIndex(x, y)].color;

                // Expand cell block sizes directly into full-resolution texture slices
                for (int cy = 0; cy < cellSize; cy++) {
                    for (int cx = 0; cx < cellSize; cx++) {
                        int pixelX                                 = x * cellSize + cx;
                        int pixelY                                 = y * cellSize + cy;
                        pixelBuffer[pixelY * screenWidth + pixelX] = pColor;
                    }
                }
            }
        }

        UpdateTexture(screenTexture, pixelBuffer.data());
        DrawTexture(screenTexture, 0, 0, WHITE);
    }

    void SpawnElement(int mouseX, int mouseY, ParticleType type, int radius) {
        int gridX = mouseX / cellSize;
        int gridY = mouseY / cellSize;

        for (int dy = -radius; dy <= radius; dy++) {
            for (int dx = -radius; dx <= radius; dx++) {
                if (dx * dx + dy * dy <= radius * radius) {
                    int nx = gridX + dx;
                    int ny = gridY + dy;

                    if (isValid(nx, ny)) {
                        int idx = getIndex(nx, ny);
                        if (grid[idx].type == STONE && type != AIR)
                            continue;

                        if (type == SAND) {
                            int variant = GetRandomValue(0, 2);
                            if (variant == 0) {
                                unsigned char tone = 190 + GetRandomValue(0, 40);
                                grid[idx]          = Particle{SAND, Color{tone, static_cast<unsigned char>(tone * 0.82f), 60, 255}, false};
                            } else {
                                unsigned char tone = 130 + GetRandomValue(0, 30);
                                grid[idx]          = Particle{SAND, Color{tone, static_cast<unsigned char>(tone * 0.75f), 40, 255}, false};
                            }
                        } else if (type == WATER) {
                            unsigned char blueTone = 200 + GetRandomValue(0, 55);
                            grid[idx]              = Particle{WATER, Color{30, 120, blueTone, 255}, false};
                        } else if (type == STONE) {
                            unsigned char gray = 90 + GetRandomValue(0, 40);
                            grid[idx]          = Particle{STONE, Color{gray, gray, gray, 255}, false};
                        } else {
                            grid[idx] = Particle{AIR, BLACK, false};
                        }
                    }
                }
            }
        }
    }
};

int main() {
    int SCREEN_WIDTH        = 800;
    int SCREEN_HEIGHT       = 600;
    constexpr int CELL_SIZE = 1;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "HIGH-PERFORMANCE SAND SIMULATOR");
    SetTargetFPS(60);

    SandEngine simulation(SCREEN_WIDTH, SCREEN_HEIGHT, CELL_SIZE);
    ParticleType activeBrush = SAND;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ONE))
            activeBrush = SAND;
        if (IsKeyPressed(KEY_TWO))
            activeBrush = WATER;
        if (IsKeyPressed(KEY_THREE))
            activeBrush = STONE;
        if (IsKeyPressed(KEY_FOUR))
            activeBrush = AIR;
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }
        if (IsWindowFullscreen()) {
            int monitor   = GetCurrentMonitor();
            SCREEN_HEIGHT = GetMonitorHeight(monitor);
            SCREEN_WIDTH  = GetMonitorWidth(monitor);
        } else {
            SCREEN_HEIGHT = GetRenderHeight();
            SCREEN_WIDTH  = GetRenderWidth();
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mPos = GetMousePosition();
            simulation.SpawnElement(static_cast<int>(mPos.x), static_cast<int>(mPos.y), activeBrush, 8);
        }

        simulation.UpdatePhysics();

        BeginDrawing();
        ClearBackground(BLACK);

        simulation.Render();

        DrawFPS(10, 10);
        DrawText("[1] Sand [2] Water [3] Stone [4] Eraser", 10, 35, 16, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
