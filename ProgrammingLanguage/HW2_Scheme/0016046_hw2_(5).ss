(define root car)
(define ltree cadr)
(define rtree caddr)
(define emptytree? null?)
(define tree '(a (b (c () ()) (d () ())) (e (f () ()) (g () ()))))
(define bombtree '(a (b (c () ()) (d () ())) (e (* () ()) (g () ()))))

(define inorder
(lambda (bt)
(if (emptytree? bt)
'()
(append (inorder (ltree bt)) (cons (root bt) (inorder (rtree bt)))))))

(define inorderCC
(lambda (bt)
(call/cc (lambda (exit)
(let loop ((bt bt))
(cond ((emptytree? bt) '())
((eq? (root bt) '*) (exit 'bomb!))
(else (append (loop (ltree bt)) (cons (root bt) (loop (rtree bt)))))))))))
