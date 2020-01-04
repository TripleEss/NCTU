// COMMAND: g++ 0016046.cpp
// Single Thread

#include<iostream>
#include<fstream>
#include<string>
#include<map>

using namespace std;

class TRANSACTION
{
public:
    string id;
    char type;
    string to;
    int amount;
    TRANSACTION* next;
};

map<string,int> ac_map;

class ACCOUNT
{
public:
    string id;
    int balance;

    void withdraw(int amount)
    {
        balance=balance-amount;
    }

    void deposit(int amount)
    {
        withdraw(-amount);
    }

    bool transfer(ACCOUNT *to, int amount)
    {
        if(balance>=amount)
        {
            withdraw(amount);
            to->deposit(amount);
            return true;
        }
        else
            return false;
    }

};


int main(void)
{
    ifstream fin("transactions.txt");

    int ac_num;
    fin>>ac_num;

    ACCOUNT *ac;
    ac=new ACCOUNT[ac_num];

    int *trans_num;
    trans_num=new int[ac_num];

    TRANSACTION **trans;
    trans=new TRANSACTION*[ac_num];

    for(int i=0; i<ac_num; i++)
    {
        fin>>ac[i].id>>ac[i].balance>>trans_num[i];
        //cout<<ac[i].id<<" "<<ac[i].balance<<" "<<trans_num[i]<<endl;
        ac_map.insert(pair<string,int>(ac[i].id,i));

        trans[i]=new TRANSACTION[trans_num[i]];
        for(int j=0; j<trans_num[i]; j++)
        {
            fin>>trans[i][j].id>>trans[i][j].type;

            if(trans[i][j].type=='T')
            {
                fin>>trans[i][j].to>>trans[i][j].amount;
            }
            else if(trans[i][j].type=='B')
            {
                trans[i][j].to="\0";
                trans[i][j].amount=0;
            }
        }
    }

    fin.close();
/*
    cout<<"-------input information-------"<<endl;
    for(int i=0; i<ac_num; i++)
    {
        cout<<"id: "<<ac[i].id<<" <- "<<ac[i].balance<<endl;

        for(int j=0; j<trans_num[i]; j++)
            if(trans[i][j].type=='T')
                cout<<trans[i][j].id<<" "<<trans[i][j].type<<" "<<trans[i][j].to<<" "<<trans[i][j].amount<<endl;
            else if(trans[i][j].type=='B')
                cout<<trans[i][j].id<<" "<<trans[i][j].type<<endl;
        if(i!=ac_num-1)
            cout<<endl;
    }
*/
/*
    ofstream fin_check("input_check.txt");
    fin_check<<ac_num<<endl;
    for(int i=0; i<ac_num; i++)
    {
        fin_check<<ac[i].id<<" "<<ac[i].balance<<" "<<trans_num[i]<<endl;
        for(int j=0; j<trans_num[i]; j++)
        {
            fin_check<<trans[i][j].id<<" "<<trans[i][j].type;

            if(trans[i][j].type=='T')
                fin_check<<" "<<trans[i][j].to<<" "<<trans[i][j].amount<<endl;
            else
                fin_check<<endl;
        }
    }
*/
    ofstream fout_state("final_state.txt");
    ofstream fout_log("transaction_log.txt");

    int finished=0;
    int timestamp=0;

    int *doing;
    doing=new int[ac_num];
    for(int i=0; i<ac_num; i++)
    {
        doing[i]=0;
        if(trans_num[i]==0)
            finished++;
    }

    for(int i=0; finished!=ac_num; i++)
    {
        if(doing[i]==trans_num[i])
        {
            //cout<<"AC # "<<i<<" done!"<<endl;
            if(i==ac_num-1)
                i=-1;
            continue;
        }
        else
        {
            //cout<<"("<<i<<","<<doing[i]<<")"<<" "<<trans[i][doing[i]].id<<endl;
            if(trans[i][doing[i]].type=='T')
            {
                if(ac[i].transfer(&ac[ac_map[trans[i][doing[i]].to]],trans[i][doing[i]].amount))
                {
                    timestamp++;
                    //cout<<trans[i][doing[i]].id<<" "<<timestamp<<endl;
                    fout_log<<trans[i][doing[i]].id<<" "<<timestamp<<endl;

                    doing[i]++;
                    if(doing[i]==trans_num[i])
                        finished++;
                }
            }
            else if(trans[i][doing[i]].type=='B')
            {
                timestamp++;
                //cout<<trans[i][doing[i]].id<<" "<<timestamp<<" "<<ac[i].balance<<endl;
                fout_log<<trans[i][doing[i]].id<<" "<<timestamp<<" "<<ac[i].balance<<endl;

                doing[i]++;
                if(doing[i]==trans_num[i])
                    finished++;
            }
        }

        if(i==ac_num-1)
            i=-1;
    }

    for(int i=0; i<ac_num; i++)
        fout_state<<ac[i].id<<" "<<ac[i].balance<<endl;

    fout_state.close();
    fout_log.close();

    delete [] doing;
    for(int i=0; i<ac_num; i++)
        delete [] trans[i];
    delete [] trans;
    delete [] ac;
}

