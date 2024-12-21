// -- TODOS --
// @@TODO-- Mimick the movements of the actual asteroids game (rotation with left-right keys, glide-effect)
// @@TODO-- deltaTime to prevent weird movements
// @@TODO-- Rotations! !?!!

// -- INCLUDES --
#include <stdio.h>
#include <math.h>
#include "../include/raylib.h"

// -- DEFINITIONS --
#define SCRWIDTH                1600
#define SCRHEIGHT               900
#define SHIPWIDTH               50 // @@NOTE: the ship's width and height are arbitrary
#define SHIPHEIGHT              150
#define SHIPSPEED               10
#define SHIPMAXACCELERATION     5
#define SHIPMAXSPEED            20
#define ROTATIONSPEED           5

// -- STRUCTS --
typedef struct PlayerVertices {
    Vector2 v1;
    Vector2 v2;
    Vector2 v3;
} PlayerVertices;

typedef struct Player {
    Vector2         position;
    PlayerVertices  vertices;
    Vector2         speed;
    float           acceleration;
    float           angle;
} Player;

// -- GLOBAL VARIABLES -- 
Player player;

// -- FUNCTIONS -- 
void InitPlayer();
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
void InitPlayer() {
    player.position         = (Vector2) {SCRWIDTH/2.0f, SCRHEIGHT/2.0f};
    player.vertices.v1      = (Vector2) {player.position.x, player.position.y};
    player.vertices.v2      = (Vector2) {player.position.x - SHIPWIDTH, player.position.y + SHIPHEIGHT};
    player.vertices.v3      = (Vector2) {player.position.x + SHIPWIDTH, player.position.y + SHIPHEIGHT};
    player.speed            = (Vector2) {0.0f, 0.0f};
    player.acceleration     =  0.0f;
    player.angle            =  0.0f;
}

void Init() {
    // -- window definition --
    InitWindow(SCRWIDTH, SCRHEIGHT, "ASTEROIDS");
    SetTargetFPS(60);

    // -- initialize player --
    InitPlayer();
}

void ProcessInput() {
    // @@NOTE: this key is for testing purposes only!!
    if (IsKeyDown(KEY_R)) {
        DrawText("RESET", 100, 100, 50, RED);
        InitPlayer();
        return;
    }

    // rotation angles
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        player.angle -= ROTATIONDELTA;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        player.angle += ROTATIONDELTA;

    // player speed @@NOTE: Why do we need to do this?
    player.speed.x = sin(player.angle * DEG2RAD) * SHIPSPEED;
    player.speed.y = cos(player.angle * DEG2RAD) * SHIPSPEED;

    // player acceleration
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {    // step on the pedal!
        player.acceleration += 0.05f;
    } else {
        if (player.acceleration > 0.0f) {           // slow down there bucko
            player.acceleration -= 0.05f;
        } else {
            player.acceleration = 0.0f;
        }
    }

    // player position
    player.position.x += player.speed.x * player.acceleration;
    player.position.y -= player.speed.y * player.acceleration;

    // player vertices
    player.vertices.v1 = (Vector2) {player.position.x, player.position.y};
    player.vertices.v2 = (Vector2) {player.position.x - SHIPWIDTH, player.position.y + SHIPHEIGHT};
    player.vertices.v3 = (Vector2) {player.position.x + SHIPWIDTH, player.position.y + SHIPHEIGHT};
}


void Draw() {
    BeginDrawing();
        ClearBackground(RAYWHITE);

        // draw player/ship
        DrawTriangle(player.vertices.v1, player.vertices.v2, player.vertices.v3, MAROON);

    EndDrawing();
}
