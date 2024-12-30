// -- INCLUDES --
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"

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


typedef struct Screen {
    int width;
    int height;
    int isMenu;
    int isSetting;
} Screen;

// -- ENUMS --
enum Difficulty {
    EASY,
    MEDIUM,
    HARD,
    NDIFFICULTIES
};

// -- GLOBAL VARIABLES -- 
Player player;
Screen screen;
int gameShouldExit = FALSE;

int difficultySetting = EASY;
int difficultyDropDownOpen = FALSE;

float masterVolume = 1.0f;

Color spaceshipColors[5] = { RED, BLACK, WHITE, MAROON, GREEN  };
Color spaceshipColor = RED;
int selectedColorIndex = 0;


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

    while (!gameShouldExit) {
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

void SettingsMenu() {
    int pos[2] = { 150, 100 };
    int dim[2] = { 200, 30 };
    int horPad = 200;
    int verPad = 200;

    DrawText("Settings", screen.width / 2 - MeasureText("Settings", 30) / 2, 30, 30, LIGHTGRAY);

    // Difficulty dropdown
    GuiLabel((Rectangle){ pos[0], pos[1], dim[0], dim[1] }, "Difficulty:");
    if (GuiDropdownBox((Rectangle){ pos[0]+2*horPad, pos[1], dim[0], dim[1] }, "Easy;Medium;Hard", &difficultySetting, difficultyDropDownOpen)) {
        difficultyDropDownOpen = !difficultyDropDownOpen;
    }

    // Master volume slider
    GuiLabel((Rectangle){ pos[0], pos[1]+verPad, dim[0]*2, dim[1] }, "Master Volume:");
    GuiSlider((Rectangle){ pos[0]+2*horPad, pos[1]+verPad, dim[0], dim[1] }, NULL, NULL, &masterVolume, 0.0f, 1.0f);

    // Spaceship color selector
    char* colorNames[5] = { "RED", "BLACK", "WHITE", "MAROON", "GREEN" };
    GuiLabel((Rectangle){ pos[0], pos[1]+2*verPad, dim[0]*2, dim[1] }, "Spaceship Color:");
    if (GuiButton((Rectangle){ pos[0]+2*horPad, pos[1]+2*verPad, dim[0], dim[1] }, colorNames[selectedColorIndex])) {
        selectedColorIndex = (selectedColorIndex + 1) % 5;
        spaceshipColor = spaceshipColors[selectedColorIndex];
    }

    // Back button
    if (GuiButton((Rectangle){ pos[0], pos[1]+3*verPad, dim[0], dim[1] }, "Back")) {
        screen.isSetting = !screen.isSetting;
    }

}

void Menu() {
    // @@TODO: Rectangular window with three options: Resume, Settings, Exit
    // Settings: Resolution
    
    if (screen.isSetting) {
        return SettingsMenu();
    }

    const char* text = "Game Paused";
    const int titleFontSize = 50;
    DrawText(text, screen.width / 2 - MeasureText(text, titleFontSize) / 2, 50, titleFontSize, DARKGRAY);

    if (GuiButton((Rectangle){ (float)screen.width / 2 - 100, 150, 200, 30 }, "Resume")) {
        TraceLog(LOG_INFO, "Game unpaused");
        screen.isMenu = !screen.isMenu;
    }
    if (GuiButton((Rectangle){ (float)screen.width / 2 - 100, 200, 200, 30 }, "Settings")) {
        TraceLog(LOG_INFO, "Settings");
        screen.isSetting = !screen.isSetting;
    }
    if (GuiButton((Rectangle){ (float)screen.width / 2 - 100, 250, 200, 30 }, "Quit")) {
        TraceLog(LOG_INFO, "Quit game");
        gameShouldExit = TRUE;
    }
}

void Init() {
    // -- screen initialization --
    screen.width = 1600;
    screen.height = 900;
    screen.isMenu = FALSE;
    screen.isSetting = FALSE;

    // -- window definition --
    InitWindow(screen.width, screen.height, "ASTEROIDS");
    SetTargetFPS(60);

    // -- redefine escape key --
    SetExitKey(KEY_Q);

    // -- raygui --
    GuiSetStyle(DEFAULT, TEXT_SIZE, 30);

    // -- initialize player --
    InitPlayer();
}

void ProcessInput() {
    if (IsKeyDown(KEY_Q))
        gameShouldExit = TRUE;

    // @@NOTE: this key is for testing purposes only!!
    if (IsKeyDown(KEY_R)) {
        DrawText("RESET", 100, 100, 50, RED);
        InitPlayer();
        return;
    }

    // menu
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (screen.isSetting)
            screen.isSetting = !screen.isSetting;
        screen.isMenu = !screen.isMenu;
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

        if (screen.isMenu) {
            Menu();
            EndDrawing();
            return;
        }

        // @@NOTE: debug screen for testing purposes only!
/*
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
*/

        // draw player/ship
        DrawTriangle(player.vertices.v1, player.vertices.v2, player.vertices.v3, spaceshipColor);

    EndDrawing();
}

