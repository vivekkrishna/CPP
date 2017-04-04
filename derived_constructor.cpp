#include<iostream>
using namespace std;
class b{
	int x;
	public:
	b(int i){
		x=i;
	}
	~b(){
	}
	displayb(){
			cout<<x<<endl;
		}
};
class d:public b{
	double y;
	public:
		d(int i,int j):b(i){			//constructor of derived class
			y=j;
		}
		~d(){
		}
		display(){
			cout<<y<<endl;
			displayb();
		}
};
main(){
	d d1(4,3);
	d1.display();
}
