#pragma once

#include <X11/Xlib.h>

#include <map>
#include <string>
#include <vector>

class Key {
public:
    static std::vector<Key> parseKeys(std::string encoding);

    int keycode;
    unsigned int modifiers;

    Key(const char *key);
    Key(unsigned int modifiers, int keycode);
    Key(const XEvent &ev);

    bool operator<(const Key &b) const;
    bool operator==(const Key &b) const;
};

typedef std::map<Key, class Action *> KeyActionMap;

extern KeyActionMap globalHotkeys;
