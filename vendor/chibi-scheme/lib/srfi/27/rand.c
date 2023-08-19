/*  rand.c -- rand_r/random_r interface                       */
/*  Copyright (c) 2009-2017 Alex Shinn.  All rights reserved. */
/*  BSD-style license: http://synthcode.com/license.txt       */

#include <time.h>
#include <chibi/eval.h>

#define SEXP_RANDOM_STATE_SIZE 128

#define ZERO sexp_make_fixnum(0)
#define ONE  sexp_make_fixnum(1)
#define STATE_SIZE sexp_make_fixnum(SEXP_RANDOM_STATE_SIZE)

#define sexp_random_source_p(self, x) (!self || ((sexp_pointerp(x) && (sexp_pointer_tag(x) == sexp_unbox_fixnum(sexp_opcode_arg1_type(self))))))

#define sexp_random_state(x) (sexp_slot_ref((x), 0))
#define sexp_random_data(x)  ((sexp_random_t*)(&sexp_slot_ref((x), 1)))

#define sexp_sizeof_random (sexp_sizeof_header + sizeof(sexp_random_t) + sizeof(sexp))

#ifdef __GNU_SOURCE__

typedef struct random_data sexp_random_t;

#define sexp_random_init(rs, seed)                                      \
  initstate_r(seed,                                                     \
              sexp_bytes_data(sexp_random_state(rs)),                   \
              SEXP_RANDOM_STATE_SIZE,                                   \
              sexp_random_data(rs))

#define sexp_call_random(rs, dst) random_r(sexp_random_data(rs), &dst)
#define sexp_seed_random(n, rs) srandom_r(n, sexp_random_data(rs))

#elif _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE

typedef unsigned int sexp_random_t;

#define sexp_random_init(rs, seed) *sexp_random_data(rs) = (seed)

#define sexp_call_random(rs, dst) ((dst) = rand_r(sexp_random_data(rs)))
#define sexp_seed_random(n, rs) *sexp_random_data(rs) = (n)

#else

typedef unsigned int sexp_random_t;

#define sexp_random_init(rs, seed) (void)0

/* FIXME: MSVC CRT has rand_s() for "cryptographically secure" random number
 *        for WinXP or later. */
#define sexp_call_random(rs, dst) ((dst) = rand())
#define sexp_seed_random(n, rs) srand(n)

#endif

sexp sexp_rs_random_integer (sexp ctx, sexp self, sexp_sint_t n, sexp rs, sexp bound) {
  sexp_gc_var1(res);
  int i;
  sexp_uint_t m;
  sexp_int32_t m2;
#if SEXP_USE_BIGNUMS
  sexp_uint_t *data;
  int hi, j;
#endif
  if (!sexp_random_source_p(self, rs))
    return sexp_type_exception(ctx, self, sexp_unbox_fixnum(sexp_opcode_arg1_type(self)), rs);
  if (sexp_fixnump(bound)) {
    if (sexp_unbox_fixnum(bound) <= 0) {
      res = sexp_xtype_exception(ctx, self, "random bound must be positive", bound);
    } else {
      /* ensure we have sufficient bits */
      for (i=m=0; i-1 <= 1<<(CHAR_BIT*sizeof(m))/RAND_MAX; ++i) {
        sexp_call_random(rs, m2);
        m = m * RAND_MAX + m2;
      }
      res = sexp_make_fixnum(m % sexp_unbox_fixnum(bound));
    }
#if SEXP_USE_BIGNUMS
  } else if (sexp_bignump(bound)) {
    sexp_gc_preserve1(ctx, res);
    hi = sexp_bignum_hi(bound);
    res = sexp_make_bignum(ctx, hi);
    data = sexp_bignum_data(res);
    for (i=0; i<hi; i++) {
      for (j=m=0; j-1 <= 1<<(CHAR_BIT*sizeof(m))/RAND_MAX; ++j) {
        sexp_call_random(rs, m2);
        m = m * (sexp_uint_t)RAND_MAX + (sexp_uint_t) m2;
      }
      data[i] = (sexp_uint_t) m;
      /* fprintf(stderr, "i: %d j: %d m: %lu (%lx) RAND_MAX: %d\n", i, j, m, (sexp_uint_t)m, RAND_MAX); */
    }
    res = sexp_remainder(ctx, res, bound);
    sexp_gc_release1(ctx);
#endif
  } else {
    res = sexp_type_exception(ctx, self, SEXP_FIXNUM, bound);
  }
  return res;
}

sexp sexp_random_integer (sexp ctx, sexp self, sexp_sint_t n, sexp bound) {
  return sexp_rs_random_integer(ctx, self, n, sexp_global(ctx, SEXP_G_RANDOM_SOURCE), bound);
}

sexp sexp_rs_random_real (sexp ctx, sexp self, sexp_sint_t n, sexp rs) {
  sexp_int32_t res;
  if (!sexp_random_source_p(self, rs))
    return sexp_type_exception(ctx, self, sexp_unbox_fixnum(sexp_opcode_arg1_type(self)), rs);
  sexp_call_random(rs, res);
  return sexp_make_flonum(ctx, (double)res / (double)RAND_MAX);
}

sexp sexp_random_real (sexp ctx, sexp self, sexp_sint_t n) {
  return sexp_rs_random_real(ctx, self, n, sexp_global(ctx, SEXP_G_RANDOM_SOURCE));
}

#if SEXP_BSD || defined(__CYGWIN__)

sexp sexp_make_random_source (sexp ctx, sexp self, sexp_sint_t n) {
  sexp res;
  res = sexp_alloc_tagged(ctx, sexp_sizeof_random, sexp_unbox_fixnum(sexp_opcode_return_type(self)));
  *sexp_random_data(res) = 1;
  return res;
}

sexp sexp_random_source_state_ref (sexp ctx, sexp self, sexp_sint_t n, sexp rs) {
  if (!sexp_random_source_p(self, rs))
    return sexp_type_exception(ctx, self, sexp_unbox_fixnum(sexp_opcode_arg1_type(self)), rs);
  else
    return sexp_make_integer(ctx, *sexp_random_data(rs));
}

sexp sexp_random_source_state_set (sexp ctx, sexp self, sexp_sint_t n, sexp rs, sexp state) {
  if (!sexp_random_source_p(self, rs))
    return sexp_type_exception(ctx, self, sexp_unbox_fixnum(sexp_opcode_arg1_type(self)), rs);
  else if (sexp_fixnump(state))
    *sexp_random_data(rs) = sexp_unbox_fixnum(state);
#if SEXP_USE_BIGNUMS
  else if (sexp_bignump(state))
    *sexp_random_data(rs)
      = sexp_bignum_data(state)[0]*sexp_bignum_sign(state);
#endif
  else
    return sexp_type_exception(ctx, self, SEXP_FIXNUM, state);
  return SEXP_VOID;
}

#else

sexp sexp_make_random_source (sexp ctx, sexp self, sexp_sint_t n) {
  sexp res;
  sexp_gc_var1(state);
  sexp_gc_preserve1(ctx, state);
  state = sexp_make_bytes(ctx, STATE_SIZE, SEXP_UNDEF);
  res = sexp_alloc_tagged(ctx, sexp_sizeof_random, sexp_unbox_fixnum(sexp_opcode_return_type(self)));
  if (sexp_exceptionp(res)) return res;
  sexp_random_state(res) = state;
  sexp_random_init(res, 1);
  sexp_gc_release1(ctx);
  return res;
}

sexp sexp_random_source_state_ref (sexp ctx, sexp self, sexp_sint_t n, sexp rs) {
  if (self && ! (sexp_pointerp(rs) && (sexp_pointer_tag(rs) == sexp_unbox_fixnum(sexp_opcode_arg1_type(self)))))
    return sexp_type_exception(ctx, self, sexp_unbox_fixnum(sexp_opcode_arg1_type(self)), rs);
  else
    return sexp_subbytes(ctx, sexp_random_state(rs), ZERO, STATE_SIZE);
}

sexp sexp_random_source_state_set (sexp ctx, sexp self, sexp_sint_t n, sexp rs, sexp state) {
  if (!sexp_random_source_p(self, rs))
    return sexp_type_exception(ctx, self, sexp_unbox_fixnum(sexp_opcode_arg1_type(self)), rs);
  else if (! (sexp_bytesp(state)
              && (sexp_bytes_length(state) == SEXP_RANDOM_STATE_SIZE)))
    return sexp_type_exception(ctx, self, SEXP_BYTES, state);
  sexp_random_state(rs) = sexp_subbytes(ctx, state, ZERO, STATE_SIZE);
  sexp_random_init(rs, 1);
  return SEXP_VOID;
}

#endif

sexp sexp_random_source_randomize (sexp ctx, sexp self, sexp_sint_t n, sexp rs) {
  if (! sexp_random_source_p(self, rs))
    return sexp_type_exception(ctx, self, sexp_unbox_fixnum(sexp_opcode_arg1_type(self)), rs);
  sexp_seed_random(time(NULL), rs);
  return SEXP_VOID;
}

sexp sexp_random_source_pseudo_randomize (sexp ctx, sexp self, sexp_sint_t n, sexp rs, sexp seed1, sexp seed2) {
  if (! sexp_random_source_p(self, rs))
    return sexp_type_exception(ctx, self, sexp_unbox_fixnum(sexp_opcode_arg1_type(self)), rs);
  if (! sexp_fixnump(seed1))
    return sexp_type_exception(ctx, self, SEXP_FIXNUM, seed1);
  if (! sexp_fixnump(seed2))
    return sexp_type_exception(ctx, self, SEXP_FIXNUM, seed2);
  sexp_seed_random(sexp_unbox_fixnum(seed1) ^ sexp_unbox_fixnum(seed2), rs);
  return SEXP_VOID;
}

sexp sexp_init_library (sexp ctx, sexp self, sexp_sint_t n, sexp env, const char* version, const sexp_abi_identifier_t abi) {
  sexp_uint_t rs_type_id;
  sexp_gc_var3(name, op, make_op);
  if (!(sexp_version_compatible(ctx, version, sexp_version)
        && sexp_abi_compatible(ctx, abi, SEXP_ABI_IDENTIFIER)))
    return SEXP_ABI_ERROR;
  sexp_gc_preserve3(ctx, name, op, make_op);

  name = sexp_c_string(ctx, "random-source", -1);
  op = sexp_register_type(ctx, name, SEXP_FALSE, SEXP_FALSE,
                          sexp_make_fixnum(sexp_offsetof_slot0),
                          ONE, ONE, ZERO, ZERO,
                          sexp_make_fixnum(sexp_sizeof_random), ZERO,
                          ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, NULL, NULL, NULL);
  if (sexp_exceptionp(op))
    return op;
  rs_type_id = sexp_type_tag(op);

  name = sexp_c_string(ctx, "random-source?", -1);
  op = sexp_make_type_predicate(ctx, name, sexp_make_fixnum(rs_type_id));
  name = sexp_intern(ctx, "random-source?", -1);
  sexp_env_define(ctx, env, name, op);

  make_op = sexp_define_foreign(ctx, env, "make-random-source", 0, sexp_make_random_source);
  if (sexp_opcodep(make_op))
    sexp_opcode_return_type(make_op) = sexp_make_fixnum(rs_type_id);
  op = sexp_define_foreign(ctx, env, "%random-integer", 2, sexp_rs_random_integer);
  if (sexp_opcodep(op))
    sexp_opcode_arg1_type(op) = sexp_make_fixnum(rs_type_id);
  op = sexp_define_foreign(ctx, env, "random-integer", 1, sexp_random_integer);
  if (sexp_opcodep(op))
    sexp_opcode_arg1_type(op) = sexp_make_fixnum(rs_type_id);
  op = sexp_define_foreign(ctx, env, "%random-real", 1, sexp_rs_random_real);
  if (sexp_opcodep(op))
    sexp_opcode_arg1_type(op) = sexp_make_fixnum(rs_type_id);
  op = sexp_define_foreign(ctx, env, "random-real", 0, sexp_random_real);
  if (sexp_opcodep(op))
    sexp_opcode_arg1_type(op) = sexp_make_fixnum(rs_type_id);
  op = sexp_define_foreign(ctx, env, "random-source-state-ref", 1, sexp_random_source_state_ref);
  if (sexp_opcodep(op))
    sexp_opcode_arg1_type(op) = sexp_make_fixnum(rs_type_id);
  op = sexp_define_foreign(ctx, env, "random-source-state-set!", 2, sexp_random_source_state_set);
  if (sexp_opcodep(op))
    sexp_opcode_arg1_type(op) = sexp_make_fixnum(rs_type_id);
  op = sexp_define_foreign(ctx, env, "random-source-randomize!", 1, sexp_random_source_randomize);
  if (sexp_opcodep(op))
    sexp_opcode_arg1_type(op) = sexp_make_fixnum(rs_type_id);
  op = sexp_define_foreign(ctx, env, "random-source-pseudo-randomize!", 3, sexp_random_source_pseudo_randomize);
  if (sexp_opcodep(op))
    sexp_opcode_arg1_type(op) = sexp_make_fixnum(rs_type_id);

  sexp_global(ctx, SEXP_G_RANDOM_SOURCE) = op = sexp_make_random_source(ctx, make_op, 0);
  name = sexp_intern(ctx, "default-random-source", -1);
  sexp_env_define(ctx, env, name, op);
  sexp_random_source_randomize(ctx, NULL, 0, op);

  sexp_gc_release3(ctx);
  return SEXP_VOID;
}
