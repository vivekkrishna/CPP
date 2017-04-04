#include <fstream>
#include <iostream>
using namespace std;
int main ()
{ 
ofstream outfile("Myfile1");

outfile<<"I find you have keen interest in C++.\n";
outfile <<"You need a compiler to practice.\n";
outfile.put('D'); // write character D
outfile.put('E').put('L').put('H').put('I').put('\n');
outfile.close();

if (outfile.is_open()) // use of is_open
cout<<"The file is open."<<endl;
else
cout<<"The file is closed."<<endl;

char kh;

ifstream infile ("Myfile1");

while (infile)
{infile.get(kh); // read the file character by character
cout <<kh;} // display it on monitor

infile.close();
if(!(outfile.is_open()))
cout<<"The file is closed"<<endl;
return 0 ;
}
