#pragma once

#include "i2c_master.h"
#include <stdbool.h>
#include <stdint.h>

#define AS5600_MAX_VALUE   4096
#define AS5600_HALF_VALUE  (AS5600_MAX_VALUE/2)

#define DEG_MARGIN_AS5600    32
#define MAX_DISTANCE_AS5600 500 // filter error/abnormal reads

#define AS5600_ADDRESS (0x36 << 1)
#define AS5600_I2C_TIMEOUT_MS 5 // bounded timeout so a bus glitch can't hang the matrix scan

#define MAGNET_DETECTED_MASK 0b00100000
#define MAGNET_WEAK_MASK     0b00110000  // Not used
#define MAGNET_STRONG_MASK   0b00101000  // Not used

enum REG_AS5600 {
    REG_STATUS = 0x0B,
    REG_ANGLE  = 0x0E
};

typedef struct {
    bool     is_present;
    uint16_t new_angle;
    uint16_t prev_angle;
    int8_t   movement;
    int8_t   prev_movement;
} magnetic_encoder_t;
extern magnetic_encoder_t magnetic_encoder;

bool is_magnet_detected(void);

// Returns 0-4095 on success, -1 on read failure or magnet not detected.
int16_t as5600_read_angle(void);

bool as5600_write(uint8_t reg_addr);
bool as5600_read(uint8_t *data, uint8_t len);

uint16_t get_distance(const magnetic_encoder_t *enc);
int8_t   get_direction(const magnetic_encoder_t *enc);

void process_magnetic_encoder(void);
void magnetic_encoder_update_kb(bool direction);
void housekeeping_task_magnetic_encoder(void);
void keyboard_post_init_magnetic_encoder(void);