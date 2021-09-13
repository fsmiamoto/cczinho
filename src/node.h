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
  ND_LVAR, // local variable
  ND_RETURN
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
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
