(define f0 (lambda args args))
(define f1 (lambda (x . args) x))
(define g1 (lambda (x . args) args))

;(2a)
(define my+ 
  (lambda args
    (if (NULL? args) 0
        (+ (car args) (apply my+ (cdr args))))))

;(apply my+ '())
;(apply my+ '(2 3/4 5.6))

;(2b)
(define mymax
  (lambda (x . args)
    (if (NULL? args) x
        (max x (apply mymax args)))))

;(apply mymax '())
;(apply mymax '(3))
;(apply mymax '(2 3/4 5.6)
   
;(2c)
;type-safety: C/C++ < Scheme
;Scheme looks args as a list
;it can access every variable argument by built-in list procedure.
;C/C++ uses va_list to hold information about variable arguments 
;but needs to know every argument's type (or, memory size) to access correct variable
;if not, it will be incorrect call
;forethermore, C/C++ allow coercions may cause erroneous call can not be detected.