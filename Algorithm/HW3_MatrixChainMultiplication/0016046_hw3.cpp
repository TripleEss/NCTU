#include<iostream>
#include <vector>

using namespace std;


int** mcm(const vector<int>& p)
{
    int n=p.size()-1;
    int **m,**s;
    int t=0;
    int q;

    m=new int*[n+1];
    s=new int*[n+1];
    for(int i=0; i<n+1; i++)
    {
        m[i]=new int[n];
        s[i]=new int[n];
    }

    for(int i=1; i<=n; i++)
    {
        m[i][i]=0;
    }
    for(int l=2; l<=n; l++)
    {
        for(int i=1; i<=n-l+1; i++)
        {
            int j=i+l-1;
            m[i][j]=0;
            for(int k=i; k<=j-1; k++)
            {
                q=m[i][k]+m[k+1][j]+p.at(i-1)*p.at(k)*p.at(j);
                if(q<m[i][j] || m[i][j]==0)
                {
                    m[i][j]=q;
                    s[i][j]=k;
                }
            }
        }
    }

    cout<<m[1][n]<<" scalar multiplications"<<endl;
    //cout<<t<<" optimal way to multiply"<<endl;
    return s;
};
void print(int** s, int i, int j)
{
    if(i==j)
        cout<<"A"<<i;
    else
    {
        cout<<"(";
        print(s,i,s[i][j]);
        print(s,s[i][j]+1,j);
        cout<<")";
    }

}
void testing_mcm(const vector<int>& p)
{
    static int test=0;
    cout << "Test " << ++test << " ...\n";
    print(mcm(p),1,p.size()-1);
    cout << endl;
}
int main()
{
    testing_mcm({30,35,15,5,10,20,25});
    testing_mcm({3,3,3,3,3});
    testing_mcm({2,2,3,2,2,3,3,2});
}
