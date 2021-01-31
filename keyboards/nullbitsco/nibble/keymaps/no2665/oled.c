#include QMK_KEYBOARD_H
#include OLED_FONT_H
#include "oled_animations/animation_observer.h"

typedef struct AnimationNode {
    AnimationObserver* item;
    struct AnimationNode* next;
} AnimationNode_t;

static AnimationNode_t observers;

/**
 * Output to screen
 */
void oled_task_user(void) {
    AnimationNode_t* node = &observers;
    while ( node != NULL && node->item != NULL ) {
        if ( node->item->oledTaskUser != NULL ) {
            node->item->oledTaskUser();
        }
        node = node->next;
    }
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

    AnimationObserver* matrix = create_matrix_animation();
    node->item = matrix;

    AnimationObserver* layerIcons = create_layer_icons_animation();

    AnimationNode_t* liNode = (AnimationNode_t*) malloc(sizeof(AnimationNode_t));
    liNode->item = layerIcons;
    liNode->next = NULL;

    node->next = liNode;

    while ( node != NULL && node->item != NULL ) {
        if ( node->item->keyboardPostInitUser != NULL ) {
            node->item->keyboardPostInitUser();
        }
        node = node->next;
    }
}
