#include "engine.hpp"
#include "raylib.h"

int main() {
    const int SCREEN_WIDTH  = 800;
    const int SCREEN_HEIGHT = 600;
    const int CELL_SIZE     = 4;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "SAND SIMULATOR in RAYLIB & CPP");
    SetTargetFPS(60);

    SandEngine simulation(SCREEN_WIDTH, SCREEN_HEIGHT, CELL_SIZE);
    ParticleType activeBrush = SAND;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ONE))
            activeBrush = SAND;
        if (IsKeyPressed(KEY_TWO))
            activeBrush = STONE;
        if (IsKeyPressed(KEY_THREE))
            activeBrush = AIR;

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mPos = GetMousePosition();
            simulation.SpawnElement(mPos.x, mPos.y, activeBrush, 3);
        }

        simulation.UpdatePhysics();

        BeginDrawing();
        ClearBackground(BLACK);

        simulation.Render();

        DrawFPS(10, 10);
        DrawText("Press: [1] Sand  [2] Stone  [3] Eraser", 10, 35, 16, RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
