#include "as5600.h"
#include "i2c_master.h"
#include "print.h"
#include "timer.h"

magnetic_encoder_t magnetic_encoder;

bool as5600_read_register(uint8_t reg_addr, uint8_t *out, uint8_t len) {
	i2c_status_t result = i2c_read_register(AS5600_ADDRESS, reg_addr, out, len, AS5600_I2C_TIMEOUT_MS);
	if (result != I2C_STATUS_SUCCESS) {
		print("Error reading from AS5600\n");
		return false;
	}
	return true;
}

bool is_magnet_detected(void) {
	uint8_t status = 0;
	if (!as5600_read_register(REG_STATUS, &status, 1)) return false;
	return (status & MAGNET_DETECTED_MASK) != 0;
}

// Reads status + angle in one pass (2 I2C transactions). Returns -1 on any I2C
// failure or if the magnet isn't detected, so callers can rely on a single,
// unambiguous error sentinel instead of a bare 0 (which is also a valid angle).
// Not one 5-byte burst from REG_STATUS: the chip's address pointer wraps
// specially on its 2-byte angle registers, so a burst isn't guaranteed to
// reach REG_ANGLE (0x0E) from 0x0B.
int16_t as5600_read_angle(void) {
	uint8_t status = 0;
	if (!as5600_read_register(REG_STATUS, &status, 1)) return -1;

	if (!(status & MAGNET_DETECTED_MASK)) {
		print("\nMagnet not present!\n");
		return -1;
	}

	uint8_t buf[2];
	if (!as5600_read_register(REG_ANGLE, buf, 2)) return -1; // high+low byte in one read, so they always belong together

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

// Reads the current angle and sets prev_angle = new_angle to it, so the next
// process_magnetic_encoder() tick measures movement fresh from here instead
// of against a stale angle from before the magnet was absent - the magnet
// can be reinstalled at a completely different physical position, and
// without this, that gap would be misread as a real rotation.
static void magnetic_encoder_reprime_angle(void) {
	int16_t raw = as5600_read_angle();
	if (raw >= 0) {
		magnetic_encoder.prev_angle = (uint16_t)raw;
		magnetic_encoder.new_angle  = magnetic_encoder.prev_angle;
	} else {
		magnetic_encoder.is_present = false;
	}
}

int8_t process_magnetic_encoder(void) {
	if (!magnetic_encoder.is_present) return 0;

	int16_t raw = as5600_read_angle();
	if (raw < 0) {
		// Covers both I2C failure and magnet-not-detected; checked on the
		// signed value before it's narrowed into the unsigned struct field,
		// so this works correctly regardless of platform int width.
		magnetic_encoder.is_present = false;
		return 0;
	}
	magnetic_encoder.new_angle = (uint16_t)raw;

	uint16_t distance = get_distance(&magnetic_encoder);
	if (distance > MAX_DISTANCE_AS5600) {
		// Far more than a hand can turn in one tick - a glitched read, not real
		// rotation. Resync to it without reporting a movement, so it can't fire
		// a burst of volume/scroll taps.
		magnetic_encoder.prev_angle = magnetic_encoder.new_angle;
		return 0;
	}

	if (distance < DEG_MARGIN_AS5600) return 0;

	magnetic_encoder.prev_movement = magnetic_encoder.movement;
	magnetic_encoder.movement      = get_direction(&magnetic_encoder);
	magnetic_encoder.last_distance = distance;
	magnetic_encoder.prev_angle    = magnetic_encoder.new_angle;
	return magnetic_encoder.movement;
}

int8_t housekeeping_task_magnetic_encoder(void) {
	// Bounded poll rate - see AS5600_POLL_INTERVAL_MS. The presence retry below
	// has its own, slower timer.
	static uint32_t last_poll = 0;
	if (timer_elapsed32(last_poll) < AS5600_POLL_INTERVAL_MS) return 0;
	last_poll = timer_read32();

	// process_magnetic_encoder() already checks magnet presence via
	// as5600_read_angle()'s status read, so we don't need a second,
	// separate is_magnet_detected() transaction here.
	int8_t movement = process_magnetic_encoder();
	if (!magnetic_encoder.is_present) {
		// Retry presence at a bounded rate. This task runs on every
		// housekeeping tick (every main loop iteration) with no throttling
		// of its own, so without this the retry below fires continuously -
		// thousands of I2C transactions/sec - for as long as the magnet is
		// missing, which is enough bus activity to be a real noise source.
		static uint32_t last_retry = 0;
		if (timer_elapsed32(last_retry) >= AS5600_PRESENCE_RETRY_MS) {
			last_retry = timer_read32();
			if (is_magnet_detected()) {
				magnetic_encoder.is_present = true;
				magnetic_encoder_reprime_angle(); // avoid a spurious jump from the stale pre-removal angle
			}
		}
	}
	return movement;
}

void keyboard_post_init_magnetic_encoder(void) {
	i2c_init();
	magnetic_encoder.is_present = is_magnet_detected();
	if (magnetic_encoder.is_present) {
		magnetic_encoder_reprime_angle();
	}
}