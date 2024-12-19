// -- INCLUDES --
#include <stdio.h>
#include "../include/raylib.h"

// -- GLOBAL VARIABLES --
const int scrWidth = 1600;
const int scrHeight = 900;
const int playerWidth = 100;
const int playerHeight = playerWidth * 2;

// -- player logic -- @@TODO: render triangle, keyboard input to update player position
struct playerTriangleVertices {
    Vector2 v1;
    Vector2 v2;
    Vector2 v3;
} typedef ptv;

ptv playerTriVert;

Vector2 playerPosition = { scrWidth/2.0f, scrHeight/2.0f };

void updatePlayerPosition() {
    // NOTE-- recall that triangles are rendered in counter clockwise direction
    playerTriVert.v1 = (Vector2) {playerPosition.x, playerPosition.y - playerHeight}; // Top vertex
    playerTriVert.v2 = (Vector2) {playerPosition.x - playerWidth, playerPosition.y};  // Bottom-left vertex
    playerTriVert.v3 = (Vector2) {playerPosition.x + playerWidth, playerPosition.y};  // Bottom-right vertex
}


void drawPlayer() {
    DrawTriangle(playerTriVert.v1, playerTriVert.v2, playerTriVert.v3, VIOLET);
}

void processInput() {
}

// -- START HERE -- 
int main(void) {
    InitWindow(scrWidth, scrHeight, "ASTEROIDS!");
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            updatePlayerPosition();
            drawPlayer();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
