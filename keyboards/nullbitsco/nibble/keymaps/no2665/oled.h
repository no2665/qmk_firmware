#pragma once

struct Characters {
    char keycode;
    uint8_t x;
    uint8_t y;
} c;

bool process_record_user(uint16_t keycode, keyrecord_t *record);
void oled_task_user(void);
