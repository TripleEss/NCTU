#include<iostream>
#include<fstream>
#include<sstream>


using namespace std;

int str_to_int(string s)   //number string to interger
{
    int value=0;
    int base=1;
    for(int i=s.length()-1; i>=0; i--)
    {
        value+=((int)s[i]-48)*base;
        base*=10;
    }
    return value;
};

class HuffTree
{
    private:

    class HuffNode
    {
        public:
        char data;   //character
        string code;   //huffman code
        HuffNode* left;
        HuffNode* right;

        HuffNode(char c, string hc)   //leaf node
        {
            data=c;
            code=hc;
            left=NULL;
            right=NULL;
        }

        HuffNode()
        {
            left=NULL;
            right=NULL;
        }

        ~HuffNode()
        {
            if(left)
            {
                delete left;
                delete right;
            }
        }

        void inorder()
        {
            if(this)
            {
                left->inorder();
                if(!left&&!right)cout<<"data: "<<data<<" / code: "<<code<<endl;
                right->inorder();
            }
        }
    };

    HuffNode* tree;

    public:

    HuffTree()
    {
        tree=NULL;
    }

    void push(string s)   //insert new leaf node
    {
        istringstream ss(s);
        string ch;
        string chcode;
        getline(ss,ch,':');
        getline(ss,chcode);
        //cout<<"data: "<<ch<<" / code: "<<chcode<<endl;
        //cout<<ch[0]<<endl;
        HuffNode* newnode=new HuffNode(ch[0], chcode);

        if(!tree)
        {   //cout<<"new tree";
            tree=new HuffNode;
        }

        HuffNode* tmp=tree;

        for(int i=0; i<chcode.length(); i++)
        {   //cout<<ch<<"-"<<i<<": "<<chcode[i]<<" ";//<<endl;
            if(chcode[i]=='0')
            {   //cout<<"go to 0\n";
                if(i==chcode.length()-1)   //leaf
                {
                    tmp->left=newnode;
                    break;
                }
                if(!tmp->left)
                {
                    tmp->left=new HuffNode;
                }
                tmp=tmp->left;
            }
            else
            {   //cout<<"go to 1\n";
                if(i==chcode.length()-1)   //leaf
                {
                    tmp->right=newnode;
                    break;
                }
                if(!tmp->right)
                {
                    tmp->right=new HuffNode;
                }
                tmp=tmp->right;
            }
        }
    }

    ~HuffTree()
    {
        delete tree;
    }


    void decode(ofstream& out, string bit)
    {
        //cout<<bit.length();
        //cout<<bitvalue<<endl;
        HuffNode* tmp=tree;
        for(int i=0; i<bit.length();)
        {   //cout<<ch<<"-"<<i<<": "<<chcode[i]<<" ";//<<endl;
            if(bit[i]=='0')   //0 for left
            {   //cout<<"go to 0\n";
                if(!tmp->left)   //leaf arrived, 0 do not belong to this
                {
                    out<<tmp->data;
                    tmp=tree;
                }
                else
                {
                    tmp=tmp->left;
                    i++;   //0 belongs to this, go to next bit
                }
            }
            else   //1 for right
            {   //cout<<"go to 1\n";
                if(!tmp->right)   //leaf arrived, 1 do not belong to this
                {
                    out<<tmp->data;
                    tmp=tree;
                }
                else
                {
                    tmp=tmp->right;
                    i++;  //1 belongs to this, go to next bit
                }
            }
        }
    }

    void show()
    {
        tree->inorder();
    }
};

string bitout(unsigned char ch, int entext_bitlen, int& current_bit)   //bitwise
{
    //cout<<current_bit<<endl;
    int block=1;
    string bitvalue;
    block<<=7;

    if(current_bit+8<=entext_bitlen)
    {
        for(int j=0; j<8; j++)
        {
            if(block&ch)
            {
                bitvalue.push_back('1');
            }
            else
            {
                bitvalue.push_back('0');
            }
            ch<<=1;
        }
        current_bit+=8;
    }
    else
    {
        for(; current_bit+1<=entext_bitlen; current_bit++)
        {
            if(block&ch)
            {
                bitvalue.push_back('1');
            }
            else
            {
                bitvalue.push_back('0');
            }
            ch<<=1;
        }
    }

    return bitvalue;
}


int main(int argc, char* argv[])
{
    ifstream fin;
    int entext_bitlen=0;
    fin.open(argv[1]);
    //fin.open("out.txt");

    if(fin.is_open())
    {
        HuffTree root;
        string str;
        int part=0;

        getline(fin,str);   //###\n
        getline(fin,str);   //entext_bitlen\n
        entext_bitlen=str_to_int(str);
        //cout<<entext_bitlen;

        getline(fin,str);   //###\n
        while(getline(fin,str))   //codemap
        {
            if(str=="###")
            {
                break;
            }
            root.push(str);
        }
        //root.show();
        //getline(fin,str);
        ofstream fout;
        fout.open(argv[2]);
        string bittext;
        //fout.open("out2.txt");
        if(fin.is_open() && fout.is_open())
        {
            int current_bit=0;

            char ch;
            while(fin.get(ch))   //encoded text
            {
                //count++;
                bittext.append(bitout((unsigned char)ch, entext_bitlen, current_bit));
                //cout<<current_bit<<endl;
            }
            //cout<<bittext;
        }
        root.decode(fout, bittext);

        fin.close();
        fout.close();
    }


}
