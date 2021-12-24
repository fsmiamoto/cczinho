#include "gen.h"
#include "error.h"

#include <stdio.h>

static char *registers[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

static int count(void) {
  static int i = 1;
  return i++;
}

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    error("left side of assignment is not a variable");

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

static void push(void) { printf("  push rax\n"); }

void gen(Node *node) {
  if (!node)
    return;

  int c;
  switch (node->kind) {
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  case ND_RETURN:
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case ND_IF:
    c = count();
    if (node->els) {
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lelse.%d\n", c);
      gen(node->then);
      printf("  jmp .Lend.%d\n", c);
      printf(".Lelse.%d:\n", c);
      gen(node->els);
      printf(".Lend.%d:\n", c);
      return;
    }
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend.%d\n", c);
    gen(node->then);
    printf(".Lend.%d:\n", c);
    return;
  case ND_WHILE:
    c = count();
    printf(".Lbegin.%d:\n", c);
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend.%d\n", c);
    gen(node->then);
    printf("  jmp .Lbegin.%d\n", c);
    printf(".Lend.%d:\n", c);
    return;
  case ND_FOR:
    c = count();
    gen(node->init);
    printf(".Lbegin.%d:\n", c);
    if (node->cond) {
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lend.%d\n", c);
    }
    gen(node->then);
    gen(node->incr);
    printf("  jmp .Lbegin.%d\n", c);
    printf(".Lend.%d:\n", c);
    return;
  case ND_BLOCK:
    gen(node->body);
    if (node->body) {
      for (Node *n = node->body->next; n; n = n->next)
        gen(n);
    }
    return;
  case ND_FUNCALL: {
    int nargs = 0;

    for (Node *arg = node->args; arg; arg = arg->next) {
      gen(arg);
      nargs++;
    }

    if (nargs > 6) {
      error("cannot call function with more than 6 arguments");
    }

    for (int n = nargs - 1; n >= 0; n--) {
      printf("  pop %s\n", registers[n]);
    }

    printf("  mov rax, 0\n");
    printf("  call %.*s\n", node->tok->len, node->tok->str);
    // FIXME: Currently we need this extra push, because somewhere we are making
    // an extra pop
    push();

    return;
  }
  default:
    break;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_EQ:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NEQ:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LT:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LTE:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_GT:
    printf("  cmp rax, rdi\n");
    printf("  setg al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_GTE:
    printf("  cmp rax, rdi\n");
    printf("  setge al\n");
    printf("  movzb rax, al\n");
    break;
  default:
    break;
  }

  printf("  push rax\n");
}
