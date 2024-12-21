// -- TODOS --
// @@TODO-- Mimick the movements of the actual asteroids game (rotation with left-right keys, glide-effect)
// @@TODO-- deltaTime to prevent weird movements
// @@TODO-- Rotations! !?!!

// -- INCLUDES --
#include <stdio.h>
#include <math.h>
#include "../include/raylib.h"

// -- DEFINITIONS --
#define SCRWIDTH    1600
#define SCRHEIGHT   900
#define SHIPWIDTH   100 // @@NOTE: the ship's width and height are arbitrary
#define SHIPHEIGHT  200
#ifndef PI
    #define PI          3.14159265359f
#endif

// -- STRUCTS --
typedef struct PlayerVertices {
    Vector2 v1;
    Vector2 v2;
    Vector2 v3;
} PlayerVertices;

typedef struct Player {
    Vector2         position;
    PlayerVertices  vertices;
    float           speed;
    float           acceleration;
    float           angle;
} Player;

// -- FUNCTIONS -- 
void Init();
void ProcessInput();
void Draw();

// -- DRAW -- 
int main(void) {
    Init();

    while (!WindowShouldClose()) {
        ProcessInput();
        Draw();
    }

    CloseWindow();
    return 0;
}


// -- FUNCTION IMPLEMENTATION --
void Init() {
    InitWindow(SCRWIDTH, SCRHEIGHT, "ASTEROIDS");
    SetTargetFPS(60);
}

void ProcessInput() {

}

void Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    EndDrawing();
}
