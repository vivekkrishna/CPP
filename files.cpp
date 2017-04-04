#include<fstream>
#include<iostream>
using namespace std;
main()
{
	char str[30];
	ofstream out_tofile("Myfile");			//out_tofile is programmer defined
	out_tofile<<"text in text file.\n";
	out_tofile<<"line 2.\n";
	out_tofile<<"line 3.\n";
	out_tofile.close();
	ifstream in_from_file("Myfile");
	in_from_file.getline(str,20);
	cout<<str<<endl;
	in_from_file.getline(str,20);
	cout<<str<<endl;
	in_from_file.close();
	
	
	if(!in_from_file)
	{
		cout<<"this file does not exist"<<endl;
	}
	else
	{
	
	while(in_from_file)
	{
		in_from_file.getline(str,60);
		cout<<str<<endl;
		
	}
	in_from_file.close();
	}
	return 0;
}
