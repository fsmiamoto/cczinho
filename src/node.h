#ifndef __NODE_H__
#define __NODE_H__

typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // *
  ND_NUM, // integer
  ND_EQ,  // ==
  ND_NEQ, // !=
  ND_LT,  // <
  ND_LTE, // <=
  ND_GT,  // >
  ND_GTE, // >=
  ND_ASSIGN, // =
  ND_CALL, // foo()
  ND_LVAR, // local variable
  ND_RETURN, // return
  ND_IF, // if
  ND_WHILE, // while
  ND_FOR, // for
  ND_BLOCK, // {}
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  Node *els;
  Node *cond;
  Node *then;
  Node *init;
  Node *incr;
  Node *next;
  Node *body;
  char * ident;
  int ident_len;
  int val;
  int offset;
};

extern Node *code[100];

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_node_lvar(char *ident);

Node *expr();
Node *mul();
Node *primary();
Node *unary();
Node *add();
Node *relational();
Node *equality();
Node *assign();
Node *stmt();
void program();
#endif
