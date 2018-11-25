#include "iTeaHandler.h"
// Public
iTeaHandlerClass::iTeaHandlerClass() {

}

uint8_t iTeaHandlerClass::setup(uint8_t initState /* = AP_STATE_NONE*/) {
	_state = initState;
	return _state;
}

uint8_t iTeaHandlerClass::loop(void *params ...) {
	if (_states[_state] == NULL) {
		_state = _states[ITEA_STATE_ERROR](_state, params);
		return _state;
	}
	return call(_state, params);
}

uint8_t iTeaHandlerClass::call(uint8_t state, void *params ...) {
	// TODO buggy calling directly
	_state = _states[state](state, params);
	return _state;
}

void iTeaHandlerClass::add(uint8_t state, StateHandler f) {
	_states[state] = f;
}

void iTeaHandlerClass::setState(uint8_t state) {
	_state = state;
}
