
/////////////////////////////////Virtual function example/////////////////////

#include<iostream>
using namespace std;
class B{
	public:
	int x=5;
	printhi(){
	cout<<"hi"<<endl;
	}
};									//two functions of same name in base and derived class
class D:public B{
	public:
	int y;
	printhi(){
	cout<<"hi2"<<endl;
	}
};
class E:public D{
	public:
	printhi(){
	cout<<"child derived class"<<endl;
	}
};
main(){
	B b;
	D d;
	E e;
	B *bptr;						//pointer to base class
	bptr=&b;						//pointer of base class pointing to base class
	bptr->printhi();
	bptr=&d;						//pointer of base class pointing to derived class
	//A base class pointer can be made to point to object of derived class but a derived class pointer
	//can't be made to point to objects of base class
	bptr->printhi();
	//cout<<ptr1<<endl;
	bptr=&e;
	bptr->printhi();
}
