#include "raylib.h"
#include "raymath.h"
#include "boardTiles.h"
#include <cmath>
#include <vector>

enum class gameState {
    MENU,
    BOARD,
    MINIGAME,
    RESULTS
}currentState;

std::vector<boardTiles> tiles;

Camera3D camera = { 0 };
    
Vector3 modelPosition = { 0.0f, 0.0f, 0.0f };
Vector3 playerPosition = { 0.0f,0.0f,0.0f };
float speed = 0.1f;
Vector2 mouseDelta = GetMouseDelta();
float sensitivity = 0.003f;
Vector3 forward = Vector3Subtract(camera.target, camera.position);
Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
Vector3 move = { 0 };

        
void renderScreen(gameState state, Model model, Model player, Model tilesType[3])
{
    if (state == gameState::BOARD)
    {
        forward = Vector3Subtract(camera.target, camera.position);

        //camera.target = Vector3Add(camera.position, forward);
        camera.target = playerPosition;

        // WASD Movement
        right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
        move = { 0 };

        if (IsKeyDown(KEY_W)) move = Vector3Add(move, forward);
        if (IsKeyDown(KEY_S)) move = Vector3Subtract(move, forward);
        if (IsKeyDown(KEY_A)) move = Vector3Subtract(move, right);
        if (IsKeyDown(KEY_D)) move = Vector3Add(move, right);

        speed = 0.1f;
        move = Vector3Scale(Vector3Normalize(move), speed);
        camera.position = Vector3Add(camera.position, move);
        //camera.target = Vector3Add(camera.position, forward);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        
        DrawModel(model, modelPosition, 0.3f, WHITE);
        DrawModel(player, playerPosition, 2.0f, WHITE);

        for (int i = 0; i < tiles.size(); i++)
        {
            if (tiles[i].type == "blue")
            {
                DrawModel(tilesType[0], tiles[i].position, 0.3f, WHITE);
            }
            else if (tiles[i].type == "red")
            {
                DrawModel(tilesType[1], tiles[i].position, 0.3f, WHITE);
            }
            else if (tiles[i].type == "green")
            {
                DrawModel(tilesType[2], tiles[i].position, 0.3f, WHITE);
            }
        }

        EndMode3D();

        DrawText("WASD to move, mouse to look, ESC to exit", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }
}

void initTiles()
{
    boardTiles initTile;
    initTile.position = { 0.0f };
    initTile.type = "blue";
    tiles.push_back(initTile);
    initTile.position = { 0.0f, 0.0f, 6.0f };
    initTile.type = "red";
    tiles.push_back(initTile);
    initTile.position = { 6.0f , 0.0f, 6.0f };
    initTile.type = "green";
    tiles.push_back(initTile);
}

int main() 
{
    initTiles();

    currentState = gameState::BOARD;

    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "New Mario Party DS");
    Model model = LoadModel("test_board.glb");
    Model player = LoadModel("mario_model.glb");
    Model tileModel[3] = { LoadModel("blue_tile.glb"), LoadModel("red_tile.glb"), LoadModel("green_tile.glb")};
    camera.position = { 5.0f, 5.0f, 5.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    //DisableCursor();  // Capture mouse for FPS-style camera

    SetTargetFPS(120);

    while (!WindowShouldClose()) 
    {
        
        switch (currentState)
        {
        case gameState::MENU:
            //menu
            break;
        case gameState::BOARD:
            //board
            renderScreen(currentState, model, player, tileModel);
            break;
        case gameState::MINIGAME:
            //minigames
            break;
        case gameState::RESULTS:
            //results
            break;
        }
        
    }

    UnloadModel(model);
    CloseWindow();

    return 0;
}
