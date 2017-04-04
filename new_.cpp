#include<iostream>
using namespace std;
main(){
	int *a;
	a=new int[5];
	for(int i=0;i<5;i++){
		cin>>*(a+i);
	}
	for(int i=0;i<5;i++){
		cout<<*(a+i)<<endl;
	}
	delete []a;
}
