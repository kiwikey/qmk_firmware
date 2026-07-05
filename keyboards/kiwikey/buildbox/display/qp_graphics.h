#pragma once

#define BOOT_ENABLE            FALSE
#define BOOT_DURATION          4000 // ms

#define DISPLAY_TIMEOUT_MIN    30	// seconds
#define DISPLAY_TIMEOUT_STEP   30
#define DISPLAY_TIMEOUT_MAX    180
#define DISPLAY_TIMEOUT_NEVER  DISPLAY_TIMEOUT_MAX + DISPLAY_TIMEOUT_STEP

extern painter_device_t my_display;

void display_init(void);
void display_bootup(void);
void ui_refresh(void);

void keyboard_post_init_display(void);
bool display_task_kb(void);
bool process_record_display(uint16_t keycode, keyrecord_t *record);

// void ui_refresh(void);
