#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "gen.h"
#include "node.h"
#include "token.h"

//  プログラム入力
char *input;

// 現在着目しているトークン
Token *token;

Node *code[100];

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "missing arguments\n");
    return 1;
  }

  input = argv[1];
  token = tokenize();
  program();

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  // Prologue
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n"); // 26 positions of 8 bytes

  for (int i = 0; code[i]; i++) {
    gen(code[i]);

    // 式の評価結果としてスタックに一つの値が残っている
    // はずなので、スタックが溢れないようにポップしておく
    printf("  pop rax\n");
  }

  // Epilogue
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}
