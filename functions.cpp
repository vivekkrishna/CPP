#include<iostream>
using namespace std;

int product(int x,int y){						//function overloading
	return x*y;
}
int product(int x,int y,int z){
	return x*y*z;
}
double product(int x,double y){
	return x*y;
}
main(){
int a=3,b=5,c=6;
double d=0.5;
cout<<product(a,b)<<endl;
cout<<product(a,b,c)<<endl;
cout<<product(c,d)<<endl;	
}
