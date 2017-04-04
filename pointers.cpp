#include<iostream>
using namespace std;
main(){
	int x=50;
	int a[10]={10,20,30,4,5,6,7,8,9,11};
	int *ptr;
	ptr=a;
//	for(int i=0;i<10;i++){
//		cin>>*(ptr+i)>>endl;
			
//	}
	ptr=a;
	for(int i=0;i<10;i++){
		cout<<*(ptr+i)<<endl;
		
	}
}
