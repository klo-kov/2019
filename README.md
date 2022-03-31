# Regular Expression to E-NFA (By Thompson's Construction)

## How to execute

- Clone or download.
- Execute Terminal.
- make and run.
  - example : ./Automata ab(a+b)*ab

## Flow of program

- Get input (regular expression) from user (argv[1]).
- Extract alphabets from input and check that the regular expression is correct or not.
   - If not correct, terminate.
- Insert additional symbol that represents concatenation operator to original regular expression.
- Convert concat-added regular expression to posfitx expression.
- Create an NFA with postfix expression and print E-NFA.
