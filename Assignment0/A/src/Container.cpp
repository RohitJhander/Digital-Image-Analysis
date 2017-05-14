#include "Container.hpp"

typedef map<int,int>::iterator itr;

vector<int> Container::getCDF(int Max){
	vector<int> CDF;
	int acc = 0;
	for(int i=0;i!=Max;i++){
		acc += freqDist[i];
		CDF.push_back(acc);
	}
	return CDF;
}

float Container::getSD(){
	float x = getMean();
	float x2 = 0;
	float n = 0;
	for(itr i = freqDist.begin(); i!=freqDist.end(); i++){
		n += (*i).second;
		x2 += (*i).first*(*i).first*(*i).second;
	}
	x2/=n;
	return sqrt(x2-x*x);
}
float Container::getMean(){
	float x = 0;
	float n = 0;
	for(itr i = freqDist.begin(); i!=freqDist.end(); i++){
		n += (*i).second;
		x += (*i).first*(*i).second;
	}
	x/=n;
	return x;
}

int Container::getSize(){
	int n = 0;
	for(itr i = freqDist.begin(); i!=freqDist.end(); i++){
		n += (*i).second;
	}
	return n;
}

void Container::insert(int val){
	freqDist[val]++;
}