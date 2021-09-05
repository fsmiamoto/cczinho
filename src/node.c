#include "node.h"
#include <stdlib.h>

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

Node *new_node_lvar(char *ident) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR;
  node->offset = (ident[0] - 'a' + 1) * 8;
  return node;
}
