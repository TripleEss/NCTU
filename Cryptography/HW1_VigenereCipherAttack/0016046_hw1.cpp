// Vigenere Cipher Attack
// 0 < keys' length <= 20
// 0016046

#include<iostream>
//#include<iomanip>
#include<string>

using namespace std;

int main()
{

    string ciphertext;
    string plaintext;

    cin>>ciphertext;
    //cin>>plaintext;

    const double p[26]={0.082, 0.015, 0.028, 0.043, 0.127,
                        0.022, 0.020, 0.061, 0.070, 0.002,
                        0.008, 0.040, 0.024, 0.067, 0.075,
                        0.019, 0.001, 0.060, 0.063, 0.091,
                        0.028, 0.010, 0.023, 0.001, 0.020,
                        0.001}; // expected probabilities

    int freq[26]={0};

    double IofC[21][21]={0}; // Index of Coincidence
                             // IofC[1~20][0] for average distance between IofC[i][1~i] and 0.065

    string Y[21];

    // compute IofC when 0 < key length (m) <= 20
    for(int m=1; m<=20 && m<=ciphertext.length(); m++)
    {
        // define Y : push ciphertext into Y[1]~Y[m]
        // clear Y[1]~Y[m]
        for(int i=1; i<=m; i++)
            Y[i].clear();
        // append ciphertext to Y
        for(int j=0, i=1; ciphertext[j]!='\0'; j++)
        {
            Y[i]+=ciphertext[j];

            if(i==m)
                i=1;
            else
                i++;
        }
        //for(int i=1; i<=m; i++)cout<<"Y["<<i<<"]="<<Y[i]<<endl;
        //cout<<"m="<<m<<endl;

        // compute IofC
        for(int i=1; i<=m; i++)
        {
            //cout<<"Y["<<i<<"]="<<Y[i]<<endl;
            //cout<<"length="<<Y[i].length()<<endl;

            for(int n=0; n<26; n++)
                freq[n]=0;

            IofC[m][i]=0;
            // frequency record
            for(int j=0; Y[i][j]!='\0'; j++)
            {
                freq[Y[i][j]-'A']++;
            }

            //for(int n=0; n<26; n++){cout<<(char)(n+'A')<<":"<<setw(3)<<freq[n];if(n%5!=4)cout<<" | ";else cout<<endl;}cout<<endl;
            // the equation
            for(int n=0; n<26; n++)
            {
                IofC[m][i]+=(double)(freq[n]*(freq[n]-1));
            }

            IofC[m][i]/=(double)(Y[i].length()*(Y[i].length()-1));

            //cout<<"IofC["<<m<<"]["<<i<<"]="<<IofC[m][i]<<endl;
        }
    }

    // compute average distance between IofC[i][1~i] and 0.065
    for(int m=1; m<=20 && m<=ciphertext.length(); m++)
    {
        IofC[m][0]=0;

        // sum up all distance
        for(int i=1; i<=m; i++)
        {
            if(IofC[m][i]>=0.065)
                IofC[m][0]+=IofC[m][i]-0.065;
            else
                IofC[m][0]+=0.065-IofC[m][i];
        }
        // average
        IofC[m][0]/=m;

        //cout<<"IofC[";if(m<10)cout<<"0";cout<<m<<"]="<<IofC[m][0]<<endl;
    }

    // find smallest difference
    double smallest=IofC[1][0];
    int m=1;
    for(int i=2; i<=20; i++)
    {
        if(IofC[i][0]<smallest)
        {
            smallest=IofC[i][0];
            m=i;
        }
    }


    // rebiuld Y with correct key length (m)
    for(int i=1; i<=m; i++)
        Y[i].clear();

    for(int j=0, i=1; ciphertext[j]!='\0'; j++)
    {
        Y[i]+=ciphertext[j];

        if(i==m)
            i=1;
        else
            i++;
    }
    //for(int i=1; i<=m; i++)cout<<"Y["<<i<<"]="<<Y[i]<<endl;

    string key;
    // compute mg
    for(int i=1; i<=m; i++)
    {
        for(int f=0; f<26; f++)
            freq[f]=0;
        // frequency record
        for(int j=0; Y[i][j]!='\0'; j++)
        {
            freq[Y[i][j]-'A']++;
        }

        double mg[26]={0};
        // the equation
        for(int j=0; j<26; j++)
        {
            for(int n=0; n<26; n++)
            {
                mg[j]+=(double)((p[n]*freq[(n+j)%26])/Y[i].length());
            }
            mg[j]-=0.065;
        }
        // find smallest (positive) difference
        double smallest=mg[0];
        int k=0;
        for(int j=1; j<26; j++)
        {
            if(smallest>=0)
            {
                if(mg[j]>=0 && mg[j]<smallest)
                {
                    smallest=mg[j];
                    k=j;
                }
            }
            else
            {
                if(mg[j]>=0 || (mg[j]<0 && mg[j]>smallest))
                {
                    smallest=mg[j];
                    k=j;
                }
            }
        }

        key+=(char)('A'+k);
    }
    // ouput key
    cout<<key<<endl;

    //int wrong=0; // check

    // output plaintext
    for(int i=0; i<ciphertext.length(); i++)
    {
        if(ciphertext[i]>=key[i%m])
        {
            cout<<(char)('A'+(ciphertext[i]-key[i%m]));
            //if((char)('A'+(ciphertext[i]-key[i%m]))!=plaintext[i])wrong++;
        }
        else
        {
            cout<<(char)('A'+26+(ciphertext[i]-key[i%m]));
            //if((char)('A'+26+(ciphertext[i]-key[i%m]))!=plaintext[i])wrong++;
        }
    }
    //cout<<endl<<"error:"<<wrong;
}
