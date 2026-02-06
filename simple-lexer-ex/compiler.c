#include <ctype.h>
#include <regex.h>
#include <stdatomic.h>
#include <stdio.h> // standard lib
#include <stdlib.h>
#include <string.h>

// Define token types
typedef enum {
  TOKEN_ASSING,
  TOKEN_INT_TYPE,
  TOKEN_CHAR_TYPE,   // *** NUEVO TOKEN ***
  TOKEN_FLOAT_TYPE,  // *** NUEVO TOKEN ***
  TOKEN_CONST,       // *** NUEVO TOKEN ***
  TOKEN_ID,
  TOKEN_EOL, // End of line
  TOKEN_EOF, // End of file
  TOKEN_ERROR
} TokenType;

const char *TOKENS_STRING[] = {
    "TOKEN_ASSING", 
    "TOKEN_INT_TYPE", 
    "TOKEN_CHAR_TYPE",   // *** NUEVO TOKEN ***
    "TOKEN_FLOAT_TYPE",  // *** NUEVO TOKEN ***
    "TOKEN_CONST",       // *** NUEVO TOKEN ***
    "TOKEN_ID",
    "TOKEN_EOL",    
    "TOKEN_EOF",      
    "TOKEN_ERROR",
};

// Define Token map
typedef struct {
  TokenType type;
  char value[32];
} Token;

const char *ID_PATTERN = "^[a-zA-Z_][a-zA-Z0-9_]*";
const char *INT_PATTERN = "^int\\>";
const char *CHAR_PATTERN = "^char\\>";
const char *FLOAT_PATTERN = "^float\\>";
const char *CONST_PATTERN = "^const\\>";

Token get_next_token(const char **input) {
  // skip white space
  Token token;
  // Skip: White spaces
  while (isspace(**input))
    (*input)++;
  
  if (**input == '\0') {
    token.type = TOKEN_EOF;
    token.value[0] = '\0';
    return token;
  }
  
  regex_t regex;
  regmatch_t match;
  /* Compile the regex pattern for ID */
  if (regcomp(&regex, ID_PATTERN, REG_EXTENDED) != 0) {
    token.type = TOKEN_ERROR;
    return token;
  }
  
  regex_t regex_int;
  regmatch_t match_int;
  /* Compile the regex pattern for INT */
  if (regcomp(&regex_int, INT_PATTERN, REG_EXTENDED) != 0) {
    token.type = TOKEN_ERROR;
    return token;
  }
  
  // *** NUEVO: Compilar regex para CHAR ***
  regex_t regex_char;
  regmatch_t match_char;
  if (regcomp(&regex_char, CHAR_PATTERN, REG_EXTENDED) != 0) {
    token.type = TOKEN_ERROR;
    return token;
  }
  
  // *** NUEVO: Compilar regex para FLOAT ***
  regex_t regex_float;
  regmatch_t match_float;
  if (regcomp(&regex_float, FLOAT_PATTERN, REG_EXTENDED) != 0) {
    token.type = TOKEN_ERROR;
    return token;
  }
  
  // *** NUEVO: Compilar regex para CONST ***
  regex_t regex_const;
  regmatch_t match_const;
  if (regcomp(&regex_const, CONST_PATTERN, REG_EXTENDED) != 0) {
    token.type = TOKEN_ERROR;
    return token;
  }
  
  /*Match assignment token */
  if (**input == '=') {
    token.type = TOKEN_ASSING;
    strcpy(token.value, "=");
    (*input)++; /* Advance pointer past the operator */
    regfree(&regex);
    regfree(&regex_int);
    regfree(&regex_char);
    regfree(&regex_float);
    regfree(&regex_const);
    return token;
  }
  
  /* Match INT keyword */
  if (regexec(&regex_int, *input, 1, &match_int, 0) == 0) {
    int length = match_int.rm_eo - match_int.rm_so;
    token.type = TOKEN_INT_TYPE;
    strncpy(token.value, *input, length);
    token.value[length] = '\0';
    *input += length; /* Advance input pointer by match length */
    regfree(&regex_int);
    regfree(&regex);
    regfree(&regex_char);
    regfree(&regex_float);
    regfree(&regex_const);
    return token;
  }
  
  // *** NUEVO: Match CHAR keyword ***
  if (regexec(&regex_char, *input, 1, &match_char, 0) == 0) {
    int length = match_char.rm_eo - match_char.rm_so;
    token.type = TOKEN_CHAR_TYPE;
    strncpy(token.value, *input, length);
    token.value[length] = '\0';
    *input += length;
    regfree(&regex_char);
    regfree(&regex);
    regfree(&regex_int);
    regfree(&regex_float);
    regfree(&regex_const);
    return token;
  }
  
  // *** NUEVO: Match FLOAT keyword ***
  if (regexec(&regex_float, *input, 1, &match_float, 0) == 0) {
    int length = match_float.rm_eo - match_float.rm_so;
    token.type = TOKEN_FLOAT_TYPE;
    strncpy(token.value, *input, length);
    token.value[length] = '\0';
    *input += length;
    regfree(&regex_float);
    regfree(&regex);
    regfree(&regex_int);
    regfree(&regex_char);
    regfree(&regex_const);
    return token;
  }
  
  // *** NUEVO: Match CONST keyword ***
  if (regexec(&regex_const, *input, 1, &match_const, 0) == 0) {
    int length = match_const.rm_eo - match_const.rm_so;
    token.type = TOKEN_CONST;
    strncpy(token.value, *input, length);
    token.value[length] = '\0';
    *input += length;
    regfree(&regex_const);
    regfree(&regex);
    regfree(&regex_int);
    regfree(&regex_char);
    regfree(&regex_float);
    return token;
  }
  
  /*Match assignment token (duplicado en el original) */
  if (**input == '=') {
    token.type = TOKEN_ASSING;
    strcpy(token.value, "=");
    (*input)++; /* Advance pointer past the operator */
    regfree(&regex);
    regfree(&regex_int);
    regfree(&regex_char);
    regfree(&regex_float);
    regfree(&regex_const);
    return token;
  }
  
  /*Match end of line */
  if (**input == ';') {
    token.type = TOKEN_EOL;
    strcpy(token.value, ";");
    (*input)++; /* Advance pointer past the operator */
    regfree(&regex);
    regfree(&regex_int);
    regfree(&regex_char);
    regfree(&regex_float);
    regfree(&regex_const);
    return token;
  }
  
  /* Execute regex matching on the current input string (ID) */
  if (regexec(&regex, *input, 1, &match, 0) == 0) {
    /* match.rm_so is start offset, match.rm_eo is end offset */
    int length = match.rm_eo - match.rm_so;
    token.type = TOKEN_ID;
    strncpy(token.value, *input, length);
    token.value[length] = '\0';
    *input += length; /* Advance input pointer by match length */
    regfree(&regex);
    regfree(&regex_int);
    regfree(&regex_char);
    regfree(&regex_float);
    regfree(&regex_const);
    return token;
  }
  
  /* State: Error - current character does not match any known patterns */
  token.type = TOKEN_ERROR;
  token.value[0] = **input;
  token.value[1] = '\0';
  regfree(&regex);
  regfree(&regex_int);
  regfree(&regex_char);
  regfree(&regex_float);
  regfree(&regex_const);
  return token;
}

int main() {
  char expression[255];
  printf("Please write an expression:\n");
  fgets(expression, 255, stdin);
  
  const char *pointer = expression; /* Pointer to track progress through the string */
  printf("Lexing expression %s\n", expression);
  
  Token token;
  do {
    // printf("pointer: %s \n", pointer);
    token = get_next_token(&pointer);
    printf("Token Type: %s| Value: '%s'\n", TOKENS_STRING[token.type],
           token.value);
  } while (token.type != TOKEN_EOF && token.type != TOKEN_ERROR);
  
  return 0;
}