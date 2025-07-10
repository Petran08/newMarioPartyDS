#include "raylib.h"
#include "raymath.h"
#include <cmath>

enum class gameState {
    MENU,
    BOARD,
    MINIGAME,
    RESULTS
}currentState;


int main() {

    currentState = gameState::BOARD;

    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Raylib C++ - Free Camera + Model");

    Camera3D camera = { 0 };
    camera.position = { 5.0f, 5.0f, 5.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Model model = LoadModel("test_board.glb");
    Vector3 modelPosition = { 0.0f, 0.0f, 0.0f };
    Model player = LoadModel("mario_model.glb");
    Vector3 playerPosition = { 1.0f,1.0f,1.0f };

    DisableCursor();  // Capture mouse for FPS-style camera

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Mouse look
        Vector2 mouseDelta = GetMouseDelta();
        float sensitivity = 0.003f;
        Vector3 forward = Vector3Subtract(camera.target, camera.position);
        float yaw = atan2f(forward.z, forward.x);
        float pitch = asinf(forward.y / Vector3Length(forward));

        yaw += mouseDelta.x * sensitivity;
        pitch -= mouseDelta.y * sensitivity;

        // Clamp pitch
        if (pitch > 1.5f) pitch = 1.5f;
        if (pitch < -1.5f) pitch = -1.5f;

        forward.x = cosf(pitch) * cosf(yaw);
        forward.y = sinf(pitch);
        forward.z = cosf(pitch) * sinf(yaw);
        forward = Vector3Normalize(forward);

        camera.target = Vector3Add(camera.position, forward);

        // WASD Movement
        Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
        Vector3 move = { 0 };

        if (IsKeyDown(KEY_W)) move = Vector3Add(move, forward);
        if (IsKeyDown(KEY_S)) move = Vector3Subtract(move, forward);
        if (IsKeyDown(KEY_A)) move = Vector3Subtract(move, right);
        if (IsKeyDown(KEY_D)) move = Vector3Add(move, right);

        float speed = 0.1f;
        move = Vector3Scale(Vector3Normalize(move), speed);
        camera.position = Vector3Add(camera.position, move);
        camera.target = Vector3Add(camera.position, forward);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        DrawModel(model, modelPosition, 0.3f, WHITE);
        DrawModel(player, playerPosition, 2.0f, WHITE);
        DrawGrid(20, 1.0f);
        EndMode3D();

        DrawText("WASD to move, mouse to look, ESC to exit", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    UnloadModel(model);
    CloseWindow();

    return 0;
}
