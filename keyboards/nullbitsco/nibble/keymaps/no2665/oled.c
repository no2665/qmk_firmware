#include QMK_KEYBOARD_H
#include OLED_FONT_H
#include "oled_animations/animation_observer.h"

typedef struct AnimationNode {
    AnimationObserver* item;
    struct AnimationNode* next;
} AnimationNode_t;

static AnimationNode_t observers;

bool flag_can_render = false;

void set_flag_can_render(bool b) {
    flag_can_render = b;
}

bool get_flag_can_render(void) {
    return flag_can_render;
}

/**
 * Output to screen
 */
bool oled_task_user(void) {
    AnimationNode_t* node = &observers;
    while ( node != NULL && node->item != NULL ) {
        if ( node->item->oledTaskUser != NULL ) {
            bool stop = node->item->oledTaskUser();
            if ( stop ) {
                return false;
            }
        }
        node = node->next;
    }
    return false;
}

/**
 * Needed as the nibble screen is rotated.
 */
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    AnimationNode_t* node = &observers;
    while ( node != NULL && node->item != NULL ) {
        if ( node->item->oledInitUser != NULL ) {
            node->item->oledInitUser(rotation);
        }
        node = node->next;
    }
    return OLED_ROTATION_90;
}

/**
 * Called on every key event
 */
bool process_record_oled(uint16_t keycode, keyrecord_t *record) {
    AnimationNode_t* node = &observers;
    while ( node != NULL && node->item != NULL ) {
        if ( node->item->processRecordOled != NULL ) {
            node->item->processRecordOled(keycode, record);
        }
        node = node->next;
    }
    return true;
}

void keyboard_post_init_oled(void) {
    AnimationNode_t* node = &observers;

    AnimationObserver* startup = create_startup_animation();
    node->item = startup;

    /*AnimationObserver* matrix = create_matrix_animation();
    AnimationNode_t* mNode = (AnimationNode_t*) malloc(sizeof(AnimationNode_t));
    mNode->item = matrix;
    mNode->next = NULL;*/

    AnimationObserver* stats = create_stats_animation();
    AnimationNode_t* statsNode = (AnimationNode_t*) malloc(sizeof(AnimationNode_t));
    statsNode->item = stats;
    statsNode->next = NULL;

    AnimationObserver* layerIcons = create_layer_icons_animation();
    AnimationNode_t* liNode = (AnimationNode_t*) malloc(sizeof(AnimationNode_t));
    liNode->item = layerIcons;
    liNode->next = statsNode;

    node->next = liNode;

    while ( node != NULL && node->item != NULL ) {
        if ( node->item->keyboardPostInitUser != NULL ) {
            node->item->keyboardPostInitUser();
        }
        node = node->next;
    }
}
