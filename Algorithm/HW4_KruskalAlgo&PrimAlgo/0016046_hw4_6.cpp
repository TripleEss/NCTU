#include<iostream>
#include<queue>

#define Gsize 9

using namespace std;

class priority_queue;

struct compare
{
    bool operator()(const int& l, const int& r)
    {
        return l > r;
    }
};

void print_MST(vector<int> A[3])
{
    for(int i=0; i<A[0].size(); i++)
    {
        cout<<"("<<(char)A[0].at(i)<<","<<(char)A[1].at(i)/*<<","<<(int)A[2].at(i)*/<<") ";
    }
    cout<<endl<<endl;
}

void decrease_key(vector<int> Q2, vector<int> Q0, char x, int v)
{
    for(int i=0; i<Q2.size(); i++)
    {
        if(Q0.at(i)==(int)x)
        {
            Q2.at(i)==v;
            break;
        }
    }
}

void prim(int (*G)[Gsize])
{
    vector<int> Q[3];   //Q[0]=u ; Q[1]=u.p ; Q[2]=u.key

    vector<char> A[2];

    for(int i=0; i<Gsize; i++)
    {
        Q[0].push_back(i+97);
        Q[1].push_back(NULL);
        Q[2].push_back(1e9);
    }

    decrease_key(Q[2], Q[0], 'a', 0);

    for (int i=Q[2].size()-1; i>=0; i--)
    {
        for(int j=0; j<=i-1; j++)
        {
            if(Q[2].at(j)<Q[2].at(j+1))
            {
                int tmp=Q[2].at(j);
                Q[2].at(j)=Q[2].at(j+1);
                Q[2].at(j+1)=tmp;

                tmp=Q[1].at(j);
                Q[1].at(j)=Q[1].at(j+1);
                Q[1].at(j+1)=tmp;

                tmp=Q[0].at(j);
                Q[0].at(j)=Q[0].at(j+1);
                Q[0].at(j+1)=tmp;
            }
        }
    }

    while(!Q[0].empty())
    {
        char u=Q[0].back();
        Q[0].pop_back();
        Q[1].pop_back();
        Q[2].pop_back();
        for(int i=0; i<Gsize; i++)
        {
            if(G[u-97][i]!=0)
            {
                for(int j=0; j<Q[0].size(); j++)
                {
                    if(Q[0].at(j)==i+97 && G[u-97][i]<Q[2].at(j))
                    {
                        Q[1].at(j)=u;
                        decrease_key(Q[2], Q[0], Q[1].at(j), G[u-97][i]);
                    }
                }
            }
        }
    }

    print_MST(Q);
}

int main()
{
    //   a  b  c  d  e  f  g  h  i
    // a -- 10 12 -- -- -- -- -- --
    // b 10 -- 09 08 -- -- -- -- --
    // c 12 09 -- -- 03 01 -- -- --
    // d -- 08 -- -- 07 -- 08 05 --
    // e -- -- 03 07 -- 03 -- -- --
    // f -- -- 01 -- 03 -- -- 06 --
    // g -- -- -- 08 -- -- -- 09 02
    // h -- -- -- 05 -- 06 09 -- 11
    // i -- -- -- -- -- -- 02 11 --

    int G1[9][9]={{ 0,10,12, 0, 0, 0, 0, 0, 0},
                  {10, 0, 9, 8, 0, 0, 0, 0, 0},
                  {12, 9, 0, 0, 3, 1, 0, 0, 0},
                  { 0, 8, 0, 0, 7, 0, 8, 5, 0},
                  { 0, 0, 3, 7, 0, 3, 0, 0, 0},
                  { 0, 0, 1, 0, 3, 0, 0, 6, 0},
                  { 0, 0, 0, 8, 0, 0, 0, 9, 2},
                  { 0, 0, 0, 5, 0, 6, 9, 0,11},
                  { 0, 0, 0, 0, 0, 0, 2,11, 0}};

    cout << "Prim's algorithm for graph in lecture \n";
    prim(G1);

    int G2[9][9]={{ 0, 4, 0, 0, 0, 0, 0, 8, 0},
                  { 4, 0, 8, 0, 0, 0, 0,11, 0},
                  { 0, 8, 0, 7, 0, 4, 0, 0, 2},
                  { 0, 0, 7, 0, 9,14, 0, 0, 0},
                  { 0, 0, 0, 9, 0,10, 0, 0, 0},
                  { 0, 0, 4,14,10, 0, 2, 0, 0},
                  { 0, 0, 0, 0, 0, 2, 0, 1, 6},
                  { 8,11, 0, 0, 0, 0, 1, 0, 7},
                  { 0, 0, 2, 0, 0, 0, 6, 7, 0}};

    cout << "Prim's algorithm for graph in book \n";
    prim(G2);
}


