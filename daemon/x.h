#pragma once

#include <X11/Xlib.h>

#include <functional>
#include <string>

#define nullwin (Window)0
#define nullatom (Atom)0
#define _8bit 8

extern Atom XA_NET_ACTIVE_WINDOW;
extern Atom XA_WM_NAME;
extern Atom XA_WM_CLASS;

extern int *IGNORE;

extern Display *display;
extern Window root;

template<typename T>
void xfree(T *&ptr) {
    if(!ptr) return;

    XFree((void *)ptr);
    ptr = nullptr;
}

void init(std::function<Window ()> getInputFocus,
	  std::function<void ()> updateState,
	  std::function<void (Window)> destroy);

Window getParent(Window win);
Window findContainer(Window win);
std::string getTitle(Window win);
std::string getClass(Window win);

void nextEvent(XEvent *ev);
bool waitKey(int type, XEvent &ev);
bool waitKey(int type, unsigned int keycode_required, XEvent &ev);

void dumpAncestors(Window win);

