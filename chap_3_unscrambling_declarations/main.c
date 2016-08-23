#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

// This problem will translate C declations into nice engish. If only C was defined in such a way that we do that already!


// example of yukky c declaration:
// void ( *signal(int sig, void (*func)(int))) (int)
// signal is a pointer to a function that returns a pointer to a function that takes an int and returns a void. 
// The arguments for signal are an int a function that takes an int and returns a void

// from http://cdecl.org/, which I will try to mimic!
// void (*signal(int, void(*)(int)))(int) 
// returns:
// 	declare signal as function (int, pointer to function (int) returning void) returning pointer to function (int) returning void
// http://cdecl.ridiculousfish.com/?q=void+%28*signal%28int%2C+void%28*%29%28int%29%29%29%28int%29

// void (*foo)(int)
// declare foo as a pointer to a function (int) returning void
// http://cdecl.ridiculousfish.com/?q=void+%28*foo%29%28int%29

// void (foo(int))
// declare foo as a function (int) that returns a pointer to void
// http://cdecl.ridiculousfish.com/?q=void+%28*foo%28int%29%29

// int(*(*foo)(void))[3]
// declare foo as a pointer to a function (void) returning a pointer to an array of 3 int
// http://cdecl.ridiculousfish.com/?q=int%28*%28*foo%29%28void%29%29%5B3%5D

// const int (* volatile bar)[64]
// declare bar as volatile pointer to array 64 of const int

// char *(*c[10])(int**)
// declare c as array 10 of pointer to function (pointer to pointer to int) returning pointer to char

// char* const *(*next)()
// declare next as pointer to function returning pointer to const pointer to char


const int FALSE = 0;
const int TRUE = 1;


// debug flags
const int DEBUG_VERBOSE = FALSE;
const int DEBUG_PRINT_STACK = TRUE;
const int DEBUG_PRINT_CURRENT_ITEM = FALSE;
const int DEBUG_PRINT_THIS = TRUE;
const int DEBUG_PRINT_GET_TOKEN_STATUS = FALSE;
const int DEBUG_PRINT_STACK_BEFORE_AND_AFTER_DEALING_WITH_POINTERS = TRUE;

// debug strings
const char* DEBUG_PRINT = "\n--";
const char* DEBUG_LINE = "\n--------------------------------------------\n";

const int MAXTOKENLENGTH = 32;
const int MAXTOKENS = 32;
const int MAXSTRINGLENGTH = 256;
const char NUL_CHAR = '\0';
typedef int ERROR_T;

struct token {
	char type;
	char string[MAXTOKENLENGTH];
};

struct token g_stack[MAXTOKENS];
struct token g_this;

// token types
const char TOKEN_TYPE_TYPE = 't';
const char TOKEN_TYPE_QUALIFIER = 'q';
const char TOKEN_TYPE_IDENTIFIER = 'i';

// there is another token type which is a non-alphanumeric type in which the token type will be the character it's self

// qualifier tokens
const char* TOKEN_Q_CONST = "const";
const char* TOKEN_Q_VOLATILE = "volatile";

// type tokens for type system
const char* TOKEN_T_INT = "int";
const char* TOKEN_T_FLOAT = "float";
const char* TOKEN_T_LONG = "long";
const char* TOKEN_T_DOUBLE = "double";
const char* TOKEN_T_UINT = "uint";
const char* TOKEN_T_ULONG = "ulong";
const char* TOKEN_T_CHAR = "char";
const char* TOKEN_T_VOID = "void";

// single character tokens
const char TOKEN_SC_OPEN_PAREN = '(';
const char TOKEN_SC_CLOSE_PAREN = ')';
const char TOKEN_SC_OPEN_BRACKET = '[';
const char TOKEN_SC_CLOSE_BRACKET = ']';
const char TOKEN_SC_WHITE_SPACE = ' ';

const char TOKEN_POINTER = '*';
const char* POINTER_TEXT = "pointer to ";

const ERROR_T ERROR_NO_ERROR = 0;
const ERROR_T ERROR_OFFSET_INCREMENTED_PAST_STRING = 1;
const int STRCMP_EQUAL = 0;

int g_stackTop = -1;
const char* g_string = NULL;
ERROR_T g_error = 0;
int g_readOffset = -1;

// stack routines
void push(struct token t);
struct token pop();
int stackHasItems();


// utility routines
char classify_string();	

void gettoken();
void readToken();
void readNonSingleChacterTokenUsingWriteOffset(int writeOffsetToGlobalTokenString);
int isSingleCharacterToken(char c);
int tokenStringCmp(const char* comparison);

void printTokenType(char token);
void printStack();
void printThis();

// error checking
void check_g_string_not_null();
ERROR_T check_read_offset_against_string();

void read_to_first_identifier();

// parsing routines
void deal_with_function_args();

void deal_with_arrays();

void deal_with_any_pointers();

void deal_with_declarator();


// main routine
int main( int argc, char** argv ) {
	const char* example1 = "void (*foo)(int)";
	const char* example2 = "int(*(*foo)(void))[3]";
	const char* example3 = "void ( *signal(int, void (*)(int))) (int)";
	const char* example4 = "const int (* volatile bar)[64]";
	const char* example5 = "char* const *(*next)()";
	const char* example6 = "char *(*c[10])(int**)"; //nearly

	g_string = example6;
	printf( "\nI will now perform magic! I will transform the phrase:\n%s\ninto english...\n", g_string );
	
	read_to_first_identifier();
	deal_with_declarator();	

	return 0;
}


void read_to_first_identifier() {
	int isIdentifierRead = FALSE;
	int endOfString = FALSE;
	check_g_string_not_null();
	
	gettoken();
	while( g_this.type != TOKEN_TYPE_IDENTIFIER ){		
		if(g_this.type != TOKEN_SC_WHITE_SPACE) {
			push( g_this );	
		}
		gettoken();	
	} 
	
	if( g_this.type == TOKEN_TYPE_IDENTIFIER ) {		
		printf( "Declare %s as ", g_this.string );
	}
	gettoken();
}


void deal_with_declarator() {	
	if( DEBUG_VERBOSE == TRUE ) {
		printf( "%sDeal with declarator called with current type as: %c\n", DEBUG_PRINT, g_this.type );
		printStack();
	}
	

	while( g_this.type == TOKEN_SC_WHITE_SPACE ){ 
		gettoken(); 
	}

	if( g_this.type == TOKEN_SC_OPEN_BRACKET ) deal_with_arrays();	
	if( g_this.type == TOKEN_SC_OPEN_PAREN ) deal_with_function_args();
	if( DEBUG_VERBOSE == TRUE ) {
		printf( "%sDealing with pointers\n", DEBUG_PRINT );
	}
	deal_with_any_pointers();
	if( DEBUG_VERBOSE == TRUE ) {
		printf( "%s Dealt with pointers\n", DEBUG_PRINT );
	}

	while( stackHasItems() ) {
		//printf("item: %s\n", g_this.string);
		//int result = g_this.type == TOKEN_SC_OPEN_PAREN;
		//printf("Comparison result between %c and '(' is: %d\n", g_this.type, result);
		if( g_stack[g_stackTop].type == TOKEN_SC_OPEN_PAREN ) {
			//printf("%c is an open parenthesis\n", g_this.type);
			pop();
			gettoken();
			if( g_this.type == TOKEN_SC_CLOSE_PAREN ) {
				if( DEBUG_VERBOSE ){
					printf( "Current item is close parenthesis. So getting next token" );
				}
				gettoken();
				if( DEBUG_VERBOSE == TRUE && DEBUG_PRINT_THIS == TRUE ){
					printf("Printing this after getting next token due to close parenthesis");
					printThis();
				}
			}
			deal_with_declarator();
		}
		else {									
			//if( g_this.type == TOKEN_SC_CLOSE_PAREN ){
			//	pop();
			//}
			if( DEBUG_VERBOSE == TRUE ) {
				printf("\nOUTPUT:");
			}
			printf( "%s ", pop().string );
			if( DEBUG_VERBOSE == TRUE ) {
				printf("\n");
			}
			if( DEBUG_VERBOSE == TRUE && DEBUG_PRINT_THIS == TRUE ) {
				printf("Current item after output is: \n");
				printThis();
			}
			//gettoken();
			if( DEBUG_VERBOSE == TRUE && DEBUG_PRINT_THIS == TRUE ) {
				printf("Current item after gettoken after output is: \n");
				printThis();
			}
			/*if( g_this.type == TOKEN_TYPE_TYPE || g_this.type == TOKEN_TYPE_QUALIFIER) {
				printf( "%s ", g_this.string );			
			}*/
			
			/*
			if( stackHasItems() ) {
				pop();
			}*/
		}
	}
}


void gettoken() {
	check_g_string_not_null();	

	int stringLength = strlen( g_string );
	if( g_readOffset >= stringLength-1 ) {
		g_error = ERROR_OFFSET_INCREMENTED_PAST_STRING;
		return;
	} 

	readToken();	

	if( strlen( g_this.string ) == 0 ) { 
		printf( "Token string is empty \n" );
		return;
	} 

	char firstCharInCurrentToken = g_this.string[0];
	if( isalnum( firstCharInCurrentToken ) == TRUE ) {		
		g_this.type = classify_string();
		
		printTokenType( g_this.type );		
	}
	else if( isSingleCharacterToken( firstCharInCurrentToken )) {
		g_this.type = firstCharInCurrentToken;		

		if(g_this.type == TOKEN_POINTER) {
			//write 'pointer to' to current token string
			int pointerToTextLength = strlen(POINTER_TEXT);
			int readIndex;
			int writeIndex = 0;
			for(readIndex = 0; readIndex<pointerToTextLength; ++readIndex) {
				g_this.string[writeIndex] = POINTER_TEXT[readIndex];
				writeIndex++;
			}
		}
	}

	
	// uncomment this for cheeky looping
	// gettoken();

	// read next token
	// 
	// char character = g_string[g_readOffset];
	// if first character isalnum then classify string
	// 

/*	if(isalnum(character) == TRUE) {

		printf("I am a non-silly character");
	}
	printf("%c\n", character);*/
}


void readToken() {		
	// read in the next token from the global string into g_this.string
	// this will either set the current string to be a single character or a word
	
	check_g_string_not_null();	

	// increment the input read head
	g_readOffset++;

	//guard
	if(check_read_offset_against_string() != ERROR_NO_ERROR) {
		g_this.string[0] = NUL_CHAR;
		return;
	}	

	// read the current character
	char currentCharacter = g_string[g_readOffset];	
	
	if(isSingleCharacterToken(currentCharacter) == TRUE) {
		g_this.string[0] = currentCharacter;
		g_this.string[1] = NUL_CHAR;			
	}
	else {
		// read up until the end of the string or one of the single character tokens is found
		readNonSingleChacterTokenUsingWriteOffset(0);		
	}		
}


int isSingleCharacterToken(char c) {
	return c == TOKEN_SC_WHITE_SPACE
		|| c == TOKEN_SC_CLOSE_PAREN
		|| c == TOKEN_SC_CLOSE_BRACKET
		|| c == TOKEN_SC_OPEN_BRACKET
		|| c == TOKEN_SC_OPEN_PAREN
		|| c == TOKEN_POINTER;
}


void readNonSingleChacterTokenUsingWriteOffset(int writeOffset) {
	check_g_string_not_null();
	if(check_read_offset_against_string() != ERROR_NO_ERROR) {
		g_this.string[0] = NUL_CHAR;
		return;
	}

	if(writeOffset >= MAXTOKENLENGTH){
		g_readOffset--;
		if(DEBUG_VERBOSE == TRUE) {
			printf("%sWrite offset above max token length \n", DEBUG_PRINT);
		}
		return;			
	} 

	char newChar = g_string[g_readOffset];

	if(isSingleCharacterToken(newChar)) {
		g_this.string[writeOffset] = NUL_CHAR;
		//reset offset 
		g_readOffset--;
		return;		
	}
	
	g_this.string[writeOffset] = newChar;		
	g_readOffset++;	

	//end of string
	if(check_read_offset_against_string() != ERROR_NO_ERROR) {
		g_this.string[writeOffset] = NUL_CHAR;
		return;
	}	

	writeOffset++;
	readNonSingleChacterTokenUsingWriteOffset(writeOffset);	
}

void check_g_string_not_null() {
	if(g_string == NULL) {
		printf("String is null :( \n");
		exit(1);
	}
}

ERROR_T check_read_offset_against_string() {
	int stringLength = strlen(g_string);
	//guard
	if( g_readOffset >= stringLength) {
		return ERROR_OFFSET_INCREMENTED_PAST_STRING;
	}
	else {
		return ERROR_NO_ERROR;
	}
}

char classify_string() {	
	if(tokenStringCmp(TOKEN_T_VOID) 
		|| tokenStringCmp(TOKEN_T_CHAR)		
		|| tokenStringCmp(TOKEN_T_INT)
		|| tokenStringCmp(TOKEN_T_FLOAT)
		|| tokenStringCmp(TOKEN_T_DOUBLE)		
		|| tokenStringCmp(TOKEN_T_LONG)
		|| tokenStringCmp(TOKEN_T_ULONG)
		|| tokenStringCmp(TOKEN_T_UINT)
		|| tokenStringCmp(TOKEN_T_ULONG)						
	){
		return TOKEN_TYPE_TYPE;
	}
	else if(tokenStringCmp(TOKEN_Q_VOLATILE) || tokenStringCmp(TOKEN_Q_CONST)) {
		return TOKEN_TYPE_QUALIFIER;
	}
	return TOKEN_TYPE_IDENTIFIER;
}


int tokenStringCmp(const char* comparison) {
	//printf("Comparing: %s With: %s\n", g_this.string, comparison);
	//printf("Result: %d\n", strcmp(g_this.string, comparison));

	if (strcmp(g_this.string, comparison) == STRCMP_EQUAL) {	
		return TRUE;
	}
	else {
		return FALSE;
	}
}


void printTokenType( char token ) {
	if( !(DEBUG_VERBOSE == TRUE && DEBUG_PRINT_GET_TOKEN_STATUS == TRUE) ) return;

	if( token == TOKEN_TYPE_TYPE ) {
		printf( "Token is: Type\n" );		
	}
	else if( token == TOKEN_TYPE_IDENTIFIER ) {
		printf( "Token is: Identifier\n" );
	}
	else if( token == TOKEN_TYPE_QUALIFIER ) {
		printf( "Token is: qualifier\n" );
	}
}


void push( struct token t ) {
	g_stackTop++;
	if( g_stackTop >= MAXTOKENS ) {
		printf( "Stack overflow. Too many tokens on stack\n" );
		return;
	}
	g_stack[g_stackTop] = t;
}


struct token pop() {
	if( stackHasItems() ){
		struct token currentToken = g_stack[g_stackTop];
		g_stackTop--;
		//g_this = currentToken;		
		return currentToken;
	}
	else {

		printf( "Pop before push!\n" );
		exit( 1 );		
		//return g_this;
	}
}


// parsing
void deal_with_arrays() {
	printf("an array ");			
	//consume '[' and put array count in g_this
	readToken();	
	printf( "with %s items", g_this.string );	
	printf( " of " );
	readToken();
}


// this will find the closing parenthesis that matches the current opening parenthesis. 
void deal_with_function_args(){
	int hasFoundClosingParen = FALSE;	
	int parensToClose = 1; // for when our current parenthesis contains other parenthesis pairs

	while( parensToClose > 0 && g_error == ERROR_NO_ERROR  ) {
		gettoken();
		if( g_this.type == TOKEN_SC_OPEN_PAREN ){			
			parensToClose++;
		}
		else if( g_this.type == TOKEN_SC_CLOSE_PAREN ){			
			parensToClose--;
		}
	}
	printf("function returning ");
}


void deal_with_any_pointers() {
	if( DEBUG_VERBOSE == TRUE && DEBUG_PRINT_STACK_BEFORE_AND_AFTER_DEALING_WITH_POINTERS == TRUE ) {		
		printStack();
	}

	while(g_stack[g_stackTop].type == TOKEN_POINTER) {
		printf("%s", POINTER_TEXT);
		pop();
	}	

	if( DEBUG_VERBOSE == TRUE && DEBUG_PRINT_STACK_BEFORE_AND_AFTER_DEALING_WITH_POINTERS == TRUE ) {
		printStack();
	}
}


int stackHasItems() {	
	if( g_stackTop == -1 ) {
		if( DEBUG_VERBOSE == TRUE) {
			printf( "%sStack has no items\n", DEBUG_PRINT );
		}
		return FALSE;
	}
	else {		
		return TRUE;
	}	
}


void printStack() {
	if( DEBUG_VERBOSE == TRUE && DEBUG_PRINT_STACK == TRUE ) {
		int i;
		printf("%s", DEBUG_LINE);
		printf( "%sPrinting stack: \n", DEBUG_PRINT );
		for( i=0; i<=g_stackTop; ++i ) {
			printf( "|| Stack item: %s ||\n", g_stack[i].string );
		}
	}

	printThis();
	printf("%s", DEBUG_LINE);
}

void printThis() {
	if( DEBUG_VERBOSE == TRUE && DEBUG_PRINT_THIS == TRUE ) {
		printf( "%sCurrent item type is: %c, String is: %s\n", DEBUG_PRINT, g_this.type, g_this.string );		
		printf( "%sCurrent read offet is: %d\n", DEBUG_PRINT, g_readOffset );

		int stringIndex; 
		int stringLength = strlen(g_string);
		for( stringIndex = 0; stringIndex < stringLength; ++stringIndex ){
			printf( "%d:%c ", stringIndex, g_string[stringIndex] );
		}		
		printf("\n");
	}
}