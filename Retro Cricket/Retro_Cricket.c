#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include<stdbool.h>
#include<stdio.h>
#include<math.h>

typedef enum GameScreen {
    SCREEN_MAIN_MENU,
    SCREEN_FLAG_SELECTION,
    SCREEN_AI_SELECTION,
    SCREEN_TOSS,
    SCREEN_TOSS_RESULT,
    SCREEN_CHOICE,
    SCREEN_GAME_START,
    SCREEN_GAME_PLAY
} GameScreen;

// Global variables to maintain state
static GameScreen currentScreen = SCREEN_MAIN_MENU;
static bool flagSelected = false;
static int selectedFlag = -1;
static int aiSelectedFlag = -1;
static bool isTossing = false;
static bool tossResult = false;
static int playerChoice = -1;
static float coinRotation = 0.0f;
static float flipSpeed = 10.0f;
static Vector2 coinPosition;
static float tossResultTimer = 0.0f;
// static bool showTossResult = false;
static bool userWonToss = false;
static int gameChoice = -1;
static float batButtonScale = 1.0f;
static float bowlButtonScale = 1.0f;
static float musicStopTimer = 0.0f;
static bool musicStopped = false;
static Sound backgroundMusic;  

// Function declarations
void InitializeGame(void);
void UpdateMainMenu(Vector2 mousePosition, Rectangle startButtonBounds, Sound clickSound);
void UpdateFlagSelection(Vector2 mousePosition, Rectangle* flagBounds, Sound clickSound);
void UpdateAISelection(void);
void UpdateToss(Vector2 mousePosition, Rectangle headButtonBounds, Rectangle tailButtonBounds, Sound clickSound);
void UpdateTossResult(void);
void UpdateChoice(Vector2 mousePosition, Rectangle batButtonBounds, Rectangle bowlButtonBounds, Sound clickSound);
void UpdateGameStart(void);
void DrawMainMenu(Texture2D logo, Texture2D startButtonSprite, Rectangle startButtonBounds, float startButtonScale);
void DrawFlagSelection(Texture2D* flags, Rectangle* flagBounds, float* flagScale);
void DrawAISelection(Texture2D* flags);
void DrawToss(Texture2D tossSprite, Texture2D headButtonSprite, Texture2D tailButtonSprite, 
             Rectangle headButtonBounds, Rectangle tailButtonBounds);
void DrawTossResult(Texture2D headWinsSprite, Texture2D tailWinsSprite, Texture2D youWonTossSprite, 
                   Texture2D youLostTossSprite);
void DrawChoice(Texture2D chooseBatOrBowlSprite, Texture2D batButtonSprite, Texture2D bowlButtonSprite,
               Rectangle batButtonBounds, Rectangle bowlButtonBounds);
void DrawGameStart(Texture2D youWillBatSprite, Texture2D youWillBowlSprite);
void CleanupResources(Texture2D* textures, Sound* sounds);
int GetAIScore(int userLastScore);
void gameplay(Texture2D userFlag, Texture2D aiFlag, bool isUserBattingFirst, Texture2D button1, Texture2D button2, Texture2D button3, Texture2D button4, Texture2D button6, Texture2D background, Texture2D handGestures[6], float buttonScale, float hoverScale);
void DrawGamePlay(Texture2D userFlag, Texture2D aiFlag, bool isUserBattingFirst, int innings, int lastUserScore, int aiScore, int totalScore, int wickets, int target, int userGesture, int aiGesture, bool isOut, float buttonScale, float hoverScale, Texture2D button1, Texture2D button2, Texture2D button3, Texture2D button4, Texture2D button6, Texture2D background, Texture2D handGestures[6], Vector2 button1Position, Vector2 button2Position, Vector2 button3Position, Vector2 button4Position, Vector2 button6Position, bool isButton1Hovered, bool isButton2Hovered, bool isButton3Hovered, bool isButton4Hovered, bool isButton6Hovered);

int main(void) {
    const int screenWidth = 1600;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Retro Cricket");
    InitAudioDevice();

    // Load textures
    Texture2D background = LoadTexture("assets/backgr2.png");
    Texture2D logo = LoadTexture("assets/test.png");
    Texture2D startButtonSprite = LoadTexture("assets/start.png");
    Texture2D flag1 = LoadTexture("assets/image.png");
    Texture2D flag2 = LoadTexture("assets/india.png");
    Texture2D flag3 = LoadTexture("assets/new zeeland.png");
    Texture2D flag4 = LoadTexture("assets/Australia.png");
    Texture2D tossSprite = LoadTexture("assets/tos.png");
    Texture2D headButtonSprite = LoadTexture("assets/heads.png");
    Texture2D tailButtonSprite = LoadTexture("assets/tails.png");
    Texture2D headWinsSprite = LoadTexture("assets/headwins.png");
    Texture2D tailWinsSprite = LoadTexture("assets/tailwins.png");
    Texture2D youWonTossSprite = LoadTexture("assets/wintoss.png");
    Texture2D youLostTossSprite = LoadTexture("assets/losstoss.png");
    Texture2D youWillBatSprite = LoadTexture("assets/ballfirst.png");
    Texture2D youWillBowlSprite = LoadTexture("assets/batfirst.png");
    Texture2D chooseBatOrBowlSprite = LoadTexture("assets/choose.png");
    Texture2D batButtonSprite = LoadTexture("assets/BAT.png");
    Texture2D bowlButtonSprite = LoadTexture("assets/BALL.png");

    // Load additional textures for batting/bowling
    Texture2D button1 = LoadTexture("assets/1.png");
    Texture2D button2 = LoadTexture("assets/2.png");
    Texture2D button3 = LoadTexture("assets/3.png");
    Texture2D button4 = LoadTexture("assets/4.png");
    Texture2D button6 = LoadTexture("assets/6.png");
    Texture2D handGestures[6];
    handGestures[0] = LoadTexture("assets/a1.png");
    handGestures[1] = LoadTexture("assets/a2.png");
    handGestures[2] = LoadTexture("assets/a3.png");
    handGestures[3] = LoadTexture("assets/a4.png");
    handGestures[4] = LoadTexture("assets/out.png");
    handGestures[5] = LoadTexture("assets/a5.png");

    // Load sounds
    backgroundMusic = LoadSound("assets/backmusic.mp3");
    Sound clickSound = LoadSound("assets/clicksound.wav");

    PlaySound(backgroundMusic);

    // Initialize button bounds
    Rectangle startButtonBounds = { screenWidth / 2 - startButtonSprite.width / 2, 380, startButtonSprite.width, startButtonSprite.height };
    Rectangle flagBounds[4] = {
        { 325, 300, flag1.width, flag1.height },
        { 575, 300, flag2.width, flag2.height },
        { 825, 300, flag3.width, flag3.height },
        { 1075, 300, flag4.width, flag4.height }
    };
    Rectangle headButtonBounds = { screenWidth / 2 - headButtonSprite.width / 2 - 150, screenHeight / 2, headButtonSprite.width, headButtonSprite.height };
    Rectangle tailButtonBounds = { screenWidth / 2 + 50, screenHeight / 2, tailButtonSprite.width, tailButtonSprite.height };

    int buttonGap = 50;
    Rectangle batButtonBounds = { 
        screenWidth / 2 - batButtonSprite.width - buttonGap / 2, 
        screenHeight / 2 + 50, 
        batButtonSprite.width, 
        batButtonSprite.height 
    };
    Rectangle bowlButtonBounds = { 
        screenWidth / 2 + buttonGap / 2, 
        screenHeight / 2 + 50, 
        bowlButtonSprite.width, 
        bowlButtonSprite.height 
    };

    coinPosition = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f - 50 };

    srand(time(NULL));
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        Vector2 mousePosition = GetMousePosition();

        // Update game state based on current screen
        switch (currentScreen) {
            case SCREEN_MAIN_MENU:
                UpdateMainMenu(mousePosition, startButtonBounds, clickSound);
                break;
            case SCREEN_FLAG_SELECTION:
                UpdateFlagSelection(mousePosition, flagBounds, clickSound);
                break;
            case SCREEN_AI_SELECTION:
                UpdateAISelection();
                break;
            case SCREEN_TOSS:
                UpdateToss(mousePosition, headButtonBounds, tailButtonBounds, clickSound);
                break;
            case SCREEN_TOSS_RESULT:
                UpdateTossResult();
                break;
            case SCREEN_CHOICE:
                UpdateChoice(mousePosition, batButtonBounds, bowlButtonBounds, clickSound);
                break;
            case SCREEN_GAME_START:
                UpdateGameStart();
                break;
            case SCREEN_GAME_PLAY:
                Texture2D flags[4] = {flag1, flag2, flag3, flag4};
                Texture2D handGesturesArr[6];
                for (int i = 0; i < 6; i++) handGesturesArr[i] = handGestures[i];
                gameplay(flags[selectedFlag], flags[aiSelectedFlag], gameChoice == 0, button1, button2, button3, button4, button6, background, handGesturesArr, batButtonScale, bowlButtonScale);
                break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(background, 0, 0, WHITE);

        // Draw current screen
        switch (currentScreen) {
            case SCREEN_MAIN_MENU:
                DrawMainMenu(logo, startButtonSprite, startButtonBounds, 
                           CheckCollisionPointRec(mousePosition, startButtonBounds) ? 1.1f : 1.0f);
                break;
            case SCREEN_FLAG_SELECTION: {
                float flagScale[4] = {1.0f, 1.0f, 1.0f, 1.0f};
                for (int i = 0; i < 4; i++) {
                    if (CheckCollisionPointRec(mousePosition, flagBounds[i])) {
                        flagScale[i] = 1.1f;
                    }
                }
                Texture2D flags[4] = {flag1, flag2, flag3, flag4};
                DrawFlagSelection(flags, flagBounds, flagScale);
                break;
            }
            case SCREEN_AI_SELECTION: {
                Texture2D flags[4] = {flag1, flag2, flag3, flag4};
                DrawAISelection(flags);
                break;
            }
            case SCREEN_TOSS:
                DrawToss(tossSprite, headButtonSprite, tailButtonSprite, headButtonBounds, tailButtonBounds);
                break;
            case SCREEN_TOSS_RESULT:
                DrawTossResult(headWinsSprite, tailWinsSprite, youWonTossSprite, youLostTossSprite);
                break;
            case SCREEN_CHOICE:
                DrawChoice(chooseBatOrBowlSprite, batButtonSprite, bowlButtonSprite, batButtonBounds, bowlButtonBounds);
                break;
            case SCREEN_GAME_START:
                DrawGameStart(youWillBatSprite, youWillBowlSprite);
                break;
            case SCREEN_GAME_PLAY:
            // The gameplay Draw function is in the gameplay function
              break;
        }

        EndDrawing();
    }

    // Cleanup
    Texture2D textures[] = {
        background, logo, startButtonSprite, flag1, flag2, flag3, flag4,
        tossSprite, headButtonSprite, tailButtonSprite, headWinsSprite,
        tailWinsSprite, youWonTossSprite, youLostTossSprite, youWillBatSprite,
        youWillBowlSprite, chooseBatOrBowlSprite, batButtonSprite, bowlButtonSprite,
        button1, button2, button3, button4, button6
    };
    Sound sounds[] = {backgroundMusic, clickSound};
    CleanupResources(textures, sounds);
    CloseWindow();

    return 0;
}

void UpdateMainMenu(Vector2 mousePosition, Rectangle startButtonBounds, Sound clickSound) {
    if (CheckCollisionPointRec(mousePosition, startButtonBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        PlaySound(clickSound);
        currentScreen = SCREEN_FLAG_SELECTION;
    }
}

void UpdateFlagSelection(Vector2 mousePosition, Rectangle* flagBounds, Sound clickSound) {
    for (int i = 0; i < 4; i++) {
        if (CheckCollisionPointRec(mousePosition, flagBounds[i]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            PlaySound(clickSound);
            selectedFlag = i;
            flagSelected = true;
            currentScreen = SCREEN_AI_SELECTION;
        }
    }
}

void UpdateAISelection(void) {
    if (aiSelectedFlag == -1) {
        do {
            aiSelectedFlag = rand() % 4;
        } while (aiSelectedFlag == selectedFlag);
        
        tossResultTimer = 2.0f;
    }
    
    tossResultTimer -= GetFrameTime();
    if (tossResultTimer <= 0.0f) {
        currentScreen = SCREEN_TOSS;
        flipSpeed = 10.0f;
    }
}

void UpdateToss(Vector2 mousePosition, Rectangle headButtonBounds, Rectangle tailButtonBounds, Sound clickSound) {
    if (playerChoice == -1) {
        if (CheckCollisionPointRec(mousePosition, headButtonBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            PlaySound(clickSound);
            playerChoice = 0;
            isTossing = true;
            tossResult = (rand() % 2) == 0;
        }
        if (CheckCollisionPointRec(mousePosition, tailButtonBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            PlaySound(clickSound);
            playerChoice = 1;
            isTossing = true;
            tossResult = (rand() % 2) == 0;
        }
    } else if (isTossing) {
        coinRotation += flipSpeed;
        if (flipSpeed > 0.1f) {
            flipSpeed *= 0.98f;
        } else {
            isTossing = false;
            tossResultTimer = 3.0f;
            if ((tossResult && (int)(coinRotation / 45) % 2 != 0) ||
                (!tossResult && (int)(coinRotation / 45) % 2 == 0)) {
                coinRotation += 45.0f;
            }
            currentScreen = SCREEN_TOSS_RESULT;
            userWonToss = (playerChoice == 0 && tossResult) || (playerChoice == 1 && !tossResult);
        }
    }
}

void UpdateTossResult(void) {
    tossResultTimer -= GetFrameTime();
    if (tossResultTimer <= 0.0f) {
        if (userWonToss) {
            currentScreen = SCREEN_CHOICE;
        } else {
            gameChoice = rand() % 2;
            currentScreen = SCREEN_GAME_START;
            musicStopTimer = 0.0f;
        }
    }
}

void UpdateChoice(Vector2 mousePosition, Rectangle batButtonBounds, Rectangle bowlButtonBounds, Sound clickSound) {
    bool batButtonHovered = CheckCollisionPointRec(mousePosition, batButtonBounds);
    bool bowlButtonHovered = CheckCollisionPointRec(mousePosition, bowlButtonBounds);

    if (batButtonHovered) {
        batButtonScale += 0.05f;
        if (batButtonScale > 1.1f) batButtonScale = 1.1f;
    } else {
        batButtonScale -= 0.05f;
        if (batButtonScale < 1.0f) batButtonScale = 1.0f;
    }

    if (bowlButtonHovered) {
        bowlButtonScale += 0.05f;
        if (bowlButtonScale > 1.1f) bowlButtonScale = 1.1f;
    } else {
        bowlButtonScale -= 0.05f;
        if (bowlButtonScale < 1.0f) bowlButtonScale = 1.0f;
    }

    if (batButtonHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        PlaySound(clickSound);
        gameChoice = 0;
        currentScreen = SCREEN_GAME_START;
        musicStopTimer = 0.0f;
    }
    if (bowlButtonHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        PlaySound(clickSound);
        gameChoice = 1;
        currentScreen = SCREEN_GAME_START;
        musicStopTimer = 0.0f;
    }
}

void UpdateGameStart(void) {
    if (!musicStopped) {
        musicStopTimer += GetFrameTime();
        if (musicStopTimer >= 4.0f) {
            StopSound(backgroundMusic);
            musicStopped = true;
            currentScreen = SCREEN_GAME_PLAY;
        }
    }
}

void DrawMainMenu(Texture2D logo, Texture2D startButtonSprite, Rectangle startButtonBounds, float startButtonScale) {
    DrawTexture(logo, GetScreenWidth() / 2 - logo.width / 2, 50, WHITE);
    DrawTextureEx(startButtonSprite, (Vector2){ startButtonBounds.x, startButtonBounds.y }, 0.0f, startButtonScale, WHITE);
}

void DrawFlagSelection(Texture2D* flags, Rectangle* flagBounds, float* flagScale) {
    DrawText("Choose Country", GetScreenWidth() / 2 - MeasureText("Choose Country", 40) / 2, 50, 50, RED);

    for (int i = 0; i < 4; i++) {
        DrawTextureEx(flags[i], (Vector2){flagBounds[i].x, flagBounds[i].y}, 0.0f, flagScale[i], WHITE);
        const char *flagName = (i == 0) ? "Pakistan" : (i == 1) ? "India" : (i == 2) ? "New Zealand" : "Australia";
        DrawText(flagName, flagBounds[i].x + flagBounds[i].width / 2 - MeasureText(flagName, 20) / 2, 
                flagBounds[i].y + flagBounds[i].height + 10, 24, BLACK);
    }
}

void DrawAISelection(Texture2D* flags) {
    const char* matchText = "Match Between";
    DrawText(matchText, GetScreenWidth() / 2 - MeasureText(matchText, 40) / 2, 50, 40, RED);
    
    float scale = 1.2f;
    // Draw player's team on the left
    DrawTextureEx(flags[selectedFlag], 
                 (Vector2){GetScreenWidth() / 4 - flags[selectedFlag].width * scale / 2, 
                          GetScreenHeight() / 2 - flags[selectedFlag].height * scale / 2}, 
                 0.0f, scale, WHITE);
    
    // Draw VS text in the middle
    const char* vsText = "VS";
    DrawText(vsText, GetScreenWidth() / 2 - MeasureText(vsText, 50) / 2, 
             GetScreenHeight() / 2 - 25, 50, DARKGRAY);
    
    // Draw AI's team on the right
    DrawTextureEx(flags[aiSelectedFlag], 
                 (Vector2){3 * GetScreenWidth() / 4 - flags[aiSelectedFlag].width * scale / 2, 
                          GetScreenHeight() / 2 - flags[aiSelectedFlag].height * scale / 2}, 
                 0.0f, scale, WHITE);
    
    // Draw team names
    const char* playerTeamName = (selectedFlag == 0) ? "Pakistan" : 
                               (selectedFlag == 1) ? "India" : 
                               (selectedFlag == 2) ? "New Zealand" : "Australia";
    const char* aiTeamName = (aiSelectedFlag == 0) ? "Pakistan" : 
                           (aiSelectedFlag == 1) ? "India" : 
                           (aiSelectedFlag == 2) ? "New Zealand" : "Australia";
    
    DrawText(playerTeamName, GetScreenWidth() / 4 - MeasureText(playerTeamName, 30) / 2, 
             GetScreenHeight() / 2 + flags[selectedFlag].height * scale / 2 + 20, 30, BLACK);
    DrawText(aiTeamName, 3 * GetScreenWidth() / 4 - MeasureText(aiTeamName, 30) / 2, 
             GetScreenHeight() / 2 + flags[aiSelectedFlag].height * scale / 2 + 20, 30, BLACK);
}

void DrawToss(Texture2D tossSprite, Texture2D headButtonSprite, Texture2D tailButtonSprite, 
             Rectangle headButtonBounds, Rectangle tailButtonBounds) {
    if (playerChoice == -1) {
        DrawTexture(tossSprite, GetScreenWidth() / 2 - tossSprite.width / 2, 50, WHITE);
        DrawTexture(headButtonSprite, headButtonBounds.x, headButtonBounds.y, WHITE);
        DrawTexture(tailButtonSprite, tailButtonBounds.x, tailButtonBounds.y, WHITE);
    } else if (isTossing) {
        float coinRadius = 100.0f;
        DrawCircleV(coinPosition, coinRadius + 2, DARKGRAY);
        DrawCircleV(coinPosition, coinRadius, YELLOW);
        const char *text = ((int)(coinRotation / 45) % 2 == 0) ? "Head" : "Tail";
        Vector2 textSize = MeasureTextEx(GetFontDefault(), text, 50, 1);
        DrawTextEx(GetFontDefault(), text,
                   (Vector2){ coinPosition.x - textSize.x / 2, coinPosition.y - textSize.y / 2 },
                   50, 1, BLACK);
    }
}

void DrawTossResult(Texture2D headWinsSprite, Texture2D tailWinsSprite, Texture2D youWonTossSprite, 
                   Texture2D youLostTossSprite) {
    if (tossResult) {
        DrawTexture(headWinsSprite, GetScreenWidth() / 2 - headWinsSprite.width / 2, 
                   GetScreenHeight() / 2 - headWinsSprite.height / 2, WHITE);
    } else {
        DrawTexture(tailWinsSprite, GetScreenWidth() / 2 - tailWinsSprite.width / 2, 
                   GetScreenHeight() / 2 - tailWinsSprite.height / 2, WHITE);
    }
    float messageYPosition = GetScreenHeight() / 2 + 100;
    if (userWonToss) {
        DrawTexture(youWonTossSprite, GetScreenWidth() / 2 - youWonTossSprite.width / 2, messageYPosition, WHITE);
    } else {
        DrawTexture(youLostTossSprite, GetScreenWidth() / 2 - youLostTossSprite.width / 2, messageYPosition, WHITE);
    }
}

void DrawChoice(Texture2D chooseBatOrBowlSprite, Texture2D batButtonSprite, Texture2D bowlButtonSprite,
               Rectangle batButtonBounds, Rectangle bowlButtonBounds) {
    DrawTexture(chooseBatOrBowlSprite, GetScreenWidth() / 2 - chooseBatOrBowlSprite.width / 2, 
               GetScreenHeight() / 2 - chooseBatOrBowlSprite.height - 100, WHITE);
    DrawTextureEx(batButtonSprite, (Vector2){ batButtonBounds.x, batButtonBounds.y }, 0.0f, batButtonScale, WHITE);
    DrawTextureEx(bowlButtonSprite, (Vector2){ bowlButtonBounds.x, bowlButtonBounds.y }, 0.0f, bowlButtonScale, WHITE);
}

void DrawGameStart(Texture2D youWillBatSprite, Texture2D youWillBowlSprite) {
    if (gameChoice == 0) {
        DrawTexture(youWillBatSprite, GetScreenWidth() / 2 - youWillBatSprite.width / 2, 
                   GetScreenHeight() / 2 - youWillBatSprite.height / 2, WHITE);
    } else {
        DrawTexture(youWillBowlSprite, GetScreenWidth() / 2 - youWillBowlSprite.width / 2, 
                   GetScreenHeight() / 2 - youWillBowlSprite.height / 2, WHITE);
    }
}

void CleanupResources(Texture2D* textures, Sound* sounds) {
    for (int i = 0; i < 21; i++) {
        UnloadTexture(textures[i]);
    }
    for (int i = 0; i < 2; i++) {
        UnloadSound(sounds[i]);
    }
}

// Function for AI scoring
int GetAIScore(int userLastScore) {
    int aiScore;
    int strategyChance = rand() % 100;  // Generate a random number between 0 and 99

    do {
        if (strategyChance < 50) {  // 50% chance to choose a number near the user's last score
            aiScore = userLastScore + (rand() % 3 - 1);  // Choose a number near the user's last score
            if (aiScore < 1) aiScore = 1;  // Clamp to valid range
            if (aiScore > 6) aiScore = 6;
        } else {  // 50% chance to choose completely random score
            aiScore = rand() % 6 + 1;  // Random score between 1 and 6
        }
    } while (aiScore == 5);  // Repeat until aiScore is not 5

    return aiScore;
}

void gameplay(Texture2D userFlag, Texture2D aiFlag, bool isUserBattingFirst, Texture2D button1, Texture2D button2, Texture2D button3, Texture2D button4, Texture2D button6, Texture2D background, Texture2D handGestures[6], float buttonScale, float hoverScale) {
    int innings = 1;
    bool cheerPlayed = false;
    Sound clicksound = LoadSound("assets/clicksound.wav");
    Sound six = LoadSound("assets/six.wav");
    Sound wicket = LoadSound("assets/wicket.wav");
    Sound cheer = LoadSound("assets/cheer.wav");

    int userScore = 0, aiScore = 0, lastUserScore = 0, wickets = 0, totalScore = 0, target = 0;
    int userGesture = -1, aiGesture = -1;
    bool isOut = false, shouldClose = false;
    float outTimer = 0.0f, closeTimer = 0.0f;

    int totalButtons = 5;
    float spacing = (GetScreenWidth() - (totalButtons * button1.width * buttonScale)) / (totalButtons + 1);
    Vector2 button1Position = { spacing, GetScreenHeight() - (button1.height * buttonScale) - 50 };
    Vector2 button2Position = { button1Position.x + button1.width * buttonScale + spacing, button1Position.y };
    Vector2 button3Position = { button2Position.x + button2.width * buttonScale + spacing, button1Position.y };
    Vector2 button4Position = { button3Position.x + button3.width * buttonScale + spacing, button1Position.y };
    Vector2 button6Position = { button4Position.x + button4.width * buttonScale + spacing, button1Position.y };

    while (!WindowShouldClose() && !shouldClose) {
        Vector2 mousePosition = GetMousePosition();
        float deltaTime = GetFrameTime();

        bool aiWon = (innings == 2 && totalScore >= target);
        bool userWon = (innings == 2 && wickets == 5);
        bool allowButtonInput = !(userWon || aiWon);
        bool isButton1Hovered = allowButtonInput && wickets < 5 && (CheckCollisionPointRec(mousePosition, (Rectangle){ button1Position.x, button1Position.y, button1.width * buttonScale, button1.height * buttonScale }) || IsKeyDown(KEY_ONE));
        bool isButton2Hovered = allowButtonInput && wickets < 5 && (CheckCollisionPointRec(mousePosition, (Rectangle){ button2Position.x, button2Position.y, button2.width * buttonScale, button2.height * buttonScale }) || IsKeyDown(KEY_TWO));
        bool isButton3Hovered = allowButtonInput && wickets < 5 && (CheckCollisionPointRec(mousePosition, (Rectangle){ button3Position.x, button3Position.y, button3.width * buttonScale, button3.height * buttonScale }) || IsKeyDown(KEY_THREE));
        bool isButton4Hovered = allowButtonInput && wickets < 5 && (CheckCollisionPointRec(mousePosition, (Rectangle){ button4Position.x, button4Position.y, button4.width * buttonScale, button4.height * buttonScale }) || IsKeyDown(KEY_FOUR));
        bool isButton6Hovered = allowButtonInput && wickets < 5 && (CheckCollisionPointRec(mousePosition, (Rectangle){ button6Position.x, button6Position.y, button6.width * buttonScale, button6.height * buttonScale }) || IsKeyDown(KEY_SIX));

        if (allowButtonInput) {
            if (isButton1Hovered && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_ONE))) { PlaySound(clicksound); lastUserScore = userScore = 1; userGesture = 0; }
            if (isButton2Hovered && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_TWO))) { PlaySound(clicksound); lastUserScore = userScore = 2; userGesture = 1; }
            if (isButton3Hovered && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_THREE))) { PlaySound(clicksound); lastUserScore = userScore = 3; userGesture = 2; }
            if (isButton4Hovered && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_FOUR))) { PlaySound(clicksound); lastUserScore = userScore = 4; userGesture = 3; }
            if (isButton6Hovered && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_SIX))) { PlaySound(clicksound); PlaySound(six); lastUserScore = userScore = 6; userGesture = 5; }

            if (userScore > 0 && wickets < 5) {
                aiScore = GetAIScore(userScore);
                aiGesture = aiScore - 1;

                if (aiScore == userScore) {
                    wickets++;
                    PlaySound(wicket);
                    isOut = true;
                    outTimer = 2.0f;
                } else {
                    if ((isUserBattingFirst && innings == 1) || (!isUserBattingFirst && innings == 2))
                        totalScore += userScore;
                    else
                        totalScore += aiScore;
                }
                userScore = 0;
            }
        }

        if (isOut) {
            outTimer -= deltaTime;
            if (outTimer <= 0.0f) {
                isOut = false;
                userGesture = -1;
                aiGesture = -1;
            }
        }

        if (innings == 1 && wickets == 5) {
            target = totalScore + 1;
            innings = 2;
            wickets = 0;
            totalScore = 0;
            userScore = 0;
            lastUserScore = 0;
            aiScore = 0;
            userGesture = -1;
            aiGesture = -1;
            isOut = false;
        }

        DrawGamePlay(userFlag, aiFlag, isUserBattingFirst, innings, lastUserScore, aiScore, totalScore, wickets, target, userGesture, aiGesture, isOut, buttonScale, hoverScale,
                     button1, button2, button3, button4, button6, background, handGestures, button1Position, button2Position, button3Position, button4Position, button6Position,
                     isButton1Hovered, isButton2Hovered, isButton3Hovered, isButton4Hovered, isButton6Hovered);

        if ((userWon || aiWon) && !cheerPlayed) {
            PlaySound(cheer);
            cheerPlayed = true;
            closeTimer = 0.0f;
        }

        if ((userWon || aiWon) && cheerPlayed) {
            closeTimer += deltaTime;
            if (closeTimer >= 4.5f) shouldClose = true;
        }

        if (IsKeyPressed(KEY_ESCAPE)) break;
    }

    UnloadSound(clicksound);
    UnloadSound(six);
    UnloadSound(wicket);
    UnloadSound(cheer);
    if (shouldClose) CloseWindow();
}

void DrawGamePlay(Texture2D userFlag, Texture2D aiFlag, bool isUserBattingFirst, int innings, int lastUserScore, int aiScore, int totalScore, int wickets, int target, int userGesture, int aiGesture, bool isOut, float buttonScale, float hoverScale, Texture2D button1, Texture2D button2, Texture2D button3, Texture2D button4, Texture2D button6, Texture2D background, Texture2D handGestures[6], Vector2 button1Position, Vector2 button2Position, Vector2 button3Position, Vector2 button4Position, Vector2 button6Position, bool isButton1Hovered, bool isButton2Hovered, bool isButton3Hovered, bool isButton4Hovered, bool isButton6Hovered) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTexture(background, 0, 0, WHITE);
    // Set a smaller scale for the buttons
    float smallButtonScale = 0.12f;
    float smallHoverScale = 0.16f;
    int totalButtons = 5;
    int buttonAreaWidth = GetScreenWidth() - 200; // leave some margin on both sides
    int buttonY = GetScreenHeight() - (int)(button1.height * smallButtonScale) - 40; // not too bottom
    int buttonSpacing = (buttonAreaWidth - (int)(button1.width * smallButtonScale) * totalButtons) / (totalButtons - 1);
    int startX = 100;
    // Draw buttons in a line, spaced evenly
    DrawTextureEx(button1, (Vector2){startX, buttonY}, 0.0f, isButton1Hovered ? smallHoverScale : smallButtonScale, WHITE);
    DrawTextureEx(button2, (Vector2){startX + (button1.width * smallButtonScale + buttonSpacing) * 1, buttonY}, 0.0f, isButton2Hovered ? smallHoverScale : smallButtonScale, WHITE);
    DrawTextureEx(button3, (Vector2){startX + (button1.width * smallButtonScale + buttonSpacing) * 2, buttonY}, 0.0f, isButton3Hovered ? smallHoverScale : smallButtonScale, WHITE);
    DrawTextureEx(button4, (Vector2){startX + (button1.width * smallButtonScale + buttonSpacing) * 3, buttonY}, 0.0f, isButton4Hovered ? smallHoverScale : smallButtonScale, WHITE);
    DrawTextureEx(button6, (Vector2){startX + (button1.width * smallButtonScale + buttonSpacing) * 4, buttonY}, 0.0f, isButton6Hovered ? smallHoverScale : smallButtonScale, WHITE);
    if (innings == 1) {
        DrawText(isUserBattingFirst ? "1st Innings - You are batting" : "1st Innings - You are bowling", GetScreenWidth() / 2 - MeasureText(isUserBattingFirst ? "1st Innings - You are batting" : "1st Innings - You are bowling", 30) / 2, 20, 30, DARKBLUE);
    } else {
        DrawText(isUserBattingFirst ? "2nd Innings - You are bowling" : "2nd Innings - You are batting", GetScreenWidth() / 2 - MeasureText(isUserBattingFirst ? "2nd Innings - You are bowling" : "2nd Innings - You are batting", 30) / 2, 20, 30, DARKBLUE);
    }
    DrawText(TextFormat("Last Entered Score: %i", lastUserScore), 50, 60, 30, DARKGREEN);
    DrawText(TextFormat("AI Score: %i", aiScore), GetScreenWidth() - 400, 60, 30, DARKGREEN);
    DrawText(TextFormat("Total Score: %i", totalScore), GetScreenWidth() / 2 - MeasureText(TextFormat("Total Score: %i", totalScore), 30) / 2, 60, 30, DARKGREEN);
    DrawText(TextFormat("Wickets: %i", wickets), GetScreenWidth() / 2 - MeasureText(TextFormat("Wickets: %i", wickets), 30) / 2, 100, 30, DARKGREEN);
    if (innings == 2) {
        DrawText(TextFormat("Target: %i", target), GetScreenWidth() / 2 - MeasureText(TextFormat("Target: %i", target), 30) / 2, 140, 30, DARKGREEN);
    }
    bool aiWon = (innings == 2 && totalScore >= target);
    bool userWon = (innings == 2 && wickets == 5);
if (userWon) {
    if (isUserBattingFirst) {
        int margin = target - totalScore; // How many runs AI fell short
        DrawText(TextFormat("User Won the Game by %i runs!", margin), 
            GetScreenWidth() / 2 - MeasureText(TextFormat("User Won the Game by %i runs!", margin), 24) / 2, 
            GetScreenHeight() / 2 - 190, 30, DARKGREEN);
    } else {
        int remainingWickets = 5 - wickets;
        DrawText(TextFormat("User Won the Game by %i wickets!", remainingWickets), 
            GetScreenWidth() / 2 - MeasureText(TextFormat("User Won the Game by %i wickets!", remainingWickets), 24) / 2, 
            GetScreenHeight() / 2 - 190, 30, DARKGREEN);
    }
} else if (aiWon) {
    if (isUserBattingFirst) {
        int remainingWickets = 5 - wickets;
        DrawText(TextFormat("AI Won the Game by %i wickets!", remainingWickets), 
            GetScreenWidth() / 2 - MeasureText(TextFormat("AI Won the Game by %i wickets!", remainingWickets), 26) / 2, 
            GetScreenHeight() / 2 - 190, 30, RED);
    } else {
        int margin = target - totalScore;
        DrawText(TextFormat("AI Won the Game by %i runs!", margin), 
            GetScreenWidth() / 2 - MeasureText(TextFormat("AI Won the Game by %i runs!", margin), 26) / 2, 
            GetScreenHeight() / 2 - 190, 30, RED);
    }
}


    DrawTexture(userFlag, 50, -20 + MeasureText(TextFormat("Wickets: %i", wickets), 30), WHITE);
    DrawTexture(aiFlag, GetScreenWidth() - aiFlag.width - 250, -20 + MeasureText(TextFormat("Wickets: %i", wickets), 30), WHITE);
    if (userGesture >= 0) {
        DrawTexture(handGestures[userGesture], GetScreenWidth() / 4 - handGestures[userGesture].width / 2, GetScreenHeight() / 2 - handGestures[userGesture].height / 2, WHITE);
    }
    if (aiGesture >= 0) {
        DrawTexture(handGestures[aiGesture], 3 * GetScreenWidth() / 4 - handGestures[aiGesture].width / 2, GetScreenHeight() / 2 - handGestures[aiGesture].height / 2, WHITE);
    }
    if (isOut) {
        DrawTexture(handGestures[4], GetScreenWidth() / 2 - handGestures[5].width / 2, GetScreenHeight() / 2 - handGestures[5].height / 2, WHITE);
    }
    EndDrawing();
}
