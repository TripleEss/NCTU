(define S (lambda (x) (lambda (y) (lambda (z) ((x z) (y z))))))
(define K (lambda (x) (lambda (y) x)))
(define I ((S K) K))
(define c+ (lambda (x) (lambda (y) (+ x y))))
(define c* (lambda (x) (lambda (y) (* x y))))

(define compose '(lambda (f) (lambda (g) (lambda (x) (f (g x))))))
(define sq '(lambda (x) ((c* x) x)))
(define db '(lambda (x) ((c+ x) x)))
(define expr `(((,compose ,sq) ,db) 5))

(define compile
	(lambda (expr)
		(cond ((or (symbol? expr)(number? expr)) expr)
			((eq? (car expr) 'lambda) (abstract (cadr expr) (compile (caddr expr))))
			(else (cons (compile (car expr)) `(,(compile (cadr expr)))))
		)
	)
)

(define abstract
	(lambda (id1 id2)
		(cond
			((eq? (car id1) id2) 'I)
			((list? id2) `((S ,(abstract id1 (car id2))) ,(abstract id1 (cadr id2))))
			(else `(K ,id2))
		)
	)
)

;(compile expr)

;(eval (compile expr))
