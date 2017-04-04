#include <fstream>
#include <iostream>
using namespace std;
int main ()
{ 

ofstream outfile("Mfile1");
outfile<<"Learn C++ file pointers\n"; //write into file
outfile.close();

char str[80];
ifstream infile ("Mfile");
infile.getline(str,80); // read the file
cout<<str<<endl<<endl; // display it on monitor
infile.close(); // close the file

infile.open ("Mfile", ios::in); // ios::in means open file for reading
int g = infile.tellg(); // tell position of get pointer
cout<<"The pointer is at position g = "<<g<<endl;
// display the pointer position

outfile.open("Mfile", ios::app); // ios::app means open for writing at end or appending
outfile<<"which depict position in file"<<endl; // add this line
int p = outfile.tellp(); // tell position of put pointer.
cout<<"The pointer is at position p = "<<p<<endl;
}
