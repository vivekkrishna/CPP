#include<iostream>
#include<fstream>
using namespace std;
main()
{
	char name[10];
	int a;
	ofstream out_file("test");
	cout<<"enter name"<<endl;
	cin>>name;
	out_file<<name;
	a=out_file.is_open();
	cout<<a<<endl;
	out_file.close();
	a=out_file.is_open();
	cout<<a;
}
