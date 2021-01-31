#include QMK_KEYBOARD_H
#include "animation_observer.h"
#include OLED_FONT_H

/**
 * 11 x 11 Apple Logo
 */
const char apple[] PROGMEM = {
    0x00, 0xf0, 0xf8, 0xf8, 0xf8, 0xf2, 0xfb, 0x99, 0x08, 0x00, 0x00, // Top

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Pad to next line
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // TODO remove padding / make automatic

    0x00, 0x00, 0x03, 0x07, 0x07, 0x03, 0x07, 0x07, 0x03, 0x00, 0x00, // Bottom
};

/**
 * 11 x 11 Windows Logo
 */
const char windows[] PROGMEM = {
    0x00, 0xde, 0xde, 0xde, 0xde, 0x00, 0xde, 0xde, 0xde, 0xde, 0x00, // Top

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Pad to next line
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // TODO remove padding / make automatic

    0x00, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03, 0x03, 0x03, 0x03, 0x00, // Bottom
};

/**
 * Render the Apple logo. x and y are pixels not columns and rows.
 */
void render_apple(uint8_t x, uint8_t y) {
    oled_set_cursor(x / OLED_FONT_WIDTH, y / OLED_FONT_HEIGHT);
    oled_write_raw_P(apple, sizeof(apple));
}

/**
 * Render the Windows logo. x and y are pixels not columns and rows.
 */
void render_windows(uint8_t x, uint8_t y) {
    oled_set_cursor(x / OLED_FONT_WIDTH, y / OLED_FONT_HEIGHT);
    oled_write_raw_P(windows, sizeof(windows));
}

bool oled_task_animation_layer_icons(void) {
    // Layer states
    if ( IS_DEFAULT_LAYER_ON(_MAC) ) {
        render_apple(0, 0);
    }
    if ( IS_DEFAULT_LAYER_ON(_WIN) ) {
        render_windows(0, 0);
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
