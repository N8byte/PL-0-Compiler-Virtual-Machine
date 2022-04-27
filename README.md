# PL-0-Compiler-Virtual-Machine
Compiler and virtual machine for the PL/0 programming language written in C.

To compile use command:
gcc driver.c vm.c lex.c parser.c -lm

To run use ./a.out [input file]
where input file is PL/0 source file.

Output flags are:

-l  the lexeme list should be printed by “lex.c”

-s  the symbol table should be printed by “parser.c”

-a  the generated assembly code should be printed by “parser.c”

-v  the program execution trace should be printed by “vm.c”
