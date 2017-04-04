#include<iostream>
#include<cmath>
using namespace std;
class ellipse{
	double a,b,area,periphery;
	float pi=3.14;
	public:
		void setdata(double l,double n){
			a=l;
			b=n;
		}
		void calculate(){
			
				area=pi*a*b;
				periphery=2*pi*sqrt((pow(a,2)+pow(b,2))/2);
			
		}
		void display(){
			cout<<area<<endl;
			cout<<periphery<<endl;
		}
};
main(){
	double a,b;
	ellipse e;
	cout<<"enter values"<<endl;
	cin>>a;
	cin>>b;
	e.setdata(a,b);
	e.calculate();
	e.display();
	
}

