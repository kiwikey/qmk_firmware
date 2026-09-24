#pragma once

/* FONTS */
	extern painter_font_handle_t thintel16;
	extern painter_font_handle_t thintel32;
	extern painter_font_handle_t font_oled;         // line_height =  8, width = 6
	extern painter_font_handle_t nanoplex16;
	extern painter_font_handle_t nanoplex32;
	extern painter_font_handle_t font16;

/* ICONS */
	// extern painter_image_handle_t lock_caps_on;
	// extern painter_image_handle_t lock_caps_off;
	// extern painter_image_handle_t lock_num_on;
	// extern painter_image_handle_t lock_num_off;
	// extern painter_image_handle_t lock_scrl_on;
	// extern painter_image_handle_t lock_scrl_off;
	extern painter_image_handle_t ico32_brightness;
	extern painter_image_handle_t ico16_arrow_up;   // All arrow icons are 
	extern painter_image_handle_t ico16_arrow_down; // color inverted BLACK <-> WHITE
	extern painter_image_handle_t ico16_arrow_left;
	extern painter_image_handle_t ico16_arrow_right;
	extern painter_image_handle_t ico12_arrow_left;
	extern painter_image_handle_t ico12_arrow_right;
	extern painter_image_handle_t ico22_gear;
	extern painter_image_handle_t ico18_heart;
	extern painter_image_handle_t ico24_application;
	extern painter_image_handle_t ico24_boss;
	extern painter_image_handle_t ico24_calculator;
	extern painter_image_handle_t ico24_component;
	extern painter_image_handle_t ico24_earth;
	extern painter_image_handle_t ico24_favourites;
	extern painter_image_handle_t ico24_film;
	extern painter_image_handle_t ico24_globe;
	extern painter_image_handle_t ico24_heart;
	extern painter_image_handle_t ico24_music;
	extern painter_image_handle_t ico24_tune;

/* IMAGES & ANIMATIONS */
	extern painter_image_handle_t gif_bootup01;
	extern painter_image_handle_t img_anya01;
	// extern painter_image_handle_t gif_pusheen;
	extern painter_image_handle_t gif_nyan120px;
	extern painter_image_handle_t gif_cat01;
	// extern painter_image_handle_t gif_cat02;
	// extern painter_image_handle_t gif_dog01;
	extern deferred_token         my_anim;

void qp_init_load_files(void);
