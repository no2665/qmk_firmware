
typedef bool (*ProcessRecordOledNotification) (uint16_t keycode, keyrecord_t *record);
typedef oled_rotation_t (*OledInitUserNotification) (oled_rotation_t rotation);
typedef bool (*OledTaskUserNotification) (void);
typedef void (*KeyboardPostInitUserNotification) (void);

typedef struct {
    ProcessRecordOledNotification processRecordOled;
    OledInitUserNotification oledInitUser;
    OledTaskUserNotification oledTaskUser;
    KeyboardPostInitUserNotification keyboardPostInitUser;
} AnimationObserver;

//AnimationObserver* create_matrix_animation(void);
AnimationObserver* create_layer_icons_animation(void);
AnimationObserver* create_startup_animation(void);
AnimationObserver* create_stats_animation(void);

bool get_flag_can_render(void);
void set_flag_can_render(bool);
