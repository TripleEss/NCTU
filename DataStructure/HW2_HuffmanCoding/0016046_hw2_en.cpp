//0016046_DS_hw2
//Huffman Encoding

#include<iostream>
#include<fstream>
#include<string>

#define MAX_NUM 256   //ASCII

using namespace std;

string codemap[MAX_NUM];

class HuffTree
{
    private:

    class HuffNode
    {
        private:
        static int entext_bitlen;

        public:
        char data;   //character
        int freq;   //frequency
        string code;   //Huffman code
        HuffNode* left;
        HuffNode* right;

        HuffNode(char c, int f)   //leaf node
        {
            data=c;
            freq=f;
            left=NULL;
            right=NULL;
        }

        HuffNode(HuffNode* l, HuffNode* r)   //internal node
        {
            freq=l->freq+r->freq;
            left=l;
            right=r;
        }

        ~HuffNode()
        {
            if(left)
            {
                delete left;
                delete right;
            }
        }

        void fillcode(string prefix)   //prefix: parent node's code
        {
            //cout<<"fillcode!\n";
            if(this)
            {
                if(left || right)   //internal node
                {
                    //cout<<data<<freq<<endl;
                    prefix.push_back('0');   //prefix+0
                    //cout<<"After prefix.push_back('0');  ->"<<prefix<<endl;
                    left->fillcode(prefix);

                    prefix.replace(prefix.end()-1,prefix.end(),"1");   //prefix+1
                    //cout<<"After prefix.replace(prefix.end()-1,prefix.end(),\"1\");  ->"<<prefix<<endl;
                    right->fillcode(prefix);

                    prefix.erase(prefix.end()-1);
                    //cout<<"After prefix.erase(prefix.end()-1);  ->"<<prefix<<endl;
                }
                else   //leaf node
                {
                    //CM->add(data, prefix);
                    code=prefix;
                    entext_bitlen+=freq*code.length();
                    //cout<<"char: "<<data<<" / code: "<<code<<endl;
                }
            }

        }

        void inorderFORcodemap()
        {
            if(this)
            {
                left->inorderFORcodemap();
                if(!left&&!right)   //leaf node
                {
                    codemap[(int)data]=code;
                }
                right->inorderFORcodemap();
            }
        }

        int textbitlen()   //encode text bit length
        {
            return entext_bitlen;
        }
    };


    class PriorityQueue   //freq & leaf or internal node as priority
    {
        private:
        struct PQitem   //linked-list
        {
            HuffNode* n;
            struct PQitem* next;
        };

        struct PQitem* PQfirst;

        public:

        PriorityQueue()
        {
            PQfirst=NULL;
        }

        ~PriorityQueue()
        {
            delete PQfirst;
        }

        void push(HuffNode* node)   //new node inserted
        {
            struct PQitem *newPQitem=new struct PQitem;

            if(newPQitem)
            {
                newPQitem->n=node;
                newPQitem->next=NULL;

                if(!PQfirst)
                {
                    PQfirst=newPQitem;
                }
                else
                {
                    struct PQitem* previous=NULL;
                    struct PQitem* current=PQfirst;
                    while(current!=NULL && newPQitem->n->freq > current->n->freq)   //freq as prority
                    {
                        previous=current;
                        current=current->next;
                    }
                    while(current!=NULL && newPQitem->n->freq == current->n->freq && newPQitem->n->left)   //leaf or internal node as priority
                    {
                        if(current->n->left)
                        {
                            break;
                        }
                        previous=current;
                        current=current->next;
                    }

                    if(current==PQfirst)  //insert to first
                    {
                        newPQitem->next=PQfirst;
                        PQfirst=newPQitem;
                    }
                    else
                    {
                        previous->next=newPQitem;
                        newPQitem->next=current;
                    }
                }
            }
        }

        HuffNode* top()
        {
            return PQfirst->n;
        }

        void pop()  //delete first element
        {
            struct PQitem* tmp=PQfirst;
            PQfirst=PQfirst->next;
            delete tmp;
        }

        bool isEmpty()
        {
            return !PQfirst;
        }

        void show()
        {
            for(struct PQitem* tmp=PQfirst; tmp!=NULL; tmp=tmp->next)
            {
                cout<<"data: "<<tmp->n->data<<" / freq:"<<tmp->n->freq;
                if(tmp->n->left)cout<<"*";
                cout<<endl;
            }
        }
    };


    HuffNode* tree;   //tree root

    public:

    HuffTree(int list[])
    {
        //cout<<"enter HuffTree constructor!\n";
        PriorityQueue PQ;
        tree=NULL;
        for(int i=0; i<MAX_NUM; i++)   //new node for character which freq. is not 0 and push it into PQ
        {
            if(list[i]!=0)
            {
                HuffNode* datanode=new HuffNode((char)i, list[i]);
                PQ.push(datanode);
                //PQ.show(); cout<<endl;
            }
        }
        //PQ.show(); cout<<endl;
        while(!PQ.isEmpty())
        {
            //cout<<"BeforeMerge:\n";PQ.show();cout<<endl;
            HuffNode* top=PQ.top();   //first priority
            PQ.pop();
            if(PQ.isEmpty())   //last item is root
            {
                tree=top;
            }
            else
            {
                HuffNode* top2=PQ.top();   //second priority
                PQ.pop();
                HuffNode* mergenode=new HuffNode(top,top2);
                PQ.push(mergenode);
            }
            //cout<<"AfterMerge:\n";PQ.show();cout<<endl;
        }

        string prefixcode;
        tree->fillcode(prefixcode);
        tree->inorderFORcodemap();

    }

    void show(ofstream& out)
    {
        out<<"###"<<endl;
        out<<tree->textbitlen()<<endl;
        out<<"###"<<endl;
        tree->inorderFORcodemap();
        for(int i=0; i<MAX_NUM; i++)
        {
            if(!codemap[i].empty())
            {
                out<<(char)i<<":"<<codemap[i]<<endl;
            }
        }
        out<<"###"<<endl;
    }

    ~HuffTree()
    {
        delete tree;
    }
};

void bitout(ofstream& out, string c, int& current_byte, int& nbits)   //bitwise: 8*(string01)->1*(unsigned char)=8*(bit01)
{
    //cout<<c<<endl;
    for(int i=0; i<c.length(); i++)
    {
        current_byte<<=1;

        if(c[i]=='1')
        {
            current_byte|=1;
            //cout<<"1";
        }
        //else cout<<"0";
        nbits++;

        if(nbits==8)
        {
            out<<(unsigned char)current_byte;
            nbits=0;
            current_byte=0;
        }
    }
}

int HuffTree::HuffNode::entext_bitlen=0;

int main(int argc, char* argv[])
{

    ifstream fin;
    //fin.open("Original");
    fin.open(argv[1]);
    int tmplist[MAX_NUM]={0};   //tmplist[ASCII:(int) character]:freq

    if(fin.is_open())
    {
        char ch;
        while(fin.get(ch))
        {
            tmplist[(int)ch]++;
        }
    }

    //for(int i=0; i<MAX_NUM; i++) cout<<"char:"<<(char)i<<"/freq:"<<tmplist[i]<<endl;
    HuffTree root(tmplist);

    ofstream fout;
    fin.close();
    //fin.open("Original");
    //fout.open("out.txt");
    fin.open(argv[1]);
    fout.open(argv[2]);
    root.show(fout);

    if(fin.is_open() && fout.is_open())
    {
        int current_byte=0;
        int nbits=0;
        char ch;
        while(fin.get(ch))
        {
            //cout<<codemap[ch]<<endl;
            bitout(fout, codemap[(int)ch], current_byte, nbits);
        }

        while(nbits)
            bitout(fout, "0", current_byte, nbits);
    }

    fin.close();
    fout.close();
}
