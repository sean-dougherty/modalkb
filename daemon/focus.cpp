#include "focus.h"
#include "util.h"

using namespace std;

FocusManager focusManager;

FocusContainer::FocusContainer() {
}

bool FocusContainer::operator==(const FocusContainer &other) const {
    return window == other.window
	&& title == other.title
	&& clazz == other.clazz;
}

FocusState::FocusState() {
}

FocusState FocusState::getInput() {
    FocusState result;
    if(display) {
	db("FocusState updating via XGetInputFocus");
	XGetInputFocus(display, &result.input, IGNORE);
	if(result.input) {
	    result.container.window = findContainer(result.input);
	    result.container.title = getTitle(result.input);
	    result.container.clazz = getClass(result.input);
	}
    }
    return result;
}

bool FocusState::operator==(const FocusState &other) const {
    return container == other.container
	&& input == other.input;
}

bool FocusState::operator!=(const FocusState &other) const {
    return !(*this == other);
}

FocusManager::FocusManager() {
}

void FocusManager::update() {
    FocusState oldState = state;
    FocusState newState = FocusState::getInput();

    if(newState == oldState) {
	db("no change in focus");
	return;
    }

    state = newState;

    if(!newState.input) {
	db("No input");
	return;
    }

    dbf("FocusState: input=%lx container=%lx title=%s clazz=%s",
	newState.input,
	newState.container.window,
	newState.container.title.c_str(),
	newState.container.clazz.c_str());
    dumpAncestors(newState.input);
}

void FocusManager::destroy(Window win) {
    if((win == state.input) || (win == state.container.window)) {
	db("Focus destroyed.");
	state = FocusState();
    }
}


