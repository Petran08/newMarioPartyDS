#include "raylib.h"
#include "raymath.h"
#include "boardTiles.h"
#include "player.h"
#include <cmath>
#include <vector>
#include <sstream>
#include <fstream>
#include <chrono>
#include <thread>
#include <iostream>
#include <random>

std::ofstream fout("position.dbg");

enum class gameState {
    MENU,
    BOARD,
    MINIGAME,
    RESULTS
} currentState;

//std::vector<boardTiles> tiles;

//in blender
//x +, y -

Camera3D camera = { 0 };

std::string boardNames[] = { "test_board", "mp7_tutorial_board" };
unsigned short int boardId = 0;

Vector3 modelPosition = { 0.0f, 0.0f, 0.0f };
Vector3 playerPosition = { 0.0f, 0.0f, 0.0f };
float cameraSpeed = 0.2f;
float playerSpeed = 120.0f; // time to go to next tile in frames(120fps)
Vector3 forward = Vector3Subtract(camera.target, camera.position);
Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
bool aboveTile = false;
Vector3 move = { 0 };
short int currentTile = 0;
float speedx, speedy, speedz;
bool isMoving = false;
float x, y, a, b, xt, yt; // for jumping
short int direction = 0;
Model tileModel[3];
Model model;
Model playerModel;
Model arrows[2];
int tilesToMove = 0;
bool choosingDirection = false;
static std::random_device rd;
static std::mt19937 gen(rd());
unsigned int frameCounter; //for arrow movement

player myPlayer;
bool pointsAreClose(Vector3 p1, Vector3 p2)
{
    Vector3 dif = Vector3Subtract(p1, p2);
    return (fabsf(dif.x) <= 0.01f && fabsf(dif.y) <= 0.01f && fabsf(dif.z) <= 0.01f);
}

float distXZ(Vector3 p1, Vector3 p2)
{
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.z - p2.z) * (p1.z - p2.z));
}

float returny(float x)
{
    return a * (x * x) + b * x;
}

bool passedTile()
{
    return ((tiles[tiles[currentTile].nextTile[direction]].position.x - myPlayer.position.x) / speedx <= 0 || /*(tiles[tiles[currentTile].nextTile[0]].position.y - myPlayer.position.y) / speedy <= 0 ||*/ (tiles[tiles[currentTile].nextTile[direction]].position.z - myPlayer.position.z) / speedz <= 0);
}

void renderScreen(gameState state, Model model, Model player, Model tilesType[3], std::string mode = "none")
{
    if (state == gameState::BOARD)
    {
        forward = Vector3Subtract(camera.target, camera.position);

        right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
        move = { 0 };

        /*if (IsKeyDown(KEY_W)) move = Vector3Add(move, forward);
        if (IsKeyDown(KEY_S)) move = Vector3Subtract(move, forward);
        if (IsKeyDown(KEY_A)) move = Vector3Subtract(move, right);
        if (IsKeyDown(KEY_D)) move = Vector3Add(move, right);

        move = Vector3Scale(Vector3Normalize(move), cameraSpeed);
        camera.position = Vector3Add(camera.position, move);
        */
        camera.position = Vector3Add(myPlayer.position, Vector3{ -12.0f, 8.0f, -4.0f });
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawModel(model, modelPosition, 1.0f, WHITE);

        //DrawLine3D(myPlayer.position, Vector3{ 0, 0 , 0 }, BLACK);

        // Update rotation angles for smooth rotation
        myPlayer.rotationX = 0.00f;
        myPlayer.rotationY = 0.00f;
        myPlayer.rotationZ = 0.00f;

        // Create combined rotation matrix
        Matrix rotX = MatrixRotateX(myPlayer.rotationX);
        Matrix rotY = MatrixRotateY(myPlayer.rotationY);
        Matrix rotZ = MatrixRotateZ(myPlayer.rotationZ);

        Matrix combinedRotation = MatrixMultiply(MatrixMultiply(rotZ, rotY), rotX);

        // Translate to player position *then* rotate
        Matrix translation = MatrixTranslate(myPlayer.position.x, myPlayer.position.y, myPlayer.position.z);

        // Full transform: translate then rotate around origin of model
        Matrix transform = MatrixMultiply(combinedRotation, translation);

        // Apply transform to player model
        player.transform = transform;

        DrawModel(player, Vector3 { 0, 0, 0 }, 1.0f, WHITE);

        // Draw tiles
        for (int i = 0; i < tiles.size(); i++)
        {
            if (tiles[i].type == "blue")
            {
                DrawModel(tilesType[0], tiles[i].position, 0.2f, WHITE);
            }
            else if (tiles[i].type == "red")
            {
                DrawModel(tilesType[1], tiles[i].position, 0.2f, WHITE);
            }
            else if (tiles[i].type == "green")
            {
                DrawModel(tilesType[2], tiles[i].position, 0.2f, WHITE);
            }
        }

        if (mode == "debug")
        {
            float x = myPlayer.position.x, y = returny(0), z = myPlayer.position.z;
            for (x = x, z = z; distXZ(Vector3{ x, 0.0f, z }, myPlayer.position) <= distXZ(myPlayer.position, tiles[tiles[currentTile].nextTile[direction]].position); x += speedx, z += speedz)
            {
                DrawSphere(Vector3{ x, returny(distXZ(Vector3{ x, 0.0f, z }, myPlayer.position)) + tiles[currentTile].position.y, z}, 0.05f, RED);
            }
        }
        else if (mode == "dirChoosing")
        {
            //DrawModel(arrows[1], Vector3{ 10.0f, 2.0f, 8.0f }, 0.2f, WHITE);
            for (int i = 0; i < tiles[currentTile].maxDir; i++)
            {
                if (direction == i)
                {
                    DrawModelEx(arrows[0], Vector3Add(tiles[tiles[currentTile].nextTile[i]].position, Vector3{0.0f, 2.5f + sin(frameCounter * DEG2RAD), 0.0f}), Vector3{0.0f, 0.0f, 1.0f}, -90.0f, Vector3{0.2f, 0.2f, 0.2f}, WHITE);
                }
                else
                {
                    DrawModelEx(arrows[1], Vector3Add(tiles[tiles[currentTile].nextTile[i]].position, Vector3{ 0.0f, 2.5f, 0.0f }), Vector3{ 0.0f, 0.0f, 1.0f }, -90.0f, Vector3{ 0.2f, 0.2f, 0.2f }, WHITE);
                }
            }
        }

        EndMode3D();


        if(mode=="debug_text" || mode == "dirChoosing")
        {
            std::stringstream debug;
            debug << "player pos: " << myPlayer.position.x << " " << myPlayer.position.y << " " << myPlayer.position.z << '\n';
            debug << "tile pos: " << tiles[tiles[currentTile].nextTile[direction]].position.x << " " << tiles[tiles[currentTile].nextTile[direction]].position.y << " " << tiles[tiles[currentTile].nextTile[direction]].position.z << '\n';
            debug << "points are close: " << pointsAreClose(myPlayer.position, tiles[tiles[currentTile].nextTile[direction]].position) << "\n";
            debug << "passed tile: " << passedTile() << '\n';
            debug << "maxDir: " << tiles[currentTile].maxDir << '\n';
            debug << "direction: " << direction << '\n';
            debug << "tiles to move: " << tilesToMove << '\n';
            debug << "is moving: " << isMoving << '\n';

            DrawText(debug.str().c_str(), 10, 10, 35, BLACK);
        }

        EndDrawing();
    }
}

void updatePlayerPos()
{
    if (isMoving && (!pointsAreClose(myPlayer.position, tiles[tiles[currentTile].nextTile[direction]].position) && !passedTile()))
    {
        myPlayer.position.x += speedx;
        myPlayer.position.z += speedz;
        if (aboveTile && tiles[currentTile].jumpOnNext)
        {
            myPlayer.position.y = returny(distXZ(myPlayer.position, tiles[currentTile].position)) + tiles[currentTile].position.y;
        }
        else if(!aboveTile && tiles[currentTile].jumpOnNext)
        {
            myPlayer.position.y = returny(distXZ(myPlayer.position, tiles[currentTile].position)) + tiles[currentTile].position.y;
        }
        else
        {
            myPlayer.position.y += speedy;
        }
        camera.target = myPlayer.position;
        fout << myPlayer.position.x << " " << myPlayer.position.y << " " << myPlayer.position.z << " " << currentTile << '\n';
    }
    else if (isMoving && (pointsAreClose(myPlayer.position, tiles[tiles[currentTile].nextTile[direction]].position) || passedTile()))
    {
        currentTile = tiles[currentTile].nextTile[direction];
        tilesToMove -= 1 * (tiles[currentTile].countDown);
        myPlayer.position.x = tiles[currentTile].position.x;
        myPlayer.position.y = tiles[currentTile].position.y;
        myPlayer.position.z = tiles[currentTile].position.z;
        direction = 0;
        isMoving = false;
    }
    else
    {
        speedx = 0;
        speedy = 0;
        speedz = 0;
        a = 0;
        b = 0;
    }
}

void movePlayer();

bool keyboardInput()
{
    if (choosingDirection)
    {
        if (IsKeyPressed(KEY_E))
        {
            direction++;
            if (direction >= tiles[currentTile].maxDir)
                direction = 0;
        }
        if (IsKeyPressed(KEY_SPACE))  // Confirm direction
        {
            choosingDirection = false;
            return true;  // Now allow movement
        }
        return false;  // Still choosing
    }

    if (IsKeyPressed(KEY_SPACE) && !isMoving)
    {
        return true;
    }
    if (IsKeyPressed(KEY_Z) && tilesToMove == 0)
    {
        std::uniform_int_distribution<int> distr(1, 10);
        int roll = distr(gen);
        tilesToMove = roll;
    }
    return false;
}


void movePlayer()
{

    if (tiles[currentTile].maxDir > 1 && pointsAreClose(myPlayer.position, tiles[currentTile].position))
    {
        frameCounter = 0;
        choosingDirection = true;
        while (choosingDirection)
        {
            renderScreen(currentState, model, playerModel, tileModel, "dirChoosing");
            frameCounter++;
            keyboardInput();
        }
        frameCounter = 0;
    }

    Vector3 startpos = myPlayer.position;
    Vector3 endpos = tiles[tiles[currentTile].nextTile[direction]].position;
    speedx = (endpos.x - startpos.x) / playerSpeed;
    speedy = (endpos.y - startpos.y) / playerSpeed;
    speedz = (endpos.z - startpos.z) / playerSpeed;
    isMoving = true;    
    
    if (tiles[currentTile].jumpOnNext)
    {
        if (tiles[tiles[currentTile].nextTile[direction]].position.y - tiles[currentTile].position.y >= 0)
        {
            aboveTile = false;
        }
        else
        {
            aboveTile = true;
        }
        
        float currentPoint;
        //isMoving = false;
        if (!aboveTile)
        {
            yt = abs(- myPlayer.position.y + tiles[tiles[currentTile].nextTile[direction]].position.y);
            xt = distXZ(myPlayer.position, tiles[tiles[currentTile].nextTile[direction]].position);
            //xt += xt / 8;
            y = yt + tiles[currentTile].jumpHeight;
            x = xt / 2;
            b = (2 * y) / x;
            a = -y / (x * x);
            currentPoint = returny(distXZ(myPlayer.position, tiles[tiles[currentTile].nextTile[direction]].position)) + tiles[currentTile].position.y;
            while (abs(currentPoint-tiles[tiles[currentTile].nextTile[direction]].position.y)>=0.1f)
            {
                x = xt / 2;
                b = (2 * y) / x;
                a = -y / (x * x);
                xt += 0.1f;
                currentPoint = returny(distXZ(myPlayer.position, tiles[tiles[currentTile].nextTile[direction]].position)) + tiles[currentTile].position.y;
                //renderScreen(currentState, model, playerModel, tileModel, "debug");
                //std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        else
        {
            yt = (-myPlayer.position.y + tiles[tiles[currentTile].nextTile[direction]].position.y);
            xt = distXZ(myPlayer.position, tiles[tiles[currentTile].nextTile[direction]].position);
            //xt += xt / 8;
            y = yt + tiles[currentTile].jumpHeight;
            x = xt / 2;
            b = (2 * y) / x;
            a = -y / (x * x);
            currentPoint = returny(distXZ(myPlayer.position, tiles[tiles[currentTile].nextTile[direction]].position)) + tiles[currentTile].position.y;
            while (abs(currentPoint - tiles[tiles[currentTile].nextTile[direction]].position.y) >= 0.3f)
            {
                x = xt / 2;
                b = (2 * y) / x;
                a = -y / (x * x);
                xt -= 0.1f;
                currentPoint = returny(distXZ(myPlayer.position, tiles[tiles[currentTile].nextTile[direction]].position)) + tiles[currentTile].position.y;
                //renderScreen(currentState, model, playerModel, tileModel, "debug");
                //std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }
}

int main()
{

    readTileData(boardNames[boardId]);

    fout << "just for debuging \n";

    currentState = gameState::BOARD;

    const int screenWidth = 1280;
    const int screenHeight = 720;

    myPlayer.position = playerPosition;

    InitWindow(screenWidth, screenHeight, "New Mario Party DS");
    //model = LoadModel("test_board.glb");
    model = LoadModel((boardNames[boardId] + ".glb").c_str());
    playerModel = LoadModel("mario_model.glb");
    tileModel[0] = LoadModel("blue_tile.glb");
    tileModel[1] = LoadModel("red_tile.glb");
    tileModel[2] = LoadModel("green_tile.glb");
    arrows[0] = LoadModel("solid_arrow.glb");
    arrows[1] = LoadModel("transparent_arrow.glb");

    camera.position = { 5.0f, 5.0f, 5.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetTargetFPS(120);

    while (!WindowShouldClose())
    {
        switch (currentState)
        {
        case gameState::MENU:
            // menu
            break;
        case gameState::BOARD:
            renderScreen(currentState, model, playerModel, tileModel, "debug_text");
            keyboardInput();
            if(!choosingDirection)
            {
                updatePlayerPos();
            }
            if (tilesToMove > 0 && !isMoving && !choosingDirection)
            {
                movePlayer();
            }
            
            break;
        case gameState::MINIGAME:
            // minigames
            break;
        case gameState::RESULTS:
            // results
            break;
        }
    }
    fout << "for debuging \n";
    UnloadModel(model);
    UnloadModel(playerModel);
    for (int i = 0; i < 3; i++) UnloadModel(tileModel[i]);
    for (int i = 0; i < 2; i++) UnloadModel(arrows[i]);
    CloseWindow();

    return 0;
}
