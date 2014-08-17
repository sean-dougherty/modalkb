#include <map>
#include <string>
#include <utility>

#include "action.h"
#include "key.h"
#include "mode.h"

using namespace std;

static pair<Key, Action *> hotkey(const char *hotkey, Action *action);
static pair<Key, Action *> keymap(const char *hotkey, const char *subst, bool mark = false);
static Action *sys(const string &cmd,
		   bool notify = false,
		   bool async = true);
static Action *seq(const map<string, Action *> &actions);


void makeGlobalHotkeys() {
    globalHotkeys = {
	{ "CM-k",	seq({
				{ "a",		new ToggleModeMajorAction() },
				{ "i",		new ToggleModeMinorAction() }
		}) },
	{ "CM-g",	sys("guake-toggle") },
	{ "CM-w",	sys("tor-browser", true) },
	{ "CM-r",	sys("xfrun4") },
	{ "CM-t",	sys("xfce4-terminal --working-directory=$HOME") },
	{ "CM-l",	seq({
				{ "e",		sys("emacs23 --no-splash", true) },
				{ "w",		sys("firefox", true) },
				{ "l",		sys("leafpad") },
				{ "f",		sys("thunar $HOME") },
				{ "c",		sys("gnome-calculator") },
				{ "s",		sys("gksudo synaptic") },
				{ "o w",	sys("libreoffice --writer") },
				{ "o c",	sys("libreoffice --calc") }
		}) },
#define SCHOOL "thunar /home/sean/Desktop/school/"
#define FALL SCHOOL "Fall2013/"
	{ "CM-s",	seq({
				{ "p",		sys(FALL "physics") },
				{ "c",		sys(FALL "cc") },
				{ "h",		sys(FALL "honors") },
				{ "t",		sys(FALL "tutor") },
				{ "b",		sys(FALL "bio") },
				{ "i",		sys(SCHOOL "bioinfo") }
		}) }
    };
}

void makeModes() {
    // ---
    // --- EMACS
    // ---
    KeyActionMap emacsKeys = {
	keymap( "C-b",		"Left",		true ),
	keymap( "C-f",		"Right",	true ),
	keymap( "C-p",		"Up",		true ),
	keymap( "C-n",		"Down",		true ),
	keymap( "M-b",		"C-Left",	true ),
	keymap( "M-f",		"C-Right",	true ),
	keymap( "C-a",		"Home",		true ),
	keymap( "C-e",		"End",		true ),
	keymap( "MS-comma",	"C-Home",	true ),
	keymap( "MS-period",	"C-End",	true ),
	keymap( "C-v",		"Page_Down",	true ),
	keymap( "M-v",		"Page_Up",	true ),

	keymap( "M-comma",	"M-Left"),
	keymap( "M-period",	"M-Right"),

	keymap( "CS-minus",	"C-z" ),
	keymap( "C-d",		"Delete" ),
	keymap( "M-d",		"CS-Right Delete" ),

	keymap( "C-k",		"S-End C-X"),

	hotkey( "C-space",	new MarkAction(MarkAction::Toggle) ),
	hotkey( "M-w",		new ChainAction({
					new KeyMapAction("C-c", false), 
					new MarkAction(MarkAction::Clear)}) ),
	hotkey( "C-w",		new ChainAction({
					new KeyMapAction("C-x", false), 
					new MarkAction(MarkAction::Clear)}) ),
	keymap( "C-w",		"C-x" ),
	keymap( "C-y",		"C-v" ),

	hotkey( "C-x",		new SuspendModeAction() ),

	keymap( "C-s",		"C-f" ),
	hotkey( "C-g",		new ChainAction({
					new KeyMapAction("Escape", false), 
					new MarkAction(MarkAction::Clear)}) )
    };

    // ---
    // --- FIREFOX
    // ---
    KeyActionMap firefoxKeys = emacsKeys;
    firefoxKeys.insert( keymap("C-m", "F8") );
    firefoxKeys.insert( keymap("M-comma", "M-Left") );
    firefoxKeys.insert( keymap("M-period", "M-Right") );

    map<string, KeyActionMap> firefoxMinor {
	{"", {}},
	{"Nav",
	    {
		keymap("comma", "M-Left"),
		keymap("period", "M-Right"),
		keymap("i", "Home"),
		keymap("o", "End"),
		keymap("n", "Down"),
		keymap("p", "Up")
	    }
	}
    };

    // ---
    // --- Modes
    // ---
    map<string, KeyActionMap> EmptyMinor {
	{"", {}}
    };

    modes = {
	{"Standard",
	 new Mode("Standard", {}, EmptyMinor, {}, {})},
	{"Emacs",
	 new Mode("Emacs", emacsKeys, EmptyMinor, {"terminal", "guake", "emacs", "evince"}, {})},
	{"Firefox",
	 new Mode("Firefox", firefoxKeys, firefoxMinor, {}, {"Navigator"})}
    };
}

static pair<Key, Action *> hotkey(const char *hotkey, Action *action) {
    return { Key(hotkey), action };
}

static pair<Key, Action *> keymap(const char *hotkey, const char *subst, bool mark) {
    return ::hotkey(hotkey, new KeyMapAction(subst, mark));
}

static Action *sys(const string &cmd,
	   bool notify,
	   bool async) {
    return new SystemAction(Command(cmd, notify, async));
}

static Action *seq(const map<string, Action *> &actions) {
    return new HotKeySequenceAction(actions);
}
