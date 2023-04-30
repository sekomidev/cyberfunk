#include "raylib.h"
#include <string>
#include <cmath>
#include <algorithm>

#define SCREEN_WIDTH (1536)
#define SCREEN_HEIGHT (864)
#define CENTER_WIDTH (SCREEN_WIDTH / 2)
#define CENTER_HEIGHT (SCREEN_HEIGHT / 2)

const float scale = SCREEN_WIDTH / 192, maxSpeed = 4.0f;
float scrollingBack = 0.0f, scrollingMid = 0.0f, scrollingFore = 0.0f, metersDriven = 0.0f, speed = 1.0f;
Sound beep = {0}, brakeSound = {0};
Texture2D uiHint = {0};
Font font = {0};

void UpdateSpeed()
{
    if (IsKeyDown(KEY_UP) && speed < maxSpeed)
    {
        speed += std::clamp(0.0035f * std::sqrt(1 / (speed+0.8f)), 0.001f, 0.0035f) * GetFrameTime() * 60;
    }
    if (IsKeyDown(KEY_DOWN) && speed > 0.0f)
    {
        speed -= std::clamp(0.004f * std::sqrt(speed), 0.002f, 0.005f) * GetFrameTime() * 60;
    }
    if (speed < 0)
    {
        speed = 0;
        PlaySound(brakeSound);
    }
}

void DrawUI()
{
    std::string speedText = std::to_string((int)std::floor(speed * 25)) + " KM/H";
    std::string metersText = std::to_string((int)std::floor(metersDriven)) + " M";
    // x coordinate at which the game should start drawing the rectangle;
    // acts as a reference point for ui elements
    const float rx = CENTER_WIDTH - 160;

    DrawTextureEx(uiHint, {5, 5}, 0.0f, 4.0f, LIGHTGRAY);
    DrawRectangle(rx, 0, 320, 50, BLACK);
    DrawTextEx(font, speedText.c_str(), {rx + 20, 10}, 32, 1, RED);
    DrawTextEx(font, metersText.c_str(), {rx + 180, 10}, 32, 1, RED);
}

int main(void)
{
    bool drawUI = true;
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "cyberfunk 2045");
    SetWindowIcon(LoadImage("resources/common/icon.png"));
    InitAudioDevice();

    Music bgm = LoadMusicStream("resources/sounds/music/ambient.ogg");
    Texture2D background = LoadTexture("resources/images/background.png");
    Texture2D midground = LoadTexture("resources/images/midground.png");
    Texture2D foreground = LoadTexture("resources/images/foreground.png");
    uiHint = LoadTexture("resources/images/ui/ui_hint.png");
    beep = LoadSound("resources/sounds/fx/beep.wav");
    brakeSound = LoadSound("resources/sounds/fx/psssh.wav");
    font = LoadFont("resources/common/fonts/pixeloid.ttf");

    SetTargetFPS(60);
    SetSoundVolume(beep, 0.6f);
    SetMusicVolume(bgm, 0.75f);
    PlayMusicStream(bgm);

    while (!WindowShouldClose())
    {
        UpdateMusicStream(bgm);
        UpdateSpeed();
        if(IsKeyPressed(KEY_F1))
        { 
            PlaySound(beep);
            drawUI = !drawUI;
        }

        //                      25 * 0.277777 (km/h to m/s)
        metersDriven += speed * 6.944445f * GetFrameTime();
        scrollingBack -= 0.025f * scale * speed * GetFrameTime() * 60;
        scrollingMid -= 0.25f * scale * speed * GetFrameTime() * 60;
        scrollingFore -= 0.75f * scale * speed * GetFrameTime() * 60;

        // scroll to the next image to create a seamless scrolling effect
        if (scrollingBack <= -background.width * scale)
            scrollingBack = 0;
        if (scrollingMid <= -midground.width * scale)
            scrollingMid = 0;
        if (scrollingFore <= -foreground.width * scale)
            scrollingFore = 0;

        // drawing
        BeginDrawing();
            ClearBackground(BLACK);

            // background
            DrawTextureEx(background, (Vector2){scrollingBack, 0}, 0.0f, scale, WHITE);
            DrawTextureEx(background, (Vector2){background.width * scale + scrollingBack, 0}, 0.0f, scale, WHITE);

            // midground
            DrawTextureEx(midground, (Vector2){scrollingMid, 0}, 0.0f, scale, LIGHTGRAY);
            DrawTextureEx(midground, (Vector2){midground.width * scale + scrollingMid, 0}, 0.0f, scale, LIGHTGRAY);

            // foreground
            DrawTextureEx(foreground, (Vector2){scrollingFore, 0}, 0.0f, scale, WHITE);
            DrawTextureEx(foreground, (Vector2){foreground.width * scale + scrollingFore, 0}, 0.0f, scale, WHITE);

            if(drawUI)
            {
                DrawUI();
            }
        EndDrawing();
    }

    // cleanup
    UnloadMusicStream(bgm);
    UnloadFont(font);
    UnloadSound(beep);
    UnloadSound(brakeSound);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);

    CloseWindow();

    return 0;
}