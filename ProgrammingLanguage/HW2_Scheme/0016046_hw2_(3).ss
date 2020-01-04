(define root car)
(define ltree cadr)
(define rtree caddr)
(define emptytree? null?)
(define tree '(a (b (c () ()) (d () ())) (e (f () ()) (g () ()))))

(define inorder
(lambda (bt)
(if (emptytree? bt)
'()
(append (inorder (ltree bt)) (cons (root bt) (inorder (rtree bt)))))))

(define inorderAPS
(lambda (bt)
(let rev ( (bt bt) (acc '()) )
(if (emptytree? bt)
acc
(rev (ltree bt) (cons (root bt) (rev (rtree bt) acc))))))
