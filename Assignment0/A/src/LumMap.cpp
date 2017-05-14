#include "LumMap.hpp"

LumMap LumMap::inverse(){
	LumMap invMap;
	for(map<int,int>::iterator itr = lumMap.begin();itr != lumMap.end();itr++){
		invMap.setLum((*itr).second,(*itr).first);
	}
	return invMap; 
}

void LumMap::setLum(int iLum,int fLum){
	lumMap[iLum] = fLum;
}

int LumMap::getLum(int lum){
	return lumMap[lum];
}

void LumMap::fill(){
	int i=0;
	while(i<256){
		int j=i;
		while(lumMap[i]==lumMap[j])
			j++;
		for(int k=i;k!=j;k++){
			lumMap[k] = lumMap[i] + ((int)(float((lumMap[j]-lumMap[i])*(k-i))/float(j-i)));
		}
		i=j;
	}
}