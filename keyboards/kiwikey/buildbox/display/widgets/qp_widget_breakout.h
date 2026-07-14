#pragma once

#include "quantum.h"
#include "display/defines.h"

/*** Encoder ***/
#define BREAKOUT_STEP_SIZE    32  // smaller than STEP_SIZE/MENU_STEP_SIZE => finer paddle control
#define BREAKOUT_PADDLE_SPEED 8   // px moved per encoder tick

/*** Playfield ***/
#define BREAKOUT_SCOREBAR_H   20                          // top HUD strip (score/lives)
#define BREAKOUT_FIELD_TOP    BREAKOUT_SCOREBAR_H
#define BREAKOUT_FIELD_LEFT   0
#define BREAKOUT_FIELD_RIGHT  (ST7789_WIDTH - 1)
#define BREAKOUT_FIELD_BOTTOM (ST7789_HEIGHT - 1)

/*** Bricks ***/
#define BREAKOUT_COLS      8
#define BREAKOUT_ROWS      4
#define BREAKOUT_BRICK_W   (ST7789_WIDTH / BREAKOUT_COLS)
#define BREAKOUT_BRICK_H   10
#define BREAKOUT_BRICK_GAP 2
#define BREAKOUT_BRICK_TOP (BREAKOUT_FIELD_TOP + 4)

/*** Paddle ***/
#define BREAKOUT_PADDLE_W 50
#define BREAKOUT_PADDLE_H 6
#define BREAKOUT_PADDLE_Y (ST7789_HEIGHT - 14)

/*** Ball ***/
#define BREAKOUT_BALL_SIZE  6
#define BREAKOUT_BALL_SPEED 3

#define BREAKOUT_LIVES_START 3
#define BREAKOUT_FRAME_MS    30 // ~33 fps game-logic tick

typedef enum {
    BREAKOUT_READY = 0, // ball resting on paddle, waiting for launch (Button 2)
    BREAKOUT_RUNNING,
    BREAKOUT_GAMEOVER,
    BREAKOUT_WIN
} breakout_state_t;

bool breakout_is_active(void);
void breakout_start(void);        // enter the game screen from the menu
void breakout_exit(void);         // leave the game screen, back to the main UI
void breakout_button_action(void);// Button 2: launch ball / restart after game over or win
void breakout_move_paddle(bool clockwise);
void housekeeping_task_breakout(void);
