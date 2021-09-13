#ifndef __TOKEN__H
#define __TOKEN__H

#include <stdbool.h>

typedef enum {
    TK_RESERVED,
    TK_IDENT,
    TK_NUM,
    TK_RETURN,
    TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
  int len;
};

extern Token * token;
extern char * input;

bool at_eof();
bool starts_with(char *p, char*q);

bool consume(char *op);
Token* consume_token(TokenKind k);
Token* consume_ident();

void expect(char *op);
char *expect_ident();
int expect_number();

Token* tokenize();
#endif
