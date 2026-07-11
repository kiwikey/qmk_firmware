#include "as5600.h"
#include "i2c_master.h"
#include "print.h"

magnetic_encoder_t magnetic_encoder;

bool as5600_write(uint8_t reg_addr) {
    i2c_status_t result = i2c_transmit(AS5600_ADDRESS, &reg_addr, sizeof(uint8_t), AS5600_I2C_TIMEOUT_MS);
    if (result != I2C_STATUS_SUCCESS) {
        print("Error transmit to AS5600\n");
        return false;
    }
    return true;
}

bool as5600_read(uint8_t *out, uint8_t len) {
    i2c_status_t result = i2c_receive(AS5600_ADDRESS, out, len, AS5600_I2C_TIMEOUT_MS);
    if (result != I2C_STATUS_SUCCESS) {
        print("Error reading from AS5600\n");
        return false;
    }
    return true;
}

bool is_magnet_detected(void) {
    uint8_t status = 0;
    if (!as5600_write(REG_STATUS)) return false;
    if (!as5600_read(&status, 1)) return false;
    return (status & MAGNET_DETECTED_MASK) != 0;
}

// Reads status + angle in one pass. Returns -1 on any I2C failure or if the
// magnet isn't detected, so callers can rely on a single, unambiguous
// error sentinel instead of a bare 0 (which is also a valid angle).
int16_t as5600_read_angle(void) {
    uint8_t status = 0;
    if (!as5600_write(REG_STATUS)) return -1;
    if (!as5600_read(&status, 1)) return -1;

    if (!(status & MAGNET_DETECTED_MASK)) {
        print("\nMagnet not present!\n");
        return -1;
    }

    uint8_t buf[2];
    if (!as5600_write(REG_ANGLE)) return -1;
    if (!as5600_read(buf, 2)) return -1; // single burst read: high+low byte read atomically

    return (int16_t)(((uint16_t)buf[0] << 8) | buf[1]);
}

uint16_t get_distance(const magnetic_encoder_t *enc) {
    int16_t delta = (int16_t)enc->new_angle - (int16_t)enc->prev_angle;

    if (delta > AS5600_HALF_VALUE)
        delta -= AS5600_MAX_VALUE;
    else if (delta < -AS5600_HALF_VALUE)
        delta += AS5600_MAX_VALUE;

    return (delta >= 0) ? delta : -delta; // just abs()
}

int8_t get_direction(const magnetic_encoder_t *enc) {
    int16_t delta = (int16_t)enc->new_angle - (int16_t)enc->prev_angle;

    if (delta > AS5600_HALF_VALUE)
        delta -= AS5600_MAX_VALUE;
    else if (delta < -AS5600_HALF_VALUE)
        delta += AS5600_MAX_VALUE;

    if (delta > 0) return 1;   // CW
    if (delta < 0) return -1;  // CCW
    return 0;
}

void process_magnetic_encoder(void) {
    if (!magnetic_encoder.is_present) return;

    int16_t raw = as5600_read_angle();
    if (raw < 0) {
        // Covers both I2C failure and magnet-not-detected; checked on the
        // signed value before it's narrowed into the unsigned struct field,
        // so this works correctly regardless of platform int width.
        magnetic_encoder.is_present = false;
        return;
    }
    magnetic_encoder.new_angle = (uint16_t)raw;

    if (get_distance(&magnetic_encoder) >= DEG_MARGIN_AS5600) {
        magnetic_encoder.prev_movement = magnetic_encoder.movement;
        magnetic_encoder.movement      = get_direction(&magnetic_encoder);

        if (magnetic_encoder.movement == -1) {
            magnetic_encoder_update_kb(false);
        } else if (magnetic_encoder.movement == 1) {
            magnetic_encoder_update_kb(true);
        }
        magnetic_encoder.prev_angle = magnetic_encoder.new_angle;
    }
}

void housekeeping_task_magnetic_encoder(void) {
    // process_magnetic_encoder() already checks magnet presence via
    // as5600_read_angle()'s status read, so we don't need a second,
    // separate is_magnet_detected() transaction here.
    process_magnetic_encoder();
    if (!magnetic_encoder.is_present) {
        // retry presence next tick without spamming reads if the magnet is gone
        magnetic_encoder.is_present = is_magnet_detected();
    }
}

void keyboard_post_init_magnetic_encoder(void) {
    i2c_init();
    magnetic_encoder.is_present = is_magnet_detected();
    if (magnetic_encoder.is_present) {
        int16_t raw = as5600_read_angle();
        if (raw >= 0) {
            magnetic_encoder.prev_angle = (uint16_t)raw;
            magnetic_encoder.new_angle  = magnetic_encoder.prev_angle;
        } else {
            magnetic_encoder.is_present = false;
        }
    }
}