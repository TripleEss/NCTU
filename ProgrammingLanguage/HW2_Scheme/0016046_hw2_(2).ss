(define power
(lambda (n)
`(lambda (x)
,(let loop ((n n))
(if (= n 0) `1 `(* ,@(let loopx ((n n)) (if (= n 0) '() (cons 'x (loopx (- n 1)))))))))))
