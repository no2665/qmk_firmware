/* Copyright 2020 Jay Greco
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H
#include OLED_FONT_H

#define _MAC 0
#define _WIN 1 // Windows
#define _FN 2

#define USB_HID_CHAR_A 61
#define USB_HID_CHAR_1 19
#define USB_HID_CHAR_0 9

enum custom_keycodes {
  KC_CUST = SAFE_RANGE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_MAC] = LAYOUT_iso(
             KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_DEL,
    KC_MUTE, KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC,          KC_HOME,
    MO(_FN), KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,  KC_NUBS, KC_PGUP,
    XXXXXXX, KC_LSFT, KC_NUHS, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_PGDN,
    XXXXXXX, KC_LCTL, KC_LALT, KC_LGUI,                            KC_SPC,                    XXXXXXX, KC_ALGR, KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
  ),
  [_WIN] = LAYOUT_iso(
             KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_DEL,
    KC_MUTE, KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC,          KC_HOME,
    MO(_FN), KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,  KC_NUHS, KC_PGUP,
    XXXXXXX, KC_LSFT, KC_NUBS, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_PGDN,
    XXXXXXX, KC_LCTL, KC_LALT, KC_LGUI,                            KC_SPC,                    XXXXXXX, KC_ALGR, KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
  ),
  [_FN] = LAYOUT_iso(
              KC_GRV,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12, _______,  KC_INS,
    RGB_TOG, _______, _______,DF(_WIN), _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,           KC_END,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______,DF(_MAC), _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______,                            _______,                   _______, _______, _______, KC_MPRV, KC_MPLY, KC_MNXT
  ),

};

#define MAX_CHARACTERS_ON_SCREEN 100
typedef struct {
    char    character;
    uint8_t x;
    uint8_t y;
    uint32_t last_update;
    uint8_t speed;
} Character;

Character characters[MAX_CHARACTERS_ON_SCREEN];
uint8_t characters_size = 0;
uint8_t character_to_update = 0;

uint32_t prevTime;

void add_character_to_screen(char c) {

    if ( characters_size >= MAX_CHARACTERS_ON_SCREEN ) {
        return;
    }

    Character n;
    n.character = c;
    n.y         = 0;
    n.x         = rand() % ( OLED_DISPLAY_HEIGHT - OLED_FONT_WIDTH );
    n.last_update = timer_read32();
    n.speed     = rand() % 3;

    characters[characters_size] = n;
    characters_size += 1;
}

void write_character_to_screen( uint8_t idx, uint8_t moved_by ) {
    Character c = characters[idx];

    uint8_t font_character[OLED_FONT_WIDTH];
    uint8_t cast_data = (uint8_t) c.character;  // font based on unsigned type for index
    if (cast_data < OLED_FONT_START || cast_data > OLED_FONT_END) {
        memset(font_character, 0x00, OLED_FONT_WIDTH);
    } else {
        const uint8_t *glyph = &font[(cast_data - OLED_FONT_START) * OLED_FONT_WIDTH];
        memcpy_P(font_character, glyph, OLED_FONT_WIDTH);
    }

    for ( uint8_t i = 0; i < OLED_FONT_WIDTH; i++ ) {
        uint8_t glyph_byte = font_character[ i ];
        for ( uint8_t y = 0; y < 8; y++ ) {
            if ( c.y + y > OLED_FONT_HEIGHT ) {
                uint8_t bit = (glyph_byte >> y) & 1;
                oled_write_pixel( c.x + i, c.y + y - OLED_FONT_HEIGHT, bit );
            }
        }
        for ( uint8_t j = 1; j <= moved_by; j++ ) {
            if ( c.y - j > OLED_FONT_HEIGHT ) {
                oled_write_pixel( c.x + i, c.y - j - OLED_FONT_HEIGHT, false );
            }
        }
    }
}

uint8_t update_character( uint8_t idx ) {
    Character *c = &characters[idx];
    uint32_t speed_time = (c->speed * 10) + 10;
    uint32_t time_now = timer_read32();
    uint32_t diff = time_now - c->last_update;
    if ( diff > speed_time ) {
        uint8_t delta = (uint8_t) (diff / speed_time);
        c->y += delta;
        c->last_update = time_now - (diff - ( speed_time * delta));
        return delta;
    }
    return 0;
}

bool remove_character_at( uint8_t idx ) {
    Character c = characters[idx];
    if ( c.y > OLED_DISPLAY_WIDTH + OLED_FONT_HEIGHT ) {
        for ( uint8_t x = idx + 1; x < characters_size; x++ ) {
            characters[x - 1] = characters[x];
        }
        characters_size -= 1;
        return true;
    }
    return false;
}

bool send_alternate_keycode(uint16_t keycode, keyrecord_t *record) {
    if ( record->event.pressed ) {
        register_code(keycode);
    } else {
        unregister_code(keycode);
    }
    return false;
}

/**
 * Set up keyboard variables
 */
void keyboard_post_init_user(void) {
    prevTime = timer_read32();
}

/**
 *
 */
void oled_task_user(void) {
    if ( characters_size == 0 ) {
        return;
    }
    if ( character_to_update >= characters_size ) {
        character_to_update = 0;
    }
    uint8_t moved_by = update_character( character_to_update );
    write_character_to_screen( character_to_update, moved_by );
    if ( ! remove_character_at( character_to_update ) ) {
        character_to_update += 1;
    }
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_90;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Send keystrokes to host keyboard, if connected (see readme)
    process_record_remote_kb(keycode, record);

    // Send keypresses to the screen
    if ( record->event.pressed ) {
        switch ( keycode ) {
            case KC_A ... KC_Z:
                add_character_to_screen( keycode + USB_HID_CHAR_A );
                break;
            case KC_1 ... KC_9:
                add_character_to_screen( keycode + USB_HID_CHAR_1 );
                break;
            case KC_0:
                add_character_to_screen( keycode + USB_HID_CHAR_0 );
                break;
            case KC_MINS:
                add_character_to_screen( keycode );
                break;
            case KC_EQL:
                add_character_to_screen( keycode + 15 );
                break;
            case KC_LBRC:
                add_character_to_screen( keycode + 44 );
                break;
            case KC_RBRC:
                add_character_to_screen( keycode + 45 );
                break;
            case KC_SCLN:
                add_character_to_screen( keycode + 8 );
                break;
            case KC_QUOT:
                add_character_to_screen( keycode + -13 );
                break;
            case KC_NUHS:
                add_character_to_screen( keycode + -15 );
                break;
            case KC_COMM:
                add_character_to_screen( keycode + -10 );
                break;
            case KC_DOT:
            case KC_SLSH:
                add_character_to_screen( keycode + -9 );
                break;
        }
    }

    // Layer specific keycode overrides
    bool shift = get_mods() & MOD_MASK_SHIFT;

    if ( IS_LAYER_ON(_MAC) ) {
        static bool shifted_kc_nubs = false;

        switch ( keycode ) {
            case KC_NUBS: // Creating the # ~ key
                if ( record->event.pressed ) {
                    if ( shift ) {
                        // Do nothing, send ~
                        shifted_kc_nubs = true;
                    } else {
                        // Send #
                        shifted_kc_nubs = false;
                        send_alternate_keycode(KC_LALT, record);
                        return send_alternate_keycode(KC_3, record);
                    }
                } else { // released
                    if ( shifted_kc_nubs ) {
                        // Do nothing
                    } else {
                        // remove #
                        send_alternate_keycode(KC_3, record);
                        return send_alternate_keycode(KC_LALT, record);
                    }
                }
        }
    }

    return true;
}

// RGB config, for changing RGB settings on non-VIA firmwares
void change_RGB(bool clockwise) {
    bool shift = get_mods() & MOD_MASK_SHIFT;
    bool alt   = get_mods() & MOD_MASK_ALT;
    bool ctrl  = get_mods() & MOD_MASK_CTRL;

    if (clockwise) {
        if (alt) {
            rgblight_increase_hue();
        } else if (ctrl) {
            rgblight_increase_val();
        } else if (shift) {
            rgblight_increase_sat();
        } else {
            rgblight_step();
        }

    } else {
        if (alt) {
            rgblight_decrease_hue();
        } else if (ctrl) {
            rgblight_decrease_val();
        } else if (shift) {
            rgblight_decrease_sat();
        } else {
            rgblight_step_reverse();
        }
    }
}

void encoder_update_kb(uint8_t index, bool clockwise) {
    if (layer_state_is(1)) {
        // change RGB settings
        change_RGB(clockwise);
    } else {
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    }
}

void matrix_init_user(void) {
    // Initialize remote keyboard, if connected (see readme)
    matrix_init_remote_kb();
}

void matrix_scan_user(void) {
    // Scan and parse keystrokes from remote keyboard, if connected (see readme)
    matrix_scan_remote_kb();
}
