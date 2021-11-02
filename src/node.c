#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "token.h"

typedef struct LVar LVar;

struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
};

LVar *locals = NULL;

LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var != NULL; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

void program() {
  int i = 0;
  while (!at_eof())
    code[i++] = stmt();
  code[i] = NULL;
}

// stmt = expr ";"
// | "{" stmt* "}"
// | "if" "(" expr ")" stmt ("else" stmt)?"
// | "while" "(" expr ")" stmt"
// | "for" "(" expr? ";" expr? ";" expr? ")" stmt
// | "return" expr ";"
Node *stmt() {
  Node *node;
  Token *tok;

  if (consume("{")) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;
    Node head = {};
    Node *cur = &head;
    while (!consume("}")) {
      cur = cur->next = stmt();
    }
    node->body = head.next;
    return node;
  }

  if ((tok = consume_token(TK_RETURN))) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr();
    expect(";");
    return node;
  }

  if ((tok = consume_token(TK_IF))) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_IF;
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    if ((tok = consume_token(TK_ELSE))) {
      node->els = stmt();
    }
    return node;
  }

  if ((tok = consume_token(TK_WHILE))) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_WHILE;
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    return node;
  }

  if ((tok = consume_token(TK_FOR))) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_FOR;
    expect("(");
    if (!consume(";")) {
      node->init = expr();
      expect(";");
    }
    if (!consume(";")) {
      node->cond = expr();
      expect(";");
    }
    if (!consume(")")) {
      node->incr = expr();
      expect(")");
    }
    node->then = stmt();
    return node;
  }

  node = expr();

  expect(";");

  return node;
}

// expr = assign
Node *expr() { return assign(); }

// assign = equality ("=" assign)?
Node *assign() {
  Node *node = equality();

  if (consume("=")) {
    node = new_node(ND_ASSIGN, node, assign());
  }

  return node;
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node(ND_NEQ, node, relational());
    else
      return node;
  }
}

// relational = add ( "<" add | "<=" add | ">" add | ">=" add )
Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<="))
      node = new_node(ND_LTE, node, add());
    else if (consume("<"))
      node = new_node(ND_LT, node, add());
    else if (consume(">="))
      node = new_node(ND_GTE, node, add());
    else if (consume(">"))
      node = new_node(ND_GT, node, add());
    else
      return node;
  }
}

// add = mul ("+" mul | "-" mul)*
Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

// mul = unary | ("*" unary | "/" unary)*
Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

// unary = ('+' | '-')? primary
Node *unary() {
  if (consume("+"))
    return primary();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), primary());
  return primary();
}

// primary =
// | num
// | ident ( "(" ")")?
// | "(" expr ")"
Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok;
  if ((tok = consume_token(TK_IDENT))) {
    Node *node = calloc(1, sizeof(Node));

    if (consume("(")) {
      expect(")");
      node->kind = ND_CALL;
      node->tok = tok;
      return node;
    }

    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(tok);
    if (lvar) {
      // variable already exists
      node->offset = lvar->offset;
      return node;
    }

    if (!locals) {
      locals = calloc(1, sizeof(LVar));
    }

    lvar = calloc(1, sizeof(LVar));
    lvar->next = locals;
    lvar->name = tok->str;
    lvar->len = tok->len;
    lvar->offset = locals->offset + 8;
    node->offset = lvar->offset;
    locals = lvar;

    return node;
  }

  return new_node_num(expect_number());
}
