#ifndef OLED_WPM_GRAPH
#define OLED_WPM_GRAPH

#include "quantum.h"
#include "oled_custom_api.h"

// Configuration for WPM Graph
#define GRAPH_ZERO_X           25  // This is zero origin
#define GRAPH_ZERO_Y           63  // of the WPM graph
#define GRAPH_WIDTH            100
#define GRAPH_HEIGHT           28
#define GRAPH_REFRESH_INTERVAL 100 // ms
#define GRAPH_LINE_THICKNESS   1

void render_wpm_graph(void);

#endif /* OLED_WPM_GRAPH */
