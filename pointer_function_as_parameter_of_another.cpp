#include<iostream>
using namespace std;
int func(int (*)(int,int,int),int,int,int);
int surface(int i,int j,int k){
	return 2*(i*j+j*k+k*i);
}
int volume(int i,int j,int k){
	return i*j*k;
}
main(){
	cout<<func(surface,4,5,6)<<endl;
	cout<<func(volume,4,5,6)<<endl;
}
int func(int (*fp)(int x,int y,int z),int a,int b,int c){
	return (*fp)(a,b,c);
}


