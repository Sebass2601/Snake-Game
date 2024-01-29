/**
 * MIT License
 *
 * Copyright (c) 2024 Sebastian Ramirez <sebas@hotai.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* INCLUDES ******************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "stdio.h"
#include <stdlib.h>

/* CONSTANTS ******************************************************************/

static const Color BACKGROUND_COLOR         = BLUE;
static const int   WINDOW_WIDTH             = 800;
static const int   WINDOW_HEIGHT            = 600;
static const int   SQUARE_WIDTH             = 800;
static const int   SQUARE_HEIGHT            = 560;
static const float BORDER_WIDTH             = 780;
static const float BORDER_HEIGHT            = 540;
static const char* WINDOW_TITLE             = "Snake";
static const float SNAKE_BLOCK_SIZE         = 10;
static const float INITIAL_POSX             = 390;
static const float INITIAL_POSY             = 270;
static const float SNAKE_SPEED              = 100;
static const float DEATH_TEXT_FONT          = 50; 
static const float DEATH_TEXT_POSX          = 250;
static const float DEATH_TEXT_POSY          = 255;
static const float RESTART_TEXT_POSX        = 310;
static const float RESTART_TEXT_POSY        = 335;
static const float RESTART_TEXT_FONT        = 40;
static const float RESTART_RECTANGLE_POSX   = 300;
static const float RESTART_RECTANGLE_POSY   = 320;
static const float RESTART_RECTANGLE_HEIGHT = 65;
static const float RESTART_RECTANGLE_WIDTH  = 220;
static const float PAUSE_TEXT_POSX          = 300;
static const float PAUSE_TEXT_POSY          = 30;
static const float PAUSE_TEXT_FONT          = 60;
static const float CONTROLS_TEXT_POSX       = 300;
static const float CONTROL_W_TEXT_POSY      = 110;
static const float CONTROL_A_TEXT_POSY      = 140;
static const float CONTROL_S_TEXT_POSY      = 170;
static const float CONTROL_D_TEXT_POSY      = 200;
static const float CONTROLS_TEXT_FONT       = 10;
static const float SCORE_POSX               = 10;
static const float SCORE_POSY               = 560;        

/* ENUMS **********************************************************************/

/**
 * The game states.
 */ 
enum GameState
{
    GAME_STATE_IS_RUNNING = 0,
    GAME_STATE_IS_PAUSED  = 1,
    GAME_STATE_GAME_OVER  = 2
};

typedef enum Direction
{
    DIRECTION_UP    = 0,
    DIRECTION_DOWN  = 1,
    DIRECTION_RIGHT = 2,
    DIRECTION_LEFT  = 3
} Direction;

/* STRUCTS ********************************************************************/

typedef struct RectangleNode {
    Rectangle snakeBodyRec;
    struct RectangleNode* next;
} RectangleNode;

/* GLOBAL VARIABLES ***********************************************************/

static Direction      g_direction        = DIRECTION_UP;
static enum GameState g_state            = GAME_STATE_IS_RUNNING;
static Rectangle      g_snakeBodyRec     = { 0 };
static Rectangle      g_snakeBody2Rec    = { 0 };
static Rectangle      g_snakeBody3Rec    = { 0 };
static Rectangle      g_appleRec         = { 0 };
static int            g_positionVariance = 0;
static int            g_countOfBlocks    = 0;
static RectangleNode* g_list             = NULL;
static Rectangle      g_snakeTaleRec     = { 0 };
static int            g_score            = 0;

/* FUNCTIONS ******************************************************************/

static int 
isSnakeHere(RectangleNode* list, Rectangle position)
{
    RectangleNode *currentNode = list;
    while (currentNode != NULL) 
    {
        if (currentNode->snakeBodyRec.x == position.x && currentNode->snakeBodyRec.y == position.y) 
        {
            return 1;
        }

        currentNode = currentNode->next;
    }

    return 0; 
}

static void 
MoveToEmptySpace(Rectangle* position) 
{ 
    while (isSnakeHere(g_list, *position) == 1) 
    {             
        position->x = position->x + SNAKE_BLOCK_SIZE;

        if (position->x >= BORDER_WIDTH) 
        {
            position->y = position->y + SNAKE_BLOCK_SIZE;
            position->x = SNAKE_BLOCK_SIZE;

            if ((position->y >= BORDER_HEIGHT))
            {
                position->y = SNAKE_BLOCK_SIZE;
                position->x = SNAKE_BLOCK_SIZE;   
            }
        }
    }
}

static RectangleNode*
createNode(Rectangle rectangle)
{
    RectangleNode* node = (RectangleNode*)malloc(sizeof(RectangleNode));
    node->snakeBodyRec = rectangle;
    node->next = NULL;

    return node;
}

static void
deleteNode(RectangleNode* node)
{
    free(node);
}

static void
deleteLast(RectangleNode* list)
{       
    RectangleNode* lastNode = list->next;
    RectangleNode* previousNode = list;

    while (lastNode->next != NULL)
    {
        lastNode = lastNode->next;
        previousNode = previousNode->next;
    }

    deleteNode(lastNode);
    previousNode->next = NULL;
}

static int
eatFood(RectangleNode* list)
{
    RectangleNode* lastNode = list;

    if (CheckCollisionRecs(list->snakeBodyRec, g_appleRec) == true) 
    {
        while (lastNode->next != NULL)
        {
            lastNode = lastNode->next;
        }

        g_snakeTaleRec.height = SNAKE_BLOCK_SIZE;
        g_snakeTaleRec.width = SNAKE_BLOCK_SIZE;
        g_snakeTaleRec.y = lastNode->snakeBodyRec.y;
        g_snakeTaleRec.x = lastNode->snakeBodyRec.x;

        if (g_direction == DIRECTION_UP) 
        {
            g_snakeTaleRec.y -= SNAKE_BLOCK_SIZE;
        }

        else if (g_direction == DIRECTION_DOWN)
        {
            g_snakeTaleRec.y += SNAKE_BLOCK_SIZE;
        }

        else if (g_direction == DIRECTION_LEFT)
        {
            g_snakeTaleRec.x -= SNAKE_BLOCK_SIZE;
        }

        else if (g_direction == DIRECTION_RIGHT)
        {
            g_snakeTaleRec.x += SNAKE_BLOCK_SIZE;
        }

        return 1;
    }

    return 0;
}

static void
deleteNodes(RectangleNode* list)
{
    RectangleNode* currentNode = list;
    RectangleNode* previousNode = NULL;

    while (currentNode != NULL) 
    {
        previousNode = currentNode;
        currentNode = currentNode->next;
        free(previousNode);
    }
}

static void 
addToEnd(RectangleNode* list, RectangleNode* node)
{
    RectangleNode* currentNode = list;

    while (currentNode->next != NULL) 
    {
        currentNode = currentNode->next;
    }

    currentNode->next = node;
}

static void
printList(RectangleNode* list)
{
    RectangleNode* currentNode = list;

    while (currentNode != NULL)
    {
        printf("%f - %f\n", currentNode->snakeBodyRec.x, currentNode->snakeBodyRec.y);
        currentNode = currentNode->next;
    }

    printf("END \n");
}


static void
paintNodes(RectangleNode* list)
{
    RectangleNode* currentNode = list;

    while(currentNode != NULL)
    {
        DrawRectangleRec(currentNode->snakeBodyRec, YELLOW);
        currentNode = currentNode->next;

    }
}

static int 
areBlocksColliding(RectangleNode* list) 
{
    RectangleNode* CurrentNode = list->next;

    while (CurrentNode->next != NULL) 
    {
        CurrentNode = CurrentNode->next;

        if(list->snakeBodyRec.y == CurrentNode->snakeBodyRec.y && list->snakeBodyRec.x == CurrentNode->snakeBodyRec.x) 
        {
            return 1;
        }
    }

    return 0;
}

static int
isTouchingBorders(int posx, int posy)
{
    if(posx > BORDER_WIDTH || posx < SNAKE_BLOCK_SIZE || posy > BORDER_HEIGHT || posy < SNAKE_BLOCK_SIZE) 
    {
        return 1;
    }

 return 0;
}

static int
isPaused() 
{
    if(IsKeyPressed(KEY_P))
    {
        return 1;
    }

 return 0;
}

static void
updateDirection()
{
    if (IsKeyDown(KEY_W) && g_direction != DIRECTION_DOWN)
    {
        g_direction = DIRECTION_UP;
        return;
    }

    if (IsKeyDown(KEY_A) && g_direction != DIRECTION_RIGHT)
    {
        g_direction = DIRECTION_LEFT;
        return; 
    }

    if (IsKeyDown(KEY_S) && g_direction != DIRECTION_UP)
    {
        g_direction = DIRECTION_DOWN;
        return;   
    }

    if (IsKeyDown(KEY_D) && g_direction != DIRECTION_LEFT)
        g_direction = DIRECTION_RIGHT;
}

static void
renderGamePlay()
{
    ClearBackground(YELLOW);
    char text[100] = { 0 };
    sprintf(text, "SCORE: %d", g_score);
    DrawText(text, SCORE_POSX, SCORE_POSY, 40, BLACK);

    DrawRectangle(SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE, BORDER_WIDTH, BORDER_HEIGHT, BLUE);

    updateDirection();

    switch (g_direction)
    {
        case DIRECTION_UP:
        {
            g_snakeBodyRec.y -= SNAKE_BLOCK_SIZE;

            RectangleNode* newHead = createNode(g_snakeBodyRec);
            RectangleNode* firstNode = g_list;
            newHead->next = firstNode;
            g_list = newHead;
            deleteLast(g_list);
            break;  
        }
        case DIRECTION_DOWN:
        {
            g_snakeBodyRec.y += SNAKE_BLOCK_SIZE;

            RectangleNode* newHead = createNode(g_snakeBodyRec);
            RectangleNode* firstNode = g_list;
            newHead->next = firstNode;
            g_list = newHead;
            deleteLast(g_list);
            break;  
        }
        case DIRECTION_LEFT:
        {
            g_snakeBodyRec.x -= SNAKE_BLOCK_SIZE;

            RectangleNode* newHead = createNode(g_snakeBodyRec);
            RectangleNode* firstNode = g_list;
            newHead->next = firstNode;
            g_list = newHead;
            deleteLast(g_list);
            break;  
        }
        case DIRECTION_RIGHT:
        {
            g_snakeBodyRec.x += SNAKE_BLOCK_SIZE;

            RectangleNode* newHead = createNode(g_snakeBodyRec);
            RectangleNode* firstNode = g_list;
            newHead->next = firstNode;
            g_list = newHead;
            deleteLast(g_list);
            break;  
    
        }
        default:
            break;
    }

    paintNodes(g_list);
    DrawRectangleRec(g_appleRec, RED);

    if(eatFood(g_list) == 1)
    {
        g_score = g_score + SNAKE_BLOCK_SIZE;
        RectangleNode* Tale = createNode(g_snakeTaleRec);
        addToEnd(g_list, Tale);
        g_appleRec.x = (GetRandomValue(SNAKE_BLOCK_SIZE, BORDER_WIDTH) / 10) * 10;
        g_appleRec.y = (GetRandomValue(SNAKE_BLOCK_SIZE, BORDER_HEIGHT) / 10) * 10;
        MoveToEmptySpace(&g_appleRec);
    }

    if(areBlocksColliding(g_list) == 1)
    {
        g_state = GAME_STATE_GAME_OVER;
    }

    if (isTouchingBorders(g_list->snakeBodyRec.x, g_list->snakeBodyRec.y)) 
    {
        g_state = GAME_STATE_GAME_OVER;
    }

    if (isPaused()) 
    {
        g_state = GAME_STATE_IS_PAUSED;
    }
}

static void renderPause()
{
    ClearBackground(YELLOW);
    DrawRectangle( 10, 10, BORDER_WIDTH, BORDER_HEIGHT, BLUE);

    Color pauseColor = { 10, 15, 15, 100 };
    DrawRectangle(0, 0, SQUARE_WIDTH, SQUARE_HEIGHT, pauseColor);
    DrawText("PAUSE", PAUSE_TEXT_POSX, PAUSE_TEXT_POSY, PAUSE_TEXT_FONT, WHITE);
    DrawText("UP = W", CONTROLS_TEXT_POSX, CONTROL_W_TEXT_POSY, CONTROLS_TEXT_FONT, WHITE);
    DrawText("RIGHT = A", CONTROLS_TEXT_POSX, CONTROL_A_TEXT_POSY, CONTROLS_TEXT_FONT, WHITE);
    DrawText("DOWN = S", CONTROLS_TEXT_POSX, CONTROL_S_TEXT_POSY, CONTROLS_TEXT_FONT, WHITE);
    DrawText("LEFT = D", CONTROLS_TEXT_POSX, CONTROL_D_TEXT_POSY, CONTROLS_TEXT_FONT, WHITE);

    if (IsKeyPressed(KEY_P))
    {
        g_state = GAME_STATE_IS_RUNNING;
    }
}

static void renderGameOver()
{
    ClearBackground(YELLOW);
    DrawRectangle( 10, 10, BORDER_WIDTH, BORDER_HEIGHT, BLUE);

    Rectangle restartRec = {
        RESTART_RECTANGLE_POSX,
        RESTART_RECTANGLE_POSY,
        RESTART_RECTANGLE_WIDTH,
        RESTART_RECTANGLE_HEIGHT
    };

    deleteNodes(g_list);

    g_snakeBodyRec.x = INITIAL_POSX;
    g_snakeBodyRec.y = INITIAL_POSY;

    g_snakeBody2Rec.x = INITIAL_POSX;
    g_snakeBody2Rec.y = INITIAL_POSY + 10;

    g_snakeBody3Rec.x = INITIAL_POSX;
    g_snakeBody3Rec.y = INITIAL_POSY + 20;

    g_list = createNode(g_snakeBodyRec);
    RectangleNode* b = createNode(g_snakeBody2Rec);
    RectangleNode* c = createNode(g_snakeBody3Rec);
    addToEnd(g_list, b);
    addToEnd(g_list, c);

    DrawText("GAME OVER", DEATH_TEXT_POSX, DEATH_TEXT_POSY, DEATH_TEXT_FONT, BLACK);
    DrawRectangleRec(restartRec, YELLOW);
    DrawText("RESTART", RESTART_TEXT_POSX, RESTART_TEXT_POSY, RESTART_TEXT_FONT, BLACK);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) 
    {
        if (CheckCollisionPointRec(GetMousePosition(), restartRec)) 
        {
            g_score = 0;
            g_state = GAME_STATE_IS_RUNNING;
            g_direction = DIRECTION_UP;

        }
    }
}

/* DEFINITONS *****************************************************************/

/**
 * Program entry point.
 */
int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

    g_snakeBodyRec.x = INITIAL_POSX;
    g_snakeBodyRec.y = INITIAL_POSY;
    g_snakeBodyRec.width = SNAKE_BLOCK_SIZE;
    g_snakeBodyRec.height = SNAKE_BLOCK_SIZE;

    g_snakeBody2Rec.x = INITIAL_POSX;
    g_snakeBody2Rec.y = INITIAL_POSY + 10;
    g_snakeBody2Rec.width = SNAKE_BLOCK_SIZE;
    g_snakeBody2Rec.height = SNAKE_BLOCK_SIZE;

    g_snakeBody3Rec.x = INITIAL_POSX;
    g_snakeBody3Rec.y = INITIAL_POSY + 20;
    g_snakeBody3Rec.width = SNAKE_BLOCK_SIZE;
    g_snakeBody3Rec.height = SNAKE_BLOCK_SIZE;

    g_appleRec.x = (GetRandomValue(SNAKE_BLOCK_SIZE, BORDER_WIDTH) / 10) * 10;
    g_appleRec.y = (GetRandomValue(SNAKE_BLOCK_SIZE, BORDER_HEIGHT) / 10) * 10;
    g_appleRec.width = SNAKE_BLOCK_SIZE;
    g_appleRec.height = SNAKE_BLOCK_SIZE;
    
    MoveToEmptySpace(&g_appleRec);

    g_list = createNode(g_snakeBodyRec);
    RectangleNode* b = createNode(g_snakeBody2Rec);
    RectangleNode* c = createNode(g_snakeBody3Rec);
    addToEnd(g_list, b);
    addToEnd(g_list, c);

    SetTargetFPS(18);
 
    while (!WindowShouldClose())
    {
        BeginDrawing();
    
        switch (g_state)
        {
            case GAME_STATE_IS_RUNNING:
                renderGamePlay();
                break;
            case GAME_STATE_IS_PAUSED:
                renderPause();
                break;
            case GAME_STATE_GAME_OVER:
                renderGameOver();
                break;
        }
        EndDrawing();
    }
    
    CloseWindow();

    return 0;
}
