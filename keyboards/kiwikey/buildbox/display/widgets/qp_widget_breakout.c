#if defined(QUANTUM_PAINTER_ENABLE)

#include "qp_widget_breakout.h"

#include <stdio.h>
#include "timer.h"
#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/defines.h"

static bool              active = false;
static breakout_state_t  state;
static bool              bricks[BREAKOUT_ROWS][BREAKOUT_COLS];
static uint8_t           bricks_alive;
static int16_t           paddle_x;
static int16_t           ball_x, ball_y;
static int8_t            ball_vx, ball_vy;
static uint16_t          score;
static uint8_t           lives;
static uint32_t          last_frame;

static const hsv_t brick_colors[BREAKOUT_ROWS] = {
    { HSV_RED },
    { HSV_ORANGE },
    { HSV_YELLOW },
    { HSV_GREEN },
};

bool breakout_is_active(void) {
    return active;
}

static inline int16_t brick_left(uint8_t col) {
    return col * BREAKOUT_BRICK_W;
}
static inline int16_t brick_top(uint8_t row) {
    return BREAKOUT_BRICK_TOP + row * (BREAKOUT_BRICK_H + BREAKOUT_BRICK_GAP);
}

static void draw_brick(uint8_t row, uint8_t col, bool alive) {
    int16_t left   = brick_left(col);
    int16_t top    = brick_top(row);
    int16_t right  = left + BREAKOUT_BRICK_W - BREAKOUT_BRICK_GAP - 1;
    int16_t bottom = top + BREAKOUT_BRICK_H - 1;
    if (alive) {
        hsv_t c = brick_colors[row];
        qp_rect(my_display, left, top, right, bottom, c.h, c.s, c.v, true);
    } else {
        qp_rect(my_display, left, top, right, bottom, HSV_BLACK, true);
    }
}

static void draw_all_bricks(void) {
    for (uint8_t row = 0; row < BREAKOUT_ROWS; row++) {
        for (uint8_t col = 0; col < BREAKOUT_COLS; col++) {
            draw_brick(row, col, bricks[row][col]);
        }
    }
}

static void reset_bricks(void) {
    bricks_alive = BREAKOUT_ROWS * BREAKOUT_COLS;
    for (uint8_t row = 0; row < BREAKOUT_ROWS; row++) {
        for (uint8_t col = 0; col < BREAKOUT_COLS; col++) {
            bricks[row][col] = true;
        }
    }
}

static void draw_scorebar(void) {
    char buf[24];
    qp_rect(my_display, 0, 0, ST7789_WIDTH - 1, BREAKOUT_SCOREBAR_H - 1, HSV_BLACK, true);
    snprintf(buf, sizeof(buf), "Score:%-4u Lives:%u", score, lives);
    qp_drawtext(my_display, 4, (BREAKOUT_SCOREBAR_H - font_proggy_clean->line_height) / 2, font_proggy_clean, buf);
}

static void draw_paddle(int16_t old_x) {
    if (old_x != paddle_x) {
        qp_rect(my_display, old_x, BREAKOUT_PADDLE_Y, old_x + BREAKOUT_PADDLE_W - 1, BREAKOUT_PADDLE_Y + BREAKOUT_PADDLE_H - 1, HSV_BLACK, true);
    }
    qp_rect(my_display, paddle_x, BREAKOUT_PADDLE_Y, paddle_x + BREAKOUT_PADDLE_W - 1, BREAKOUT_PADDLE_Y + BREAKOUT_PADDLE_H - 1, HSV_WHITE, true);
}

static void draw_ball(int16_t old_x, int16_t old_y) {
    if (old_x != ball_x || old_y != ball_y) {
        qp_rect(my_display, old_x, old_y, old_x + BREAKOUT_BALL_SIZE - 1, old_y + BREAKOUT_BALL_SIZE - 1, HSV_BLACK, true);
    }
    qp_rect(my_display, ball_x, ball_y, ball_x + BREAKOUT_BALL_SIZE - 1, ball_y + BREAKOUT_BALL_SIZE - 1, HSV_WHITE, true);
}

static void clear_playfield_message_area(void) {
    // area between bricks and paddle, used for "ready"/"game over"/"win" messages
    qp_rect(my_display,
            0, brick_top(BREAKOUT_ROWS - 1) + BREAKOUT_BRICK_H + BREAKOUT_BRICK_GAP,
            ST7789_WIDTH - 1, BREAKOUT_PADDLE_Y - 1,
            HSV_BLACK, true);
}

static void draw_center_message(const char *line1, const char *line2) {
    clear_playfield_message_area();
    int16_t y = (brick_top(BREAKOUT_ROWS - 1) + BREAKOUT_BRICK_H + BREAKOUT_PADDLE_Y) / 2 - roboto20->line_height;
    qp_drawtext_recolor_center(my_display, ST7789_WIDTH / 2, y, roboto20, line1, HSV_WHITE, HSV_BLACK);
    if (line2) {
        qp_drawtext_recolor_center(my_display, ST7789_WIDTH / 2, y + roboto20->line_height + 2, roboto20, line2, HSV_WHITE, HSV_BLACK);
    }
}

static void reset_ball_on_paddle(void) {
    ball_x  = paddle_x + BREAKOUT_PADDLE_W / 2 - BREAKOUT_BALL_SIZE / 2;
    ball_y  = BREAKOUT_PADDLE_Y - BREAKOUT_BALL_SIZE;
    ball_vx = 0;
    ball_vy = 0;
    state   = BREAKOUT_READY;
    draw_ball(ball_x, ball_y);
    draw_center_message("Turn knob to aim", "Press knob to launch");
}

void breakout_start(void) {
    active = true;
    score  = 0;
    lives  = BREAKOUT_LIVES_START;
    paddle_x = (ST7789_WIDTH - BREAKOUT_PADDLE_W) / 2;

    qp_rect(my_display, 0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1, HSV_BLACK, true);
    draw_scorebar();
    reset_bricks();
    draw_all_bricks();
    draw_paddle(paddle_x);
    reset_ball_on_paddle();

    last_frame = timer_read32();
    qp_flush(my_display);
}

void breakout_exit(void) {
    active = false;
    ui_refresh();
}

void breakout_button_action(void) {
    if (!active) return;
    switch (state) {
        case BREAKOUT_READY:
            clear_playfield_message_area();
            ball_vy = -BREAKOUT_BALL_SPEED;
            ball_vx = (timer_read() & 0x01) ? (BREAKOUT_BALL_SPEED / 2) : -(BREAKOUT_BALL_SPEED / 2);
            state   = BREAKOUT_RUNNING;
            qp_flush(my_display);
            break;
        case BREAKOUT_GAMEOVER:
        case BREAKOUT_WIN:
            breakout_start();
            break;
        case BREAKOUT_RUNNING:
        default:
            break;
    }
}

void breakout_move_paddle(bool clockwise) {
    if (!active) return;
    int16_t old_x = paddle_x;
    paddle_x += clockwise ? BREAKOUT_PADDLE_SPEED : -BREAKOUT_PADDLE_SPEED;
    if (paddle_x < BREAKOUT_FIELD_LEFT) paddle_x = BREAKOUT_FIELD_LEFT;
    if (paddle_x > BREAKOUT_FIELD_RIGHT - BREAKOUT_PADDLE_W + 1) paddle_x = BREAKOUT_FIELD_RIGHT - BREAKOUT_PADDLE_W + 1;
    if (paddle_x == old_x) return;

    draw_paddle(old_x);
    if (state == BREAKOUT_READY) {
        int16_t old_bx = ball_x;
        ball_x = paddle_x + BREAKOUT_PADDLE_W / 2 - BREAKOUT_BALL_SIZE / 2;
        draw_ball(old_bx, ball_y);
    }
    qp_flush(my_display);
}

static void end_round(bool life_lost) {
    if (life_lost) {
        lives--;
        draw_scorebar();
        if (lives == 0) {
            state = BREAKOUT_GAMEOVER;
            draw_center_message("GAME OVER", "Press knob to restart");
        } else {
            reset_ball_on_paddle();
        }
    } else { // all bricks cleared
        state = BREAKOUT_WIN;
        draw_center_message("YOU WIN!", "Press knob to restart");
    }
    qp_flush(my_display);
}

void housekeeping_task_breakout(void) {
    if (!active || state != BREAKOUT_RUNNING) return;
    if (timer_elapsed32(last_frame) < BREAKOUT_FRAME_MS) return;
    last_frame = timer_read32();

    int16_t old_x = ball_x, old_y = ball_y;
    int16_t nx = ball_x + ball_vx;
    int16_t ny = ball_y + ball_vy;

    // side walls
    if (nx <= BREAKOUT_FIELD_LEFT) {
        nx = BREAKOUT_FIELD_LEFT;
        ball_vx = -ball_vx;
    } else if (nx + BREAKOUT_BALL_SIZE - 1 >= BREAKOUT_FIELD_RIGHT) {
        nx = BREAKOUT_FIELD_RIGHT - BREAKOUT_BALL_SIZE + 1;
        ball_vx = -ball_vx;
    }

    // top wall
    if (ny <= BREAKOUT_FIELD_TOP) {
        ny = BREAKOUT_FIELD_TOP;
        ball_vy = -ball_vy;
    }

    // paddle
    if (ball_vy > 0 &&
        ny + BREAKOUT_BALL_SIZE - 1 >= BREAKOUT_PADDLE_Y &&
        ny <= BREAKOUT_PADDLE_Y + BREAKOUT_PADDLE_H &&
        nx + BREAKOUT_BALL_SIZE - 1 >= paddle_x &&
        nx <= paddle_x + BREAKOUT_PADDLE_W - 1) {
        ny = BREAKOUT_PADDLE_Y - BREAKOUT_BALL_SIZE;
        ball_vy = -ball_vy;
        int16_t hit_offset = (nx + BREAKOUT_BALL_SIZE / 2) - (paddle_x + BREAKOUT_PADDLE_W / 2);
        ball_vx = hit_offset / 8;
        if (ball_vx == 0) ball_vx = (hit_offset >= 0) ? 1 : -1;
        if (ball_vx > BREAKOUT_BALL_SPEED) ball_vx = BREAKOUT_BALL_SPEED;
        if (ball_vx < -BREAKOUT_BALL_SPEED) ball_vx = -BREAKOUT_BALL_SPEED;
    }

    // bricks - at most one hit per frame
    bool brick_hit = false;
    for (uint8_t row = 0; row < BREAKOUT_ROWS && !brick_hit; row++) {
        for (uint8_t col = 0; col < BREAKOUT_COLS && !brick_hit; col++) {
            if (!bricks[row][col]) continue;
            int16_t bl = brick_left(col);
            int16_t bt = brick_top(row);
            int16_t br = bl + BREAKOUT_BRICK_W - BREAKOUT_BRICK_GAP - 1;
            int16_t bb = bt + BREAKOUT_BRICK_H - 1;
            if (nx + BREAKOUT_BALL_SIZE - 1 >= bl && nx <= br &&
                ny + BREAKOUT_BALL_SIZE - 1 >= bt && ny <= bb) {
                bricks[row][col] = false;
                bricks_alive--;
                score += 10;
                draw_brick(row, col, false);
                ball_vy   = -ball_vy;
                brick_hit = true;
            }
        }
    }
    if (brick_hit) draw_scorebar();

    // bottom - miss
    if (ny + BREAKOUT_BALL_SIZE - 1 >= BREAKOUT_FIELD_BOTTOM) {
        end_round(true);
        return;
    }

    if (bricks_alive == 0) {
        end_round(false);
        return;
    }

    ball_x = nx;
    ball_y = ny;
    draw_ball(old_x, old_y);
    qp_flush(my_display);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
