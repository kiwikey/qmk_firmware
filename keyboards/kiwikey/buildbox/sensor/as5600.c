#include "i2c_master.h"
#include "as5600.h"
#include "print.h"

magnetic_encoder_t magnetic_encoders;

int16_t as5600read_angle(void) {
    uint8_t data = 0;
    int16_t angle = 0;

    as5600_write(REG_STATUS);
    if(as5600_read(&data)) {
        if(data & MAGNET_DETECTED_MASK) {
            as5600_write(REG_ANGLE);
            if(as5600_read(&data)) {
                angle = data;
                angle <<= 8;
                as5600_read(&data);
                angle |= data;
                }else {
                    return -1;
                }
        }else {
            print("\nMagnet not present!\n");
        }    
    }else {
        return -1;   
    }
    return angle;
}

bool ping_as5600(void) {
    uint8_t response = 0;
    as5600_write(REG_STATUS);
    as5600_read(&response);
    return response & MAGNET_DETECTED_MASK;
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
    if(magnetic_encoder.prev_angle > magnetic_encoder.new_angle)
        return magnetic_encoder.prev_angle - magnetic_encoder.new_angle;
    if(magnetic_encoder.prev_angle < magnetic_encoder.new_angle)
        return magnetic_encoder.new_angle - magnetic_encoder.prev_angle;   
    
    return 0;
}

int8_t get_movement(int max_distance, magnetic_encoder_t magnetic_encoder) {
    uint16_t distance = get_distance(magnetic_encoder);

    if(magnetic_encoder.new_angle > magnetic_encoder.prev_angle) {
        if(distance < max_distance) {
            magnetic_encoder.prev_movement = 1;
            return 1;
        }else {
            return magnetic_encoder.prev_movement;
        }
    }

    if(magnetic_encoder.prev_angle > magnetic_encoder.new_angle) {
        if(distance < max_distance) {
            magnetic_encoder.prev_movement = -1;
            return -1;
        }else {
            return magnetic_encoder.prev_movement;
        }
    }
    return 0;
}

void process_magnetic_encoder(void) {
    if(magnetic_encoders.is_present) {
        magnetic_encoders.new_angle = as5600read_angle();
        if(magnetic_encoders.new_angle == -1) {
            magnetic_encoders.is_present = false;
            return;
        }
        if(get_distance(magnetic_encoders) >= DEG_MARGIN_AS5600) {
            magnetic_encoders.movement = get_movement(MAX_DISTANCE_AS5600, magnetic_encoders);
            if(magnetic_encoders.movement == -1) {                       
                magnetic_encoder_update_user(false);
            }

            if(magnetic_encoders.movement == 1) {
                magnetic_encoder_update_user(true);
            }
            magnetic_encoders.prev_angle = magnetic_encoders.new_angle;
        }   
    } 

}

void housekeeping_task_magnetic_encoder(void) {
    process_magnetic_encoder();
	if(ping_as5600()) {
        magnetic_encoders.is_present = true;
    }
}

void keyboard_post_init_magnetic_encoder(void) {
    i2c_init();
    if(ping_as5600()) {
        magnetic_encoders.is_present = true;
    }
}