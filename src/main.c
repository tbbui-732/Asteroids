// -- TODOS --
// @@TODO-- Mimick the movements of the actual asteroids game (rotation with left-right keys, glide-effect)
// @@TODO-- deltaTime to prevent weird movements
// @@TODO-- Rotations! !?!!

// -- INCLUDES --
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/raylib.h"

// -- DEFINITIONS --
#define SHIPWIDTH               50 // @@NOTE: the ship's width and height are arbitrary
#define SHIPHEIGHT              150
#define SHIPSPEED               10
#define SHIPMAXACCELERATION     2
#define SHIPMAXSPEED            20
#define ROTATIONSPEED           5
#define TRUE                    1
#define FALSE                   0

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
void RotateVertex(Vector2* origVector, float* centerX, float* centerY, float* angle);
void WallCollision();

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

void RotateVertex(Vector2* origVector, float* centerX, float* centerY, float* angle) {
    // translate to origin 
    float tx, ty;
    tx = origVector->x - *centerX;
    ty = origVector->y - *centerY;

    // rotate
    float rx, ry;
    float rad = *angle * DEG2RAD;
    rx = tx * cos(rad) - ty * sin(rad);
    ry = tx * sin(rad) + ty * cos(rad);

    // translate back
    origVector->x = rx + *centerX;
    origVector->y = ry + *centerY;
}

void WallCollision() {
    // @@NOTE: Invoked every time player position is updated
    // check left and right walls
    float verticalPad   = SHIPHEIGHT*1.15f;
    float horizontalPad = SHIPWIDTH*1.5f;
    float verticalSmoothingPad = -SHIPHEIGHT*0.50;
    float horizontalSmoothingPad = -SHIPWIDTH*1.55;

    if (player.position.x + horizontalPad < 0)                  // left wall
        player.position.x = screen.width - horizontalSmoothingPad;

    else if (player.position.x - horizontalPad > screen.width)  // right wall
        player.position.x = 0.0f + horizontalSmoothingPad;

    if (player.position.y + verticalPad < 0)                    // top wall
        player.position.y = screen.height - verticalSmoothingPad;

    else if (player.position.y - verticalPad > screen.height)   // bottom wall
        player.position.y = 0.0f + verticalSmoothingPad;
}

void MovePlayer() {
    float angle = player.angle;
    float centerX = player.position.x;
    float centerY = player.position.y;
    
    // original vertices
    Vector2 v1 = (Vector2) { centerX, centerY - SHIPHEIGHT/2.0f };
    Vector2 v2 = (Vector2) { centerX - SHIPWIDTH, centerY + SHIPHEIGHT/2.0f };
    Vector2 v3 = (Vector2) { centerX + SHIPWIDTH, centerY + SHIPHEIGHT/2.0f };

    // rotate them around
    RotateVertex(&v1, &centerX, &centerY, &angle);
    RotateVertex(&v2, &centerX, &centerY, &angle);
    RotateVertex(&v3, &centerX, &centerY, &angle);

    // apply vertices
    player.vertices.v1 = v1;
    player.vertices.v2 = v2;
    player.vertices.v3 = v3;
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
        if (player.acceleration <= SHIPMAXACCELERATION) {
            player.acceleration += 0.05f;
        }
    } else {
        if (player.acceleration > 0.0f) {           // slow down there bucko
            player.acceleration -= 0.03f;
        } else if (player.acceleration < 0.0f) {
            player.acceleration = 0.0f;
        }
    }

    // player position
    float deltaTime = GetFrameTime() * 40;
    player.position.x += player.speed.x * player.acceleration * deltaTime;
    player.position.y -= player.speed.y * player.acceleration * deltaTime;

    WallCollision();
    MovePlayer();
}


void Draw() {
    BeginDrawing();
        ClearBackground(RAYWHITE);

        // @@NOTE: debug screen for testing purposes only!
        char angleBuffer[128];
        char speedBuffer[128];
        char accelBuffer[128];
        int out;
        
        out = snprintf(angleBuffer, 128, "angle (degrees):\t%.0f", player.angle);
        if (out <= -1) {
            printf("ERROR: Unable to pass angle value to buffer\n");
            exit(1);
        }
        out = snprintf(speedBuffer, 128, "speed direction:\t(%.0f, %.0f)", player.speed.x, player.speed.y);
        if (out <= -1) {
            printf("ERROR: Unable to pass speed value to buffer\n");
            exit(1);
        }
        out = snprintf(accelBuffer, 128, "acceleration:\t%.2f", player.acceleration);
        if (out <= -1) {
            printf("ERROR: Unable to pass acceleration value to buffer\n");
            exit(1);
        }

        DrawText(angleBuffer, screen.width/25, screen.height/10, 50, MAROON);
        DrawText(speedBuffer, screen.width/25, screen.height/6,  50, MAROON);
        DrawText(accelBuffer, screen.width/25, screen.height/4,  50, MAROON);

        // draw player/ship
        DrawTriangle(player.vertices.v1, player.vertices.v2, player.vertices.v3, MAROON);

    EndDrawing();
}

