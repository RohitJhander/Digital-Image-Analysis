#ifndef LUM_MAP
#define LUM_MAP

#include <map>

using namespace std;

class LumMap{
    map<int,int> lumMap;
    public:
    LumMap inverse();
    void setLum(int iLum, int fLum);
    int getLum(int lum);
    void fill();
};

#endif