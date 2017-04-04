#include<iostream>
using namespace std;
class c1{
	public:
	int a=3,b=5;
};
main(){
	c1 co;
	c1 *ptr=&co;
//	ptr=&co;
	int c1::*ptr1=&c1::a;		//pointer to data member of a class
	cout<<co.*ptr1<<endl;		//data member pointer used with an object
	cout<<ptr->a<<endl;			
	cout<<ptr->b<<endl;
}
