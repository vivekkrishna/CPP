#include <iostream>
using namespace std;
class Cuboid {
friend class paint; // Declaration of friend class
public:
void sides(int , int, int);
int Area();
int volume();
int x , y, z;
}; //end of class Cuboid

void Cuboid:: sides (int L, int W, int H )
{x = L, y = W,z = H; } // Setting the sides of Cuboid
int Cuboid::Area() //Definition of area
{return 2*(x*y +y*z +z*x);}
int Cuboid::volume() // definition of volume
{return x*y*z ;}

class paint{ //declaration of friend class paint
private:
int R;
public:
paint () { R = 1;} // default constructor
paint ( int S) { R = S;} // parametric constructor
Cuboid C; // C is the object of class Cuboid
int area (){return C.Area ();}
int cost(int R , Cuboid C ) // R is the rate and C is object
{return R* C.Area () ;} // of Cuboid. cost() is a function
};

int main()
{
Cuboid C1 ; // C1 is object of class Cuboid
C1.sides (5,6,5 );
paint P1 ; // P1 is object of class paint
int k = 4;
cout << "Volume of C1 = " <<C1.volume()<<"\n";
cout<<"Surface area of C1 = "<<C1.Area()<<"\n";
cout<<"Cost of painting P1 = "<< P1.cost(k, C1)<<"\n";
return 0 ;
}
