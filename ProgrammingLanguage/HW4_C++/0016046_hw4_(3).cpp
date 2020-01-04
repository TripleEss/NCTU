#include <iostream>
using namespace std;
int k;
int f() // compute k!, where k is global
{
    int r=1;
    for (int i=2;i<=k;i++) r*=i;
    //cout<<r<<endl;
    return r;
}

int c(int m,int n) // compute m!/(n!*(m-n)!)
{
    return ((k=m/*,cout<<k<<endl*/),(f()))/( ((k=n/*,cout<<k<<endl*/),(f()))*((k=m-n/*,cout<<k<<endl*/),(f())) ); //*
}

int main() { cout << c(5,2) << endl; }

/*
(3a)
6!/(2!*2!*2!)=90
*/

/*
(3b)
clan g++: 10
VC++/GNU C++: 0
arugment evaluation order
VC++/GNU C++: evaluate all k=... then evaluate f(), so all f() produce (m-n)!=3!=6 -> 6/(6*6)=1/6=(int)0
clan g++: evaluate tuple (k=...,f()) together
*/
