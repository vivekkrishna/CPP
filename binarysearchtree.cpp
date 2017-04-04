#include<iostream>
#include<conio.h>
#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<Map>
using namespace std;

class node
{
	public:
	int value;
	node* parent;
	node* leftnode=NULL;
	node* rightnode=NULL;	
};

class tree
{
	public:
	node* startnode=NULL;
	void insertnode(node*,node*);
//	void deletenode(node*);
	//void searchforvalue(int value);
};

void tree::insertnode(node* nd,node* atnode)
{
	static int startnodeset=0;
	if(startnodeset)
	{
		cout<<nd->value<<" values "<<atnode->value<<endl;
		if(atnode->value>nd->value)
		{
			if(atnode->leftnode==NULL)
			{
				*(atnode->leftnode)=*nd;
				cout<<nd->value<<" left to "<<atnode->value<<endl;
			}
			else
			{
				insertnode(nd,atnode->leftnode);
			}
		}
		else if(atnode->value<nd->value)
		{
			if(atnode->rightnode==NULL)
			{
				*(atnode->rightnode)=*nd;
				//cout << a << " " << b << endl
				cout<<nd->value<<" right to "<<atnode->value<<endl;
			}
			else
			{
				insertnode(nd,atnode->rightnode);
			}
		}
		else
		{
			cout<<"A node with this value already exists"<<endl;
		}
	}
	else
	{
		*atnode=*nd;
		startnodeset=1;
		cout<<"here"<<endl;
	}
}

void findverticallists(node* root,int hd,map<int,vector<int> > &mp)
{
	mp[hd].push_back(root->value);
	if(root->leftnode!=NULL)
	{
		findverticallists(root->leftnode,hd-1,mp);	
	}
	if(root->rightnode!=NULL)
	{
		findverticallists(root->rightnode,hd+1,mp);
	}
	
}
int main()
{
	
	int i;
	bool j=0;
	tree* tr=new tree;
	do{
	cout<<"enter an integer to insert into binary search tree"<<endl;
	cin>>i;
	cout<<i<<endl;
	node* nd=new node;
	nd->value=i;
	tr->insertnode(nd,tr->startnode);
	cout<<"enter 1 if you want to enter another node into bst"<<endl;
	cin>>j;
	cout<<j<<endl;
	}while(j);
	map <int,vector<int> > mp;
	findverticallists(tr->startnode,0,mp);
	map <int,vector<int> > :: iterator it;
	for(it=mp.begin();it!=mp.end();it++)
	{
		for(i=0;i<it->second.size();++i)
		{
			cout << it->second[i] << " ";
		}
		cout<<endl;
	}
}
