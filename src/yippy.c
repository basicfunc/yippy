#include "eval.h"
#ifndef _WIN32
#include "linenoise.h"
#endif
#include "mpc.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HIST_FILE ".yippy_hsts"
#define YIPPY_PROMPT ">>> "

#ifdef _WIN32
#define GET_INPUT(x) line(x)
#else
#define GET_INPUT(x) linenoise(x)
#endif

char *line(char *prompt) {
  char buffer[4096];
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char *cpy = malloc(strlen(buffer) + 1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy) - 1] = '\0';
  return cpy;
}

int main(void) {
  char *input;
  linenoiseHistoryLoad(HIST_FILE);
  lenv *env = lenv_new();
  lenv_add_builtins(env);

  parser *p = parse();

  while (1) {

    input = GET_INPUT(YIPPY_PROMPT);

    if (!input) {

      linenoiseFree(input);
      printf("BYE!!!\n");
      break;
    }

    if (!strlen(input)) {
      linenoiseFree(input);
      continue;
    }

    linenoiseHistoryAdd(input);
    linenoiseHistorySave(HIST_FILE);

    mpc_result_t r;
    if (mpc_parse("<stdin>", input, p->Yippy, &r)) {
      lval *x = lval_eval(env, lval_read(r.output));
      lval_println(x);
      lval_del(x);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    linenoiseFree(input);
  }

  p = parse_clean(p);
  lenv_del(env);

  return 0;
}
