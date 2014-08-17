#include "action.h"
#include "grab.h"
#include "mode.h"
#include "util.h"
#include "x.h"

using namespace std;


// TODO get rid of these
extern Window getInputFocus();
extern bool isMarkActive();
extern void toggleMark();

// ---
// --- KeyMap
// ---
KeyMapAction::KeyMapAction(const char *substEncoding, bool _mark)
    : substSeq(Key::parseKeys(substEncoding))
    , mark(_mark) {
}

void KeyMapAction::execute(const XEvent &trigger) {
    switch(trigger.type) {
    case KeyPress: 
    case KeyRelease: {
	long event_mask = 
	    trigger.type == KeyPress ?
	    KeyPressMask
	    : KeyReleaseMask;

	for(auto &subst: substSeq) {
	    XEvent evSend = trigger;
	    XKeyEvent &xkey = evSend.xkey;

	    xkey.state = subst.modifiers;
	    if(mark && isMarkActive())
		xkey.state |= ShiftMask;
	    xkey.keycode = subst.keycode;
	    xkey.window = getInputFocus();
	    XSendEvent(display,
		       getInputFocus(),
		       true,
		       event_mask,
		       &evSend);
	}
    } break;
    default:
	warnf("Unexpected event: %d", trigger.type);
	break;
    }
}

// ---
// --- HotKeySequence
// ---
HotKeySequenceAction::HotKeySequenceAction(const std::map<std::string, class Action *> &actions) {
    for(auto &kv: actions) {
	vector<Key> keys = Key::parseKeys(kv.first);
	Action *action = kv.second;

	Node *node = &root;
	for(auto &key: keys) {
	    node = &(node->children[key]);
	}

	errif(node->action);
	errif(!node->children.empty());

	node->action = action;
    }
}

void HotKeySequenceAction::execute(const XEvent &trigger) {
    if(trigger.type != KeyPress)
	return;

    Node *node = &root;

    while(true) {
	XEvent ev;

	if(!waitKey(KeyPress, ev))
	    return;

	try {
	    node = &( node->children.at(Key(ev)) );
	} catch(...) {
	    db("Failed finding key in hotkey sequence");
	    break;
	}

	if(node->action) {
	    node->action->execute(ev);
	    break;
	}
    }

    db("Hotkey sequence done.");
}

// ---
// --- SuspendMode
// ---
SuspendModeAction::SuspendModeAction() {
}

void SuspendModeAction::execute(const XEvent &trigger) {
    if(trigger.type != KeyPress)
	return;

    XEvent ev;
    if(!waitKey(KeyPress, 0, ev)) {
	return;
    } else {
	XSendEvent(display,
		   getInputFocus(),
		   true,
		   KeyPressMask,
		   &ev);
    }

    if(!waitKey(KeyRelease, ev.xkey.keycode, ev)) {
	return;
    } else {
	XSendEvent(display,
		   getInputFocus(),
		   true,
		   KeyReleaseMask,
		   &ev);
    }
}

void ToggleModeMajorAction::execute(const XEvent &trigger) {
    if(trigger.type == KeyPress) {
	dbf("---\n--- TOGGLE MODE MAJOR\n---");
	modeManager->toggleMajor();
	grabManager.update();
    }
}

void ToggleModeMinorAction::execute(const XEvent &trigger) {
    if(trigger.type == KeyPress) {
	dbf("---\n--- TOGGLE MODE MINOR\n---");
	modeManager->toggleMinor();
	grabManager.update();
    }
}

Command::Command(const string &_cmd, bool _notify, bool _async)
    : cmd(_cmd), notify(_notify), async(_async) {
}

void Command::execute() {
    dbf("---\n--- COMMAND: %s\n---", cmd.c_str());
    if(notify) {
	::notify("System Command", cmd);
    }

    char cmdbuf[4096];
    sprintf(cmdbuf, "%s%s", cmd.c_str(), async ? " &" : "");

    int rc = system(cmdbuf);
    dbf("  system rc = %d", rc);
}

SystemAction::SystemAction(Command _cmd)
    : cmd(_cmd) {
}

void SystemAction::execute(const XEvent &trigger) {
    if(trigger.type == KeyPress) {
	cmd.execute();
    }
}

MarkAction::MarkAction(Type _type) : type(_type) {
}

void MarkAction::execute(const XEvent &trigger) {
    if(trigger.type == KeyPress) {
	switch(type) {
	case Toggle:
	    db("---\n--- TOGGLE MARK\n---");
	    toggleMark();
	    break;
	case Clear:
	    if(isMarkActive())
		toggleMark();
	    break;
	default:
	    errf("Invalid: %x", type);
	    break;
	}
    }
}

ChainAction::ChainAction(const vector<Action *> &_actions) : actions(_actions) {
}

void ChainAction::execute(const XEvent &trigger) {
    for(auto &x: actions)
	x->execute(trigger);
}
