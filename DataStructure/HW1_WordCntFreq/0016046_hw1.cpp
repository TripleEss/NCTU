#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<ctime>

using namespace std;

struct word      //record words and their number
{
    string data;
    int num;
};

int char_to_int(char* c)      //convert (char*)argv[2]=k to (int)value=k
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

main(int argc, char* argv[])      //main(int argc, char* argv[]): ./(exe=argv[0]) (option1=argv[1]) (option2=argv[2]) ...(option_n=argv[argc-1])
{                                 //                      HERE  : ./.exe           input file        1~kth
    clock_t start=clock();      //compute execution-time: start
    int k=char_to_int(argv[2]);

    ifstream file(argv[1]);      //file.open()
    //ofstream outfile("outfile.txt");
    char punc[15]={'.',',',';',':','?','!','"','\'','-','(',')','[',']','{','}'};      //the punctuations(*15) should be ignored
    char letter[53]={"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"};      //for convertion: upper-case <=> lower-case , HERE: upper-case[(0~25)+26] => lower-case[(26~51)]
    struct word w[500];      //implemented in array? (=> linked list?)
    int count;      //the number of string categories from the begining

    string str1;
    if(file.is_open())
    {
        count=0;
        while(getline(file,str1))
        {
            //cout<<str1<<"\n";
            for(int i=0; i<15; i++)      //find out punctuations and replace them with " "
            {
                while(str1.find(punc[i])<str1.length())      //string.find((char)): find fisrt (char) in (string) and return the position of fisrt (char)
                                                           //               HERE: find punctuations in string
                {
                    str1.replace(str1.find(punc[i]),1," ");      //string.replace((start-position),(number of chars need to be replaced),(replacement))
                }                                              //HERE: replace punctuations in string with " "
            }
            for(int i=0; i<26; i++)      //find out upper-case letters and convert them to lower-case letters
            {
                while(str1.find(letter[i])<str1.length())
                {
                    char p[2]={letter[i+26]};      //p[2]={letter[i+26]+'\0'}
                    str1.replace(str1.find(letter[i]),1,p);      //HERE: replace upper-case letters in string with lower-case letters
                }
            }
            //cout<<str1<<"\n";
            istringstream ss(str1);      //istringstream (stream)((string)): input-string => stream
            string str2;
            while(ss>>str2)      //>>:extraction operator , HERE: extrate string(str2) from stream(ss)
            {
                bool insertion=false;      //check if the words have been inserted to w

                for(int i=0; i<count; i++)
                {
                    if(str2==w[i].data)    //the word has been inserted => exists in w[0~count-1]
                    {
                        w[i].num++;
                        insertion=true;
                    }
                }

                if(!insertion)
                {
                    w[count].data=str2;      //insert the new word to w[count]
                    w[count].num=1;
                    count++;
                }
            }
        }

        /*
        for(int i=0; i<count; i++)
        {
            cout<<w[i].data<<" "<<w[i].num<<endl;
        }
        */


        for(int i=0; i<count-1; i++)      //bubble-sort: sorting w[0~count-1] by w[0~count-1].num;
        {                                 //       HERE: more => less
            for(int j=0; j<count-1-i; j++)
            {
                if(w[j].num<w[j+1].num)      //if less => more
                {
                    struct word tempw;      //swap(w[j],w[j+1])
                    tempw.data=w[j].data;
                    tempw.num=w[j].num;
                    w[j].data=w[j+1].data;
                    w[j].num=w[j+1].num;
                    w[j+1].data=tempw.data;
                    w[j+1].num=tempw.num;
                }
            }
        }
    }
    /*
    for(int i=0; i<count; i++)
    {
        outfile<<w[i].data<<" "<<w[i].num<<endl;
    }
    */
    for(int i=0; i<k; i++)      //print out NO.1(w[0])~NO.k(w[k-1])
    {
        cout<<"("<<w[i].data<<","<<w[i].num<<")\n";
    }


    for(int i=k; w[i].num==w[k-1].num; i++)      //print out NO.(k+1)~(w[k]~) if w[k]~.num equ. w[k-1].num
    {
        cout<<"("<<w[i].data<<","<<w[i].num<<")\n";
    }



    file.close();      //file.close();
    clock_t end=clock();      //compute execution-time: end
    double time=(double)(end-start)/CLOCKS_PER_SEC;
    cout<<time<<endl;
}
