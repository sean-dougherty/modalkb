#include <X11/keysym.h>

#include "util.h"
#include "x.h"

using namespace std;

Atom XA_NET_ACTIVE_WINDOW = nullatom;
Atom XA_WM_NAME = nullatom;
Atom XA_WM_CLASS = nullatom;

static int _IGNORE;
int *IGNORE = &_IGNORE;

Display *display = nullptr;
Window root = nullwin;

static function<Window ()> getInputFocus;
static function<void ()> updateState;
static function<void (Window)> destroy;

static void internAtoms();
static string getStringProperty(Window win, Atom atom);
static string getStringProperty_recurse(Window win, Atom atom);

void init(function<Window ()> _getInputFocus,
	  function<void ()> _updateState,
	  function<void (Window)> _destroy) {

    getInputFocus = _getInputFocus;
    updateState = _updateState;
    destroy = _destroy;

    display = XOpenDisplay(nullptr);
    errif(!display);

    root = XDefaultRootWindow(display);
    errif(!root);

    internAtoms();
}

Window getParent(Window win) {
    Window parent;
    unsigned int nchildren = 0;
    Window *children;
    XQueryTree(display, win, &root, &parent, &children, &nchildren);
    XFree(children);
    return parent;
}

Window findContainer(Window win) {
    while(win != nullwin) {
	Window parent = getParent(win);
	if(parent == root) {
	    return win;
	}
	win = parent;
    }
    return nullwin;
}

string getTitle(Window win) {
    return getStringProperty_recurse(win, XA_WM_NAME);
}

// WM_CLASS		8		+
string getClass(Window win) {
    return getStringProperty_recurse(win, XA_WM_CLASS);
}

void dumpAncestors(Window win) {
    prf("---> Ancestors of %lx", win);
    while(win) {
	win = getParent(win);
	prf("%lx", win);
    }
    prf("<---");
}

static void internAtoms() {
    XA_NET_ACTIVE_WINDOW = XInternAtom(display, "_NET_ACTIVE_WINDOW", true);
    errif(!XA_NET_ACTIVE_WINDOW)

    XA_WM_NAME = XInternAtom(display, "WM_NAME", true);
    errif(!XA_WM_NAME)

    XA_WM_CLASS = XInternAtom(display, "WM_CLASS", true);
    errif(!XA_WM_CLASS)
}

static string getStringProperty(Window win, Atom atom) {
    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long bytes;
    unsigned char *data;

    XGetWindowProperty(display,
		       win,
		       atom,
		       0,
		       ~(0L),
		       false,
		       AnyPropertyType,
		       &actual_type,
		       &actual_format,
		       &nitems,
		       &bytes,
		       &data);
    if(nitems == 0)
	return ""; // Property not defined for window.

    errif(actual_format != _8bit);

    string result((char *)data);
    xfree(data);

    return result;
}

static string getStringProperty_recurse(Window win, Atom atom) {
    while(win) {
	string value = getStringProperty(win, atom);
	if(!value.empty())
	    return value;
	win = getParent(win);
    }
    return "";
}

void nextEvent(XEvent *ev) {
    //db("Waiting on next event...");
    while(true) {
	errif( Success != XNextEvent(display, ev) );
	
	switch(ev->type) {
	case DestroyNotify: {
	    Window win = ev->xdestroywindow.window;
	    dbf("Destroy %lx", win);
	    destroy(win);
	} break;
	default:
	    return;
	}

    }
}

bool waitKey(int type, XEvent &ev) {
    return waitKey(type, 0, ev);
}

bool waitKey(int type, unsigned int keycode_required, XEvent &ev) {
    errif( (type != KeyPress) && (type != KeyRelease) );

    dbf("---\n--- wait KEYSTROKE %u\n---", keycode_required);
    int rc = XGrabKeyboard(display,
			   getInputFocus(),
			   false,
			   GrabModeAsync, // pointer_mode
			   GrabModeAsync, // keyboard_mode
			   CurrentTime);
    dbf("rc=%d", rc);

    const int SANITY_MAX = 20;
    int sanity = 0;
    bool success = false;

    while(!success && (++sanity < SANITY_MAX)) {
	nextEvent(&ev);

	dbf("ev.type=%d", ev.type);

	if(ev.type != type) {
	    dbf("Ignoring event type %d [sanity=%d]", ev.type, sanity);
	    continue;
	}


	db("Key{Press,Release} [Grabbed]");

	XKeyEvent &xkey = ev.xkey;
	KeySym keysym = XLookupKeysym(&xkey, 0);
	dbf("state=%x, keycode=%x, keysym=%lx",
	    xkey.state, xkey.keycode, keysym);

	switch(keysym) {
	case XK_Shift_L:
	case XK_Shift_R:
	case XK_Control_L:
	case XK_Control_R:
	case XK_Caps_Lock:
	case XK_Shift_Lock:
	case XK_Meta_L:
	case XK_Meta_R:
	case XK_Alt_L:
	case XK_Alt_R:
	case XK_Super_L:
	case XK_Super_R:
	case XK_Hyper_L:
	case XK_Hyper_R:
	    dbf("Modifier. Ignoring. [sanity=%d]", sanity);
	    break;
	default:
	    if(keycode_required == 0) {
		db("No accept filter. Accepting key.");
		success = true;
		break;
	    } else if(keycode_required == xkey.keycode) {
		db("Key matches required.");
		success = true;
		break;
	    }
	    break;
	}
    }

    dbf("Exited grab event loop, sanity=%d",
	sanity);

    rc = XUngrabKeyboard(display, CurrentTime);
    dbf("XUngrabKeyboard rc=%d", rc);

    updateState();

    return success;
}
