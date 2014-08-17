#pragma once

#include <X11/Xlib.h>
#include <map>
#include "focus.h"
#include "key.h"
#include "mode.h"

class GrabManager {
public:
    ModeDesc mode;
    FocusState focus;

    void update();
    void destroy(Window win);
};

extern GrabManager grabManager;
