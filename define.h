#ifndef __DEFINE_DATA_STRUCTURE_H__
#define __DEFINE_DATA_STRUCTURE_H__
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FALSE 0
#define TRUE 1
#define BUFSIZE 100

typedef short STACKPTR;
typedef short STATE;
typedef short BOOL;
typedef short PRIORITY;
typedef char SYMBOL;
typedef char* STRING;
typedef STRING REGEX;
typedef STRING POSTFIX_REGEX;
typedef SYMBOL* ALPHABET;

BOOL is_symbol(ALPHABET sigma, char _target);

typedef struct _Transition {
	SYMBOL input_symbol;
	STATE current_state;
	STATE next_state;

	struct _Transition *prevFunc, *nextFunc;

}TRANSITION;


STATE search_minimum_state(TRANSITION*);
STATE search_maximum_state(TRANSITION*);
TRANSITION* find_leftmost_transition_func(TRANSITION*);
TRANSITION* find_rightmost_transition_func(TRANSITION*);
TRANSITION*
new_transition_func
(
	STATE curr_state,
	STATE next_state, 
	TRANSITION* prevFunc, 
	TRANSITION* nextFunc,
	SYMBOL _symbol
);


#endif