#include<iostream>
#include<cstdarg>
#include<cstring>

using namespace std;

double max(const char* typestr,double x,...)
{
    va_list arg_ptr;
    va_start(arg_ptr,strlen(typestr));
    double max=x;
    double y=0;
    for(int i=1; i<=strlen(typestr); i++)
    {
        if(typestr[i-1]=='d')
        {
            y=va_arg(arg_ptr,int);
            if(y>max)
                max=y;
        }
        if(typestr[i-1]=='f')
        {
            y=va_arg(arg_ptr,double);
            if(y>max)
                max=y;
        }
    }

    return max;
}
int main()
{
    //cout<<max("")<<endl;
    cout<<max( "ddffdd",29,'a',(short)255,34.56f,78.9,254,true);
}
