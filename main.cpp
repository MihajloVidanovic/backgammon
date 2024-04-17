#include "raylib.h"
#include <iostream>

int main()
{
    InitWindow(512, 512, "Backgammon");
    
    SetTargetFPS(60);
    
    Texture2D spritesheet = LoadTexture("assets/spritesheet.png");
    
    while(!WindowShouldClose()) {
        
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            
            DrawTexturePro(spritesheet, (Rectangle){48, 0, 128, 128}, (Rectangle){0, 0, 512, 512}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
