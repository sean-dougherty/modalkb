#include "grab.h"
#include "mode.h"
#include "util.h"
#include "x.h"

using namespace std;

GrabManager grabManager;

static void grabWindow(Window win, const KeyActionMap &hotkeys, const char *reason);
static void ungrabWindow(Window win, const KeyActionMap &hotkeys, const char *reason);

void GrabManager::update() {
    db("---\n--- GRAB\n---");
    ModeDesc oldMode = mode;
    FocusState oldFocus = focus;
    ModeDesc newMode = modeManager->actual;
    FocusState newFocus = focusManager.state;

    if((oldMode != newMode) || (oldFocus != newFocus)) {
	dbf("oldmode=%s oldfocus=%lx",
	    oldMode.maj.c_str(),
	    oldFocus.input);
	if(oldMode && oldFocus.input) {
	    for(auto keys: modeManager->getKeys(oldMode)) {
		ungrabWindow(oldFocus.input, *keys, "");
	    }
	}

	if(newMode && newFocus.input) {
	    dbf("new grab mode=%s", newMode.maj.c_str());
	    for(auto keys: modeManager->getKeys(newMode)) {
		grabWindow(newFocus.input, *keys, "");
	    }
	}

	mode = newMode;
	focus = newFocus;
    }
}

void GrabManager::destroy(Window win) {
    if((win == focus.input) || (win == focus.container.window)) {
	db("Grab focus destroyed.");
	focus = FocusState();
    }
}

void grabWindow(Window win, const map<Key, Action *> &hotkeys, const char *reason) {
    dbf("grabWindow %lx (%s)", win, reason);
    for(auto &kv: hotkeys) {
	const Key &key = kv.first;

	dbf("  mods=%x keycode=%x", key.modifiers, key.keycode);

	XGrabKey(display,
		 key.keycode,
		 key.modifiers,
		 win,
		 true,
		 GrabModeAsync,
		 GrabModeAsync);
    }
}

void ungrabWindow(Window win, const map<Key, Action *> &hotkeys, const char *reason) {
    dbf("UNgrabWindow %lx (%s)", win, reason);
    for(auto &kv: hotkeys) {
	const Key &key = kv.first;

	dbf("  mods=%x keycode=%x", key.modifiers, key.keycode);

	XUngrabKey(display,
		   key.keycode,
		   key.modifiers,
		   win);
    }
}
