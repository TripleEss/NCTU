//#include <iostream>

//using namespace std;

//#define k 20

void isort(int* a,int n)
{
    for (int j=1;j<n;j++)
    {
        int key=a[j],i=j-1;

        while (i>=0&&a[i]>key)
        {
            a[i+1]=a[i];
            i--;
        }
        a[i+1]=key;
    }
}
int main()
{
    int a[k];

    for (int i=0;i<k;i++) a[i]=k-i;

    isort(a,k);

    //for (int i=0;i<k;i++)cout << a[i] << ' ';
}
