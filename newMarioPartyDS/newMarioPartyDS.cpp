#include "raylib.h"
#include "raymath.h"
#include "boardTiles.h"
#include "player.h"
#include <cmath>
#include <vector>
#include <sstream>
#include <fstream>

std::ofstream fout("position.dbg");

enum class gameState {
    MENU,
    BOARD,
    MINIGAME,
    RESULTS
} currentState;

std::vector<boardTiles> tiles;

Camera3D camera = { 0 };

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
    if ((tiles[tiles[currentTile].nextTile[0]].position.x - myPlayer.position.x) / speedx <= 0 || /*(tiles[tiles[currentTile].nextTile[0]].position.y - myPlayer.position.y) / speedy <= 0 ||*/ (tiles[tiles[currentTile].nextTile[0]].position.z - myPlayer.position.z) / speedz <= 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void updatePlayerPos()
{
    if (isMoving && (!pointsAreClose(myPlayer.position, tiles[tiles[currentTile].nextTile[0]].position) && !passedTile()))
    {
        myPlayer.position.x += speedx;
        myPlayer.position.z += speedz;
        if (aboveTile)
        {
            myPlayer.position.y = returny(distXZ(myPlayer.position, tiles[tiles[currentTile].nextTile[0]].position));
        }
        else
        {
            myPlayer.position.y = returny(distXZ(myPlayer.position, tiles[currentTile].position));
        }
        camera.target = myPlayer.position;
        fout << myPlayer.position.x << " " << myPlayer.position.y << " " << myPlayer.position.z << " " << currentTile << '\n';
    }
    else if (isMoving && (pointsAreClose(myPlayer.position, tiles[tiles[currentTile].nextTile[0]].position) || passedTile()))
    {
        myPlayer.position.x = round(myPlayer.position.x);
        myPlayer.position.y = round(myPlayer.position.y);
        myPlayer.position.z = round(myPlayer.position.z);
        currentTile = tiles[currentTile].nextTile[0];
        isMoving = false;
    }
    else
    {
        speedx = 0;
        //speedy = 0;
        speedz = 0;
        a = 0;
        b = 0;
    }
}

void movePlayer()
{
    Vector3 startpos = myPlayer.position;
    Vector3 endpos = tiles[tiles[currentTile].nextTile[0]].position;
    speedx = (endpos.x - startpos.x) / playerSpeed;
    //speedy = (endpos.y - startpos.y) / playerSpeed;
    speedz = (endpos.z - startpos.z) / playerSpeed;
    isMoving = true;
    if (tiles[currentTile].jumpOnNext)
    {
        yt = abs(- myPlayer.position.y + tiles[tiles[currentTile].nextTile[0]].position.y);
        xt = distXZ(myPlayer.position, tiles[tiles[currentTile].nextTile[0]].position);
        xt += xt / 8;
        x = xt / 2;
        y = yt + tiles[currentTile].jumpHeight;
        if(tiles[tiles[currentTile].nextTile[0]].position.y - tiles[currentTile].position.y >=0)
        {
            aboveTile = false;
        }
        else
        {
            aboveTile = true;
        }
        b = (2 * y) / x;
        a = -y / (x * x);
    }
}

void keyboardInput()
{
    if (IsKeyPressed(KEY_SPACE) && !isMoving)
    {
        movePlayer();
    }
}

void renderScreen(gameState state, Model model, Model player, Model tilesType[3])
{
    if (state == gameState::BOARD)
    {
        forward = Vector3Subtract(camera.target, camera.position);

        right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
        move = { 0 };

        if (IsKeyDown(KEY_W)) move = Vector3Add(move, forward);
        if (IsKeyDown(KEY_S)) move = Vector3Subtract(move, forward);
        if (IsKeyDown(KEY_A)) move = Vector3Subtract(move, right);
        if (IsKeyDown(KEY_D)) move = Vector3Add(move, right);

        move = Vector3Scale(Vector3Normalize(move), cameraSpeed);
        camera.position = Vector3Add(camera.position, move);
        
        //camera.position = Vector3Add(myPlayer.position, Vector3{ -12.0f, 8.0f, 0.0f });
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawModel(model, modelPosition, 1.0f, WHITE);

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
                DrawModel(tilesType[0], tiles[i].position, 0.1f, WHITE);
            }
            else if (tiles[i].type == "red")
            {
                DrawModel(tilesType[1], tiles[i].position, 0.1f, WHITE);
            }
            else if (tiles[i].type == "green")
            {
                DrawModel(tilesType[2], tiles[i].position, 0.1f, WHITE);
            }
        }

        EndMode3D();

        std::stringstream debug;
        debug << "player pos: " << myPlayer.position.x << " " << myPlayer.position.y << " " << myPlayer.position.z << '\n';
        debug << "tile pos: " << tiles[tiles[currentTile].nextTile[0]].position.x << " " << tiles[tiles[currentTile].nextTile[0]].position.y << " " << tiles[tiles[currentTile].nextTile[0]].position.z << '\n';
        debug << "points are close: " << pointsAreClose(myPlayer.position, tiles[tiles[currentTile].nextTile[0]].position) << "\n";
        debug << "passed tile: " << passedTile() << '\n';

        DrawText(debug.str().c_str(), 10, 10, 35, BLACK);

        EndDrawing();
    }
}

void initTiles()
{
    boardTiles initTile;
    initTile.position = { 0.0f, 0.0f, 0.0f };
    initTile.type = "blue";
    initTile.nextTile[0] = 1;
    initTile.jumpOnNext = false;
    initTile.jumpHeight = 0;
    tiles.push_back(initTile);

    initTile.position = { 0.0f, 0.0f, 3.0f };
    initTile.type = "red";
    initTile.nextTile[0] = 2;
    initTile.jumpOnNext = true;
    initTile.jumpHeight = 8;
    tiles.push_back(initTile);

    initTile.position = { 3.0f, 6.0f, 3.0f };
    initTile.type = "green";
    initTile.nextTile[0] = 0;
    initTile.jumpOnNext = true;
    initTile.jumpHeight = 8;
    tiles.push_back(initTile);
}

int main()
{
    initTiles();

    currentState = gameState::BOARD;

    const int screenWidth = 1280;
    const int screenHeight = 720;

    myPlayer.position = playerPosition;

    InitWindow(screenWidth, screenHeight, "New Mario Party DS");
    Model model = LoadModel("test_board.glb");
    Model player = LoadModel("mario_model.glb");
    Model tileModel[3] = { LoadModel("blue_tile.glb"), LoadModel("red_tile.glb"), LoadModel("green_tile.glb") };

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
            renderScreen(currentState, model, player, tileModel);
            keyboardInput();
            updatePlayerPos();
            break;
        case gameState::MINIGAME:
            // minigames
            break;
        case gameState::RESULTS:
            // results
            break;
        }
    }

    UnloadModel(model);
    UnloadModel(player);
    for (int i = 0; i < 3; i++) UnloadModel(tileModel[i]);
    CloseWindow();

    return 0;
}
