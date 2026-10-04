#pragma once

/*** Idle-screen geometry shared by the layer-name box (qp_widget_layer.h) and the
	keymap grid below it (qp_widget_matrix.h). Each depends on the other's size or
	position, so both live here instead of the two headers including each other.
***/

// Layer-name box, top-left of the idle screen
#define WIDGET_LAYER_POSX          5
#define WIDGET_LAYER_POSY          0
#define WIDGET_LAYER_HEIGHT        25

// Keymap grid (4x4 keys), directly below the layer-name box
#define WIDGET_MATRIX_KEY_WIDTH    50
#define WIDGET_MATRIX_KEY_HEIGHT   50
#define WIDGET_MATRIX_KEY_SPACING  1
#define WIDGET_MATRIX_WIDTH        (WIDGET_MATRIX_KEY_WIDTH*4 + WIDGET_MATRIX_KEY_SPACING*3)
#define WIDGET_MATRIX_POSX         WIDGET_LAYER_POSX
#define WIDGET_MATRIX_POSY         (WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT + 10)

// The layer-name box spans the grid's full width
#define WIDGET_LAYER_WIDTH         WIDGET_MATRIX_WIDTH
