#include<iostream>
using namespace std;
main(){
	int KBS[2][3][4]={1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5};
	int (*pkbs)[3][4]=KBS;
	for(int i=0;i<2;i++){
		for(int j=0;j<3;j++){
			for(int k=0;k<4;k++){
				cout<<*(*(*(pkbs+i)+j)+k)<<" ";
			}
			cout<<"\n";
		}
		cout<<"\n";
	}
	return 0;
}
