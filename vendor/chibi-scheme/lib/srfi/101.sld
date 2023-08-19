
(define-library (srfi 101)
  (import (scheme base)
          (scheme case-lambda)
          (srfi 1)
          (srfi 125)
          (rename (srfi 151)
                  (arithmetic-shift bitwise-arithmetic-shift)))
  (export (rename ra:quote quote)
          (rename ra:pair? pair?)
          (rename ra:cons cons)
          (rename ra:car car)
          (rename ra:cdr cdr)
          (rename ra:caar caar)
          (rename ra:cadr cadr)
          (rename ra:cddr cddr)
          (rename ra:cdar cdar)
          (rename ra:caaar caaar)
          (rename ra:caadr caadr)
          (rename ra:caddr caddr)
          (rename ra:cadar cadar)
          (rename ra:cdaar cdaar)
          (rename ra:cdadr cdadr)
          (rename ra:cdddr cdddr)
          (rename ra:cddar cddar)
          (rename ra:caaaar caaaar)
          (rename ra:caaadr caaadr)
          (rename ra:caaddr caaddr)
          (rename ra:caadar caadar)
          (rename ra:cadaar cadaar)
          (rename ra:cadadr cadadr)
          (rename ra:cadddr cadddr)
          (rename ra:caddar caddar)
          (rename ra:cdaaar cdaaar)
          (rename ra:cdaadr cdaadr)
          (rename ra:cdaddr cdaddr)
          (rename ra:cdadar cdadar)
          (rename ra:cddaar cddaar)
          (rename ra:cddadr cddadr)
          (rename ra:cddddr cddddr)
          (rename ra:cdddar cdddar)
          (rename ra:null? null?)
          (rename ra:list? list?)
          (rename ra:list list)
          (rename ra:make-list make-list)
          (rename ra:length length)
          (rename ra:append append)
          (rename ra:reverse reverse)
          (rename ra:list-tail list-tail)
          (rename ra:list-ref list-ref)
          (rename ra:list-set list-set)
          (rename ra:list-ref/update list-ref/update)
          (rename ra:map map)
          (rename ra:for-each for-each)
          (rename ra:random-access-list->linear-access-list
                  random-access-list->linear-access-list)
          (rename ra:linear-access-list->random-access-list
                  linear-access-list->random-access-list))
  (include "101.scm"))
