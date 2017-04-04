#include<iostream>
#include<cstring>
using namespace std;
class student{
	string name;
	int roll_no;
	public:
		studen(string n,int r){
			name=n;
			roll_no=r;
		}
		displays(){
			cout<<name<<"\t\t"<<roll_no<<"\t\t";
		}
};
class Grades:public student{
	int roll_no1;
	char grade1;
	char grade2;
	public:
		Grade(string n,int r,char g1,char g2){
			studen(n,r);
			roll_no1=r;
			grade1=g1;
			grade2=g2;
		}		
		display(){
			displays();
			cout<<grade1<<"\t\t"<<grade2<<endl;
		}	
};
int main(){
	string name;
	int roll;
	char g1;
	char g2;
	Grades *store;
	Grades *ptr=new Grades[2];
	for(int i=1;i<3;i++){
		cout<<"enter name"<<endl;
		cin>>name;
		cout<<"enter roll"<<endl;
		cin>>roll;
		cout<<"g1"<<endl;
		cin>>g1;
		cout<<"g2"<<endl;
		cin>>g2;
		ptr->Grade(name,roll,g1,g2);
		if(i==1){
			store=ptr;
		}
		//ptr->Grades(name,roll,g1,g2);
		cout<<ptr<<endl;
		ptr++;
}
cout<<store<<endl;
Grades *ptr1;
	ptr1=store;
for(int i=1;i<3;i++){
	
	ptr1->display();
	ptr1++;
}
	
	
}
