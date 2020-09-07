#include "define.h"

BOOL is_symbol(ALPHABET sigma, char _target) {

	for (SYMBOL* s_ptr = sigma; *s_ptr; s_ptr++) {
		if (*s_ptr == _target)
			return TRUE;
	}

	return FALSE;
}

TRANSITION* find_leftmost_transition_func(TRANSITION* target) {
	TRANSITION* curr;
	if (target->prevFunc == NULL) {
		return target;
	}
	else {
		for (curr = target->prevFunc; curr->prevFunc; curr = curr->prevFunc) {}
		return curr;
	}
}

TRANSITION* find_rightmost_transition_func(TRANSITION* target) {
	TRANSITION* curr;
	if (target->nextFunc == NULL) {
		return target;
	}
	else {
		for (curr = target->nextFunc;curr->nextFunc; curr = curr->nextFunc) {}
		return curr;
	}
}

TRANSITION*
new_transition_func
(
	STATE curr_state,
	STATE next_state,
	TRANSITION *prevFunc,
	TRANSITION *nextFunc,
	SYMBOL _symbol
)
{
	TRANSITION* new_func = (TRANSITION*)malloc(sizeof(TRANSITION));
	new_func->input_symbol = _symbol;
	new_func->current_state = curr_state;
	new_func->next_state = next_state;
	new_func->prevFunc = prevFunc;
	new_func->nextFunc = nextFunc;

	return new_func;
}