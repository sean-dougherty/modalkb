#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>
#include "focus.h"
#include "key.h"

extern std::map<std::string, class Mode *> modes;
extern class ModeManager *modeManager;

class Mode {
public:
    std::string name;
    KeyActionMap hotkeys;
    std::map<std::string, KeyActionMap> min;
    std::vector<std::string> exclude;
    std::vector<std::string> include;

    Mode(std::string _name,
	 const KeyActionMap &_hotkeys,
	 const std::map<std::string, KeyActionMap> &_min,
	 const std::vector<std::string> &_exclude,
	 const std::vector<std::string> &_include);
};

class ModeDesc {
public:
    std::string maj;
    std::string min;

    ModeDesc();
    ModeDesc(std::string maj);
    ModeDesc(std::string maj, std::string min);

    bool operator==(const ModeDesc &other) const;
    bool operator!=(const ModeDesc &other) const;
    operator bool() const;
};

class ModeManager {
public:
    ModeDesc requested;
    ModeDesc actual;

    std::function<FocusState ()> getFocus;
    
    ModeManager(std::function<FocusState ()> _getFocus);

    std::vector<KeyActionMap *> getKeys(const ModeDesc &desc);

    void toggleMajor();
    void toggleMinor();
    void request(const ModeDesc &desc);
    void update(bool changeRequest = false);

private:
    bool isExcluded(const FocusState &focus, const ModeDesc &desc);
    ModeDesc getIncluded(const FocusState &focus);
    void setMode(const ModeDesc &newDesc, std::string msg);

};
