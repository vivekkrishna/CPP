#include<iostream>
using namespace std;
class ellipse{
	float a,b;
	friend class measure;
	public:
		set(int l,int w){
			a=l;
			b=w;
		}
};
class measure{
		int area,circ;
	public:
		ellipse e;
		float measure1(ellipse e){
		area=e.a*e.b;
		circ=e.a+e.b;
}
		void display(){
			cout<<area<<endl;
			cout<<circ<<endl;
		}
};
main(){
	ellipse e1;
	e1.set(4,5);
	measure m;
	m.measure1(e1);				//you have to pass object of original class to a function in friend class to access original class data in friend class..				
	m.display();
}
