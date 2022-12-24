#include "include/raylib.h"
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "player.h"
#include "platform.h"

const double pi = 3.1415926535897;

const int gravity = 1;

Platform platforms[4] = {{0}, {1}, {2}, {3}};
Player player(platforms[0].getX() + platforms[0].getWidth()/2 - 26/2, platforms[0].getY() - player.getHeight(), 26, 32);

int scoreInt = 0;
int highscoreInt = LoadStorageValue(0);
char score[32];
char highscore[32];

bool titleScreen = true;
bool playCoinFX = false;

void addScore(int amount)
{
    scoreInt += amount;
    if (scoreInt < 10)
    {
        sprintf(score, "00%d", scoreInt);
    } else if (scoreInt < 100)
    {
        sprintf(score, "0%d", scoreInt);
    } else {
        sprintf(score, "%d", scoreInt);
    }
    if (scoreInt > highscoreInt)
    {
        highscoreInt = scoreInt;
        sprintf(highscore, "Best Score: %d", highscoreInt);
    }
}
void resetScore()
{
    scoreInt = 0;
    sprintf(score, "00%d", scoreInt);
    SaveStorageValue(0, highscoreInt);
}
void resetGame()
{
    resetScore();
    for (int i = 0; i < 4; i++)
    {
        platforms[i] = Platform(i);
    }
    player.setVelocity(0, 0);
    player.setX(platforms[0].getX() + platforms[0].getWidth()/2 - 26/2);
    player.setY(platforms[0].getY() - player.getHeight());
}


void checkPlayerCollision()
{
   bool onPlatform = false;
    for (int i = 0; i < 4; i++)
    {
        if (platforms[i].getHasCoin() && player.getX() + player.getWidth() - 3 > platforms[i].getCoinX() && player.getX() + 3 < platforms[i].getCoinX() + 24 && player.getY() + player.getHeight() - 3 > platforms[i].getCoinY() && player.getY() + 3 < platforms[i].getCoinY() + 24)
        {
            addScore(1);
            platforms[i].setHasCoin(false);
            playCoinFX = true;
        }
        if (player.getX() + 1 < platforms[i].getX() + platforms[i].getWidth() && player.getX() + player.getWidth() > platforms[i].getX() && player.getY() + player.getHeight() >= platforms[i].getY() && player.getY() < platforms[i].getY() + platforms[i].getHeight())
        {
            if (player.getY() > platforms[i].getY() + platforms[i].getHeight()/2)
            {
                player.setVelocity(player.getVelocity().x, 5);
            } 
            else if (player.getY() + player.getHeight() <  platforms[i].getY() + platforms[i].getHeight())
            {    
                onPlatform = true;
                player.setY(platforms[i].getY() - player.getHeight());
                player.setY(player.getY() + 1);
            }
        }
        
    }
    player.setOnPlatform(onPlatform);
}


int main(void)
{
    srand (time(NULL));
    resetScore();
    sprintf(highscore, "Best Score: %d", highscoreInt);
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    int mouseDownX = 0;
    int mouseDownY = 0;
    double lavaY = screenHeight - 32;
    double timer = 0; 
    double splashTimer = 0;
    bool firstTime = true;
    bool playedSplash = false;
    bool playedSelect = false;
    Image thor = LoadImage("resources/thorlogo.png");
    InitWindow(screenWidth, screenHeight, "Sander15 - Game");
    SetWindowIcon(thor);
    InitAudioDevice();
    SetMasterVolume(0.3f);
    
    Texture2D thorSprite = LoadTexture("resources/character.png");
    Texture2D lavaSprite = LoadTexture("resources/cage.png");
    Texture2D platformSprite = LoadTexture("resources/platforms.png");
    Texture2D ballSprite = LoadTexture("resources/ball.png");
    Texture2D scoreBoxSprite = LoadTexture("resources/box.png");
    Texture2D logo = LoadTexture("resources/thorlogo.png");
    Texture2D splashThorSprite = LoadTexture("resources/thor.png");
    Texture2D sky = LoadTexture("resources/sky.png");

    Sound fxLaunch = LoadSound("resources/launch.wav"); 
    Sound fxClick = LoadSound("resources/click.wav"); 
    Sound fxDeath = LoadSound("resources/death.mp3"); 
    Sound fxCoin = LoadSound("resources/coin.mp3"); 
    Sound fxSplash = LoadSound("resources/splash.mp3");
    Font font = LoadFontEx("resources/font.otf", 64, 0, NULL);
    
    SetTargetFPS(60);  

    while (!WindowShouldClose())   
    {
        if (titleScreen)
        {   if (splashTimer > 120)
            {
                if (!playedSelect)
                {
                    playedSelect = true;
                }
                BeginDrawing();
                ClearBackground(LIGHTGRAY);
                DrawText(highscore, 300, 125, 25, BLACK);
                DrawText("Press SPACE To Start!", 250, 250, 25, BLACK);
                EndDrawing();
                if (IsKeyPressed(KEY_SPACE))
                {
                    titleScreen = false;
                    mouseDownX = GetMouseX();
                    mouseDownY = GetMouseY();
                }
            } else {
                if (!playedSplash)
                {
                    PlaySound(fxSplash);
                    playedSplash = true;
                }
                BeginDrawing();
                ClearBackground(LIGHTGRAY);
                //DrawTextEx(font, "Made with <3 by\n", Vector2{screenWidth/2 - 54, screenHeight/2 + 3}, 32, 0, BLACK);
                DrawText("Made by\nSander15 & Daneu", 300, 125, 25, BLACK);
                //DrawTextEx(font, "Sander15\n&\nDaneu", Vector2{screenWidth/2 - 54, screenHeight/2 + 3}, 32, 0, BLACK); 
                DrawTexture(splashThorSprite, screenWidth/2 - 16, screenHeight/2 - 16 - 23, WHITE);
                EndDrawing();
                splashTimer += 1.5;
            }
        } 
        else 
        {
            if (playCoinFX)
            {
                PlaySound(fxCoin);
                playCoinFX = false;
            }
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && player.isOnGround())
            {
                    PlaySound(fxClick);
                    mouseDownX = GetMouseX();
                    mouseDownY = GetMouseY();
            }
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && player.isOnGround())
            {
                if (firstTime)
                {
                    firstTime = false;
                } else {
                    PlaySound(fxLaunch);
                    if (player.isOnPlatform())
                    {
                        player.setY(player.getY() - 1);
                    }
                    int velocityX = GetMouseX() - mouseDownX;
   
                    int velocityY = GetMouseY() - mouseDownY;

                    player.setVelocity((double)velocityX*.08, (double)velocityY*.08);
                }
            }
            checkPlayerCollision();
            player.updatePosition();
            if (player.getY() > screenHeight)
            {
                PlaySound(fxDeath);
                resetGame();
            }
            for (int i = 0; i < 4; i++)
            {
                platforms[i].updatePosition();
            }
        
            lavaY = screenHeight - 43 - sin(timer) * 5;
            timer += 0.05;
            BeginDrawing();
        
            ClearBackground(SKYBLUE);
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && player.isOnGround())
            {   
                DrawLineEx((Vector2){static_cast<float>(mouseDownX + (player.getX() - mouseDownX) + (player.getWidth()/2)), static_cast<float>(mouseDownY + (player.getY() - mouseDownY) + (player.getHeight()/2))}, (Vector2){static_cast<float>(GetMouseX() + (player.getX() - mouseDownX) + (player.getWidth()/2)), static_cast<float>(GetMouseY() + (player.getY() - mouseDownY) + (player.getHeight()/2))}, 3.0f, BLACK);
            }
            //DrawRectangle(player.getX(), player.getY(), player.getWidth(), player.getHeight(), WHITE);  
            for (int i = 0; i < 4; i++)
            {
                DrawTexture(platformSprite, platforms[i].getX(), platforms[i].getY(), ColorFromNormalized((Vector4){.698, .588, .49, 1.0}));       
                if (platforms[i].getHasCoin())
                {
                    DrawTexture(ballSprite, platforms[i].getCoinX(), platforms[i].getCoinY(), WHITE);
                }
            }
            DrawTexture(thorSprite, player.getX(), player.getY(), WHITE);  
            DrawTexture(lavaSprite, 0, lavaY, WHITE); 
            DrawTexture(scoreBoxSprite, 17, 17, WHITE); 
            DrawTextEx(font, score,Vector2{28, 20},64, 0, BLACK);        
            DrawTextEx(font, highscore,Vector2{17, 90}, 32, 0, BLACK);        

            EndDrawing();
        }
       
    }

    UnloadTexture(thorSprite);
    UnloadTexture(lavaSprite);
    UnloadTexture(platformSprite);
    UnloadTexture(ballSprite);
    UnloadTexture(scoreBoxSprite);
    UnloadTexture(logo);
    UnloadTexture(splashThorSprite);
    UnloadSound(fxClick);
    UnloadSound(fxLaunch);
    UnloadSound(fxDeath);
    UnloadSound(fxCoin);
    UnloadSound(fxSplash);
    UnloadFont(font);
    CloseAudioDevice();
    CloseWindow();        
   
    return 0;
}
