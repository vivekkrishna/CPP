#include<iostream>
#include<string>
#include<cmath>
using namespace std;
class polygon{
	double a;
	double s;
	double area;
	double periphery;
	public:
		void setsides(double l,double n){
			a=l;
			s=n;
			this->calculate();
			this->display();						//using this pointer
			cout<<this<<endl;
		}
		void calculate(){
			if(s==3){
				area=(sqrt(3)/4)*a*a;
				periphery=3*a;
			}
			else if(s==4){
				area=a*a;
				periphery=4*a;
			}
		}
		void display(){
			cout<<area<<endl;
			cout<<periphery<<endl;
		}
};
main(){
	double a,s;
	polygon p1;
	cout<<"input num of sides and length of each side of a regular polgon"<<endl;
	cin>>s;
	cin>>a;
	p1.setsides(a,s);
	//p1.calculate();
	//p1.display();
	
}

