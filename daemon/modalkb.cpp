#include <string.h>

#include <X11/keysym.h>

#include <functional>
#include <list>
#include <map>

#include "action.h"
#include "focus.h"
#include "grab.h"
#include "key.h"
#include "keydef.h"
#include "mark.h"
#include "modalkb.h"
#include "mode.h"
#include "util.h"
#include "x.h"

using namespace std;

Window getInputFocus() {
    return focusManager.state.input;
}

bool isMarkActive() {
    return markManager.marked;
}
void toggleMark() {
    markManager.toggleMark();
}

void updateState() {
    focusManager.update();
    modeManager->update();
    grabManager.update();
    markManager.update();
}

void destroy(Window win) {
    focusManager.destroy(win);
    grabManager.destroy(win);
    markManager.destroy(win);
}

int errorHandler(Display *display, XErrorEvent *ev) {
    fprintf(stderr,
	    "X Error: error=%u major=%u xid=%lx serial=%ld\n",
	    (unsigned int)ev->error_code,
	    (unsigned int)ev->request_code,
	    ev->resourceid,
	    ev->serial);
    return 0;
}

Action *getAction(const XEvent &ev) {
    struct local {
	static Action *get(const map<Key, Action *> &hotkeys, const Key &key) {
	    map<Key, Action *>::const_iterator it = hotkeys.find(key);

	    return it == hotkeys.end()
		? nullptr
		: it->second;
	}
    };

    Key key(ev);

    for(auto keys: modeManager->getKeys(modeManager->actual)) {
	Action *result = local::get(*keys, key);
	if(result)
	    return result;
    }

    return nullptr;
}

int main(int argc, char **argv) {
    init(getInputFocus,
	 updateState,
	 destroy);

    makeModes();
    makeGlobalHotkeys();
    modeManager = new ModeManager([]() {return focusManager.state;});
    focusManager.update();
    modeManager->request(ModeDesc("Emacs"));
    grabManager.update();

    XSelectInput(display, root, PropertyChangeMask|SubstructureNotifyMask);

    XSetErrorHandler(errorHandler);

    XEvent ev;
    while(true) {
	nextEvent(&ev);

	switch(ev.type) {
	case KeyPress:
	case KeyRelease: {
	    if(ev.type == KeyPress) {
		db("KeyPress");
	    } else {
		db("KeyRelease");
	    }
	    
	    if(focusManager.state.input == nullwin) {
		warn("Null input");
	    } else {
		XKeyEvent &xkey = ev.xkey;
		dbf("state=%x, keycode=%x", xkey.state, xkey.keycode);

		Action *action = getAction(ev);

		if(action) {
		    action->execute(ev);
		} else {
		    warnf("CANNOT FIND KEY: state=%x keycode=%x", xkey.state, xkey.keycode);
		}
	    }
	} break;
	case PropertyNotify: {
	    XPropertyEvent xprop = ev.xproperty;
	    if(xprop.atom == XA_NET_ACTIVE_WINDOW) {
		db("NET_ACTIVE_WINDOW");

		updateState();
	    } else if(xprop.atom == XA_WM_NAME) {
		db("XA_WM_NAME");
	    } /*else {
		char *name = XGetAtomName(display, xprop.atom);
		dbf("PropertyNotify: %s", name);
		XFree(name);
		}*/
	} break;
	}
    }

    db("closing display....");
    XCloseDisplay(display);
}
