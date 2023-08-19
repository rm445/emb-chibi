
(define (vector-empty? vec)
  (zero? (uvector-length vec)))

(define (vector= . vecs)
  (let lp1 ((ls vecs))
    (or (null? ls)
        (null? (cdr ls))
        (let* ((v1 (car ls))
               (v2 (cadr ls))
               (len (uvector-length v1)))
          (and (= len (uvector-length v2))
               (let lp2 ((i 0))
                 (or (>= i len)
                     (and (= (uvector-ref v1 i)
                             (uvector-ref v2 i))
                          (lp2 (+ i 1)))))
               (lp1 (cdr ls)))))))

(define (list->uvector ls)
  (let ((res (make-uvector (length ls))))
    (do ((ls ls (cdr ls))
         (i 0 (+ i 1)))
        ((null? ls) res)
      (uvector-set! res i (car ls)))))

(define (reverse-list->uvector ls)
  (list->uvector (reverse ls)))

(define (vector . ls)
  (list->uvector ls))

(define (uvector-unfold f len seed)
  (let ((res (make-uvector len)))
    (let lp ((i 0) (seed seed))
      (if (>= i len)
          res
          (call-with-values (lambda () (f i seed))
            (lambda (x seed)
              (uvector-set! res i x)
              (lp (+ i 1) seed)))))))

(define (uvector-unfold-right f len seed)
  (let ((res (make-uvector len)))
    (let lp ((i (- len 1)) (seed seed))
      (if (< i 0)
          res
          (call-with-values (lambda () (f i seed))
            (lambda (x seed)
              (uvector-set! res i x)
              (lp (- i 1) seed)))))))

(define (vector-copy vec . o)
  (let ((start (if (pair? o) (car o) 0))
        (end (if (and (pair? o) (pair? (cdr o)))
                 (cadr o)
                 (uvector-length vec))))
    (uvector-unfold (lambda (i _) (values (uvector-ref vec (+ i start)) _))
                    (- end start)
                    #f)))

(define (vector-reverse-copy vec . o)
  (let ((start (if (pair? o) (car o) 0))
        (end (if (and (pair? o) (pair? (cdr o)))
                 (cadr o)
                 (uvector-length vec))))
    (uvector-unfold (lambda (i _) (values (uvector-ref vec (- end i 1)) _))
                    (- end start)
                    #f)))

(define (vector-concatenate vecs)
  (let* ((len (apply + (map uvector-length vecs)))
         (res (make-uvector len)))
    (let lp ((ls vecs) (i 0))
      (if (null? ls)
          res
          (let ((v-len (uvector-length (car ls))))
            (vector-copy! res i (car ls))
            (lp (cdr ls) (+ i v-len)))))))

(define (vector-append . vecs)
  (vector-concatenate vecs))

(define (vector-append-subvectors . o)
  (let lp ((ls o) (vecs '()))
    (if (null? ls)
        (vector-concatenate (reverse vecs))
        (lp (cdr (cddr ls))
            (cons (vector-copy (car ls) (cadr ls) (car (cddr ls)))
                  vecs)))))

(define (vector-fill! vec x . o)
  (let ((start (if (pair? o) (car o) 0))
        (end (if (and (pair? o) (pair? (cdr o)))
                 (cadr o)
                 (uvector-length vec))))
    (let lp ((i (- end 1)))
      (when (>= i start)
        (uvector-set! vec i x)
        (lp (- i 1))))))

(define (vector-swap! vec i j)
  (let ((tmp (uvector-ref vec i)))
    (uvector-set! vec i (uvector-ref vec j))
    (uvector-set! vec j tmp)))

(define (vector-reverse! vec . o)
  (let lp ((left (if (pair? o) (car o) 0))
           (right (- (if (and (pair? o) (pair? (cdr o)))
                         (cadr o)
                         (uvector-length vec))
                     1)))
    (cond
     ((>= left right) (if #f #f))
     (else
      (vector-swap! vec left right)
      (lp (+ left 1) (- right 1))))))

(define (vector-copy! to at from . o)
  (let* ((start (if (pair? o) (car o) 0))
         (end (if (and (pair? o) (pair? (cdr o)))
                  (cadr o)
                  (uvector-length from)))
         (limit (min end (+ start (- (uvector-length to) at)))))
    (if (<= at start)
        (do ((i at (+ i 1)) (j start (+ j 1)))
            ((>= j limit))
          (uvector-set! to i (uvector-ref from j)))
        (do ((i (+ at (- end start 1)) (- i 1))
             (j (- limit 1) (- j 1)))
            ((< j start))
          (uvector-set! to i (uvector-ref from j))))))

(define (vector-reverse-copy! to at from . o)
  (let ((start (if (pair? o) (car o) 0))
        (end (if (and (pair? o) (pair? (cdr o)))
                 (cadr o)
                 (uvector-length from))))
    (vector-copy! to at from start end)
    (vector-reverse! to at (+ at (- end start)))))

(define (vector-take vec n)
  (vector-copy vec 0 n))

(define (vector-take-right vec n)
  (vector-copy vec (- (uvector-length vec) n)))

(define (vector-drop vec n)
  (vector-copy vec n))

(define (vector-drop-right vec n)
  (vector-copy vec 0 (- (uvector-length vec) n)))

(define (vector-segment vec n)
  (let ((len (uvector-length vec)))
    (let lp ((i 0) (res '()))
      (if (>= i len)
          (reverse res)
          (lp (+ i n)
              (cons (vector-copy vec i (min (+ i n) len)) res))))))

(define (vector-fold kons knil vec1 . o)
  (let ((len (uvector-length vec1)))
    (if (null? o)
        (let lp ((i 0) (acc knil))
          (if (>= i len)
              acc
              (lp (+ i 1) (kons acc (uvector-ref vec1 i)))))
        (let lp ((i 0) (acc knil))
          (if (>= i len)
              acc
              (lp (+ i 1)
                  (apply kons acc (uvector-ref vec1 i)
                         (map (lambda (v) (uvector-ref v i)) o))))))))

(define (vector-fold-right kons knil vec1 . o)
  (let ((len (uvector-length vec1)))
    (if (null? o)
        (let lp ((i (- len 1)) (acc knil))
          (if (negative? i)
              acc
              (lp (- i 1) (kons acc (uvector-ref vec1 i)))))
        (let lp ((i (- len 1)) (acc knil))
          (if (negative? i)
              acc
              (lp (- i 1)
                  (apply kons acc (uvector-ref vec1 i)
                         (map (lambda (v) (uvector-ref v i)) o))))))))

(define (vector-map! f vec1 . o)
  (apply vector-fold
         (lambda (i . o)
           (uvector-set! vec1 i (apply f o))
           (+ i 1))
         0 vec1 o))

(define (vector-map f vec1 . o)
  (let ((res (vector-copy vec1)))
    (apply vector-map! f res o)
    res))

(define (vector-for-each f vec1 . o)
  (apply vector-fold (lambda (acc . o) (apply f o) acc) (if #f #f) vec1 o))

(define (vector-count f vec1 . o)
  (apply vector-fold
         (lambda (sum . o) (+ sum (if (apply f o) 1 0)))
         0 vec1 o))

(define (vector-cumulate f knil vec)
  (let* ((len (uvector-length vec))
         (res (make-uvector len)))
    (let lp ((i 0) (acc knil))
      (if (>= i len)
          res
          (let ((acc (f acc (uvector-ref vec i))))
            (uvector-set! res i acc)
            (lp (+ i 1) acc))))))

(define (vector-index pred vec)
  (let ((len (uvector-length vec)))
    (let lp ((i 0))
      (cond ((>= i len) #f)
            ((pred (uvector-ref vec i)) i)
            (else (lp (+ i 1)))))))

(define (vector-index-right pred vec)
  (let lp ((i (- (uvector-length vec) 1)))
    (cond ((negative? i) #f)
          ((pred (uvector-ref vec i)) i)
          (else (lp (- i 1))))))

(define (vector-skip pred vec)
  (vector-index (lambda (x) (not (pred x))) vec))

(define (vector-skip-right pred vec)
  (vector-index-right (lambda (x) (not (pred x))) vec))

(define (vector-take-while vec pred)
  (vector-copy vec 0 (or (vector-skip pred vec)
                         (uvector-length vec))))

(define (vector-take-while-right vec pred)
  (vector-copy vec (or (vector-skip-right pred vec) 0)))

(define (vector-drop-while vec pred)
  (vector-copy vec (or (vector-index pred vec) 0)))

(define (vector-drop-while-right vec pred)
  (vector-copy vec 0 (or (vector-index-right pred vec)
                         (uvector-length vec))))

(define (vector-binary-search vec value cmp)
  (let lp ((lo 0) (hi (- (uvector-length vec) 1)))
    (and (<= lo hi)
         (let* ((mid (quotient (+ lo hi) 2))
                (x (uvector-ref vec mid))
                (y (cmp value x)))
           (cond
            ((< y 0) (lp lo (- mid 1)))
            ((> y 0) (lp (+ mid 1) hi))
            (else mid))))))

(define (vector-any pred? vec1 . o)
  (let ((len (apply min (uvector-length vec1)
                    (map uvector-length o))))
    (let lp ((i 0))
      (and (< i len)
           (or (apply pred? (uvector-ref vec1 i)
                      (map (lambda (v) (uvector-ref v i)) o))
               (lp (+ i 1)))))))

(define (vector-every pred? vec1 . o)
  (let ((len (apply min (uvector-length vec1)
                    (map uvector-length o))))
    (let lp ((i 0))
      (let ((x (apply pred? (uvector-ref vec1 i)
                      (map (lambda (v) (uvector-ref v i)) o))))
        (if (= i (- len 1))
            x
            (and x (lp (+ i 1))))))))

(define (vector-partition pred? vec)
  (let* ((len (uvector-length vec))
         (res (make-uvector len)))
    (let lp ((i 0) (left 0) (right (- len 1)))
      (cond
       ((= i len)
        (if (< left len)
            (vector-reverse! res left))
        (values res left))
       (else
        (let ((x (uvector-ref vec i)))
          (cond
           ((pred? x)
            (uvector-set! res left x)
            (lp (+ i 1) (+ left 1) right))
           (else
            (uvector-set! res right x)
            (lp (+ i 1) left (- right 1))))))))))

(define (vector-filter pred vec)
  (list->uvector
   (reverse
    (vector-fold (lambda (ls elt) (if (pred elt) (cons elt ls) ls))
                 '() vec))))

(define (vector-remove pred vec)
  (vector-filter (lambda (x) (not (pred x))) vec))

(define (reverse-vector->list vec . o)
  (let ((vec (if (pair? o) (apply vector-copy vec o) vec)))
    (vector-fold (lambda (ls x) (cons x ls)) '() vec)))

(define (reverse-list->vector ls)
  (list->uvector (reverse ls)))

(define (uvector->list vec . o)
  (reverse (apply reverse-vector->list vec o)))

(define (uvector->vector vec . o)
  (list->vector (apply uvector->list vec o)))

(define (vector->uvector vec . o)
  (list->uvector (apply vector->list vec o)))

(define make-vector-generator
  (let ((eof (read-char (open-input-string ""))))
    (lambda (vec)
      (let ((i 0) (len (uvector-length vec)))
        (lambda ()
          (if (>= i len)
              eof
              (let ((res (uvector-ref vec i)))
                (set! i (+ i 1))
                res)))))))

(define write-vector write)
