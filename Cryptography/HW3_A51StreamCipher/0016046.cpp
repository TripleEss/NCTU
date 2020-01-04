/*Hand in this file ONLY!*/
/*Hand in this file ONLY!*/
/*Hand in this file ONLY!*/

#include<vector>

#define LFSR1_SIZE 19
#define LFSR2_SIZE 22
#define LFSR3_SIZE 23

#define LFSR1_CLK 8
#define LFSR2_CLK 10
#define LFSR3_CLK 10

using namespace std;

vector<bool> DECIMALtoBINARY(unsigned long long decimal, int size)
{
    vector<bool> binary;

    while(decimal)
    {
        binary.push_back(decimal%2);
        decimal/=2;
    }

    for(int i=binary.size(); i<size; i++)
        binary.push_back(0);

    return binary;
}

bool XOR(bool a, bool b)
{
    return !(a==b);
}

void* A51Initial(unsigned long long sessionKey,unsigned int frameCounter){
/* Using this funciton to initialize your A5-1 stream cipher state.
 * sessionKey: a 64-bit unsigned integer
 *             This should be used from LSB to MSB
 *   Ex. sessionKey = 0x0000_0000_0000_000b = 0b0000...00001011
 *       Then, it will be translate to "1101 0000 ... 0000" in the video
 * frameCounter: a 22-bit unsigned integer
 *               This should be used from LSB to MSB, the same as sessionKey
 * This funtion returns a structure defined by yourself to keep the
 * stream cipher state
 */
    vector<bool> SK;
    SK=DECIMALtoBINARY(sessionKey, 64);
    /*
    cout<<"Session Key = "<<endl;
    for(int i=0; i<SK.size(); i++)
    {
        cout<<SK[i];
        if(i%4==3)
            cout<<" ";
    }
    cout<<endl;
    */
    vector<bool> FC;
    FC=DECIMALtoBINARY(frameCounter, 22);
    /*
    cout<<"Frame Counter = "<<endl;
    for(int i=0; i<FC.size(); i++)
    {
        cout<<FC[i];
        if(i%4==3)
            cout<<" ";
    }
    cout<<endl<<endl;
    */
    // STEP 1
    bool LFSR1[LFSR1_SIZE]={0};
    bool LFSR2[LFSR2_SIZE]={0};
    bool LFSR3[LFSR3_SIZE]={0};

    /*
    cout<<"LFSR1: ";
    for(int j=0; j<LFSR1_SIZE; j++)
    {
        if(j==18 || j==17 || j==16 || j==13)
            cout<<"("<<LFSR1[j]<<")";
        else
            cout<<" "<<LFSR1[j]<<" ";
    }
    cout<<endl;
    cout<<"LFSR2: ";
    for(int j=0; j<LFSR2_SIZE; j++)
    {
        if(j==21 || j==20)
            cout<<"("<<LFSR2[j]<<")";
        else
            cout<<" "<<LFSR2[j]<<" ";
    }
    cout<<endl;
    cout<<"LFSR3: ";
    for(int j=0; j<LFSR3_SIZE; j++)
    {
        if(j==22 || j==21 || j==20 || j==7)
            cout<<"("<<LFSR3[j]<<")";
        else
            cout<<" "<<LFSR3[j]<<" ";
    }
    cout<<endl;
    */

    // STEP 2
    // Session Key
    for(int i=0; i<64; i++)
    {
        bool x;
        // LFSR1.TappedBits = 18,17,16,13
        x=XOR(XOR(XOR(XOR(LFSR1[18],LFSR1[17]),LFSR1[16]),LFSR1[13]),SK[i]);
        for(int j=LFSR1_SIZE-1; j>0; j--)
            LFSR1[j]=LFSR1[j-1];
        LFSR1[0]=x;
        // LFSR2.TappedBits = 21,20
        x=XOR(XOR(LFSR2[21],LFSR2[20]),SK[i]);
        for(int j=LFSR2_SIZE-1; j>0; j--)
            LFSR2[j]=LFSR2[j-1];
        LFSR2[0]=x;
        // LFSR3.TappedBits = 22,21,20,7
        x=XOR(XOR(XOR(XOR(LFSR3[22],LFSR3[21]),LFSR3[20]),LFSR3[7]),SK[i]);
        for(int j=LFSR3_SIZE-1; j>0; j--)
            LFSR3[j]=LFSR3[j-1];
        LFSR3[0]=x;

        /*
        cout<<"Cycle #"<<i<<"   Session Key="<<SK[i]<<endl;
        cout<<"LFSR1: ";
        for(int j=0; j<LFSR1_SIZE; j++)
        {
            if(j==18 || j==17 || j==16 || j==13)
                cout<<"("<<LFSR1[j]<<")";
            else
                cout<<" "<<LFSR1[j]<<" ";
        }
        cout<<endl;
        cout<<"LFSR2: ";
        for(int j=0; j<LFSR2_SIZE; j++)
        {
            if(j==21 || j==20)
                cout<<"("<<LFSR2[j]<<")";
            else
                cout<<" "<<LFSR2[j]<<" ";
        }
        cout<<endl;
        cout<<"LFSR3: ";
        for(int j=0; j<LFSR3_SIZE; j++)
        {
            if(j==22 || j==21 || j==20 || j==7)
                cout<<"("<<LFSR3[j]<<")";
            else
                cout<<" "<<LFSR3[j]<<" ";
        }
        cout<<endl;
        */
    }
    /*
    cout<<"LFSR1: ";
    for(int j=0; j<LFSR1_SIZE; j++)
    {
        if(j==18 || j==17 || j==16 || j==13)
            cout<<"("<<LFSR1[j]<<")";
        else
            cout<<" "<<LFSR1[j]<<" ";
    }
    cout<<endl;
    cout<<"LFSR2: ";
    for(int j=0; j<LFSR2_SIZE; j++)
    {
        if(j==21 || j==20)
            cout<<"("<<LFSR2[j]<<")";
        else
            cout<<" "<<LFSR2[j]<<" ";
    }
    cout<<endl;
    cout<<"LFSR3: ";
    for(int j=0; j<LFSR3_SIZE; j++)
    {
        if(j==22 || j==21 || j==20 || j==7)
            cout<<"("<<LFSR3[j]<<")";
        else
            cout<<" "<<LFSR3[j]<<" ";
    }
    cout<<endl;
    */
    // STEP 3
    // Frame Counter
    for(int i=0; i<22; i++)
    {
        bool x;
        // LFSR1.TappedBits = 18,17,16,13
        x=XOR(XOR(XOR(XOR(LFSR1[18],LFSR1[17]),LFSR1[16]),LFSR1[13]),FC[i]);
        for(int j=LFSR1_SIZE-1; j>0; j--)
            LFSR1[j]=LFSR1[j-1];
        LFSR1[0]=x;
        // LFSR2.TappedBits = 21,20
        x=XOR(XOR(LFSR2[21],LFSR2[20]),FC[i]);
        for(int j=LFSR2_SIZE-1; j>0; j--)
            LFSR2[j]=LFSR2[j-1];
        LFSR2[0]=x;
        // LFSR3.TappedBits = 22,21,20,7
        x=XOR(XOR(XOR(XOR(LFSR3[22],LFSR3[21]),LFSR3[20]),LFSR3[7]),FC[i]);
        for(int j=LFSR3_SIZE-1; j>0; j--)
            LFSR3[j]=LFSR3[j-1];
        LFSR3[0]=x;

        /*
        cout<<"Cycle #"<<i<<"   Frame Counter="<<FC[i]<<endl;
        cout<<"LFSR1: ";
        for(int j=0; j<LFSR1_SIZE; j++)
        {
            if(j==18 || j==17 || j==16 || j==13)
                cout<<"("<<LFSR1[j]<<")";
            else
                cout<<" "<<LFSR1[j]<<" ";
        }
        cout<<endl;
        cout<<"LFSR2: ";
        for(int j=0; j<LFSR2_SIZE; j++)
        {
            if(j==21 || j==20)
                cout<<"("<<LFSR2[j]<<")";
            else
                cout<<" "<<LFSR2[j]<<" ";
        }
        cout<<endl;
        cout<<"LFSR3: ";
        for(int j=0; j<LFSR3_SIZE; j++)
        {
            if(j==22 || j==21 || j==20 || j==7)
                cout<<"("<<LFSR3[j]<<")";
            else
                cout<<" "<<LFSR3[j]<<" ";
        }
        cout<<endl;
        */
    }

    // STEP 4
    for(int i=0; i<100; i++)
    {
        int clk[2]={0};
        clk[LFSR1[LFSR1_CLK]]++;
        clk[LFSR2[LFSR2_CLK]]++;
        clk[LFSR3[LFSR3_CLK]]++;

        int major=0;
        if(clk[1]>clk[0])
            major=1;

        bool x;
        if(LFSR1[LFSR1_CLK]==major)
        {
            x=XOR(XOR(XOR(LFSR1[18],LFSR1[17]),LFSR1[16]),LFSR1[13]);
            for(int j=LFSR1_SIZE-1; j>0; j--)
                LFSR1[j]=LFSR1[j-1];
            LFSR1[0]=x;
        }
        if(LFSR2[LFSR2_CLK]==major)
        {
            x=XOR(LFSR2[21],LFSR2[20]);
            for(int j=LFSR2_SIZE-1; j>0; j--)
                LFSR2[j]=LFSR2[j-1];
            LFSR2[0]=x;
        }
        if(LFSR3[LFSR3_CLK]==major)
        {
           x=XOR(XOR(XOR(LFSR3[22],LFSR3[21]),LFSR3[20]),LFSR3[7]);
            for(int j=LFSR3_SIZE-1; j>0; j--)
                LFSR3[j]=LFSR3[j-1];
            LFSR3[0]=x;
        }
        /*
        cout<<"Cycle #"<<i<<"   major="<<major<<endl;
        cout<<"LFSR1: ";
        for(int j=0; j<LFSR1_SIZE; j++)
        {
            if(j==18 || j==17 || j==16 || j==13)
                cout<<"("<<LFSR1[j]<<")";
            else if(j==LFSR1_CLK)
                cout<<"."<<LFSR1[j]<<".";
            else
                cout<<" "<<LFSR1[j]<<" ";
        }
        cout<<endl;
        cout<<"LFSR2: ";
        for(int j=0; j<LFSR2_SIZE; j++)
        {
            if(j==21 || j==20)
                cout<<"("<<LFSR2[j]<<")";
            else if(j==LFSR2_CLK)
                cout<<"."<<LFSR2[j]<<".";
            else
                cout<<" "<<LFSR2[j]<<" ";
        }
        cout<<endl;
        cout<<"LFSR3: ";
        for(int j=0; j<LFSR3_SIZE; j++)
        {
            if(j==22 || j==21 || j==20 || j==7)
                cout<<"("<<LFSR3[j]<<")";
            else if(j==LFSR3_CLK)
                cout<<"."<<LFSR3[j]<<".";
            else
                cout<<" "<<LFSR3[j]<<" ";
        }
        cout<<endl;
        */
    }

    /*
    cout<<"LFSR1: ";
    for(int j=0; j<LFSR1_SIZE; j++)
        cout<<LFSR1[j];
    cout<<endl;
    cout<<"LFSR2: ";
    for(int j=0; j<LFSR2_SIZE; j++)
        cout<<LFSR2[j];
    cout<<endl;
    cout<<"LFSR3: ";
    for(int j=0; j<LFSR3_SIZE; j++)
        cout<<LFSR3[j];
    cout<<endl;
    */

    bool *LFSR;
    //LFSR=new bool[LFSR1_SIZE+LFSR2_SIZE+LFSR3_SIZE];

    for(int i=0; i<LFSR1_SIZE; i++)
        LFSR[i]=LFSR1[i];
    for(int i=0; i<LFSR2_SIZE; i++)
        LFSR[i+LFSR1_SIZE]=LFSR2[i];
    for(int i=0; i<LFSR3_SIZE; i++)
        LFSR[i+LFSR1_SIZE+LFSR2_SIZE]=LFSR3[i];

    return LFSR;
}

unsigned char A51GetByte(void* LFSR){
/* Using this function to get a 8-bit key stream from the current state
 * LFSR: the current A5-1 stream cipher state
 * This function returns an unsigned char which generated by your A5-1
 *      The 8-bit key stream is generated from MSB to LSB
 *   Ex. The generated 8-bit key stream = 0 1 0 0 1 1 0 1
 *       Then, the returned unsigned char is 0x4d = 77
 */
    bool* L=(bool*)LFSR;
    bool LFSR1[LFSR1_SIZE]={0};
    bool LFSR2[LFSR2_SIZE]={0};
    bool LFSR3[LFSR3_SIZE]={0};

    for(int i=0; i<LFSR1_SIZE; i++)
        LFSR1[i]=L[i];
    for(int i=0; i<LFSR2_SIZE; i++)
        LFSR2[i]=L[i+LFSR1_SIZE];
    for(int i=0; i<LFSR3_SIZE; i++)
        LFSR3[i]=L[i+LFSR1_SIZE+LFSR2_SIZE];
    /*
    cout<<"<IN>"<<endl;
    cout<<"LFSR1: ";
    for(int j=0; j<LFSR1_SIZE; j++)
        cout<<LFSR1[j];
    cout<<endl;
    cout<<"LFSR2: ";
    for(int j=0; j<LFSR2_SIZE; j++)
        cout<<LFSR2[j];
    cout<<endl;
    cout<<"LFSR3: ";
    for(int j=0; j<LFSR3_SIZE; j++)
        cout<<LFSR3[j];
    cout<<endl;
    */

    bool keystream[8];
    for(int i=7; i>=0; i--)
    {
        keystream[i]=XOR(XOR(LFSR1[LFSR1_SIZE-1],LFSR2[LFSR2_SIZE-1]),LFSR3[LFSR3_SIZE-1]);

        int clk[2]={0};
        clk[LFSR1[LFSR1_CLK]]++;
        clk[LFSR2[LFSR2_CLK]]++;
        clk[LFSR3[LFSR3_CLK]]++;

        int major=0;
        if(clk[1]>clk[0])
            major=1;

        bool x;
        if(LFSR1[LFSR1_CLK]==major)
        {
            x=XOR(XOR(XOR(LFSR1[18],LFSR1[17]),LFSR1[16]),LFSR1[13]);
            for(int j=LFSR1_SIZE-1; j>0; j--)
                LFSR1[j]=LFSR1[j-1];
            LFSR1[0]=x;
        }
        if(LFSR2[LFSR2_CLK]==major)
        {
            x=XOR(LFSR2[21],LFSR2[20]);
            for(int j=LFSR2_SIZE-1; j>0; j--)
                LFSR2[j]=LFSR2[j-1];
            LFSR2[0]=x;
        }
        if(LFSR3[LFSR3_CLK]==major)
        {
            x=XOR(XOR(XOR(LFSR3[22],LFSR3[21]),LFSR3[20]),LFSR3[7]);
            for(int j=LFSR3_SIZE-1; j>0; j--)
                LFSR3[j]=LFSR3[j-1];
            LFSR3[0]=x;
        }
    }

    /*
    cout<<"Key Stream: ";
    for(int i=7; i>=0; i--)
        cout<<keystream[i];
    cout<<endl;
    */
    /*
    cout<<"<OUT>"<<endl;
    cout<<"LFSR1: ";
    for(int j=0; j<LFSR1_SIZE; j++)
        cout<<LFSR1[j];
    cout<<endl;
    cout<<"LFSR2: ";
    for(int j=0; j<LFSR2_SIZE; j++)
        cout<<LFSR2[j];
    cout<<endl;
    cout<<"LFSR3: ";
    for(int j=0; j<LFSR3_SIZE; j++)
        cout<<LFSR3[j];
    cout<<endl;
    */
    int key=0;
    int tmp=1;
    for(int i=0; i<8; i++)
    {
        if(keystream[i])
            key+=tmp;
        tmp*=2;
    }

    //cout<<"Key: "<<key;

    for(int i=0; i<LFSR1_SIZE; i++)
        L[i]=LFSR1[i];
    for(int i=0; i<LFSR2_SIZE; i++)
        L[i+LFSR1_SIZE]=LFSR2[i];
    for(int i=0; i<LFSR3_SIZE; i++)
        L[i+LFSR1_SIZE+LFSR2_SIZE]=LFSR3[i];

    return (unsigned char)key;
}
