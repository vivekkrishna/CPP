#include<iostream>
#include<cstring>
using namespace std;
main()
{
	char str1[10]="vivekrish";
	char str2[6]="krish";
	memcpy(str1,str2,5);	//str1=krishrish
	cout<<str1<<endl;
	cout<<str2<<endl;
	//cout<<static_cast<char*>(memchr(str1,'r',3))<<endl;
	cout<<memcmp(str2,str1,4)<<endl;
	memset(str2,'k',3);
	cout<<str2<<endl;
	//cout<<static_cast<char*>(memset(str2,'k',3))<<endl;
}
