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

/*** Paddle (per-difficulty defaults; MEDIUM matches these) ***/
#define BREAKOUT_PADDLE_W_DEFAULT 50
#define BREAKOUT_PADDLE_H         6
#define BREAKOUT_PADDLE_Y         (ST7789_HEIGHT - 14)

/*** Ball (per-difficulty defaults; MEDIUM matches these) ***/
#define BREAKOUT_BALL_SIZE          6
#define BREAKOUT_BALL_SPEED_DEFAULT 3

#define BREAKOUT_LIVES_DEFAULT 3
#define BREAKOUT_FRAME_MS      30 // ~33 fps game-logic tick

/*** Difficulty select screen ***/
#define BREAKOUT_DIFF_ITEM_H         34
#define BREAKOUT_DIFF_LIST_TOP       60
#define BREAKOUT_DIFF_TICKS_PER_STEP 24 // encoder ticks needed to move the cursor by one option (lower = more sensitive)

// Width of the selected-option highlight, as % of screen width, centered horizontally.
// 100 = full screen width fill, 50 = half screen width fill.
#define BREAKOUT_DIFF_FILL_PERCENT 50
#define BREAKOUT_DIFF_FILL_W       ((ST7789_WIDTH * BREAKOUT_DIFF_FILL_PERCENT) / 100)
#define BREAKOUT_DIFF_FILL_LEFT    ((ST7789_WIDTH - BREAKOUT_DIFF_FILL_W) / 2)
#define BREAKOUT_DIFF_FILL_RIGHT   (BREAKOUT_DIFF_FILL_LEFT + BREAKOUT_DIFF_FILL_W - 1)

typedef enum {
    BREAKOUT_DIFF_EASY = 0,
    BREAKOUT_DIFF_MEDIUM,
    BREAKOUT_DIFF_HARD,
    BREAKOUT_DIFF_MUTANT,
    BREAKOUT_DIFF_COUNT
} breakout_difficulty_t;

typedef enum {
    BREAKOUT_SELECT_DIFFICULTY = 0, // choosing EASY/MEDIUM/HARD/MUTANT! before the game starts
    BREAKOUT_READY,                 // ball resting on paddle, waiting for launch (Button 2)
    BREAKOUT_RUNNING,
    BREAKOUT_GAMEOVER,
    BREAKOUT_WIN
} breakout_state_t;

bool breakout_is_active(void);
void breakout_open(void);          // enter from the menu: shows the difficulty select screen first
void breakout_exit(void);          // leave the game screen, back to the main UI
void breakout_button_action(void); // Button 2: confirm difficulty / launch ball / restart
void breakout_encoder_tick(bool clockwise);
void housekeeping_task_breakout(void);
