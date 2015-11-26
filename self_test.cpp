#include "SharedPtr.hpp"
#include <iostream>
  
using namespace cs540;

class Base{
	protected:
		~Base(){
			std::cout << "Calling Base destructor" << std::endl;	
		};
};

class Derived : public Base{
	public:
		~Derived(){
			std::cout << "Calling Derived destructor" << std::endl;
		};
		int value;
};


int main(int argc, char** argv){
//	int * a  = new int(10);
//	SharedPtr<int> p(a);
//	SharedPtr<int> copied(p);
//	SharedPtr<int> s;
//	s = copied;
	{
		SharedPtr<Base> sp;
		{
			Derived * d1 = new Derived;
			SharedPtr<Derived> sp2(d1);
			sp = sp2;
		}
	}
}
