#include <dlfcn.h>
#include <X11/X.h>
#include <X11/Xlib.h>

#include "util.h"

extern "C" {
int XNextEvent(Display *display, XEvent *ev) {
    typedef int (*XNextEvent_t)(Display *, XEvent *);
    static XNextEvent_t X11_XNextEvent = NULL;
    static int n = 0;

    if(!X11_XNextEvent) {
	db("libsendev.so: loading XNextEvent from X11.so");
	void *handle = dlopen("libX11.so", RTLD_LAZY);
	errif(!handle);

	X11_XNextEvent = (XNextEvent_t)dlsym(handle, "XNextEvent");
	if(dlerror()) {
	    errf("Failed loading XNextEvent. dlerror=%s", dlerror());
	}
    }

    int rc = X11_XNextEvent(display, ev);
    if(ev->type == KeyPress) {
	dbf("KeyPress: state=%u keycode=%u ", ev->xkey.state, ev->xkey.keycode);
    } else if(ev->type == GenericEvent) {
	dbf("GenericEvent: type=%d ext=%d (%x) length=%d", ev->xgeneric.evtype, ev->xgeneric.extension, ev->xgeneric.extension, ev->xgeneric.length);
    } else {
	dbf("[%d] event type: %d", n++, ev->type);
    }
    ev->xany.send_event = false;
    return rc;
}
}
