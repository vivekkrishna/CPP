#include<iostream>
#include<fstream>
using namespace std;
int main()
{
char Name[40];
char RegNo[11];
int Age;
char Grade [2];
ofstream outfile ;
outfile.open("Student", ios::app); // Appending at the end of file

cout<< "Enter the grade of student: ";
cin>> Grade;
outfile << Grade;
outfile.close();
ifstream infile ("Student"); // input stream from file
cout<< "Output from file is :"<<endl;
infile>>Name; // extracting name from file
cout<<"Name = "<<Name<<endl;
infile >>RegNo ; // extracting registration number
cout<<"Reg.No= "<<RegNo<<endl;
infile >> Age;
cout<<"Age = "<<Age<<endl; // extracting age of student
infile>> Grade;
cout << "Grade = " << Grade<<endl;
infile.close();
return 0;
}
