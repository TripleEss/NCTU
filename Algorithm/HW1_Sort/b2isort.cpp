//#include <iostream>

//using namespace std;

//#define k 20

void isort(int* a,int n)
{
    int l,h,m;

    for (int j=1;j<n;j++)
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
}
int main()
{
    int a[k];

    for (int i=0;i<k;i++) a[i]=k-i;

    isort(a,k);

    //for (int i=0;i<k;i++)cout << a[i] << ' ';
}
