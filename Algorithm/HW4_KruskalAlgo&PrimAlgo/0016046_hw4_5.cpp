#include<iostream>
#include<vector>

#define Gsize 9

using namespace std;


struct node
{
    char  n;
    node* p;
    int   rank;
};

void make_set(node* x)
{
    x->p=x;
    x->rank=0;
}

void link_node(node* x, node* y)
{
    if(x->rank > y->rank)
        y->p=x;
    else
    {
        x->p=y;
        if(x->rank==y->rank)
        {
            y->rank=y->rank+1;
        }
    }
}

node* find_set(node* x)
{
    if(x->p!=x)
        x->p=find_set(x->p);

    return x->p;
}

void union_set(node* x, node* y)
{
    link_node(find_set(x), find_set(y));
}

void print_MST(vector<char> A[3])
{
    for(int i=0; i<A[0].size(); i++)
    {
        cout<<"("<<(char)A[0].at(i)<<","<<(char)A[1].at(i)/*<<","<<(int)A[2].at(i)*/<<") ";
    }
    cout<<endl<<endl;
}

void kruskal(int (*G)[Gsize])
{
    vector<char> A[2];          // edge: (A[0],A[1])
    vector<char> sort_edge[3];  // edge: (sort_edge[0],sort_edge[1]) ; weight: sort_edge[2]

    node *N;
    N=new node[Gsize];

    for(int i=0; i<Gsize; i++)
    {
        N[i].n=(char)(i+97);
        make_set(&N[i]);
    }

    // push non-zero weight edge
    for(int i=0; i<Gsize; i++)
    {
        for(int j=i; j<Gsize; j++)
        {
            if(G[i][j]!=0)
            {
                sort_edge[0].push_back(i+97);
                sort_edge[1].push_back(j+97);
                sort_edge[2].push_back(G[i][j]);
            }
        }
    }
    //print_MST(sort_edge);
    // sort non-zero weight edge

    for (int i=sort_edge[0].size()-1; i>=0; i--)
    {
        for(int j=0; j<=i-1; j++)
        {
            if(sort_edge[2].at(j)<sort_edge[2].at(j+1))
            {
                int tmp=sort_edge[2].at(j);
                sort_edge[2].at(j)=sort_edge[2].at(j+1);
                sort_edge[2].at(j+1)=tmp;

                tmp=sort_edge[1].at(j);
                sort_edge[1].at(j)=sort_edge[1].at(j+1);
                sort_edge[1].at(j+1)=tmp;

                tmp=sort_edge[0].at(j);
                sort_edge[0].at(j)=sort_edge[0].at(j+1);
                sort_edge[0].at(j+1)=tmp;
            }
        }
    }
    //print_MST(sort_edge);

    while(!sort_edge[0].empty())
    {
        char u=sort_edge[0].back();
        char v=sort_edge[1].back();

        if(find_set(&N[u-97])!=find_set(&N[v-97]))
        {
            A[0].push_back(u);
            A[1].push_back(v);
            union_set(&N[u-97],&N[v-97]);
        }

        sort_edge[0].pop_back();
        sort_edge[1].pop_back();
        sort_edge[2].pop_back();
    }

    print_MST(A);
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

    cout << "Kruskal's algorithm for graph in lecture \n";
    kruskal(G1);

    int G2[9][9]={{ 0, 4, 0, 0, 0, 0, 0, 8, 0},
                  { 4, 0, 8, 0, 0, 0, 0,11, 0},
                  { 0, 8, 0, 7, 0, 4, 0, 0, 2},
                  { 0, 0, 7, 0, 9,14, 0, 0, 0},
                  { 0, 0, 0, 9, 0,10, 0, 0, 0},
                  { 0, 0, 4,14,10, 0, 2, 0, 0},
                  { 0, 0, 0, 0, 0, 2, 0, 1, 6},
                  { 8,11, 0, 0, 0, 0, 1, 0, 7},
                  { 0, 0, 2, 0, 0, 0, 6, 7, 0}};

    cout << "Kruskal's algorithm for graph in book \n";
    kruskal(G2);
}
