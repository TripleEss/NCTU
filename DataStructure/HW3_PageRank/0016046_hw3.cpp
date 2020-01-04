#include<iostream>
#include<fstream>
#include<string>
using namespace std;

int char_to_int(const char* c)      //convert (char*)argv[2]=k to (int)value=k
{
    int length;      //length of char* c;
    int value=0;
    int base=1;
    for(length=0; c[length]!='\0'; length++);
    for(int i=length-1; i>=0; i--)
    {
        int temp=(int)c[i]-48;      //(char)'0'=(int)48 ~ (char)'9'=(int)57
        value+=temp*base;      //c[0]*10^(length-1)..+c[length-2]*10^1+c[length-1]*10^0
        base*=10;
    }

    return value;
}
double char_to_double(const char* c)
{
    int length;      //length of char* c;
    double value=0;
    double base=0.1;
    for(length=0; c[length]!='\0'; length++);
    for(int i=2; i<length; i++)
    {
        int temp=(int)c[i]-48;      //(char)'0'=(int)48 ~ (char)'9'=(int)57
        value+=temp*base;      //c[0]*10^(length-1)..+c[length-2]*10^1+c[length-1]*10^0
        base/=10;
    }

    return value;
}

int main(int argc, char* argv[])
{
    ifstream fin(argv[1]);
    //ifstream fin("test_hw3");
    int iteration=char_to_int(argv[2]);
    double dampingfactor=char_to_double(argv[3]);
    int k=char_to_int(argv[4]);

    string str;
    fin>>str;
    int n=char_to_int(str.c_str());

    bool** network;
    network=new bool*[n+1];
    for(int i=0; i<n+1; i++)
    {
        network[i]=new bool;
    }

    for(int i=0; i<n+1; i++)
    {
        for(int j=0; j<n+1; j++)
            network[i][j]=false;
    }
    int* count=new int[n+1];
    double* pagerank=new double[n+1];
    double* pagerank2=new double[n+1];
    for(int i=0; i<n+1; i++)
    {
        count[i]=0;
        pagerank[i]=1;
        pagerank2[i]=1-dampingfactor;
    }

    while(getline(fin,str))
    {
        network[str[0]-48][str[2]-48]=true;
        count[str[0]-48]++;
    }

    for(int i=1; i<=iteration; i++)
    {
        for(int i=1; i<n+1; i++)
        {
            for(int j=1; j<n+1; j++)
            {
                if(network[i][j]==true)
                {
                    pagerank2[i]+=pagerank[j]*dampingfactor;
                }
            }
        }

        for(int i=1; i<n+1; i++)
        {
            pagerank[i]=pagerank2[i];
        }
    }

    double** topk=new double*[n+1];
    for(int i=1; i<n+1; i++)
    {
        topk[i]=new double[2];
        topk[i][0]=i;
        topk[i][1]=pagerank[i];
    }

    for(int i=1; i<n; i++)
    {
       for(int j=1; j<n+1-j; j++)
       {
           if(topk[j][1]<topk[j+1][1])
           {
                int tmp[2];
                tmp[0]=topk[j][0];
                tmp[1]=topk[j][1];
                topk[j][0]=topk[j+1][0];
                topk[j][1]=topk[j+1][1];
                topk[j+1][0]=tmp[0];
                topk[j+1][1]=tmp[1];
           }
       }
    }
    for(int i=1; i<=k; i++)
    {
        cout<<topk[i][0]<<"\t"<<topk[i][1]<<endl;
    }

    delete network;

    delete count;
    delete pagerank;
    delete pagerank2;
}
