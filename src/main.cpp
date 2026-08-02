#include "../raylib/src/raylib.h"
#include <vector>

static int currentWidth        = 800;
static int currentHeight       = 600;
static constexpr int CELL_SIZE = 1;

enum ParticleType { AIR = 0, SAND, WATER, STONE };

struct Particle {
    ParticleType type = AIR;
    Color color       = BLACK;
    bool hasUpdated   = false;
};

class SandEngine {
    int width  = 0;
    int height = 0;
    Texture2D screenTexture{};
    int cellSize;
    std::vector<Particle> grid{};
    std::vector<Color> pixelBuffer{};

    [[nodiscard]] int getIndex(int x, int y) const { return y * width + x; }
    [[nodiscard]] bool isValid(int x, int y) const { return (x >= 0 && x < width && y >= 0 && y < height); }

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

        int belowIdx = getIndex(x, belowY);
        if (grid[belowIdx].type == AIR || grid[belowIdx].type == WATER) {
            swapParticles(currentIdx, belowIdx);
            return;
        }

        bool canLeft  = (x > 0);
        bool canRight = (x < width - 1);

        int bottomLeftIdx  = canLeft ? getIndex(x - 1, belowY) : -1;
        int bottomRightIdx = canRight ? getIndex(x + 1, belowY) : -1;

        canLeft  = canLeft && (grid[bottomLeftIdx].type == AIR || grid[bottomLeftIdx].type == WATER);
        canRight = canRight && (grid[bottomRightIdx].type == AIR || grid[bottomRightIdx].type == WATER);

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

        int belowIdx = getIndex(x, belowY);

        if (grid[belowIdx].type == AIR) {
            swapParticles(currentIdx, belowIdx);
            return;
        }

        bool canLeft  = (x > 0);
        bool canRight = (x < width - 1);

        int bottomLeftIdx  = canLeft ? getIndex(x - 1, belowY) : -1;
        int bottomRightIdx = canRight ? getIndex(x + 1, belowY) : -1;

        canLeft  = canLeft && (grid[bottomLeftIdx].type == AIR);
        canRight = canRight && (grid[bottomRightIdx].type == AIR);

        if (canLeft && canRight) {
            int targetIdx = (GetRandomValue(0, 1) == 0) ? bottomLeftIdx : bottomRightIdx;
            swapParticles(currentIdx, targetIdx);
            return;
        }
        if (canLeft) {
            swapParticles(currentIdx, bottomLeftIdx);
            return;
        }
        if (canRight) {
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
    SandEngine(int screenWidth, int screenHeight, int cellSize) : cellSize(cellSize) { ResizeEngine(screenWidth, screenHeight); }

    ~SandEngine() {
        if (screenTexture.id > 0)
            UnloadTexture(screenTexture);
    }

    void ResizeEngine(int newScreenWidth, int newScreenHeight) {
        if (newScreenWidth <= 0 || newScreenHeight <= 0)
            return;

        int oldWidth                  = width;
        int oldHeight                 = height;
        std::vector<Particle> oldGrid = grid;

        if (screenTexture.id > 0) {
            UnloadTexture(screenTexture);
        }

        width  = newScreenWidth / cellSize;
        height = newScreenHeight / cellSize;

        grid.assign(width * height, Particle{AIR, BLACK, false});
        pixelBuffer.assign(static_cast<std::vector<Color, std::allocator<Color>>::size_type>(newScreenWidth) * newScreenHeight, BLACK);

        if (!oldGrid.empty()) {
            int yOffset = height - oldHeight;

            for (int y = 0; y < oldHeight; y++) {
                for (int x = 0; x < oldWidth; x++) {
                    int targetY = y + yOffset;
                    if (x < width && targetY >= 0 && targetY < height) {
                        int oldIdx   = y * oldWidth + x;
                        int newIdx   = targetY * width + x;
                        grid[newIdx] = oldGrid[oldIdx];
                    }
                }
            }
        }

        Image blankImage = GenImageColor(newScreenWidth, newScreenHeight, BLACK);
        screenTexture    = LoadTextureFromImage(blankImage);
        UnloadImage(blankImage);
    }

    void UpdatePhysics() {
        for (auto &i : grid) {
            i.hasUpdated = false;
        }

        bool leftToRight = (GetRandomValue(0, 1) == 0);

        for (int y = height - 1; y >= 0; y--) {
            if (leftToRight) {
                for (int x = 0; x < width; x++) {
                    int idx = getIndex(x, y);
                    if (grid[idx].hasUpdated || grid[idx].type == AIR || grid[idx].type == STONE)
                        continue;

                    if (grid[idx].type == SAND)
                        updateSandParticle(x, y);
                    else if (grid[idx].type == WATER)
                        updateWaterParticle(x, y);
                }
            } else {
                for (int x = width - 1; x >= 0; x--) {
                    int idx = getIndex(x, y);
                    if (grid[idx].hasUpdated || grid[idx].type == AIR || grid[idx].type == STONE)
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
        if (cellSize == 1) {
            for (size_t i = 0; i < grid.size(); ++i) {
                pixelBuffer[i] = grid[i].color;
            }
        } else {
            int screenWidth = width * cellSize;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    Color pColor = grid[getIndex(x, y)].color;
                    for (int cy = 0; cy < cellSize; cy++) {
                        for (int cx = 0; cx < cellSize; cx++) {
                            pixelBuffer[(y * cellSize + cy) * screenWidth + (x * cellSize + cx)] = pColor;
                        }
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
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(currentWidth, currentHeight, "HIGH-PERFORMANCE SAND SIMULATOR");
    SetTargetFPS(60);

    SandEngine simulation(currentWidth, currentHeight, CELL_SIZE);
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
        if (IsWindowResized()) {
            currentWidth  = GetRenderWidth();
            currentHeight = GetRenderHeight();
            simulation.ResizeEngine(currentWidth, currentHeight);
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mPos = GetMousePosition();
            simulation.SpawnElement(static_cast<int>(mPos.x), static_cast<int>(mPos.y), activeBrush, 15);
        }

        simulation.UpdatePhysics();
        BeginDrawing();
        ClearBackground(BLACK);
        simulation.Render();
        DrawFPS(10, 10);
        DrawText(" (1)Sand (2)Water (3)Stone (4)Eraser (F11)Fullscreen", 10, 35, 16, RAYWHITE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}