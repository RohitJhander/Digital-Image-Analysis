#ifndef SWATCH
#define SWATCH
#include <iostream>

using namespace std;

class Swatch{
    int top = 0;
    int bot = 0;
    int right = 0;
    int left = 0;
    bool isValid = false;
    public:
    int getTop();
    int getBot();
    int getRight();
    int getLeft();
    void print();
    void includePoint(int x,int y);
    void clear();
};

#endif