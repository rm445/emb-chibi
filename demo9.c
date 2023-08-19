// Demo 9: take user input string and run in a continued context.
// (This is more about feeding in a C string than trying to make a REPL (or a REL)

#include <stdio.h>
#include <stdlib.h>
#include <chibi/eval.h>

#define FNAME_LENGTH (256)
#define LINE_LENGTH (1024)

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

char *get_line(char *s) {
    printf("> ");
    fflush(stdout);
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
  char line[LINE_LENGTH];
  char *sfile;

  ctx = sexp_make_eval_context(NULL, NULL, NULL, 0, 0);
  sexp_load_standard_env(ctx, NULL, SEXP_SEVEN);
  sexp_load_standard_ports(ctx, NULL, stdin, stdout, stderr, 1);
  for(;;) {  
    if  (get_line(line) == NULL)
      break;
    dostuff(ctx, line);
  }
  printf("\n");
  sexp_destroy_context(ctx);
}
