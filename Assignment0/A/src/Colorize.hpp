#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <map>
#include <vector>
#include "LumMap.hpp"
#include "Swatch.hpp"
#include "LuminanceUtils.hpp"

#define SAMPLE_SIZE_TEXTURE 50
#define SAMPLE_SIZE_TRANSFER 200
#define N_GAP 2
//corresponds to (2*N_GAP+1)*(2*N_GAP+1) neighbourhood
using namespace cv;
using namespace std;

void Colorize( Mat& src, Mat& target, vector<pair<Swatch,Swatch> > swatches,LumMap(*getMap)(Mat&,Swatch,Mat&,Swatch),string name);
