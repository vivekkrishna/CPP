#include<iostream>
using namespace std;
class D{
	protected:
		int x;
	public:
			D(int a){
				x=a;
			}	
};
class D1:public D{
	protected:
		int y;
		public:
			D1(int a,int b):D(a){
				y=b;
			}
};
class D2:public D1{
	int z;
	public:
		D2(int a,int b,int c):D1(a,b){
			z=c;
		}
		display(){
			cout<<z<<endl;
		}
};
main(){
	D2 d2(1,2,3);
	d2.display();
}
