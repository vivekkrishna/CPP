#include<iostream>
#include<cstring>
using namespace std;
main(){
	string *ptr=new string;
	cin>>*ptr;
	while(cin)
	{
		cout<<ptr<<endl;
		string *ptr=new string;
		cin>>*ptr;		
	}
}
