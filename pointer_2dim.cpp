#include<iostream>
using namespace std;
main(){
	int KBS[3][4]={6,7,4,8,10,13,11,12,5,7,6,4};
	int(*pKBS)[4]=KBS;
	cout<<**pKBS<<"\t"<<*(*(pKBS)+1)<<"\t"<<*(*(pKBS)+2)<<"\t"<<*(*(pKBS)+3)<<endl;
	cout<<**(pKBS+1)<<"\t"<<*(*(pKBS+1)+1)<<"\t"<<*(*(pKBS+1)+2)<<"\t"<<*(*(pKBS+1)+3)<<endl;
	cout<<**(pKBS+2)<<"\t"<<*(*(pKBS+2)+1)<<"\t"<<*(*(pKBS+2)+2)<<"\t"<<*(*(pKBS+2)+3)<<endl;
}
