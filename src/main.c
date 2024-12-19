// -- INCLUDES --
#include <stdio.h>
#include "../include/raylib.h"

// -- GLOBAL VARIABLES --
const int scrWidth = 1600;
const int scrHeight = 900;

// -- START HERE -- 
int main(void) {
    InitWindow(scrWidth, scrHeight, "ASTEROIDS!");
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
