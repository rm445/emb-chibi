// Demo 4: Load a user-defined Scheme file and run it from the command line
#include <stdio.h>
#include <stdlib.h>
#include <chibi/eval.h>


void dostuff(sexp ctx, char *fname) {
  /* declare and preserve local variables */
  sexp_gc_var1(obj1);
  //sexp_gc_var2(obj1, obj2);
  //sexp_gc_preserve2(ctx, obj1, obj2);

  /* load a file containing Scheme code */
  obj1 = sexp_c_string(ctx, fname, -1);
  sexp_load(ctx, obj1, NULL);

  /* eval a C string as Scheme code */
  //sexp_eval_string(ctx, "(begin (display 100) (newline) (display 200) (newline))", -1, NULL);

  /* construct a Scheme expression to eval */
  //obj1 = sexp_intern(ctx, "my-procedure", -1);
  //obj2 = sexp_cons(ctx, obj1, SEXP_NULL);
  //sexp_eval(ctx, obj2, NULL);

  /* release the local variables */
  //sexp_gc_release2(ctx);
}



int main(int argc, char** argv) {
  if (argc < 2) {
      printf("Not enough arguments.\n");
      exit(EXIT_FAILURE);
  }
  sexp ctx;
  ctx = sexp_make_eval_context(NULL, NULL, NULL, 0, 0);
  sexp_load_standard_env(ctx, NULL, SEXP_SEVEN);
  sexp_load_standard_ports(ctx, NULL, stdin, stdout, stderr, 1);
  dostuff(ctx, argv[1]);
  sexp_destroy_context(ctx);
}
