#include "as5600.h"
#include "i2c_master.h"
#include "print.h"

magnetic_encoder_t magnetic_encoder;

int16_t as5600_read_angle(void) {
    uint8_t data = 0;
    uint16_t angle = 0;

    if (!as5600_write(REG_STATUS)) return -1;
    if (as5600_read(&data)) {
        if (data & MAGNET_DETECTED_MASK) {
            if (!as5600_write(REG_ANGLE)) return -1;
            if(as5600_read(&data)) {
                angle = ((uint16_t)data << 8);
                if (!as5600_read(&data)) return -1;
                angle |= data;
                } else {
                    return -1;
                }
        } else {
            print("\nMagnet not present!\n");
        }    
    } else {
        return -1;   
    }
    return angle;
}

bool is_magnet_detected(void) {
    uint8_t response = 0;
    if (!as5600_write(REG_STATUS))
        return false;
    if (!as5600_read(&response))
        return false;
    return (response & MAGNET_DETECTED_MASK) != 0;
}

bool as5600_write(uint8_t reg_addr) {
    i2c_status_t result = i2c_transmit(AS5600_ADDRESS, &reg_addr, sizeof(uint8_t), I2C_TIMEOUT_INFINITE);
    if(result != I2C_STATUS_SUCCESS) {
        print("Error transmit to AS5600\n");
        return false;
    }
    return true;
}

bool as5600_read(uint8_t* out) { 
    i2c_status_t result = i2c_receive(AS5600_ADDRESS, out, sizeof(uint8_t), I2C_TIMEOUT_INFINITE);
    if(result != I2C_STATUS_SUCCESS) {
        print("Error reading from AS5600\n");
        return false;
    }
    return true;
}

uint16_t get_distance(magnetic_encoder_t magnetic_encoder) {
    int16_t delta = magnetic_encoder.new_angle - magnetic_encoder.prev_angle;

    if (delta > AS5600_HALF_VALUE)
        delta -= AS5600_MAX_VALUE;
    else if (delta < -AS5600_HALF_VALUE)
        delta += AS5600_MAX_VALUE;
    
    return (delta >= 0) ? delta : -delta; // just abs()
}

int8_t get_direction(magnetic_encoder_t magnetic_encoder) {
    int16_t delta = magnetic_encoder.new_angle - magnetic_encoder.prev_angle;

    if (delta > AS5600_HALF_VALUE)
        delta -= AS5600_MAX_VALUE;
    else if (delta < -AS5600_HALF_VALUE)
        delta += AS5600_MAX_VALUE;

    // TODO: check if delta > max_distance

    if (delta > 0) { // CW
        return 1;
    }
    else if (delta < 0) { // CCW
        return -1;
    }
    return 0;
}

void process_magnetic_encoder(void) {
    if(magnetic_encoder.is_present) {
        magnetic_encoder.new_angle = as5600_read_angle();
        if(magnetic_encoder.new_angle == -1) {
            magnetic_encoder.is_present = false;
            return;
        }
        if(get_distance(magnetic_encoder) >= DEG_MARGIN_AS5600) {
            magnetic_encoder.movement = get_direction(magnetic_encoder);
            if(magnetic_encoder.movement == -1) {
                magnetic_encoder_update_kb(false);
            }
            if(magnetic_encoder.movement == 1) {
                magnetic_encoder_update_kb(true);
            }
            magnetic_encoder.prev_angle = magnetic_encoder.new_angle;
        }
    }
}

void housekeeping_task_magnetic_encoder(void) {
    magnetic_encoder.is_present = is_magnet_detected();
    if (magnetic_encoder.is_present)
        process_magnetic_encoder();
}

void keyboard_post_init_magnetic_encoder(void) {
    i2c_init();
    magnetic_encoder.is_present = is_magnet_detected();
    if (magnetic_encoder.is_present) {
        magnetic_encoder.prev_angle = as5600_read_angle();
        magnetic_encoder.new_angle = magnetic_encoder.prev_angle;
    }
}