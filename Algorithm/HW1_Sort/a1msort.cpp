//#include <iostream>

//using namespace std;

//#define n 20

void merge(int* a,int l,int m,int h)
{
    static int b[n];
    int i=l,j=m+1,k=l;

    while (i<=m&&j<=h)
        if (a[i]<a[j])
            b[k++]=a[i++];
        else b[k++]=a[j++];

    while (i<=m) b[k++]=a[i++];
    while (j<=h) b[k++]=a[j++];

    for (i=l;i<=h;i++) a[i]=b[i];
}

void msort(int* a,int l, int h)
{
    if(l<h)
    {
        int m=(l+h)/2;
        msort(a,l,m);
        msort(a,m+1,h);
        merge(a,l,m,h);
    }
}

int main()
{
    int a[n];

    for(int i=0;i<n;i++)
        a[i]=n-i;

    msort(a,0,n-1);

    //for (int i=0;i<n;i++)cout << a[i] << ' ';
}
