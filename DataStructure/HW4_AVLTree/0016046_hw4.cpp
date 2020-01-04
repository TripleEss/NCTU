#include<iostream>
#include<fstream>
#include<string>

using namespace std;

int str_to_int(string s)
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

int max(int x1, int x2)
{
    if(x1>x2)
        return x1;
    else
        return x2;
}

class AVLnode
{
    public:
        int data;
        int height;
        AVLnode *left;
        AVLnode *right;

        AVLnode()
        {
            left=NULL;
            right=NULL;
        }
        ~AVLnode()
        {
            delete left;
            delete right;
        }
};

class AVLtree
{
    public:
        int LLtimes, RRtimes, LRtimes, RLtimes;
        AVLnode *root;

        AVLtree()
        {
            LLtimes=0;
            RRtimes=0;
            LRtimes=0;
            RLtimes=0;
            root=NULL;
        }
        ~AVLtree()
        {
            delete root;
        }

        int height(AVLnode* n)   //to get the height of left & right subtree
        {
            //3 conditions: NULL, root only, the others
            if(n==NULL)
                return -1;
            else if(!n->left && !n->right)
                return 0;
            else
            {
                return max(height(n->left),height(n->right))+1;
            }
        }

        void insert(int x)
        {
            if(root==NULL)
            {
                //cout<<"root:"<<x<<endl;
                AVLnode *newnode=new AVLnode;
                newnode->data=x;
                newnode->height=0;
                root=newnode;
            }
            else
            {
                AVLnode *tmp=root;
                while(1)
                {
                    if(x<tmp->data)
                    {
                        if(tmp->left)
                        {
                            tmp=tmp->left;
                            continue;
                        }
                        else
                        {
                            //cout<<"l:"<<x<<endl;
                            AVLnode *newnode=new AVLnode;
                            newnode->data=x;
                            newnode->height=0;
                            tmp->left=newnode;
                            //break;
                            //cout<<x;
                            //cout<<"lh:"<<height(root->left)<<" / rh:"<<height(root->right)<<endl;
                            //cout<<height(root->left)-height(root->right)<<endl;
                            //preorder(root);
                            if(height(root->left)-height(root->right)==2)
                            {
                                if(x<root->left->data)
                                    LL(root);
                                else
                                    LR(root);
                            }
                            else if(height(root->right)-height(root->left)==2)
                            {
                                if(x>root->right->data)
                                    RR(root);
                                else
                                    RL(root);
                            }
                            break;
                        }
                    }

                    else if(x>tmp->data)
                    {
                        if(tmp->right)
                        {
                            tmp=tmp->right;
                            continue;
                        }
                        else
                        {   //cout<<"r:"<<x<<endl;
                            AVLnode *newnode=new AVLnode;
                            newnode->data=x;
                            newnode->height=0;
                            tmp->right=newnode;
                            //cout<<x;
                            //cout<<"lh:"<<height(root->left)<<" / rh:"<<height(root->right)<<endl;
                            //cout<<root->left->data<<endl;
                            //preorder(root);
                            if(height(root->left)-height(root->right)==2)
                            {
                                if(x<root->left->data)
                                    LL(root);
                                else
                                    LR(root);
                            }
                            else if(height(root->right)-height(root->left)==2)
                            {
                                if(x>root->right->data)
                                    RR(root);
                                else
                                    RL(root);
                            }
                            break;
                        }
                    }
                }
            }
            //preorder(root);
            //cout<<endl;
        }

        void LL(AVLnode *&n)
        {   //cout<<"!";
            LLtimes++;
            AVLnode *tmp=n->left;
            n->left=tmp->right;
            tmp->right=n;

            n->height=max(height(n->left),height(n->right))+1;

            n=tmp;
        }

        void RR(AVLnode *&n)
        {
            RRtimes++;
            AVLnode *tmp=n->right;
            n->right=tmp->left;
            tmp->left=n;

            n->height=max(height(n->left),height(n->right))+1;

            n=tmp;
        }

        void LR(AVLnode *&n)
        {
            LRtimes++;
            RR(n->left);
            RRtimes--;
            LL(n);
            LLtimes--;
        }

        void RL(AVLnode *&n)
        {
            RLtimes++;
            LL(n->right);
            LLtimes--;
            RR(n);
            RRtimes--;
        }

        void preorder(AVLnode* n)
        {
            if(n)
            {
                if(n==root)
                {
                    cout<<"Pre-order:"<<n->data;
                }
                else
                {
                    cout<<","<<n->data;
                }
                preorder(n->left);
                preorder(n->right);
            }
        }

        void show_rotation()
        {
            cout<<endl;
            cout<<"LL:"<<LLtimes<<endl;
            cout<<"LR:"<<LRtimes<<endl;
            cout<<"RL:"<<RLtimes<<endl;
            cout<<"RR:"<<RRtimes<<endl;
        }
};

/*
class AVLnode
{
    public:
        int data;
        int height;
        AVLnode* left;
        AVLnode* right;
        AVLnode* parent;

        AVLnode(int value)
        {
            data=value;
            height=0;
            left=NULL;
            right=NULL;
            parent=NULL;
        }
        ~AVLnode()
        {
            delete left;
            delete right;
        }

        int update_height()
        {
            if(left && right)
            {
                if(left->height>right->height)
                    height=left->height+1;
                else
                    height=right->height+1;
            }
            else if(left)
            {
                height=left->height+1;
            }
            else if(right)
            {
                height=right->height+1;
            }
            else
            {
                height=0;
            }

            return height;
        }

        void rotateLeft(AVLnode *n)
        {
            AVLnode * p=n->parent;
            enum {left, right} side;
            if(p)
            {
                if(p->left==n)
                    side=left;
                else
                    side=right;
            }
            AVLnode * tmp=n->right;
            n->right=tmp->left;
            tmp->left->parent=n;
            n->update_height();
            tmp->left=n;
            n->parent=tmp;
            tmp->update_height();

            if(p)
            {
                if(side==left)
                {
                    p->left=tmp;
                    tmp->parent=p;
                    p->update_height();
                }
                if(side==right)
                {
                    p->right=tmp;
                    tmp->parent=p;
                    p->update_height();
                }
            }
            else
            {
                root=tmp;
            }
        }


        void rotateRight(AVLnode *n)
        {
            AVLnode* p=n->parent;
            enum {left, right} side;
            if(p)
            {
                if(p->left==n)
                    side = left;
                else
                    side = right;
            }
            AVLnode * temp=n->left;
            n->left=tmp->right;
            tmp->right->parent=n;
            n->update_height();
            tmp->right=n;
            n->parent=tmp;
            tmp->update_height();

            if(p)
            {
                if(side==left)
                {
                    p->left=tmp;
                    tmp->parent=p;
                    p->update_height();
                }
                else if(side==right)
                {
                    p->right=tmp;
                    tmp->parent=p;
                    p->update_height();
                }
            }
            else
            {
                root=tmp;
            }
        }

        void balance()
        {
            int bal;
            if(left && right)
            {
                bal=left->height-right->height;
            }
            else(left)
            {
                bal=left->height+1;
            }
            else(right)
            {
                bal=right->height+1;
            }
            else
            {
                bal=0;
            }


            if(bal>1)
            {
                if(left->left->height-left->right->height<0)
                    rotateLeft(n->left);
                rotateRight(n);
            }
            else if(bal<-1)
            {
                if(right->left->height-right->right->height>0)
                    rotateRight(n->right);
                rotateLeft(n);
            }
    }
};

class AVLtree
{
    public:
        AVLnode *root;

        AVLtree()
        {
            root=NULL;
        }
        ~AVLtree()
        {
            delete root;
        }

        void insert(int value)
        {
            AVLnode* newnode;
            if(root==NULL)
            {
                root=new AVLnode(value);
            }
            else
            {
                AVLnode *tmp=root;

                while(true)
                {
                    if(value<tmp->data)
                    {
                        if(!tmp->left)
                        {
                            newnode=new AVLnode(value);
                            newnode->parent=tmp;
                            tmp->left=newnode;
                            break;
                        }
                        else
                            tmp=tmp->left;
                    }
                    else if(value>tmp->data)
                    {
                        if(!tmp->right)
                        {
                            newnode=new AVLnode(value);
                            newnode->parent=tmp;
                            tmp->right=newnode;
                            break;
                        }
                        else
                            tmp=tmp->right;
                    }
                }

                for(tmp=newnode; tmp; tmp=tmp->parent)
                {
                    tmp->update_height();
                    tmp->balance();
                }
            }
        }
}
*/
int main(int argc, char* argv[])
{
    ifstream fin;
    fin.open(argv[1]);
    fin.open("test_hw4");
    AVLtree *tree=new AVLtree;

    string str;
    while(getline(fin,str))
    {
        int i=str_to_int(str);
        //cout<<i<<endl;
        tree->insert(i);
    }

    tree->preorder(tree->root);
    tree->show_rotation();

}
