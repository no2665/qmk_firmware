#include QMK_KEYBOARD_H
#include "animation_observer.h"

uint16_t stats_timer = 0;
uint8_t wpm_high = 0;
#define STATS_UPDATES 1000

void render_wpm(void) {
    uint8_t wpm_i = get_current_wpm();
    if ( wpm_i > wpm_high ) {
        wpm_high = wpm_i;
    }
    char wpm_current_s[6];
    char wpm_high_s[6];
    sprintf( wpm_current_s, "W %3u", wpm_i );
    sprintf( wpm_high_s, "M %3u", wpm_high );
    oled_write( wpm_current_s, false );
    oled_write( "P:---", false );
    oled_write( wpm_high_s, false );
}

void render_rgb(void) {

}

void render_stats(void) {
    oled_set_cursor( 0, 6 );
    render_wpm();
}

bool oled_task_animation_stats(void) {
    uint16_t now = timer_read();
    if ( now - stats_timer > STATS_UPDATES ) {
        render_stats();
        stats_timer = now;
    }
    return false;
}

AnimationObserver* create_stats_animation(void) {
    AnimationObserver* stats = malloc(sizeof(AnimationObserver));
    if ( stats != NULL ) {
        stats->processRecordOled = NULL;
        stats->oledTaskUser = oled_task_animation_stats;
        stats->oledInitUser = NULL;
        stats->keyboardPostInitUser = NULL;
    }
    return stats;
}
