// -- INCLUDES --
#include <stdio.h>
#include <math.h>
#include "../include/raylib.h"

// -- GLOBAL VARIABLES --
const int scrWidth = 1600;
const int scrHeight = 900;
const int playerWidth = 100;
const int playerHeight = playerWidth * 2;
Vector2 playerSpeedVec = {10.0f, 10.0f};
const int playerSpeed = 10.0f;
const float pi = 3.14159265359f;
float rotationAngle = 0.0f;

// -- player logic -- @@TODO: render triangle, keyboard input to update player position
struct playerTriangleVertices {
    Vector2 v1;
    Vector2 v2;
    Vector2 v3;
} typedef ptv;

ptv playerTriVert;

Vector2 playerPosition = { scrWidth/2.0f, scrHeight/2.0f };

// @@TODO-- Mimick the movements of the actual asteroids game (rotation with left-right keys, glide-effect)
// @@TODO-- deltaTime to prevent weird movements
// @@TODO-- Rotations! !?!!


void updatePlayerPosition() {
    // NOTE-- triangles are rendered in counter clockwise direction
    // apply rotation to triangle
    float radians = rotationAngle * pi / 180.0f;
    float sinTheta = sinf(radians);
    float cosTheta = cosf(radians);

    playerTriVert.v1 = (Vector2) { // top 
        playerPosition.x + sinTheta * playerHeight, 
        playerPosition.y - cosTheta * playerHeight
    };
    playerTriVert.v2 = (Vector2) { // bottom left
        playerPosition.x - cosTheta * playerWidth,
        playerPosition.y - sinTheta * playerWidth 
    };
    playerTriVert.v3 = (Vector2) { // bottom right
        playerPosition.x + cosTheta * playerWidth,
        playerPosition.y + sinTheta * playerWidth 
    };
}

void processInput() {
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        rotationAngle -= 2.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))    
        rotationAngle += 2.0f;

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        playerPosition.y -= playerSpeedVec;
    else

    
    playerSpeedVec.x = sin(rotationAngle * pi / 180.0f) * playerSpeed;
    playerSpeedVec.y = cos(rotationAngle * pi / 180.0f) * playerSpeed;


    updatePlayerPosition();
}

// -- START HERE -- 
int main(void) {
    InitWindow(scrWidth, scrHeight, "ASTEROIDS!");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        processInput();
        BeginDrawing();
            ClearBackground(RAYWHITE);
            updatePlayerPosition();
            DrawTriangle(playerTriVert.v1, playerTriVert.v2, playerTriVert.v3, VIOLET);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
