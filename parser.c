// Edited by Nathan Waskiewicz

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 150
#define MAX_SYMBOL_COUNT 50
#define MAX_REG_HEIGHT 10

instruction *code;
int cIndex;
symbol *table;
int tIndex;
int level;
//added by Nate
lexeme *list;
int lIndex;
lexeme currentToken;
int error = 0;
int registerCounter = 0;

void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void mark();
int multipledeclarationcheck(char name[]);
int findsymbol(char name[], int kind);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();
//added by Nate
void block();
void expression();
void logic();

//epic code time
void getNextToken()
{
    currentToken = list[++lIndex];
    // printf("token: %d\n", currentToken.type);
}


void constDeclaration()
{
    if (currentToken.type == constsym)
    {
        do
        {
            getNextToken();
            if (currentToken.type == identsym)
            {
                if (multipledeclarationcheck(currentToken.name) == -1)
                {
                    getNextToken();
                    if (currentToken.type == assignsym)
                    {
                        getNextToken();
                        if (currentToken.type == numbersym)
                        {
                            addToSymbolTable(1, list[lIndex - 2].name, currentToken.value, level, 0, 0); // add const
                            getNextToken();
                            if (currentToken.type == semicolonsym)
                                getNextToken();
                            else if (currentToken.type == identsym)
                                printparseerror(13);
                            else if (currentToken.type != commasym)
                                printparseerror(14);
                        }
                        else
                            printparseerror(2);
                    }
                    else
                        printparseerror(2);
                }
                else
                    printparseerror(18);
            }
            else
                printparseerror(2);
        }
        while (currentToken.type == commasym);
    }

    return;
}


int varDeclaration()
{
    int numVars = 0;

    if (currentToken.type == varsym)
    {
        do
        {
            getNextToken();
            if (currentToken.type == identsym)
            {
                if (multipledeclarationcheck(currentToken.name) == -1)
                {
                    addToSymbolTable(2, currentToken.name, 0, level, numVars + 3, 0); //add var
                    numVars++;

                    getNextToken();
                    if (currentToken.type == semicolonsym)
                        getNextToken();
                    else if (currentToken.type == identsym)
                        printparseerror(13);
                    else if (currentToken.type != commasym)
                        printparseerror(14);
                }
                else
                    printparseerror(18);
            }
            else
                printparseerror(3);
        }
        while (currentToken.type == commasym);
    }

    return numVars;
}


void procedureDeclaration()
{
    while (currentToken.type == procsym)
    {
        getNextToken();
        if (currentToken.type == identsym)
        {
            if (multipledeclarationcheck(currentToken.name) == -1)
            {
                addToSymbolTable(3, currentToken.name, 0, level, 0, 0); //add procedure

                getNextToken();
                if (currentToken.type == semicolonsym)
                {
                    getNextToken();
                    block();
                    emit(2, 0, 0); //ret
                    if (currentToken.type == semicolonsym)
                        getNextToken();
                    else
                        printparseerror(14);
                }
                else
                    printparseerror(4);
            }
            else
                printparseerror(18);
        }
        else
            printparseerror(4);
    }

    return;
}


void factor()
{
    int constIndex;
    int varIndex;

    if (currentToken.type == identsym)
    {
        constIndex = findsymbol(currentToken.name, 1);
        varIndex = findsymbol(currentToken.name, 2);
        if (constIndex > -1) //constant found
        {
            if (varIndex > -1) //variable also found
            {
                if (table[constIndex].level >= table[varIndex].level) //const level larger
                {
                    if (registerCounter > 9)
                        printparseerror(20);
                    emit(1, 0, table[constIndex].val); //lit
                    registerCounter++;
                }
                else //variable level larger
                {
                    if (registerCounter > 9)
                        printparseerror(20);
                    emit(3, level - table[varIndex].level, table[varIndex].addr); //lod
                    registerCounter++;
                }
            }
            else //no variable, use constant
            {
                if (registerCounter > 9)
                    printparseerror(20);
                emit(1, 0, table[constIndex].val); //lit
                registerCounter++;
            }
        }
        else if (varIndex > -1) //no constant, use variable
        {
            if (registerCounter > 9)
                printparseerror(20);
            emit(3, level - table[varIndex].level, table[varIndex].addr); //lod
            registerCounter++;
        }
        else //error, neither found
        {
            if (findsymbol(currentToken.name, 3) > -1)
                printparseerror(11);
            else
                printparseerror(19);
        }
    }
    else if (currentToken.type == numbersym)
    {
        if (registerCounter > 9)
            printparseerror(20);
        emit(1, 0, currentToken.value); //lit
        registerCounter++;
    }
    else if (currentToken.type == lparensym)
    {
        getNextToken();
        expression();
        if (currentToken.type != rparensym)
            printparseerror(12);
    }
    else
        printparseerror(11);

    getNextToken();
    return;
}


void term()
{
    factor();
    while (currentToken.type == multsym || currentToken.type == divsym)
    {
        if (currentToken.type == multsym)
        {
            getNextToken();
            factor();
            emit(2, 0, 4); //mul
        }
        else
        {
            getNextToken();
            factor();
            emit(2, 0, 5); //div
        }
        registerCounter--;
    }

    return;
}


void expression()
{
    if (currentToken.type == minussym)
    {
        getNextToken();
        term();
        emit(2, 0, 1); //negate
        while (currentToken.type == plussym || currentToken.type == minussym)
        {
            if (currentToken.type == plussym)
            {
                getNextToken();
                term();
                emit(2, 0, 2); //add
                registerCounter--;
            }
            else
            {
                getNextToken();
                term();
                emit(2, 0, 3); //subtract
                registerCounter--;
            }
        }
    }
    else
    {
        if (currentToken.type == plussym)
        {
            getNextToken();
        }
        term();
        while (currentToken.type == plussym || currentToken.type == minussym)
        {
            if (currentToken.type == plussym)
            {
                getNextToken();
                term();
                emit(2, 0, 2); //add
                registerCounter--;
            }
            else
            {
                getNextToken();
                term();
                emit(2, 0, 3); //subtract
                registerCounter--;
            }
        }
    }

    switch (currentToken.type) //bad arithmetic check
    {
        case plussym:
        case minussym:
        case multsym:
        case divsym:
        case lparensym:
        case identsym:
        case numbersym:
            printparseerror(17);
    }

    return;
}


void condition()
{
    int prevType;

    // logic
    if (currentToken.type == lparensym)
    {
        getNextToken();
        logic();
        if (currentToken.type != rparensym)
            printparseerror(12);
        getNextToken();
    }
    // expression
    else
    {
        expression();
        prevType = currentToken.type;
        getNextToken();
        switch(prevType)
        {
            case eqlsym:
                expression();
                emit(2, 0, 6);
                registerCounter--;
                break;
            case neqsym:
                expression();
                emit(2, 0, 7);
                registerCounter--;
                break;
            case lsssym:
                expression();
                emit(2, 0, 8);
                registerCounter--;
                break;
            case leqsym:
                expression();
                emit(2, 0, 9);
                registerCounter--;
                break;
            case gtrsym:
                expression();
                emit(2, 0, 10);
                registerCounter--;
                break;
            case geqsym:
                expression();
                emit(2, 0, 11);
                registerCounter--;
                break;
            default:
                printparseerror(10);
        }
    }

    return;
}


void logic()
{
    if (currentToken.type == notsym)
    {
        // printf("this runs\n");
        getNextToken();
        condition();
        emit(2, 0, 14); //not
    }
    else
    {
        condition();
        while (currentToken.type == andsym || currentToken.type == orsym)
        {
            if (currentToken.type == andsym)
            {
                getNextToken();
                condition();
                emit(2, 0, 12); //and
                registerCounter--;
            }
            else
            {
                getNextToken();
                condition();
                emit(2, 0, 13); //orr
                registerCounter--;
            }
        }
    }
}


void statement()
{
    int symIndex;
    int jpcIndex;
    int jmpIndex;
    int loopIndex;

    if (currentToken.type == identsym)
    {
        symIndex = findsymbol(currentToken.name, 2);
        if (symIndex > -1)
        {
            getNextToken();
            if (currentToken.type = assignsym)
            {
                getNextToken();
                expression();
                emit(4, level - table[symIndex].level, table[symIndex].addr); //sto
                registerCounter--;
            }
            else
                printparseerror(5);
        }
        else if (currentToken.type != 2)
            printparseerror(6);
        else
            printparseerror(19);
    }
    else if (currentToken.type == callsym)
    {
        getNextToken();
        if (currentToken.type == identsym)
        {
            symIndex = findsymbol(currentToken.name, 3);
            if (symIndex > -1)
            {
                emit(5, level - table[symIndex].level, symIndex); //cal
                getNextToken();
            }
            else if (findsymbol(currentToken.name, 1) > -1) //check for const with this name
                printparseerror(7);
            else if (findsymbol(currentToken.name, 2) > -1) //check for var with this name
                printparseerror(7);
            else
                printparseerror(19); //nothing found
        }
        else
            printparseerror(7);
    }
    else if (currentToken.type == beginsym)
    {
        do
        {
            getNextToken();
            statement();
        }
        while (currentToken.type == semicolonsym);
        switch (currentToken.type)
        {
            case endsym:
                getNextToken();
                break;
            case identsym:
            case callsym:
            case beginsym:
            case ifsym:
            case whilesym:
            case readsym:
            case writesym:
            printparseerror(15);
                break;
            default:
                printparseerror(16);
        }
    }
    else if (currentToken.type == ifsym)
    {
        getNextToken();
        logic();

        if (currentToken.type == thensym)
        {
            getNextToken();
            jpcIndex = cIndex;
            emit(8, 0, 0); //jpc
            registerCounter--;
            statement();

            if (currentToken.type == elsesym) //there is an else statement
            {
                getNextToken();
                jmpIndex = cIndex;
                emit(7, 0, 0); //jmp
                code[jpcIndex].m = cIndex;
                statement();
                code[jmpIndex].m = cIndex;
            }
            else //there is no else statement
            {
                code[jpcIndex].m = cIndex;
            }
        }
        else
            printparseerror(8);
    }
    else if (currentToken.type == whilesym)
    {
        getNextToken();
        loopIndex = cIndex;
        logic();

        if (currentToken.type == dosym)
        {
            getNextToken();
            jpcIndex = cIndex;
            emit(8, 0, 0); //jpc
            statement();
            emit(7, 0, loopIndex); //jump
            code[jpcIndex].m = cIndex;
        }
        else
            printparseerror(9);
    }
    else if (currentToken.type == readsym)
    {
        getNextToken();
        if (currentToken.type == identsym)
        {
            symIndex = findsymbol(currentToken.name, 2);
            if (symIndex > -1)
            {
                if (registerCounter > 9)
                    printparseerror(20);
                emit(9, 0, 2); //read
                registerCounter++;
                emit(4, level - table[symIndex].level, table[symIndex].addr); //store
                registerCounter--;
            }
            else if (findsymbol(currentToken.name, 1) > -1) //trying to read a const
                printparseerror(6);
            else if (findsymbol(currentToken.name, 3) > -1) //trying to read a procedure
                printparseerror(6);
            else
                printparseerror(19);
        }
        else
            printparseerror(6);
    }
    else if (currentToken.type == writesym)
    {
        getNextToken();
        expression();
        emit(9, 0, 1); //write
    }

    return;
}


void block()
{
    int numVars; //number of variable spaced needed in AR
    int pIndex; //procedure index

    level++;
    pIndex = tIndex - 1;

    constDeclaration();
    numVars = varDeclaration();
    procedureDeclaration();

    table[pIndex].addr = cIndex; //fix procedure address
    emit(6, 0, numVars + 3); //increase
    statement();

    mark();
    level--;

    return;
}


instruction *parse(lexeme *inputList, int printTable, int printCode)
{
    int i;
	code = malloc(sizeof(instruction) * MAX_CODE_LENGTH);
	table = malloc(sizeof(symbol) * MAX_SYMBOL_COUNT);
	tIndex = 0;
	cIndex = 0;
    //added by Nate
    list = inputList;
    lIndex = 0;
    currentToken = list[0];

    //program
    emit(7, 0, 0);
    addToSymbolTable(3, "main", 0, 0, 0, 0);
    level = -1;
    block();

    if (currentToken.type != periodsym)
        printparseerror(1);
    emit(9, 0, 3); //halt
    code[0].m = table[0].addr;
    for (i = 0; i < cIndex; i++)
    {
        if (code[i].opcode == 5)
        {
            code[i].m = table[code[i].m].addr;
        }
    }
    //that's all my code ^
	if (printTable)
		printsymboltable();
	if (printCode)
		printassemblycode();

	code[cIndex].opcode = -1;
	return code;
}


// adds a line of code to the program
void emit(int opname, int level, int mvalue)
{
	code[cIndex].opcode = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

// add a symbol to the symbol table
void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].val = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}

// mark the symbols belonging to the current procedure, should be called at the end of block
void mark()
{
	int i;
	for (i = tIndex - 1; i >= 0; i--)
	{
		if (table[i].mark == 1)
			continue;
		if (table[i].level < level)
			return;
		table[i].mark = 1;
	}
}

// checks if a new symbol has a valid name, by checking if there's an existing symbol
// with the same name in the procedure
int multipledeclarationcheck(char name[])
{
	int i;
	for (i = 0; i < tIndex; i++)
		if (table[i].mark == 0 && table[i].level == level && strcmp(name, table[i].name) == 0)
			return i;
	return -1;
}

// returns the index of a symbol with a given name and kind in the symbol table
// returns -1 if not found
// prioritizes lower lex levels
int findsymbol(char name[], int kind)
{
	int i;
	int max_idx = -1;
	int max_lvl = -1;
	for (i = 0; i < tIndex; i++)
	{
		if (table[i].mark == 0 && table[i].kind == kind && strcmp(name, table[i].name) == 0)
		{
			if (max_idx == -1 || table[i].level > max_lvl)
			{
				max_idx = i;
				max_lvl = table[i].level;
			}
		}
	}
	return max_idx;
}

void printparseerror(int err_code)
{
    // printf("[ERROR CODE %d] ", err_code);

	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
			break;
		case 3:
			printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
			break;
		case 4:
			printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
			break;
		case 5:
			printf("Parser Error: Variables must be assigned using :=\n");
			break;
		case 6:
			printf("Parser Error: Only variables may be assigned to or read\n");
			break;
		case 7:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 8:
			printf("Parser Error: if must be followed by then\n");
			break;
		case 9:
			printf("Parser Error: while must be followed by do\n");
			break;
		case 10:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 11:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
			break;
		case 12:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 13:
			printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
			break;
		case 14:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 15:
			printf("Parser Error: statement within begin-end must be separated by a semicolon\n");
			break;
		case 16:
			printf("Parser Error: begin must be followed by end\n");
			break;
		case 17:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 18:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 19:
			printf("Parser Error: Undeclared identifier\n");
			break;
		case 20:
			printf("Parser Error: Register Overflow Error\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}

	free(code);
	free(table);
    exit(0);
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);

	free(table);
	table = NULL;
}

void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RET\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("EQL\t");
						break;
					case 7:
						printf("NEQ\t");
						break;
					case 8:
						printf("LSS\t");
						break;
					case 9:
						printf("LEQ\t");
						break;
					case 10:
						printf("GTR\t");
						break;
					case 11:
						printf("GEQ\t");
						break;
                    case 12:
                        printf("AND\t");
                        break;
                    case 13:
                        printf("ORR\t");
                        break;
                    case 14:
                        printf("NOT\t");
                        break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				switch (code[i].m)
				{
					case 1:
						printf("WRT\t");
						break;
					case 2:
						printf("RED\t");
						break;
					case 3:
						printf("HAL\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	if (table != NULL)
		free(table);
}
