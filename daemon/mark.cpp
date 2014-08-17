#include "mark.h"
#include "util.h"

using namespace std;

MarkManager markManager;

MarkState::MarkState() {
}

MarkState::MarkState(const FocusState &focus, bool _marked)
    : container(focus.container.window)
    , input(focus.input)
    , marked(_marked) {
}

bool MarkManager::findFocus(function<void (MarkState &)> action) {
    const FocusState &focus = focusManager.state;

    for(auto &state: states) {
	if((state.container == focus.container.window)
	   && (state.input == focus.input)) {

	    action(state);
	    return true;
	}
    }

    return false;
}

void MarkManager::update() {
    if(findFocus([this](MarkState &state) {marked = state.marked;})) {
	return;
    } else {
	marked = false;
    }
}

void MarkManager::toggleMark() {
    if(findFocus([this](MarkState &state) {marked = (state.marked = !state.marked);})) {
	return;
    } else {
	marked = false;
    }

    const FocusState &focus = focusManager.state;

    dbf("created MarkState for {%lx,%lx}", focus.container.window, focus.input);
    states.push_back(MarkState(focus, true));
    marked = true;
}

void MarkManager::destroy(Window win) {
    dbf("MarkManager::destroy(%lx), nstate=%lu", win, states.size());

    states.remove_if([win](const MarkState &state) {
	    return state.container == win || state.input == win;
	});

    dbf("  nstate=%lu", states.size());
}



