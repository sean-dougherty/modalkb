#include <string.h>

#include "key.h"
#include "util.h"
#include "x.h"

using namespace std;

KeyActionMap globalHotkeys;

static void parseKey(const char *encoding, unsigned int &modifiers, int &keycode);
static int compare(const Key &a, const Key &b);


vector<Key> Key::parseKeys(string encoding) {
    vector<Key> result;

    vector<string> tokens = split(encoding);
    for(auto tok: tokens) {
	result.push_back( Key(tok.c_str()) );
    }

    return result;
}

Key::Key(const char *key) {
    parseKey(key, this->modifiers, this->keycode);
}

Key::Key(unsigned int modifiers, int keycode) {
    this->modifiers = modifiers;
    this->keycode = keycode;
}

Key::Key(const XEvent &ev) {
    errif( (ev.type != KeyPress) && (ev.type != KeyRelease) );

    this->modifiers = ev.xkey.state;
    this->keycode = ev.xkey.keycode;
}

bool Key::operator<(const Key &b) const {
    return compare(*this, b) < 0;
}

bool Key::operator==(const Key &b) const {
    return compare(*this, b) == 0;
}


static void parseKey(const char *encoding, unsigned int &modifiers, int &keycode) {
    const char *dash = strchr(encoding, '-');
    const char *keyStart;

    modifiers = 0;

    if(dash) {
	keyStart = dash + 1;
	for(const char *c = encoding; c != dash; c++) {
	    switch(*c) {
	    case 'C':
		modifiers |= ControlMask;
		break;
	    case 'M':
		modifiers |= Mod1Mask;
		break;
	    case 'S':
		modifiers |= ShiftMask;
		break;
	    default:
		errf("encoding=%s, c=%c", encoding, *c);
		break;
	    }
	}
    } else {
	keyStart = encoding;
    }

    KeySym keysym = XStringToKeysym(keyStart);
    keycode = XKeysymToKeycode(display, keysym);

    //dbf("%s --> %u %u", encoding, modifiers, keycode);
}

static int compare(const Key &a, const Key &b) {
    int cmp = a.keycode - b.keycode;
    if(cmp == 0) {
	cmp = a.modifiers - b.modifiers;
    }
    return cmp;
}
