!0016046_hw1_1
!FORTRAN 95
!mergesort

program sorting

implicit none

integer, dimension(10)::a

print*,"Enter 10 integers"
read*,a

call msort(a)

print*,"The 10 integers in sorted order"
print*,a

contains

recursive subroutine msort(a)

   integer, dimension(:)::a 
   
   integer::i              !index of left part   
   integer::amid   
   integer::j              !index of right part
   
                           !print*,"befor msort a:", a
   if(size(a,1)==1)then    
      return

   else
      i=lbound(a,1)        !left_part(lower-bound of a:middle of a)
      amid=(lbound(a,1)+ubound(a,1))/2
      j=amid+1             !right_part((middle of a)+1:upper-bound of a)
      
      call msort(a(lbound(a,1):amid))
      call msort(a(amid+1:ubound(a,1)))
      call merge(a)
   
   end if

end subroutine msort
  

subroutine merge(a)
   
   integer, dimension(:)::a
   integer, dimension(lbound(a,1):ubound(a,1))::tempa  !same size as a
   
   integer::n             !index of tempa
   integer::i             !index of left part
   integer::amid    
   integer::j             !index of right part

   n=lbound(tempa,1)
   i=lbound(a,1)
   amid=(lbound(a,1)+ubound(a,1))/2
   j=amid+1

   do while(i<=amid .and. j<=ubound(a,1))    !left and right parts are not empty
      if(a(i)<=a(j))then
         tempa(n)=a(i)
         i=i+1
         n=n+1
      else
         tempa(n)=a(j)
         j=j+1
         n=n+1
      end if
   end do

   do while(i<=amid)                         !only left part is not empty
      tempa(n)=a(i)
      i=i+1
      n=n+1
   end do

   do while(j<=ubound(a,1))                  !only right part is not empty
      tempa(n)=a(j)
      j=j+1
      n=n+1
   end do

  !do n=lbound(a,1),ubound(a,1),1
   a(:)=tempa(:)                             !copy sorted tempa to unsorted a
  !end do

end subroutine merge


end program sorting
