#include<iostream>
#include<fstream>
using namespace std;
main()
{
	char name[30];
	char regno[11];
	int age;
	ofstream to_file;
	to_file.open("student");
	cout<<"enter name"<<endl;
	cin>>name;
	to_file<<name<<endl;
	cout<<"enter regno"<<endl;
	cin>>regno;
	to_file<<regno<<endl;
	cout<<"enter age"<<endl;
	cin>>age;
	to_file<<age<<endl;
	to_file.close();
	
	ifstream from_file;
	from_file.open("student");
	from_file>>name;
	cout<<"name: "<<name<<endl;
	from_file>>regno;
	cout<<"regno: "<<regno<<endl;
	from_file>>age;
	cout<<"age: "<<age<<endl;
}
