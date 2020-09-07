#include "define.h"

static const STATE _start_state = 0;
static STATE _current_state = 1;
static TRANSITION* _t_head[BUFSIZE] = { NULL, };
ALPHABET _sigma;

void regex_extract_alphabets(char* regex) {
	
	char* ptr = regex;
	static SYMBOL alphabets[BUFSIZE] = { NULL, };
	BOOL isSame = FALSE;

	for (SYMBOL* s_ptr = alphabets ; *ptr; ptr++) {
		if (*ptr == '(' || *ptr == ')' || *ptr == '+' || *ptr == '*') {
			continue;
		}
		else {
			for (SYMBOL* inner_s_ptr = alphabets; *inner_s_ptr; inner_s_ptr++) {
				if (*inner_s_ptr != NULL && *inner_s_ptr == *ptr) {
					isSame = TRUE;
					break;
				}
				else {
					isSame = FALSE;
				}
			}
			if (!isSame) {
				*s_ptr++ = *ptr;
			}
		}
			
	}
	_sigma = alphabets;
}

BOOL regex_input_validation(char* regex) {
	
	short paren_counter = 0;
	char *ptr;

	for (ptr = regex; *ptr; ptr++) {
		if (*ptr == '(') {
			paren_counter++;
			if (*(ptr + 1) == ')')
				return FALSE;
		}
		else if (*ptr == ')') {
			paren_counter--;
			if (paren_counter < 0)
				return FALSE;
		}
		else if (*ptr == '+') {
			if (is_symbol(_sigma, *(ptr + 1)) == FALSE || *(ptr+1) == ')') {
				if (*(ptr + 1) == '(') {
					continue;
				}
				return FALSE; 
			}
		}
	}
	if (paren_counter != 0) {
		return FALSE;
	}

	
	
	return TRUE;
}

REGEX regex_insert_concat(STRING regex) {

	REGEX _c_regex = (REGEX)malloc(sizeof(char) * strlen(regex) * 2);
	char *ptr = _c_regex;
	
	for (; *regex; regex++) {
		*ptr++ = *regex;
		if (*(ptr-1) == '(') {
			continue;
		}
		if (*(regex + 1) == '+' || *(regex + 1) == '*' || *(regex+1) == ')') {
			continue;
		}
		if (*(ptr-1) == '+') {
			continue;
		}
		*ptr++ = '.';
	}
	*(ptr-1) = NULL;
	
	return _c_regex;
}

PRIORITY get_priority(char c) {
	switch (c) {
	case '*':
		return 3;
	case '.':
		return 2;
	case '+':
		return 1;
	default:
		return 0;
	}
}

POSTFIX_REGEX regex_to_postfix(REGEX _c_regex) {
	
	POSTFIX_REGEX _postfix_regex = (POSTFIX_REGEX)malloc(sizeof(char) * strlen(_c_regex) + 1);
	char* p_regex_ptr = _postfix_regex;
	char op_stack[BUFSIZE] = { NULL , };
	char el;
	STACKPTR top = 0; // top == 0 : stack empty

	for (; *_c_regex; _c_regex++) {
		if (is_symbol(_sigma, *_c_regex)) {
			*p_regex_ptr++ = *_c_regex;
			continue;
		}
		else {
			switch (*_c_regex)
			{

			case '(':
				op_stack[++top] = *_c_regex;
				break;
			case ')':
				while (op_stack[top] != '(') {
					*p_regex_ptr++ = op_stack[top];
					top--;
				}
				top--;
				break;

			default:
				while (top > 0) {
					el = op_stack[top];
					if (get_priority(el) >= get_priority(*_c_regex)) {
						*p_regex_ptr++ = op_stack[top--];
					}
					else
						break;
				}
				op_stack[++top] = *_c_regex;
				break;
			}
		}
	
	}
	while (top > 0) {
		*p_regex_ptr++ = op_stack[top--];
	}
	*p_regex_ptr = NULL;

	return _postfix_regex;
}

TRANSITION* nfa_symbol(SYMBOL _symbol) {

	TRANSITION *new_func;
	STATE curr_state, next_state;
	curr_state = _current_state++;
	next_state = _current_state++;

	new_func = new_transition_func(curr_state, next_state, NULL, NULL, _symbol);
	
	return new_func;
}

TRANSITION* nfa_concat(TRANSITION* prev, TRANSITION* next) {
	
	TRANSITION *new_func;
	prev = find_rightmost_transition_func(prev);

	new_func = new_transition_func(prev->next_state, next->current_state, prev, next, 'E');
	prev->nextFunc = new_func;
	next->prevFunc = new_func;

	return new_func;
}

void nfa_union(TRANSITION* prev, TRANSITION* next, TRANSITION* _t_stack[], short* p_top) {
	TRANSITION* new_func[4];
	
	prev = find_leftmost_transition_func(prev);
	next = find_leftmost_transition_func(next);

	new_func[0] = new_transition_func 
	(
		_current_state, 
		prev->current_state,
		NULL, NULL, 'E'
	);

	new_func[1] = new_transition_func
	(
		_current_state,
		next->current_state,
		NULL, NULL, 'E'
	);
	_current_state += 1;

	prev = find_rightmost_transition_func(prev);
	next = find_rightmost_transition_func(next);

	new_func[2] = new_transition_func
	(
		prev->next_state,
		_current_state,
		NULL, NULL, 'E'
	);

	new_func[3] = new_transition_func
	(
		next->next_state,
		_current_state,
		NULL, NULL, 'E'
	);
	_current_state += 1;

	for (int i = 0; i < 4; i++) {
		_t_stack[++(*p_top)] = new_func[i];
	}
}

TRANSITION* nfa_kleene(TRANSITION* curr, short* p_t_head_curr) {
	
	STATE initial_curr_state, final_curr_state, curr_state;
	TRANSITION *kleene_inner_func, *kleene_outer_func;
	TRANSITION *start_from_left, *end_to_right;

	initial_curr_state = find_leftmost_transition_func(curr)->current_state;
	final_curr_state = find_rightmost_transition_func(curr)->next_state;
	
	kleene_inner_func = new_transition_func
	(
		final_curr_state,
		initial_curr_state,
		NULL, NULL, 'E'
	);
	
	curr_state = _current_state;
	kleene_outer_func = new_transition_func
	(
		curr_state,
		curr_state+1,
		NULL, NULL, 'E'
	);
	_current_state += 2;

	start_from_left = new_transition_func
	(
		kleene_outer_func->current_state,
		find_leftmost_transition_func(curr)->current_state,
		NULL, NULL, 'E'
	);

	end_to_right = new_transition_func
	(
		find_rightmost_transition_func(curr)->next_state,
		kleene_outer_func->next_state,
		NULL, NULL, 'E'
	);

	_t_head[(*p_t_head_curr)++] = start_from_left;
	_t_head[(*p_t_head_curr)++] = kleene_outer_func;
	_t_head[(*p_t_head_curr)++] = kleene_inner_func;
	_t_head[(*p_t_head_curr)++] = end_to_right;

	return kleene_outer_func;
}

void postfix_to_nfa(POSTFIX_REGEX _p_regex) {
	
	TRANSITION *_t_stack[BUFSIZE];
	TRANSITION *prev, *next, *curr;
	TRANSITION *temp;

	STACKPTR top = 0; /* top == 0 : stack empty */
	short _t_head_curr = 1; 
	
	for (; *_p_regex; _p_regex++) {
		if (is_symbol(_sigma, *_p_regex)) {
			_t_stack[++top] = nfa_symbol(*_p_regex);
			_t_head[_t_head_curr++] = _t_stack[top];
			continue;
		}
		else {

			switch (*_p_regex)
			{
			case '.':
				next = _t_stack[top--];
				prev = _t_stack[top--];
				_t_stack[++top] = nfa_concat(prev, next);
				_t_head[_t_head_curr++] = _t_stack[top];
				_t_stack[top] = find_leftmost_transition_func(_t_stack[top]);
				break;

			case '+':
				next = _t_stack[top--];
				prev = _t_stack[top--];
				nfa_union(prev, next, _t_stack, &top);
				for (int cnt = 0; cnt < 4; cnt++) {
					_t_head[_t_head_curr++] = _t_stack[top - 3 + cnt];
				}

				temp = new_transition_func
				(
					_t_stack[top - 3]->current_state,
					_t_stack[top]->next_state,
					NULL, NULL, NULL
				);
				_t_stack[top - 3] = temp;
				top -= 3;
				break;

			case '*':
				curr = _t_stack[top--];
				_t_stack[++top] = nfa_kleene(curr, &_t_head_curr);
				break;

			default:
				break;
			}
		}
	}

	curr = _t_stack[top--];
	prev = new_transition_func
	(
		_start_state,
		curr->current_state,
		NULL, NULL, 'E'
	);
	next = new_transition_func
	(
		find_rightmost_transition_func(curr)->next_state,
		_current_state,
		NULL, NULL, 'E'
	);

	_t_head[0] = prev;
	_t_head[_t_head_curr] = next;

}

void _t_print_transition_function(TRANSITION* func) {
	printf("\tDelta(%d,%c) = %d\n",
		func->current_state,
		func->input_symbol,
		func->next_state);
}

void print_transition_function() {
	for (int i = 0; _t_head[i]; i++) {
		_t_print_transition_function(_t_head[i]);
	}
}

void print_result() {

	printf(" Sigma\t\t= { ");
	for (SYMBOL* ptr = _sigma; *ptr; ptr++) {
		printf("%c ", *ptr);
	}
	printf("}\n");
	printf(" Q\t\t= { ");
	for (int i = 0; i <= _current_state; i++) {
		printf("q%d ", i);
	}
	printf("}\n");

	printf(" Start-state\t= q0\n");
	printf(" Final-states\t= { q%d }\n", _current_state);

	printf(" Where delta is,\n {\n");
	print_transition_function();
	printf(" }\n");
}

void _t_free() {
	for (int i = 0; _t_head[i]; i++) {
		free(_t_head[i]);
	}
}

int main(int argc, char* argv[]) {

	REGEX regex_concat_inserted;
	POSTFIX_REGEX regex_postfix;

	if (argc < 2) {
		printf("Invalid Usage!\n");
		printf("1. Open Cmd(or Terminal)\n");
		printf("2. In Windows, \". /Automata.exe [regexp]\"\n");
		printf("3. In Unix,    \". /Automata [regexp]\"\n");
		return -1;
	}

	regex_extract_alphabets(argv[1]);
	if (!regex_input_validation(argv[1])) {
		printf("Invalid RE!\n");
		return -1;
	}
	printf(" Input RE : %s\n\n", argv[1]);

	regex_concat_inserted = regex_insert_concat(argv[1]);
	regex_postfix = regex_to_postfix(regex_concat_inserted);
	postfix_to_nfa(regex_postfix);
	print_result();

	free(regex_concat_inserted);
	free(regex_postfix);
	_t_free();

	return 0;
}