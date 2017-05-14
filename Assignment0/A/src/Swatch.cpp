#include "Swatch.hpp"

int Swatch::getTop(){
	return top;
}
int Swatch::getBot(){
	return bot;
}
int Swatch::getRight(){
	return right;
}
int Swatch::getLeft(){
	return left;
}

void Swatch::includePoint(int x,int y){
	if(x<0 || y<0)
		return;
	if(!isValid){
		isValid = true;
		top = y;
		bot = y;
		left = x;
		right = x;
		return;
	}
	if(x < left)
		left = x;
	if(x > right)
		right = x;
	if(y < bot)
		bot = y;
	if(y > top)
		top = y;
}

void Swatch::clear(){
	isValid = false;
}

void Swatch::print(){
	cout<<top<<" "<<bot<<" "<<right<<" "<<left<<endl;
}
