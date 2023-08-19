// Demo 8: Load a user-defined Scheme file, display it, and offer to run it, 
// interactively, in a loop, in a single scheme context
// and error-check scheme exceptions
// NOTE: only does one expression from code (wrap in (begin ...) for now)

#include <stdio.h>
#include <stdlib.h>
#include <chibi/eval.h>

#define FNAME_LENGTH (256)

void dostuff(sexp ctx, char *prog) {
  /* declare and preserve local variables */
  sexp_gc_var1(obj1);
  sexp res;
  //sexp_gc_var2(obj1, obj2);
  //sexp_gc_preserve2(ctx, obj1, obj2);

  /* load a file containing Scheme code */
  //obj1 = sexp_c_string(ctx, fname, -1);
  //res = sexp_load(ctx, obj1, NULL);
  //if (sexp_exceptionp(res)) {
  //  sexp_print_exception(ctx, res, sexp_current_error_port(ctx));
  //}
  /* eval a C string as Scheme code */
  res = sexp_eval_string(ctx, prog, -1, NULL);
  if (sexp_exceptionp(res)) {
      sexp_debug(ctx, "res: ", res);
      sexp_print_exception(ctx, res, sexp_current_error_port(ctx));
  }

    /* construct a Scheme expression to eval */
  //obj1 = sexp_intern(ctx, "my-procedure", -1);
  //obj2 = sexp_cons(ctx, obj1, SEXP_NULL);
  //sexp_eval(ctx, obj2, NULL);

  /* release the local variables */
  //sexp_gc_release2(ctx);
}

char *get_filename(char *s) {
    printf("Enter filename: (ctrl-d to quit)\n");
    char *i = fgets(s, FNAME_LENGTH-2, stdin);
    s[strcspn(s, "\r\n")] = 0;
    return i;
}

char *load_file(char *fname) {
    char * buffer = 0;
    long length;
    FILE * f = fopen (fname, "rb");

    if (f) {
        fseek (f, 0, SEEK_END);
        length = ftell (f);
        fseek (f, 0, SEEK_SET);
        buffer = malloc (length+1);
        if (buffer != NULL)
            fread (buffer, 1, length, f);
            buffer[length] = '\0';
        fclose(f);
    }
    return buffer;
}

int main(int argc, char** argv) {
  sexp ctx;
  char fname[FNAME_LENGTH];
  char *sfile;

  ctx = sexp_make_eval_context(NULL, NULL, NULL, 0, 0);
  sexp_load_standard_env(ctx, NULL, SEXP_SEVEN);
  sexp_load_standard_ports(ctx, NULL, stdin, stdout, stderr, 1);
  for(;;) {  
    if  (get_filename(fname) == NULL)
      break;
    sfile = load_file(fname);
    if (sfile) {
        printf("=====\n%s\n=====\n", sfile);
        dostuff(ctx, sfile);
        free(sfile);
    } else {
        break;
    }
  }

  sexp_destroy_context(ctx);
}
