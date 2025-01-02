// --------------
// -- INCLUDES --
// --------------
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"

// -----------------
// -- DEFINITIONS --
// -----------------
#define SHIPWIDTH               25 // @@NOTE: the ship's width and height are arbitrary
#define SHIPHEIGHT              75 
#define SHIPSPEED               5
#define SHIPMAXACCELERATION     2
#define SHIPMAXSPEED            20
#define ROTATIONSPEED           5
#define MAXNUMPROJECTILES       30
#define PROJECTILESPEED         20
#define MAXNUMASTEROIDS         10
#define TRUE                    1
#define FALSE                   0

// -----------
// -- ENUMS --
// -----------
enum Difficulty {
    EASY,
    MEDIUM,
    HARD,
    NDIFFICULTIES
};

enum Size {
    SMALL,
    REGULAR,
    LARGE
};

// -------------
// -- STRUCTS --
// -------------
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

typedef struct ProjEntity {
    Vector2 position;
    float   velocity;
    float   angle;
    int     active;
} ProjEntity;

typedef struct AsteroidEntity {
    Vector2     position;
    float       velocity;
    float       angle;
    int         numVertices;
    int         active;
    enum Size   size;    
} AsteroidEntity;

// ---------------------- 
// -- GLOBAL VARIABLES -- 
// ---------------------- 
Player player;
Screen screen;
int gameShouldExit = FALSE;
float deltaTime;

// settings
int difficultySetting = EASY;
int difficultyDropDownOpen = FALSE;
float masterVolume = 1.0f;
Color spaceshipColors[5] = { RED, BLACK, WHITE, MAROON, GREEN };
Color spaceshipColor = RED;
int selectedColorIndex = 0;

// --------------
// -- ENTITIES --
// --------------
// projectiles
int projEntIdx = 0;
ProjEntity projectiles[MAXNUMPROJECTILES];
int numProjectiles = 0;

// asteroids (enemies)
int astEntIdx = 0;
AsteroidEntity asteroids[MAXNUMASTEROIDS];
int numAsteroids = 0;

// --------------- 
// -- FUNCTIONS -- 
// --------------- 
void SettingsMenu();
void Menu();
void PlayerWallCollision();
void PlayerShootProjectile();
void PlayerRotate(Vector2* origVector, float* centerX, float* centerY, float* angle);
void PlayerMove();
void ProcessInput();
void Draw();
Vector2 AsteroidGenerateSpawnPosition();
void AsteroidSpawn();
void AsteroidDraw(AsteroidEntity* pAsteroid);
void AsteroidMove(AsteroidEntity* pAsteroid);
int AsteroidWallCollision(AsteroidEntity* pAsteroid);

// ---------- 
// -- DRAW -- 
// ---------- 
int main(void) {
    // --------------------------------
    // ---------- INITIALIZE ----------
    // --------------------------------
    srand(time(NULL)); // rng

    // screen initialization
    screen.width = 1600;
    screen.height = 900;
    screen.isMenu = FALSE;
    screen.isSetting = FALSE;

    // window definition
    InitWindow(screen.width, screen.height, "ASTEROIDS");
    SetTargetFPS(300);

    // redefine escape key
    SetExitKey(KEY_Q);

    // raygui
    GuiSetStyle(DEFAULT, TEXT_SIZE, 30);

    // initialize player
    player.position         = (Vector2) {screen.width/2.0f, screen.height/2.0f};
    player.speed            = (Vector2) {0.0f, 0.0f};
    player.acceleration     =  0.0f;
    player.angle            =  0.0f;

    // ---------------------------------
    // ---------- RENDER LOOP ----------
    // ---------------------------------
    while (!gameShouldExit) {
        ProcessInput();
        Draw();
    }
    CloseWindow();
    return 0;
}

// -------------------------------------------------------------------------------------
// ------------------------------ MENU / PAUSE / SETTINGS ------------------------------
// -------------------------------------------------------------------------------------
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
// -------------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------
// ------------------------------ PLAYER / SHIP LOGIC ------------------------------
// ---------------------------------------------------------------------------------
void PlayerWallCollision() {
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

void PlayerShootProjectile() {
    ProjEntity projectile = (ProjEntity) { 
        (Vector2){player.position.x, player.position.y}, PROJECTILESPEED, player.angle, TRUE 
    };
    projectiles[projEntIdx++] = projectile;

    if (projEntIdx >= MAXNUMPROJECTILES)
        projEntIdx = 0;
}

void PlayerRotate(Vector2* origVector, float* centerX, float* centerY, float* angle) {
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

void PlayerMove() {
    float angle = player.angle;
    float centerX = player.position.x;
    float centerY = player.position.y;

    // original vertices
    Vector2 v1 = (Vector2) { centerX, centerY - SHIPHEIGHT/2.0f };
    Vector2 v2 = (Vector2) { centerX - SHIPWIDTH, centerY + SHIPHEIGHT/2.0f };
    Vector2 v3 = (Vector2) { centerX + SHIPWIDTH, centerY + SHIPHEIGHT/2.0f };

    // rotate them around
    PlayerRotate(&v1, &centerX, &centerY, &angle);
    PlayerRotate(&v2, &centerX, &centerY, &angle);
    PlayerRotate(&v3, &centerX, &centerY, &angle);

    // apply vertices
    player.vertices.v1 = v1;
    player.vertices.v2 = v2;
    player.vertices.v3 = v3;
}
// ---------------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// ------------------------------ KEYBOARD INPUT ------------------------------
// ----------------------------------------------------------------------------
void ProcessInput() {
    if (IsKeyDown(KEY_Q))
        gameShouldExit = TRUE;

    // ------- TESTING --------
    // spawn an asteroid on command
    if (IsKeyPressed(KEY_Y)) {
        numAsteroids++;
        AsteroidSpawn();
    }
    // ------------------------

    // menu
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (screen.isSetting)
            screen.isSetting = !screen.isSetting;
        screen.isMenu = !screen.isMenu;
        return;
    }

    // rotation angles
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        player.angle -= ROTATIONSPEED * deltaTime;
        if (player.angle < 0) {
            player.angle = 360.0f;
        }
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        player.angle += ROTATIONSPEED * deltaTime;
        if (player.angle > 360.0f) {
            player.angle = 0.0f;
        }
    }

    // player speed
    player.speed.x = sin(player.angle * DEG2RAD) * SHIPSPEED;
    player.speed.y = cos(player.angle * DEG2RAD) * SHIPSPEED;

    // player acceleration
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {    // step on the pedal!
        if (player.acceleration <= SHIPMAXACCELERATION) {
            player.acceleration += 0.05f * deltaTime;
        }
    } else {
        if (player.acceleration > 0.0f) {           // slow down there bucko
            player.acceleration -= 0.03f * deltaTime;
        } else if (player.acceleration < 0.0f) {
            player.acceleration = 0.0f;
        }
    }

    // player position
    player.position.x += player.speed.x * player.acceleration * deltaTime;
    player.position.y -= player.speed.y * player.acceleration * deltaTime;

    PlayerWallCollision();
    PlayerMove();

    // shoot projectiles
    if (IsKeyPressed(KEY_SPACE)) {
        numProjectiles++;
        PlayerShootProjectile(); 
    }
}
// ----------------------------------------------------------------------------


// ------------------------------------------------------------------
// ------------------------------ DRAW ------------------------------
// ------------------------------------------------------------------
void Draw() {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    deltaTime = GetFrameTime() * 40;

    if (screen.isMenu) {
        Menu();
        EndDrawing();
        return;
    }

    // @@NOTE: debug screen for testing purposes only!
    char angleBuffer[128];
    char speedBuffer[128];
    char accelBuffer[128];
    char fpsBuffer[128];
    char numProjectBuffer[128];
    char numAsteroidBuffer[128];

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
    out = snprintf(fpsBuffer, 128, "FPS:\t%d", GetFPS());
    if (out <= -1) {
        printf("ERROR: Unable to pass FPS value to buffer\n");
        exit(1);
    }
    out = snprintf(numProjectBuffer, 128, "num projectiles:\t%d", numProjectiles);
    if (out <= -1) {
        printf("ERROR: Unable to pass FPS value to buffer\n");
        exit(1);
    }
    out = snprintf(numAsteroidBuffer, 128, "num asteroids:\t%d", numAsteroids);
    if (out <= -1) {
        printf("ERROR: Unable to pass number of asteroids value to buffer\n");
        exit(1);
    }

    int xpos = screen.width/25;
    int ypos = screen.width/25;

    GuiWindowBox((Rectangle){ xpos-10, ypos, 350, 350 }, "Debug Info");
    GuiLabel((Rectangle){ xpos, 100, 350, 40 }, angleBuffer);
    GuiLabel((Rectangle){ xpos, 150, 350, 40 }, speedBuffer);
    GuiLabel((Rectangle){ xpos, 200, 350, 40 }, accelBuffer);
    GuiLabel((Rectangle){ xpos, 250, 350, 40 }, fpsBuffer);
    GuiLabel((Rectangle){ xpos, 300, 350, 40 }, numProjectBuffer);
    GuiLabel((Rectangle){ xpos, 350, 350, 40 }, numAsteroidBuffer);

    // draw player/ship
    DrawTriangle(player.vertices.v1, player.vertices.v2, player.vertices.v3, spaceshipColor);

    // draw projectiles
    for (int i = 0; i < MAXNUMPROJECTILES; i++) {
        if (projectiles[i].active == FALSE) continue;

        float* xpos = &projectiles[i].position.x;
        float* ypos = &projectiles[i].position.y;
        float* angle = &projectiles[i].angle;

        *xpos += sin(*angle*DEG2RAD) * projectiles[i].velocity * deltaTime; 
        *ypos -= cos(*angle*DEG2RAD) * projectiles[i].velocity * deltaTime; 

        DrawRectangle(*xpos, *ypos, 10, 10, BLACK);

        // check for projectile collision with wall
        if (*xpos < 0 || *xpos > screen.width || *ypos < 0 || *ypos > screen.height) {
            projectiles[i].active = FALSE;
            numProjectiles--;
        }
    }

    // draw asteroid
    for (int i = 0; i < MAXNUMASTEROIDS; i++) {
        AsteroidEntity* pAsteroid = &asteroids[i];

        if (!pAsteroid->active) 
            continue;

        AsteroidDraw(pAsteroid);
        AsteroidMove(pAsteroid);

        if (AsteroidWallCollision(pAsteroid)) {
            numAsteroids--;
            pAsteroid->active = FALSE; 
        }
    }

    EndDrawing();
}
// ------------------------------------------------------------------


// ----------------------------------------------------------------------------
// ------------------------------ ASTEROID LOGIC ------------------------------
// ----------------------------------------------------------------------------
Vector2 AsteroidGenerateSpawnPosition() {
    float x, y;

    float hbound = (float)screen.width  + 300.0f;
    float vbound = (float)screen.height + 300.0f;

    int spawnOnSides = GetRandomValue(0, 1);
    if (spawnOnSides == TRUE) {
        int left_hbounds[2]     = { -hbound,         0.0f };         // no vertical bounds 
        int right_hbounds[2]    = { screen.width,    hbound };
        int vbounds[2]          = { -vbound,         vbound };
        
        x = GetRandomValue(0, 1) 
            ? GetRandomValue(left_hbounds[0], left_hbounds[1]) 
            : GetRandomValue(right_hbounds[0], right_hbounds[1]);
        y = GetRandomValue(vbounds[0], vbounds[1]);
    } else {
        int top_vbounds[2]      = { -vbound,         0.0f };         // no horizontal bounds
        int bot_vbounds[2]      = { screen.height,   vbound };
        int hbounds[2]          = { -hbound,         hbound };
        
        x = GetRandomValue(hbounds[0], hbounds[1]);
        y = GetRandomValue(0, 1)
            ? GetRandomValue(top_vbounds[0], top_vbounds[1])
            : GetRandomValue(bot_vbounds[0], bot_vbounds[1]);
    }
    
    return (Vector2) { x, y };
}

void AsteroidSpawn() {
    AsteroidEntity asteroid;

    asteroid.position = AsteroidGenerateSpawnPosition();
    asteroid.velocity = 10.0f; // TODO: set this to a global value
    asteroid.numVertices = GetRandomValue(3, 6);
    asteroid.active = TRUE;
    asteroid.size = REGULAR;

    //@@TODO: Determine asteroid angle (not just any random value !!!)
    Vector2 direction;
    direction = Vector2Subtract(player.position, asteroid.position);
    direction = Vector2Normalize(direction);
    asteroid.angle = atan2(direction.y, direction.x);

    // Add asteroids to array
    asteroids[astEntIdx++] = asteroid;
    if (astEntIdx >= MAXNUMASTEROIDS)
        astEntIdx = 0;
}

void AsteroidDraw(AsteroidEntity* pAsteroid) {
    float xpos = pAsteroid->position.x;
    float ypos = pAsteroid->position.y;
    DrawCircle(xpos, ypos, 150.0f, BLACK);
}

void AsteroidMove(AsteroidEntity* pAsteroid) {
    pAsteroid->position.x += cos( pAsteroid->angle ) * pAsteroid->velocity * deltaTime; // NOTE: asteroid.angle is already a RADIAN
    pAsteroid->position.y -= sin( pAsteroid->angle ) * pAsteroid->velocity * deltaTime; 
}

int AsteroidWallCollision(AsteroidEntity* pAsteroid) {
    int hbound = screen.width  + 300;
    int vbound = screen.height + 300;
    
    float xpos = (pAsteroid->position).x;
    float ypos = (pAsteroid->position).y;

    if (xpos < -hbound || xpos > hbound || ypos < -vbound || ypos > vbound) {
        return TRUE;
    }
    return FALSE;
}
// ----------------------------------------------------------------------------
