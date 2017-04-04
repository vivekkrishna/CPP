#include<iostream>
using namespace std;
main(){
	int A[]={1,2,3,4};
	int B[]={5,6,7,8};
	
	int *pab[]={A,B};	//*pab=Address A and *(pab+1) is address of B
	cout<<*(*pab)<<*(*pab+1)<<*(*pab+2)<<*(*pab+3)<<endl;
	cout<<*(*(pab+1))<<*(*(pab+1)+1)<<*(*(pab+1)+2)<<*(*(pab+1)+3)<<endl;
	
}
