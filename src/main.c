#include <raylib.h>
#include <car.h>

int main()
{
    int screenWidth = GetMonitorWidth(0);
    int screenHeight = GetMonitorHeight(0);
    const int virtualScreenWidth = 1920;
    const int virtualScreenHeight = 1080;
    CAR player = {20.0f,20.0f,0.0f,64.0f,32.0f};

    InitWindow(screenWidth, screenHeight, "Rock and Race");
    ToggleFullscreen();
    
    RenderTexture2D screen = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    while (!WindowShouldClose())    
    {
        updateCar(&player);
        BeginTextureMode(screen);
            ClearBackground(RAYWHITE); 
            drawCar(&player);
            DrawFPS(virtualScreenWidth - 100, 0);

        EndTextureMode();
        BeginDrawing();
            ClearBackground(BLACK); 

            float realScreenWidth = (float)GetScreenWidth();
            float realScreenHeight = (float)GetScreenHeight();

            float scaleX = realScreenWidth / virtualScreenWidth;
            float scaleY = realScreenHeight / virtualScreenHeight;
            float scale = fminf(scaleX, scaleY);

            float destX = (realScreenWidth - (virtualScreenWidth * scale)) * 0.5f;
            float destY = (realScreenHeight - (virtualScreenHeight * scale)) * 0.5f;

            Rectangle sourceRec = { 0.0f, 0.0f, (float)screen.texture.width, -(float)screen.texture.height };
            Rectangle destRec = { destX, destY, (float)virtualScreenWidth * scale, (float)virtualScreenHeight * scale };
            
            DrawTexturePro(screen.texture, sourceRec, destRec, (Vector2){0, 0}, 0.0f, WHITE);

        EndDrawing();
        }
    UnloadRenderTexture(screen);
    CloseWindow();        

    return 0;
}