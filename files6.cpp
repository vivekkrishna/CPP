#include <fstream>
#include <iostream>
using namespace std;
int main ()
{ 

ofstream outfile("Mfile");
outfile<<"Learn C++ file pointers\n"; // write into file
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

infile.getline(str,80); // read first line
cout<<str<<endl;
infile.getline(str,80); // read the next line
cout<<str<<endl;
outfile.seekp(5, ios::beg ); 	//move 5 bytes from beginning
int n = outfile.tellp(); 		// tell position of pointer
cout<<"The pointer is at position n = "<<n<<endl;
outfile.seekp(-3,ios::cur ); 	// move the back 3 bytes
int m = outfile.tellp(); 		// now tell the position
cout<<"The pointer is at position m = "<<m<<endl;
outfile.close();
//infile.seekg(–6,ios::end ); 
int a=infile.tellg(); 			// tell pointer position
cout<<"The pointer is at position a = "<<a<<endl;
outfile.close();
infile.close();
return 0;
}
