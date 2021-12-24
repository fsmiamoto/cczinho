#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "token.h"

#define LEN(x) (sizeof x - 1)

bool consume(char *op) {
  if (token->kind != TK_RESERVED || (int)strlen(op) != token->len ||
      memcmp(token->str, op, token->len)) {
    return false;
  }
  token = token->next;
  return true;
}

Token *consume_token(TokenKind kind) {
  if (token->kind != kind)
    return NULL;
  Token *t = token;
  token = token->next;
  return t;
}

Token *consume_ident() {
  if (token->kind != TK_IDENT)
    return NULL;
  Token *ident = token;
  token = token->next;
  return ident;
}

void expect(char *op) {
  if (token->kind != TK_RESERVED || (int)strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    error_at(input, token->str, "'%s' was expected, found '%c'", op,
             token->str);
  token = token->next;
}

char *expect_ident() {
  if (token->kind != TK_IDENT)
    error_at(input, token->str, "expected an identifier");
  char *ident = token->str;
  token = token->next;
  return ident;
}

int expect_number() {
  if (token->kind != TK_NUM) {
    error_at(input, token->str, "token is not a number");
  }
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() { return token->kind == TK_EOF; }

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

bool starts_with(char *p, char *q) { return memcmp(p, q, strlen(q)) == 0; }

static bool is_ident_char(char c) { return isalnum(c) || c == '_'; }

static bool is_ident_start(char c) { return isalpha(c) || c == '_'; }

Token *tokenize() {
  Token head;
  head.next = NULL;
  Token *curr = &head;

  char *p = input;
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (starts_with(p, "==") || starts_with(p, "!=") || starts_with(p, "<=") ||
        starts_with(p, ">=")) {
      curr = new_token(TK_RESERVED, curr, p, 2);
      p += 2;
      continue;
    }

    if (strchr("+-*/()<>;={},", *p)) {
      curr = new_token(TK_RESERVED, curr, p++, 1);
      continue;
    }

    if (isdigit(*p)) {
      curr = new_token(TK_NUM, curr, p, 0);
      char *q = p;
      curr->val = strtol(p, &p, 10);
      curr->len = p - q;
      continue;
    }

    if (strncmp(p, "return", LEN("return")) == 0 &&
        !isalnum(p[LEN("return")])) {
      curr = new_token(TK_RETURN, curr, p, LEN("return"));
      p += LEN("return");
      continue;
    }

    if (strncmp(p, "if", LEN("if")) == 0 && !isalnum(p[LEN("if")])) {
      curr = new_token(TK_IF, curr, p, LEN("if"));
      p += LEN("if");
      continue;
    }

    if (strncmp(p, "else", LEN("else")) == 0 && !isalnum(p[LEN("ELSE")])) {
      curr = new_token(TK_ELSE, curr, p, LEN("else"));
      p += LEN("else");
      continue;
    }

    if (strncmp(p, "while", LEN("while")) == 0 && !isalnum(p[LEN("while")])) {
      curr = new_token(TK_WHILE, curr, p, LEN("while"));
      p += LEN("while");
      continue;
    }

    if (strncmp(p, "for", LEN("for")) == 0 && !isalnum(p[LEN("for")])) {
      curr = new_token(TK_FOR, curr, p, LEN("for"));
      p += LEN("for");
      continue;
    }

    if (is_ident_start(*p)) {
      char *start = p;
      while (is_ident_char(*p))
        p++;
      int len = p - start;
      curr = new_token(TK_IDENT, curr, start, len);
      continue;
    }

    error_at(input, p, "unexpected token found: '%c'", *p);
  }

  new_token(TK_EOF, curr, p, 0);
  return head.next;
}
