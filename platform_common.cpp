struct Button_state {
    bool is_down;
    bool changed;
};

enum {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_W,
    BUTTON_S,
    BUTTON_SPACE,

    BUTTON_COUNT, // Should be the last item
};

struct Input {
    Button_state buttons[BUTTON_COUNT];
};
