// Demo 3: return a value from scheme to C

#include <stdio.h>
#include <chibi/eval.h>

int dostuff(sexp ctx) {
  /* declare and preserve local variables */
  sexp_gc_var1(obj1);
  sexp_gc_preserve1(ctx, obj1);
  int retval = 0;
  //sexp_gc_var2(obj1, obj2);
  //sexp_gc_preserve2(ctx, obj1, obj2);

  /* load a file containing Scheme code */
  //obj1 = sexp_c_string(ctx, "./demo2.scm", -1);
  //sexp_load(ctx, obj1, NULL);

  /* eval a C string as Scheme code */
  obj1 = sexp_eval_string(ctx, "(+ 99 1)", -1, NULL);
  if (sexp_integerp(obj1)) {
	  retval = sexp_unbox_fixnum(obj1);
  }

  /* construct a Scheme expression to eval */
  //obj1 = sexp_intern(ctx, "my-procedure", -1);
  //obj2 = sexp_cons(ctx, obj1, SEXP_NULL);
  //sexp_eval(ctx, obj2, NULL);

  /* release the local variables */
  sexp_gc_release1(ctx);
  return retval;
}

int main(int argc, char** argv) {
  int value;
  sexp ctx;
  ctx = sexp_make_eval_context(NULL, NULL, NULL, 0, 0);
  sexp_load_standard_env(ctx, NULL, SEXP_SEVEN);
  sexp_load_standard_ports(ctx, NULL, stdin, stdout, stderr, 1);
  value = dostuff(ctx);
  printf("Value (printed from C): %d\n", value);
  sexp_destroy_context(ctx);
}
