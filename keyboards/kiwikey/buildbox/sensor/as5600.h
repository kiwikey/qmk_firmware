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
#define AS5600_PRESENCE_RETRY_MS 250 // how often to retry I2C presence-detect while the magnet is missing
#define AS5600_POLL_INTERVAL_MS  2   // read the angle at most this often - every main-loop pass was far more I2C traffic than the knob needs

#define MAGNET_DETECTED_MASK 0b00100000

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
	uint16_t last_distance; // size (sensor counts) of the movement last reported by housekeeping_task_magnetic_encoder()
} magnetic_encoder_t;
extern magnetic_encoder_t magnetic_encoder;

bool is_magnet_detected(void);

// Returns 0-4095 on success, -1 on read failure or magnet not detected.
int16_t as5600_read_angle(void);

// Reads `len` bytes starting at register `reg_addr`, as one I2C transaction
// (register write + repeated-start read).
bool as5600_read_register(uint8_t reg_addr, uint8_t *data, uint8_t len);

uint16_t get_distance(const magnetic_encoder_t *enc);
int8_t   get_direction(const magnetic_encoder_t *enc);

int8_t process_magnetic_encoder(void);
// Reads the sensor once; returns the movement since the last reported one:
// +1 CW, -1 CCW, 0 = none (below DEG_MARGIN_AS5600, glitch, or no magnet).
// Its size is left in magnetic_encoder.last_distance. The driver never calls
// into keyboard/UI code itself - sensor/sensors_handler.c acts on the result.
int8_t housekeeping_task_magnetic_encoder(void);
void keyboard_post_init_magnetic_encoder(void);