#include QMK_KEYBOARD_H
#include "animation_observer.h"
#include OLED_FONT_H

/**
 * Render the Apple logo. x and y are pixels not columns and rows.
 */
void render_apple(uint8_t x, uint8_t y) {
    /**
     * 11 x 11 Apple Logo
     */
    static const char apple[] PROGMEM = {
0x00, 0xf0, 0xf8, 0xf8, 0xf8, 0xf2, 0xfb, 0x99, 0x08, 0x00, 0x00, // Top

0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Pad to next line
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // TODO remove padding / make automatic

0x00, 0x00, 0x03, 0x07, 0x07, 0x03, 0x07, 0x07, 0x03, 0x00, 0x00, // Bottom
    };
    oled_set_cursor(x / OLED_FONT_WIDTH, y / OLED_FONT_HEIGHT);
    oled_write_raw_P(apple, sizeof(apple));
}

/**
 * Render the Windows logo. x and y are pixels not columns and rows.
 */
void render_windows(uint8_t x, uint8_t y) {
    /**
     * 11 x 11 Windows Logo
     */
    static const char windows[] PROGMEM = {
0x00, 0xde, 0xde, 0xde, 0xde, 0x00, 0xde, 0xde, 0xde, 0xde, 0x00, // Top

0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Pad to next line
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // TODO remove padding / make automatic

0x00, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03, 0x03, 0x03, 0x03, 0x00, // Bottom
    };
    oled_set_cursor(x / OLED_FONT_WIDTH, y / OLED_FONT_HEIGHT);
    oled_write_raw_P(windows, sizeof(windows));
}

void render_layer(uint8_t y, bool on) {
    static const char layer_off[] PROGMEM = {
0x00, 0x00, 0x80, 0xc0, 0x60, 0x20, 0x30, 0x10, 0x18, 0x08, 0x0c, 0x04, 0x06, 0x02, 0x03, 0x01,
0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x18, 0x10, 0x30, 0x20, 0x60, 0xc0, 0x80, 0x00, 0x00,
0x00, 0x00, 0x00, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x18, 0x10, 0x30, 0x20, 0x60, 0x40,
0x40, 0x60, 0x20, 0x30, 0x10, 0x18, 0x08, 0x0c, 0x04, 0x06, 0x02, 0x03, 0x01, 0x00, 0x00, 0x00
    };
    static const char layer_on[] PROGMEM = {
0x00, 0x00, 0x80, 0xc0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xfe, 0xff, 0xff,
0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xfc, 0xf8, 0xf8, 0xf0, 0xf0, 0xe0, 0xe0, 0xc0, 0x80, 0x00, 0x00,
0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x07, 0x07, 0x0f, 0x0f, 0x1f, 0x1f, 0x3f, 0x3f, 0x7f, 0x7f,
0x7f, 0x7f, 0x3f, 0x3f, 0x1f, 0x1f, 0x0f, 0x0f, 0x07, 0x07, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00
    };

    uint8_t display_width = oled_get_display_width();
    const char* layer = layer_off;
    if ( on ) {
        layer = layer_on;
    }

    for (uint8_t x = 0; x < display_width; x++) {
        for (uint8_t j = 0; j < 2; j++) {
            char b = pgm_read_byte(layer + x + (j * display_width));
            for (uint8_t i = 0; i < 8; i++) {
                oled_write_pixel_xor(x, y + i + (j * 8), (b >> i) & 1);
            }
        }
    }
}

void clear_area_for_layers(uint8_t num_lines) {
    static const char* clear_line = "     ";
    while ( num_lines > 0 ) {
        oled_set_cursor(0, num_lines - 1);
        oled_write(clear_line, false);
        num_lines -= 1;
    }
}

void render_layers(void) {
    clear_area_for_layers(5);
    render_layer(0, IS_LAYER_ON(_FN));
    render_layer(9, IS_DEFAULT_LAYER_ON(_WIN));
    render_layer(18, IS_DEFAULT_LAYER_ON(_MAC));
}

bool oled_task_animation_layer_icons(void) {
    static bool rendered = false;
    if ( ! rendered ) {
        render_layers();
        rendered = true;
    }
    return false;
}

AnimationObserver* create_layer_icons_animation(void) {
    AnimationObserver* layerIcons = malloc(sizeof(AnimationObserver));
    if ( layerIcons != NULL ) {
        layerIcons->processRecordOled = NULL;
        layerIcons->oledTaskUser = oled_task_animation_layer_icons;
        layerIcons->oledInitUser = NULL;
        layerIcons->keyboardPostInitUser = NULL;
    }
    return layerIcons;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    if ( get_flag_can_render() ) {
        clear_area_for_layers(5);
        render_layer(0, IS_LAYER_ON_STATE(state, _FN));
        render_layer(9, IS_DEFAULT_LAYER_ON(_WIN));
        render_layer(18, IS_DEFAULT_LAYER_ON(_MAC));
    }
    return state;
}

layer_state_t default_layer_state_set_user(layer_state_t state) {
    if ( get_flag_can_render() ) {
        clear_area_for_layers(5);
        render_layer(0, IS_LAYER_ON(_FN));
        render_layer(9, IS_LAYER_ON_STATE(state, _WIN));
        render_layer(18, IS_LAYER_ON_STATE(state, _MAC));
    }
    return state;
}
