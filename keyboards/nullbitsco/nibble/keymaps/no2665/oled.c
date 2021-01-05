#include QMK_KEYBOARD_H

struct Characters {
    char    keycode;
    uint8_t x;
    uint8_t y;
} c;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        c.keycode = keycode;
        c.x = 0;
        c.y = 0;
    }
    return true;
}

void oled_task_user(void) {
    //oled_set_cursor(c.x, c.y);
    //oled_write_char(c.keycode, false);
    oled_set_cursor(0, 0);
    oled_write_char('H', false);
}
