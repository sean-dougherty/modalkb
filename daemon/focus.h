#pragma once

#include <X11/Xlib.h>
#include <string>
#include "x.h"

class FocusContainer {
public:
    Window window = nullwin;
    std::string title;
    std::string clazz;

    FocusContainer();

    bool operator==(const FocusContainer &other) const;
};

class FocusState {
public:
    FocusContainer container;
    Window input = nullwin;

    FocusState();

    static FocusState getInput();
    bool operator==(const FocusState &other) const;
    bool operator!=(const FocusState &other) const;

};

class FocusManager {
public:
    FocusState state;

    FocusManager();

    void update();
    void destroy(Window win);
};

extern FocusManager focusManager;
