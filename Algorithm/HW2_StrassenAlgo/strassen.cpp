#include<iostream>
#include<iomanip>

using namespace std;

template<typename T>
class matrix
{
    int size;
    T   **sq;

    public:

    matrix(int s)
    {
        size=s;
        sq=new T*[size+size%2];

        for(int i=0; i<size+size%2; i++)
        {
            sq[i]=new T[size+size%2];
        }

        for(int i=0; i<size+size%2; i++)
        {
            for(int j=0; j<size+size%2; j++)
            {
                sq[i][j]=(T)0;
                //cout<<"sq["<<i<<"]["<<j<<"]="<<sq[i][j]<<" ";
            }
            //cout<<endl;
        }

    }

    matrix(int s, T value)
    {
        size=s;
        sq=new T*[size+size%2];

        for(int i=0; i<size+size%2; i++)
        {
            sq[i]=new T[size+size%2];
        }

        for(int i=0; i<size+size%2; i++)
        {
            for(int j=0; j<size+size%2; j++)
            {
                sq[i][j]=value;
                //cout<<"sq["<<i<<"]["<<j<<"]="<<sq[i][j]<<" ";
            }
            //cout<<endl;
        }
    }

    ~matrix()
    {
        for(int i=0; i<size+size%2; i++)
        {
                delete [] sq[i];
        }
        delete [] sq;
    }

    T* operator[](int i)
    {
        return sq[i];
    }

    friend ostream& operator<<(ostream& os, const matrix& A)
    {
        for(int i=0; i<A.size; i++)
        {
            for(int j=0; j<A.size; j++)
                os<<setw(5)<<A.sq[i][j]<<" ";
            os<<endl;
        }

        return os;
    }

    friend matrix operator^(const matrix& A, const matrix& B)
    {
        matrix<T> C(A.size);
        for(int i=0; i<C.size; i++)
        {
            for(int j=0; j<C.size; j++)
            {
                C[i][j]=0;
                for(int k=0; k<C.size; k++)
                {
                    C[i][j]=C[i][j]+A.sq[i][k]*B.sq[k][j];
                }
            }
        }

        return C;
    }

    friend matrix operator+(const matrix& A, const matrix& B)
    {
        matrix<T> C(A.size);
        for(int i=0; i<C.size; i++)
        {
            for(int j=0; j<C.size; j++)
            {
                C.sq[i][j]=A.sq[i][j]+B.sq[i][j];
            }
        }

        return C;
    }

    friend matrix operator-(const matrix& A, const matrix& B)
    {
        matrix<T> C(A.size);
        for(int i=0; i<C.size; i++)
        {
            for(int j=0; j<C.size; j++)
            {
                C.sq[i][j]=A.sq[i][j]-B.sq[i][j];
            }
        }

        return C;
    }

    const matrix& operator=(const matrix& A)
    {
        if(&A!=this)
        {
            for(int i=0; i<size+size%2; i++)
            {
                delete [] sq[i];
            }
            delete [] sq;

            sq=new T*[size+size%2];
            for(int i=0; i<size+size%2; i++)
            {
                sq[i]=new T[size+size%2];
            }

            for(int i=0; i<size+size%2; i++)
            {
                for(int j=0; j<size+size%2; j++)
                {
                    sq[i][j]=A.sq[i][j];
                }
            }
        }

        return *this;
    }

    friend matrix operator*(const matrix& A, const matrix& B)
    {
        //STEP1
        //A11 A12 | B11 B12
        //A21 A22 | B21 B22
        matrix<T> A11((A.size+A.size%2)/2);
        matrix<T> A12((A.size+A.size%2)/2);
        matrix<T> A21((A.size+A.size%2)/2);
        matrix<T> A22((A.size+A.size%2)/2);

        matrix<T> B11((B.size+B.size%2)/2);
        matrix<T> B12((B.size+B.size%2)/2);
        matrix<T> B21((B.size+B.size%2)/2);
        matrix<T> B22((B.size+B.size%2)/2);

        for(int i=0; i<(A.size+A.size%2)/2; i++)
        {
            for(int j=0; j<(A.size+A.size%2)/2; j++)
            {
                A11[i][j]=A.sq[i][j];
                B11[i][j]=B.sq[i][j];
            }
        }

        for(int i=0; i<(A.size+A.size%2)/2; i++)
        {
            for(int j=0; j<(A.size+A.size%2)/2; j++)
            {
                A12[i][j]=A.sq[i][(A.size+A.size%2)/2+j];
                B12[i][j]=B.sq[i][(A.size+A.size%2)/2+j];
            }
        }

        for(int i=0; i<(A.size+A.size%2)/2; i++)
        {
            for(int j=0; j<(A.size+A.size%2)/2; j++)
            {
                A21[i][j]=A.sq[(A.size+A.size%2)/2+i][j];
                B21[i][j]=B.sq[(A.size+A.size%2)/2+i][j];
            }
        }

        for(int i=0; i<(A.size+A.size%2)/2; i++)
        {
            for(int j=0; j<(A.size+A.size%2)/2; j++)
            {
                A22[i][j]=A.sq[(A.size+A.size%2)/2+i][(A.size+A.size%2)/2+j];
                B22[i][j]=B.sq[(A.size+A.size%2)/2+i][(A.size+A.size%2)/2+j];
            }
        }
        //cout<<"A11:\n"<<A11;cout<<"A12:\n"<<A12;cout<<"A21:\n"<<A21;cout<<"A22:\n"<<A22;
        //cout<<"B11:\n"<<B11;cout<<"B12:\n"<<B12;cout<<"B21:\n"<<B21;cout<<"B22:\n"<<B22<<endl;

        //STEP2
        matrix<T> S1((A.size+A.size%2)/2);
        matrix<T> S2((A.size+A.size%2)/2);
        matrix<T> S3((A.size+A.size%2)/2);
        matrix<T> S4((A.size+A.size%2)/2);
        matrix<T> S5((A.size+A.size%2)/2);
        matrix<T> S6((A.size+A.size%2)/2);
        matrix<T> S7((A.size+A.size%2)/2);
        matrix<T> S8((A.size+A.size%2)/2);
        matrix<T> S9((A.size+A.size%2)/2);
        matrix<T> S10((A.size+A.size%2)/2);

        S1=B12-B22;
        S2=A11+A12;
        S3=A21+A22;
        S4=B21-B11;
        S5=A11+A22;

        S6=B11+B22;
        S7=A12-A22;
        S8=B21+B22;
        S9=A11-A21;
        S10=B11+B12;

        //cout<<"S1:\n"<<S1;cout<<"S2:\n"<<S2;cout<<"S3:\n"<<S3;cout<<"S4:\n"<<S4;cout<<"S5:\n"<<S5;
        //cout<<"S6:\n"<<S6;cout<<"S7:\n"<<S7;cout<<"S8:\n"<<S8;cout<<"S9:\n"<<S9;cout<<"S10:\n"<<S10<<endl;

        //STEP3
        matrix<T> P1((A.size+A.size%2)/2);
        matrix<T> P2((A.size+A.size%2)/2);
        matrix<T> P3((A.size+A.size%2)/2);
        matrix<T> P4((A.size+A.size%2)/2);
        matrix<T> P5((A.size+A.size%2)/2);
        matrix<T> P6((A.size+A.size%2)/2);
        matrix<T> P7((A.size+A.size%2)/2);

        P1=A11^S1;
        P2=S2^B22;
        P3=S3^B11;
        P4=A22^S4;

        P5=S5^S6;
        P6=S7^S8;
        P7=S9^S10;

        //cout<<"P1:\n"<<P1;cout<<"P2:\n"<<P2;cout<<"P3:\n"<<P3;cout<<"P4:\n"<<P4;cout<<"P5:\n"<<P5;
        //cout<<"P6:\n"<<P6;cout<<"P7:\n"<<P7<<endl;
        //STEP4
        matrix<T> C(A.size+A.size%2);
        if(A.size%2)
            C.size=C.size-1;

        for(int i=0; i<(A.size+A.size%2)/2; i++)
        {
            for(int j=0; j<(A.size+A.size%2)/2; j++)
            {
                C[i][j]=P5[i][j]+P4[i][j]-P2[i][j]+P6[i][j];
            }
        }

        for(int i=0; i<(A.size+A.size%2)/2; i++)
        {
            for(int j=0; j<(A.size+A.size%2)/2; j++)
            {
                C[i][j+(A.size+A.size%2)/2]=P1[i][j]+P2[i][j];
            }
        }

        for(int i=0; i<(A.size+A.size%2)/2; i++)
        {
            for(int j=0; j<(A.size+A.size%2)/2; j++)
            {
                C[i+(A.size+A.size%2)/2][j]=P3[i][j]+P4[i][j];
            }
        }

        for(int i=0; i<(A.size+A.size%2)/2; i++)
        {
            for(int j=0; j<(A.size+A.size%2)/2; j++)
            {
                C[i+(A.size+A.size%2)/2][j+(A.size+A.size%2)/2]=P5[i][j]+P1[i][j]-P3[i][j]-P7[i][j];
            }
        }

        return C;
    }

};



int main()
{

    const int n=9;

    matrix<int> A(n),B(n);

    for(int i=0; i<n; i++)
    {
        for(int j=0; j<n; j++)
        {
            A[i][j]=rand()%5;
            B[i][j]=rand()%5;
        }
    }

    cout<<"Matrix A\n"<<A;
    cout<<"Matrix B\n"<<B;
    cout<<"Matrix AxB\n"<<A*B;
    cout<<"Matrix A^B\n"<<(A^B);

}
