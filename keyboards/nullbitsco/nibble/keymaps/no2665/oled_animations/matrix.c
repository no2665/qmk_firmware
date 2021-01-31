#include QMK_KEYBOARD_H
#include "animation_observer.h"
#include OLED_FONT_H

#define USB_HID_CHAR_A 61
#define USB_HID_CHAR_1 19
#define USB_HID_CHAR_0 9
#define USB_HID_CHAR_EQL 15
#define USB_HID_CHAR_LBRC 44
#define USB_HID_CHAR_RBRC 45
#define USB_HID_CHAR_SCLN 8
#define USB_HID_CHAR_QUOT -13
#define USB_HID_CHAR_NUHS -15
#define USB_HID_CHAR_COMM -10
#define USB_HID_CHAR_DOT -9

#define MAX_CHARACTERS_ON_SCREEN 100
#define MIN_SPEED_TIME 10
#define SPEED_TIME_GAP 10
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

/**
 * Creates a new Character and adds it to the characters array
 */
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

/**
 * Output Character at idx in the characters array to the screen.
 */
void write_character_to_screen( uint8_t idx, uint8_t moved_by ) {
    Character c = characters[idx];

    // Get font glyph
    // Copied from oled_driver.c
    uint8_t font_character[OLED_FONT_WIDTH];
    uint8_t cast_data = (uint8_t) c.character;  // font based on unsigned type for index
    if (cast_data < OLED_FONT_START || cast_data > OLED_FONT_END) {
        memset(font_character, 0x00, OLED_FONT_WIDTH);
    } else {
        const uint8_t *glyph = &font[(cast_data - OLED_FONT_START) * OLED_FONT_WIDTH];
        memcpy_P(font_character, glyph, OLED_FONT_WIDTH);
    }

    // Loop over the glyph and output bit by bit
    // First loop goes through the columns
    for ( uint8_t x = 0; x < OLED_FONT_WIDTH; x++ ) {
        uint8_t glyph_byte = font_character[ x ];
        // Second loop goes through the rows
        for ( uint8_t y = 0; y < OLED_FONT_HEIGHT; y++ ) {
            // First OLED_FONT_HEIGHT rows are ignored to allow the character
            // to start above the top of the screen.
            if ( c.y + y > OLED_FONT_HEIGHT ) {
                uint8_t bit = (glyph_byte >> y) & 1;
                oled_write_pixel( c.x + x, c.y + y - OLED_FONT_HEIGHT, bit );
            }
        }
        // Turn the rows above (where the character used to be) off to prevent a trail
        for ( uint8_t j = 1; j <= moved_by; j++ ) {
            if ( c.y - j > OLED_FONT_HEIGHT ) {
                oled_write_pixel( c.x + x, c.y - j - OLED_FONT_HEIGHT, false );
            }
        }
    }
}

/**
 * See if the character should be moved, and move it.
 * Returns the amount of pixels it has moved.
 */
uint8_t update_character( uint8_t idx ) {
    // Get pointer so we can modify the character
    Character *c = &characters[idx];
    // Amount of time between movements
    uint32_t speed_time = (c->speed * SPEED_TIME_GAP) + MIN_SPEED_TIME;
    uint32_t time_now = timer_read32();
    uint32_t diff = time_now - c->last_update;
    // Check if it's time to update
    if ( diff > speed_time ) {
        // The amount of pixels to move
        uint8_t delta = (uint8_t) (diff / speed_time);
        c->y += delta;
        // Reset last_update, but also take off any remainder time.
        // Should make the animation smoother.
        c->last_update = time_now - (diff - ( speed_time * delta));
        return delta;
    }
    return 0;
}

/**
 * Determine if a character is off the screen and 'remove' it
 * by shuffling characters and reducing characters_size
 */
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

bool process_record_animation_matrix(uint16_t keycode, keyrecord_t *record) {
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
                add_character_to_screen( keycode + USB_HID_CHAR_EQL );
                break;
            case KC_LBRC:
                add_character_to_screen( keycode + USB_HID_CHAR_LBRC );
                break;
            case KC_RBRC:
                add_character_to_screen( keycode + USB_HID_CHAR_RBRC );
                break;
            case KC_SCLN:
                add_character_to_screen( keycode + USB_HID_CHAR_SCLN );
                break;
            case KC_QUOT:
                add_character_to_screen( keycode + USB_HID_CHAR_QUOT );
                break;
            case KC_NUHS:
                add_character_to_screen( keycode + USB_HID_CHAR_NUHS );
                break;
            case KC_COMM:
                add_character_to_screen( keycode + USB_HID_CHAR_COMM );
                break;
            case KC_DOT:
            case KC_SLSH:
                add_character_to_screen( keycode + USB_HID_CHAR_DOT );
                break;
        }
    }
    return true;
}

void oled_task_animation_matrix(void) {
    // Falling characters animation
    if ( characters_size > 0 ) {
        // Possibly rollover character_to_update
        if ( character_to_update >= characters_size ) {
            character_to_update = 0;
        }
        uint8_t moved_by = update_character( character_to_update );
        write_character_to_screen( character_to_update, moved_by );
        if ( ! remove_character_at( character_to_update ) ) {
            // When the character is removed the array is shuffled down
            // so we don't want to increment
            character_to_update += 1;
        }
    }
}

AnimationObserver* create_matrix_animation(void) {
    AnimationObserver* matrix = malloc(sizeof(AnimationObserver));
    if ( matrix != NULL ) {
        matrix->processRecordOled = process_record_animation_matrix;
        matrix->oledTaskUser = oled_task_animation_matrix;
        matrix->oledInitUser = NULL;
        matrix->keyboardPostInitUser = NULL;
    }
    return matrix;
}
