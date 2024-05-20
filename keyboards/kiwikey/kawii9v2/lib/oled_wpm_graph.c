// Configuration for WPM Graph
#define GRAPH_ZERO_X           25  // This is zero origin
#define GRAPH_ZERO_Y           63  // of the WPM graph
#define GRAPH_WIDTH            100
#define GRAPH_HEIGHT           28
#define GRAPH_REFRESH_INTERVAL 100 // ms
#define GRAPH_LINE_THICKNESS   1
float max_wpm = 100.0;
uint16_t graph_timer = 0;

void render_wpm_graph(void) {
    uint8_t curr_wpm = (get_current_wpm() < max_wpm ? get_current_wpm() : max_wpm);
    if (timer_elapsed(graph_timer) > GRAPH_REFRESH_INTERVAL) {
        uint8_t graph_pointer = (curr_wpm / max_wpm) * GRAPH_HEIGHT;
        draw_line_h(GRAPH_ZERO_X + 1, GRAPH_ZERO_Y - graph_pointer, GRAPH_LINE_THICKNESS, true);
        for (char i = 0; i < graph_pointer; i++) {
            draw_line_h(GRAPH_ZERO_X + 1, GRAPH_ZERO_Y - i, GRAPH_LINE_THICKNESS, true);
        }
        for (char i = 0; i < GRAPH_LINE_THICKNESS; i++) {
            oled_pan_area(GRAPH_ZERO_X,
                          GRAPH_ZERO_X + GRAPH_WIDTH,
                          (GRAPH_ZERO_Y + 1 - GRAPH_HEIGHT) / 8,
                          (GRAPH_ZERO_Y + 1) / 8 - 1,
                          false);
        }
        graph_timer = timer_read();
    }
}
