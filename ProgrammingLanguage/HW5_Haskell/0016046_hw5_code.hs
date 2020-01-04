--(5a)
interleave x xs=[ (take i xs)++[x]++(drop i xs) | i<-[0,1..(length xs)]]
--(5b)
permutation []=[[]]
permutation (x:xs)=[ zs | ys<-permutation xs, zs<-interleave x ys]
--(5c)
nondecreasing []=True
nondecreasing xs=and [ x<=y | (x,y)<-zip xs (tail xs)]
--(5d)
sort xs=[p | p<-permutation xs,nondecreasing p]
--(6b)
pow2s6b1 x=1:[x*2 | x<-(pow2s6b1 x)]
pow2s6b2 x=x:pow2s6b2(x*2)
--(6d)
pow2s6c1=1:[x*2 | x<-pow2s6c1]
pow2s6c2=1:map (*2) pow2s6c2
--(7a)
merge xs []=xs
merge [] ys=ys
merge (x:xs) (y:ys) | x<y = x:merge xs (y:ys)
		    | x>y = y:merge (x:xs) ys
		    | otherwise = x:merge xs ys

--(7b)
ham a b c=1:(merge [a*x|x<-[1..]] (merge [b*x|x<-[1..]] [c*x|x<-[1..]]))
