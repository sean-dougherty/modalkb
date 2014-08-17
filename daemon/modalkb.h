#pragma once

#include <X11/Xlib.h>

#include <string>

// ---
// --- Functions
// ---
Window getInputFocus();
bool isMarkActive();
void nextEvent(XEvent *ev);
void updateState();
void notify(const std::string &title, const std::string &message);

