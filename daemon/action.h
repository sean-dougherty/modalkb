#pragma once

#include <X11/Xlib.h>
#include <string>
#include <vector>
#include "key.h"

// ---
// --- Action
// ---
class Action {
public:
    virtual void execute(const XEvent &trigger) = 0;
};

// ---
// --- KeyMap
// ---
class KeyMapAction : public Action {
    std::vector<Key> substSeq;
    bool mark;
public:
    KeyMapAction(const char *substEncoding, bool _mark);

    virtual void execute(const XEvent &trigger);
};

// ---
// --- HotKeySequence
// ---
class HotKeySequenceAction : public Action {
public:
    HotKeySequenceAction(const std::map<std::string, class Action *> &actions);

    virtual void execute(const XEvent &trigger);

private:
    class Node {
    public:
	Action *action = nullptr;
	std::map<Key, Node> children;
    } root;
};

// ---
// --- SuspendMode
// ---
class SuspendModeAction : public Action {
public:
    SuspendModeAction();

    virtual void execute(const XEvent &trigger);
};

// ---
// --- ToggleModeMajor
// ---
class ToggleModeMajorAction : public Action {
public:
    virtual void execute(const XEvent &trigger);
};

// ---
// --- ToggleModeMinor
// ---
class ToggleModeMinorAction : public Action {
public:
    virtual void execute(const XEvent &trigger);
};

// ---
// --- Command
// ---
class Command {
public:
    std::string cmd;
    bool notify;
    bool async;

    Command(const std::string &_cmd, bool _notify = true, bool _async = true);

    void execute();
};

// ---
// --- System
// ---
class SystemAction : public Action {
    Command cmd;
public:
    SystemAction(Command _cmd);

    virtual void execute(const XEvent &trigger);
};

// ---
// --- Mark
// ---
class MarkAction : public Action {
public:
    enum Type { Toggle, Clear };
    Type type;

    MarkAction(Type _type);

    virtual void execute(const XEvent &trigger);
};

// ---
// --- Chain
// ---
class ChainAction : public Action {
    std::vector<Action *> actions;
public:
    ChainAction(const std::vector<Action *> &_actions);

    virtual void execute(const XEvent &trigger);
};
