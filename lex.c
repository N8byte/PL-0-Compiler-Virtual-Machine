/*
Edited by Nathan Waskiewicz
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#define MAX_NUMBER_TOKENS 1000
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5

lexeme *list;
int lex_index;

void printlexerror(int type);
void printtokens();


lexeme *lexanalyzer(char *input, int printFlag)
{
    int i, j;
    char *word;

    word = malloc(sizeof(char) * 12);

    list = malloc(sizeof(lexeme) * MAX_NUMBER_TOKENS);
	lex_index = 0;


    for (i = 0; input[i] != '\0'; i++)
    {
        //skip over comments
        if (input[i] == '/' && input[i + 1] == '*')
        {
            i+=2;
            while (1)
            {
                if (input[i] == '\0')
                {
                    printlexerror(5);
                    return NULL;
                }
                if (input[i] == '*' && input[i + 1] == '/')
                    break;
                i++;
            }
            i+=2;
        }

        //found an identifier
        if (isalpha(input[i]))
        {
            word[0] = input[i];

            //check if it's valid
            for (j = 1; isalnum(input[i + j]); j++)
            {
                if (j > 11)
                {
                    printlexerror(3);
                    return NULL;
                }

                word[j] = input[i + j];
            }
            word[j] = '\0';

            //check if it's a reserved word
            if (strcmp(word, "const") == 0)
            {
                list[lex_index].type = constsym;
                strcpy(list[lex_index].name, word);
                lex_index++;
            }
            else if (strcmp(word, "var") == 0)
            {
                list[lex_index].type = varsym;
                strcpy(list[lex_index].name, word);
                lex_index++;
            }
            else if (strcmp(word, "procedure") == 0)
            {
                list[lex_index].type = procsym;
                strcpy(list[lex_index].name, word);
                lex_index++;
            }
            else if (strcmp(word, "call") == 0)
            {
                list[lex_index].type = callsym;
                strcpy(list[lex_index].name, word);
                lex_index++;
            }
            else if (strcmp(word, "if") == 0)
            {
                list[lex_index].type = ifsym;
                strcpy(list[lex_index].name, word);
                lex_index++;
            }
            else if (strcmp(word, "then") == 0)
            {
                list[lex_index].type = thensym;
                strcpy(list[lex_index].name, word);
                lex_index++;
            }
            else if (strcmp(word, "else") == 0)
            {
                list[lex_index].type = elsesym;
                strcpy(list[lex_index].name, word);
                lex_index++;
            }
            else if (strcmp(word, "while") == 0)
            {
                list[lex_index].type = whilesym;
                strcpy(list[lex_index].name, word);
                lex_index++;
            }
            else if (strcmp(word, "do") == 0)
            {
                list[lex_index].type = dosym;
                strcpy(list[lex_index].name, word);
                lex_index++;
            }
            else if (strcmp(word, "begin") == 0)
            {
                list[lex_index].type = beginsym;
                strcpy(list[lex_index].name, word);
                lex_index++;
            }
            else if (strcmp(word, "end") == 0)
            {
                list[lex_index].type = endsym;
                strcpy(list[lex_index].name, word);
                lex_index++;
            }
            else if (strcmp(word, "read") == 0)
            {
                list[lex_index].type = readsym;
                strcpy(list[lex_index].name, word);
                lex_index++;
            }
            else if (strcmp(word, "write") == 0)
            {
                list[lex_index].type = writesym;
                strcpy(list[lex_index].name, word);
                lex_index++;
            }
            else
            {
                //printf("word: %s\n", word);
                strcpy(list[lex_index].name, word);
                list[lex_index].type = identsym;
                lex_index++;
            }

            i += j - 1;
        }

        //found a number
        else if (isdigit(input[i]))
        {
            word[0] = input[i];
            //check if it's valid
            for (j = 1; isdigit(input[i + j]); j++)
            {
                if (j > 6)
                {
                    printlexerror(2);
                    return NULL;
                }

                word[j] = input[i + j];
            }

            if (isalpha(input[i + j]))
            {
                printlexerror(1);
                return NULL;
            }

            word[j] = '\0';
            //printf("number: %s\n", word);

            //tokenize
            list[lex_index].value = atoi(word);
            list[lex_index].type = numbersym;
            lex_index++;

            i += j - 1;
        }

        //special symbols
        else if (ispunct(input[i]))
        {
            //check if it's valid
            for (j = 0; ispunct(input[i + j]); j++)
            {
                word[j] = input[i + j];
                word[j + 1] = '\0';

                //check if it's a valid special symbol
                if (strcmp(word, "==") == 0)
                {
                    list[lex_index].type = eqlsym;
                    strcpy(list[lex_index].name, word);
                    lex_index++;
                    break;
                }
                else if (strcmp(word, "!=") == 0)
                {
                    list[lex_index].type = neqsym;
                    strcpy(list[lex_index].name, word);
                    lex_index++;
                    break;
                }
                else if ((strcmp(word, "<") == 0) && (input[i + j + 1] != '='))
                {
                    list[lex_index].type = lsssym;
                    strcpy(list[lex_index].name, word);
                    lex_index++;
                    break;
                }
                else if (strcmp(word, "<=") == 0)
                {
                    list[lex_index].type = leqsym;
                    strcpy(list[lex_index].name, word);
                    lex_index++;
                    break;
                }
                else if ((strcmp(word, ">") == 0) && (input[i + j + 1] != '='))
                {
                    list[lex_index].type = gtrsym;
                    strcpy(list[lex_index].name, word);
                    lex_index++;
                    break;
                }
                else if (strcmp(word, ">=") == 0)
                {
                    list[lex_index].type = geqsym;
                    strcpy(list[lex_index].name, word);
                    lex_index++;
                    break;
                }
                else if (strcmp(word, "*") == 0)
                {
                    list[lex_index].type = multsym;
                    strcpy(list[lex_index].name, word);
                    lex_index++;
                    break;
                }
                else if (strcmp(word, "/") == 0)
                {
                    list[lex_index].type = divsym;
                    strcpy(list[lex_index].name, word);
                    lex_index++;
                    break;
                }
                else if (strcmp(word, "+") == 0)
                {
                    list[lex_index].type = plussym;
                    strcpy(list[lex_index].name, word);
                    lex_index++;
                    break;
                }
                else if (strcmp(word, "-") == 0)
                {
                    list[lex_index].type = minussym;
                    strcpy(list[lex_index].name, word);
                    lex_index++;
                    break;
                }
                else if (strcmp(word, "(") == 0)
                {
                    list[lex_index].type = lparensym;
                    strcpy(list[lex_index].name, word);
                    lex_index++;
                    break;
                }
                else if (strcmp(word, ")") == 0)
                {
                    list[lex_index].type = rparensym;
                    strcpy(list[lex_index].name, word);
                    lex_index++;
                    break;
                }
                else if (strcmp(word, ",") == 0)
                {
                    list[lex_index].type = commasym;
                    strcpy(list[lex_index].name, word);
                    lex_index++;
                    break;
                }
                else if (strcmp(word, ".") == 0)
                {
                    list[lex_index].type = periodsym;
                    strcpy(list[lex_index].name, word);
                    lex_index++;
                    break;
                }
                else if (strcmp(word, ";") == 0)
                {
                    list[lex_index].type = semicolonsym;
                    strcpy(list[lex_index].name, word);
                    lex_index++;
                    break;
                }
                else if (strcmp(word, ":=") == 0)
                {
                    list[lex_index].type = assignsym;
                    strcpy(list[lex_index].name, word);
                    lex_index++;
                    break;
                }
                else if (strcmp(word, "&&") == 0)
                {
                    list[lex_index].type = andsym;
                    strcpy(list[lex_index].name, word);
                    lex_index++;
                    break;
                }
                else if (strcmp(word, "||") == 0)
                {
                    list[lex_index].type = orsym;
                    strcpy(list[lex_index].name, word);
                    lex_index++;
                    break;
                }
                else if (strcmp(word, "!") == 0)
                {
                    list[lex_index].type = notsym;
                    strcpy(list[lex_index].name, word);
                    lex_index++;
                    break;
                }
                else if (!ispunct(input[i + j + 1]))
                {
                    printlexerror(4);
                    return NULL;
                }
            }

            i += j;
        }
    }


	if (printFlag)
		printtokens();

	// these last two lines are really important for the rest of the package to run
	list[lex_index].type = -1;
	return list;
}

void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case eqlsym:
				printf("%11s\t%d", "==", eqlsym);
				break;
			case neqsym:
				printf("%11s\t%d", "!=", neqsym);
				break;
			case lsssym:
				printf("%11s\t%d", "<", lsssym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case divsym:
				printf("%11s\t%d", "/", divsym);
				break;
			case plussym:
				printf("%11s\t%d", "+", plussym);
				break;
			case minussym:
				printf("%11s\t%d", "-", minussym);
				break;
			case lparensym:
				printf("%11s\t%d", "(", lparensym);
				break;
			case rparensym:
				printf("%11s\t%d", ")", rparensym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case assignsym:
				printf("%11s\t%d", ":=", assignsym);
				break;
			case beginsym:
				printf("%11s\t%d", "begin", beginsym);
				break;
			case endsym:
				printf("%11s\t%d", "end", endsym);
				break;
			case ifsym:
				printf("%11s\t%d", "if", ifsym);
				break;
			case thensym:
				printf("%11s\t%d", "then", thensym);
				break;
			case elsesym:
				printf("%11s\t%d", "else", elsesym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case constsym:
				printf("%11s\t%d", "const", constsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
            case andsym:
				printf("%11s\t%d", "&&", andsym);
				break;
            case orsym:
				printf("%11s\t%d", "||", orsym);
				break;
            case notsym:
				printf("%11s\t%d", "!", notsym);
				break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n");
}

void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Number Length\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Identifier Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 5)
		printf("Lexical Analyzer Error: Never-ending comment\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");

	free(list);
	return;
}
