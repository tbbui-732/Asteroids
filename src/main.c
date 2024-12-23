// -- TODOS --
// @@TODO-- Mimick the movements of the actual asteroids game (rotation with left-right keys, glide-effect)
// @@TODO-- deltaTime to prevent weird movements
// @@TODO-- Rotations! !?!!

// -- INCLUDES --
#include <stdio.h>
#include <math.h>
#include "../include/raylib.h"

// -- DEFINITIONS --
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


// @@TODO: Work on this!!!!!!!!!!!!
typedef struct Screen {
    int width;
    int height;
} Screen;

// -- GLOBAL VARIABLES -- 
Player player;
Screen screen;

// -- FUNCTIONS -- 
void InitPlayer();
void MovePlayer();
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
    player.position         = (Vector2) {screen.width/2.0f, screen.height/2.0f};
    MovePlayer();
    player.speed            = (Vector2) {0.0f, 0.0f};
    player.acceleration     =  0.0f;
    player.angle            =  0.0f;
}

void MovePlayer() {
    player.vertices.v1 = (Vector2) {
        player.position.x + sinf(player.angle * DEG2RAD) * SHIPHEIGHT,
        player.position.y - cosf(player.angle * DEG2RAD) * SHIPHEIGHT
    };

    player.vertices.v2 = (Vector2) {
        player.position.x - cosf(player.angle * DEG2RAD) * SHIPWIDTH,
        player.position.y - sinf(player.angle * DEG2RAD) * SHIPWIDTH 
    };

    player.vertices.v3 = (Vector2) {
        player.position.x + cosf(player.angle * DEG2RAD) * SHIPWIDTH,
        player.position.y + sinf(player.angle * DEG2RAD) * SHIPWIDTH 
    };
}

void Init() {
    // -- window definition --
    screen.width = 1600;
    screen.height = 900;
    InitWindow(screen.width, screen.height, "ASTEROIDS");
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
        player.angle -= ROTATIONSPEED;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        player.angle += ROTATIONSPEED;

    // player speed @@NOTE: Why do we need to do this?
    player.speed.x = sin(player.angle * DEG2RAD) * SHIPSPEED;
    player.speed.y = cos(player.angle * DEG2RAD) * SHIPSPEED;

    // player acceleration
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {    // step on the pedal!
        player.acceleration += 0.05f;
        if (player.acceleration > SHIPMAXACCELERATION) {
            player.acceleration = SHIPMAXACCELERATION;
        }
    } else {
        if (player.acceleration > 0.0f) {           // slow down there bucko
            player.acceleration -= 0.05f;
        } else {
            player.acceleration = 0.0f;
        }
    }

    // player position
    player.position.x += (player.speed.x * player.acceleration > SHIPMAXSPEED) ? SHIPMAXSPEED : player.speed.x * player.acceleration;
    player.position.y -= (player.speed.y * player.acceleration > SHIPMAXSPEED) ? SHIPMAXSPEED : player.speed.y * player.acceleration;
    MovePlayer();
}


void Draw() {
    BeginDrawing();
        ClearBackground(RAYWHITE);

        // draw player/ship
        DrawTriangle(player.vertices.v1, player.vertices.v2, player.vertices.v3, MAROON);

    EndDrawing();
}
