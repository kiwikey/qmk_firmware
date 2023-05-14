#include "lvgl_ui.h"

/*************************************************/
static void set_angle(void* obj, int32_t v) {
    lv_arc_set_value(obj, v);
    dprintf("arc:%d\n", (int)v);
}
/**
 * Create an arc which acts as a loader.
 */
void lv_example_arc_2(void) {
    /*Create an Arc*/
    lv_obj_t* arc = lv_arc_create(lv_scr_act());
    lv_arc_set_rotation(arc, 270);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);  /*Be sure the knob is not displayed*/
    lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE); /*To not allow adjusting by click*/
    lv_obj_center(arc);

    static lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, arc);
    lv_anim_set_exec_cb(&a, set_angle);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE); /*Just for the demo*/
    lv_anim_set_repeat_delay(&a, 500);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_start(&a);
}

/*************************************************/
static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;

        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Button: %d", cnt);
    }
}
/**
 * Create a button with a label and react on click event.
 */
void lv_example_get_started_1(void)
{
    lv_obj_t * btn = lv_btn_create(lv_scr_act());     /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
    lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
    lv_label_set_text(label, "Button");                     /*Set the labels text*/
    lv_obj_center(label);
}

// /*************************************************/
// static lv_obj_t * label;

// static void slider_event_cb(lv_obj_t * slider, lv_event_t event)
// {
    // if(event == LV_EVENT_VALUE_CHANGED) {
        // /*Refresh the text*/
        // lv_label_set_text_fmt(label, "%d", lv_slider_get_value(slider));
    // }
// }

// /**
 // * Create a slider and write its value on a label.
 // */
// void lv_ex_get_started_3(void)
// {
    // /* Create a slider in the center of the display */
    // lv_obj_t * slider = lv_slider_create(lv_scr_act(), NULL);
    // lv_obj_set_width(slider, 200);                        /*Set the width*/
    // lv_obj_align(slider, NULL, LV_ALIGN_CENTER, 0, 0);    /*Align to the center of the parent (screen)*/
    // lv_obj_set_event_cb(slider, slider_event_cb);         /*Assign an event function*/

    // /* Create a label below the slider */
    // label = lv_label_create(lv_scr_act(), NULL);
    // lv_label_set_text(label, "0");
    // lv_obj_set_auto_realign(slider, true);                          /*To keep center alignment when the width of the text changes*/
    // lv_obj_align(label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);    /*Align below the slider*/
// }
