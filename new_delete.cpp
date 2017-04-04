#include<iostream>
using namespace std;
main(){
	int *k;
	k=new int[5];
	for(int i=0;i<5;i++){
		cin>>k[i];
	}
	for(int i=0;i<5;i++){
		cout<<k[i];
	}
	delete []k;
}

