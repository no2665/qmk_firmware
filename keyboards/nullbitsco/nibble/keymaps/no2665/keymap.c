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

enum custom_keycodes {
  KC_CUST = SAFE_RANGE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_MAC] = LAYOUT_iso(
             KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_DEL,
    KC_MUTE, KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC,          KC_PGUP,
    MO(_FN), KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_NUBS, KC_ENT,  KC_PGDN,
    XXXXXXX, KC_LSFT, KC_NUHS, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_END,
    XXXXXXX, KC_LCTL, KC_LOPT, KC_LCMD,                            KC_SPC,                    XXXXXXX, KC_ALGR, KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
  ),
  [_WIN] = LAYOUT_iso(
             KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_DEL,
    KC_MUTE, KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC,          KC_PGUP,
    MO(_FN), KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_NUHS, KC_ENT,  KC_PGDN,
    XXXXXXX, KC_LSFT, KC_NUBS, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_END,
    XXXXXXX, KC_LCTL, KC_LALT, KC_LWIN,                            KC_SPC,                    XXXXXXX, KC_ALGR, KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
  ),
  [_FN] = LAYOUT_iso(
              KC_GRV,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12, _______,  KC_INS,
    RM_TOGG, _______, _______,DF(_WIN), _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______,DF(_MAC), _______, _______, _______, _______, _______, KC_HOME,
    _______, _______, _______, _______,                            _______,                   _______, _______, _______, KC_MPRV, KC_MPLY, KC_MNXT
  ),
};

/**
 * Send register_code or unregister_code depending on record->event.pressed
 */
bool send_alternate_keycode(uint16_t keycode, keyrecord_t *record) {
    if ( record->event.pressed ) {
        register_code(keycode);
    } else {
        unregister_code(keycode);
    }
    return false;
}

__attribute__((weak)) bool process_record_oled(uint16_t keycode, keyrecord_t *record) { return false; }

/**
 * Called on every key event
 */
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Send keystrokes to host keyboard, if connected (see readme)
    process_record_remote_kb(keycode, record);

    // Send keystrokes to oled controller
    process_record_oled(keycode, record);

    // Layer specific keycode overrides
    bool shift = get_mods() & MOD_MASK_SHIFT;
    bool gui = get_mods() & MOD_MASK_GUI;

    if ( IS_DEFAULT_LAYER_ON(_MAC) ) {
        static bool shifted_kc_nubs = false;
        static bool guied_kc_nuhs = false;

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
                break;
            case KC_NUHS: // Creating the normal Cmd + ` combo for switching windows. ` is normally next to the left shift
                if ( record->event.pressed ) {
                    if ( gui ) {
                        // Send backslash
                        guied_kc_nuhs = true;
                        return send_alternate_keycode(KC_NUBS, record);
                    } else {
                        // Do nothing
                        guied_kc_nuhs = false;
                    }
                } else { // released
                    if ( guied_kc_nuhs ) {
                        // Remove backslash
                        return send_alternate_keycode(KC_NUBS, record);
                    } else {
                        // Do nothing
                    }
                }
                break;
        }
    }

    if ( IS_DEFAULT_LAYER_ON(_WIN) ) {
        static bool shifted_kc_2 = false;
        static bool shifted_kc_quot = false;

        switch ( keycode ) {
            case KC_2: // Send @ instead of " on Windows
                if ( record->event.pressed ) {
                    if ( shift ) {
                        // Send @
                        shifted_kc_2 = true;
                        return send_alternate_keycode(KC_QUOT, record);
                    } else {
                        // Do nothing
                        shifted_kc_2 = false;
                    }
                } else { // released
                    if ( shifted_kc_2 ) {
                        // remove "
                        return send_alternate_keycode(KC_QUOT, record);
                    } else {
                        // Do nothing
                    }
                }
                break;
            case KC_QUOT: // Send " instead of @ on Windows
                if ( record->event.pressed ) {
                    if ( shift ) {
                        // Send @
                        shifted_kc_quot = true;
                        return send_alternate_keycode(KC_2, record);
                    } else {
                        // Do nothing
                        shifted_kc_quot = false;
                    }
                } else { // released
                    if ( shifted_kc_quot ) {
                        // remove "
                        return send_alternate_keycode(KC_2, record);
                    } else {
                        // Do nothing
                    }
                }
                break;
        }
    }

    return true;
}

__attribute__((weak)) void keyboard_post_init_oled(void) {}

void keyboard_post_init_user(void) {
    keyboard_post_init_oled();
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

bool encoder_update_kb(uint8_t index, bool clockwise) {
    if (layer_state_is(_FN)) {
        // change RGB settings
        change_RGB(clockwise);
    } else {
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    }
    return true;
}

void matrix_init_user(void) {
    // Initialize remote keyboard, if connected (see readme)
    matrix_init_remote_kb();
}

void matrix_scan_user(void) {
    // Scan and parse keystrokes from remote keyboard, if connected (see readme)
    matrix_scan_remote_kb();
}
