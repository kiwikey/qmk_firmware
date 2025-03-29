#ifndef QP_INCLUDES
#define QP_INCLUDES

/* FONTS */
	extern painter_font_handle_t thintel;
	extern painter_font_handle_t roboto20;
	extern painter_font_handle_t roboto25;
	extern painter_font_handle_t robotobold25;

/* IMAGES & GIFS */
	// extern painter_image_handle_t img_pikachu;
	// extern painter_image_handle_t img_anya01;
	// extern painter_image_handle_t gif_pusheen;
	// extern painter_image_handle_t gif_nyan120px;
	// extern painter_image_handle_t gif_cat01;
	// extern painter_image_handle_t gif_cat02;
	// extern painter_image_handle_t gif_dog01;
	// extern deferred_token         my_anim;

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

void qp_init_load_files(void);

#endif /* QP_INCLUDES */
