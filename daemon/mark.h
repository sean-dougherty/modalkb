#pragma once

#include <X11/Xlib.h>
#include <functional>
#include <list>
#include "focus.h"

class MarkState {
public:
    Window container = nullwin;
    Window input = nullwin;
    bool marked = false;

    MarkState();
    MarkState(const FocusState &focus, bool _marked);
};

class MarkManager {
    bool findFocus(std::function<void (MarkState &)> action);

public:
    std::list<MarkState> states;
    bool marked = false;

    void update();
    void toggleMark();
    void destroy(Window win);
};

extern MarkManager markManager;
