(define root car)
(define ltree cadr)
(define rtree caddr)
(define emptytree? null?)
(define tree '(a (b (c () ()) (d () ())) (e (f () ()) (g () ()))))
(define bombtree '(a (b (c () ()) (d () ())) (e (* () ()) (g () ()))))

(define inorderCPS
(lambda (bt)
(let loop ((bt bt) (c (lambda (v) v))) 
(cond ((emptytree? bt) (c '()) )
((eq? (root bt) '*) 'bomb!)
(else  (loop (ltree bt) (lambda (v2) (loop (rtree bt) (lambda (v1) (c (append v2 (cons (root bt) v1))))))))))))

