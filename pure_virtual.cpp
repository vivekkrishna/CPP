#include<iostream>
using namespace std;
class Animal{
	virtual void speak()=0;
};
class Dog:public Animal{
	public:
	void speak(){ cout<<"bow bow"<<endl;}
};
class Cat:public Animal{
	public:
	void speak(){cout<<"miam"<<endl;
	}
};
class Eman:public Animal{
	public:
	void speak(){cout<<"hi"<<endl;
	}
};
main(){
	Eman e1;
	e1.speak();
	Cat c1;
	c1.speak();
}
