//#include <iostream>

//using namespace std;

//#define n 20

static int _level=1;

void merge(int* a,int* b,int l,int m,int h, int level)
{
    //static int b[n];

    //if()
    int i=l,j=m+1,k=l;
    //while (i<=m) b[k++]=a[i++];
    //while (j<=h) b[k++]=a[j++];
/*
cout<<"b: "; for(int i=0;i<n;i++){if(b[i]<10)cout<<" ";cout<<b[i]<<' ';}  cout<<endl;
cout<<"a: "; for(int i=0;i<n;i++){if(a[i]<10)cout<<" ";cout<<a[i]<<' ';}  cout<<endl;
cout<<endl;
*/

    //i=l;j=m+1;k=l;
    if(level%2==_level%2){
    while (i<=m&&j<=h)
        if (b[i]<b[j])
            a[k++]=b[i++];
        else
            a[k++]=b[j++];

    while (i<=m) a[k++]=b[i++];
    while (j<=h) a[k++]=b[j++];
    }
    else{
    while (i<=m&&j<=h)
        if (a[i]<a[j])
            b[k++]=a[i++];
        else
            b[k++]=a[j++];

    while (i<=m) b[k++]=a[i++];
    while (j<=h) b[k++]=a[j++];
    }

/*
cout<<"b: "; for(int i=0;i<n;i++){if(b[i]<10)cout<<" ";cout<<b[i]<<' ';}  cout<<endl;
cout<<"a: "; for(int i=0;i<n;i++){if(a[i]<10)cout<<" ";cout<<a[i]<<' ';}  cout<<endl;
cout<<"----------------------------------------------"<<endl;
*/
};

void msort(int* a,int* b,int l, int h, int level)
{
    if(l<h)
    {
        int m=(l+h)/2;
        msort(a,b,l,m,level-1);
        msort(a,b,m+1,h,level-1);
        merge(a,b,l,m,h,level);
        //cout<<"a:"; for(int i=0;i<n;i++){if(a[i]<10)cout<<" ";cout<<a[i]<<' ';} cout<<"; level:"<<level<<endl;
    }
};

void isort(int* a,int k)
{
    int l,h,m;

    for (int j=1;j<k;j++)
    {
        int key=a[j];

        for(l=0, h=j-1; l<=h; )
        {
            m=(l+h)/2;

            if(a[m]>key)
                h=m-1;
            else if(a[m]<key)
                l=m+1;
            else
                break;
        }
        for(int i=j; i>l; i--)
            a[i]=a[i-1];
        a[l]=key;
    }
};

int main()
{
    int a[n],b[n];
    int level=1;for(int i=n; i>0; i/=2, level++,_level++);//cout<<level<<_level<<endl;
    for(int i=0;i<n;i++)
    {b[i]=n-i;a[i]=n-i;}

    if(n<15)
        isort(a,n);
    else
        msort(a,b,0,n-1,level);

    //for (int i=0;i<n;i++)cout << a[i] << ' ';
}
