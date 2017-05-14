#ifndef CONTAINER
#define CONTAINER

#include <map>
#include <vector>
#include <math.h>

using namespace std;

class Container{
    map<int,int> freqDist;
    public:
    vector<int> getCDF(int Max);
    float getSD();
    float getMean();
    int getSize();

    void insert(int val);
};

#endif