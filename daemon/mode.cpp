#include <string.h>
#include "mode.h"
#include "util.h"

using namespace std;

std::map<std::string, class Mode *> modes;
class ModeManager *modeManager;

Mode::Mode(string _name,
	   const KeyActionMap &_hotkeys,
	   const map<string, KeyActionMap> &_min,
	   const vector<string> &_exclude,
	   const vector<string> &_include)
    : name(_name)
    , hotkeys(_hotkeys)
    , min(_min)
    , exclude(_exclude)
    , include(_include) {
}

ModeDesc::ModeDesc() {}
ModeDesc::ModeDesc(string _maj) : maj(_maj) {}
ModeDesc::ModeDesc(string _maj, string _min) : maj(_maj), min(_min) {}

bool ModeDesc::operator==(const ModeDesc &other) const {
    return this->maj == other.maj && this->min == other.min;
}

bool ModeDesc::operator!=(const ModeDesc &other) const {
    return !(*this == other);
}

ModeDesc::operator bool() const {
    return !maj.empty();
}

ModeManager::ModeManager(function<FocusState ()> _getFocus)
    : getFocus(_getFocus) {
    }

vector<KeyActionMap *> ModeManager::getKeys(const ModeDesc &desc) {
    Mode *mode = modes[desc.maj];
    return {&mode->min[desc.min], &mode->hotkeys, &globalHotkeys};
}

void ModeManager::toggleMajor() {
    ModeDesc oldMode = requested;
    ModeDesc newMode;

    if(oldMode.maj == "Standard")
	newMode.maj = "Emacs";
    else
	newMode.maj = "Standard";

    request(newMode);
}

void ModeManager::toggleMinor() {
    Mode *mode = modes[actual.maj];

    dbf("actual.min=%s", actual.min.c_str());

    map<string, KeyActionMap>::iterator it = mode->min.find(actual.min);
    it++;
    if(it == mode->min.end()) {
	it = mode->min.begin();
    }
    dbf("new min=%s", it->first.c_str());

    setMode(ModeDesc(actual.maj, it->first), "Minor mode change requested.");

    dbf("actual.min=%s", actual.min.c_str());
}

void ModeManager::request(const ModeDesc &desc) {
    dbf("---\n--- REQUEST MODE: %s/%s\n---", desc.maj.c_str(), desc.min.c_str());
    if(desc == requested) {
	db("Already requested mode. Ignoring.");
	return;
    }

    requested = desc;
    update(true);
}

void ModeManager::update(bool changeRequest) {
    ModeDesc Standard("Standard");

    FocusState focus = getFocus();
    if(!focus.input) {
	db("Fallback to Standard due to null input");
	setMode(Standard, "No input window.");
	return;
    }

    {
	ModeDesc included = getIncluded(focus);
	if(included) {
	    if(included.maj != actual.maj) {
		setMode(included, string("Window included for ") + included.maj);
	    }
	    return;
	}
    }

    bool excluded = isExcluded(focus, requested);

    if(excluded) {
	string desc = string("Window excluded from ") + requested.maj +" Mode.";
	if(actual != Standard) {
	    db("focus excluded from requested mode");
	    setMode(Standard, desc);
	} else if(changeRequest) {
	    notify(Standard.maj + " Mode", desc);
	}
    } else if(!excluded && (actual != requested)) {
	db("focus allowed for requested mode");
	string desc;
	if(changeRequest) {
	    desc = "Requested mode set.";
	} else {
	    desc = "Requested mode restored.";
	}
	setMode(requested, desc);
    } else if(changeRequest && (actual == requested)) {
	notify(requested.maj + " Mode", "Requested mode set.");
    }
}

bool ModeManager::isExcluded(const FocusState &focus, const ModeDesc &desc) {
    Mode *mode = modes[desc.maj];
    for(auto &pattern: mode->exclude) {
	if(strstr(focus.container.clazz.c_str(),
		  pattern.c_str())) {
	    return true;
	}
    }

    return false;
}

ModeDesc ModeManager::getIncluded(const FocusState &focus) {
    for(auto mode: modes) {
	for(auto &pattern: mode.second->include) {
	    if(strstr(focus.container.clazz.c_str(),
		      pattern.c_str())) {
		return ModeDesc(mode.second->name);
	    }
	}
    }

    return ModeDesc();
}

void ModeManager::setMode(const ModeDesc &newDesc, string msg) {
    errif(!newDesc);
    dbf("---\n--- SET MODE: %s\n---", newDesc.maj.c_str());
    actual = newDesc;

/*	
    if(!msg.empty()) {
	string title = newDesc.maj + " Mode";
	if(!newDesc.min.empty()) {
	    title += string(" (") + newDesc.min + ")";
	}
	notify(title, msg);
    }
*/
}

