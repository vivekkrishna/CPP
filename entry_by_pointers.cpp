#include<iostream>
#include<cstring>
using namespace std;

int main(){
	int a,count;
	
	string *store[10];
	count=0;
	do{
		a=0;
	
	cout<<"enter 1 for entry and 2 for display enter 0 for stop"<<endl;
	cin>>a;
	if(a==1){
		string *ptr=new string;
		cout<<"enter name"<<endl;
		cin>>*ptr;
		
		
		store[count]=ptr;
		
		//cout<<ptr<<endl;
		//ptr++;
		//cout<<count++<<endl;
	}
	else if(a==2){
		
		for(int j=0;j<count;j++){
			cout<<*(*(store+j))<<endl;
		}
	}
	
}while(a!=0);
}
