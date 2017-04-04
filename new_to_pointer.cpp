#include<iostream>
#include<cstring>
using namespace std;
class applicant{
	public:
	string name;
	string address;
};

main(){
	int a;
	do{
	
	cout<<"if u want to register enter 1 , to close enter 0"<<endl;
	cin>>a;
	if(a==1){
		applicant *ptr;
		ptr=new applicant;
		cout<<"enter name"<<endl;
		cin>>ptr->name;
		cout<<"enter address"<<endl;
		cin>>ptr->address;
		cout<<ptr->name<<endl;
		cout<<ptr->address<<endl;
		cout<<ptr<<endl;
		delete ptr;
	}
			
}while(a!=0);
}
