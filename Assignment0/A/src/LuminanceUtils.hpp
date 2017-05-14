#include <opencv2/opencv.hpp>
#include <map>
#include <vector>
#include <algorithm>
#include "LumMap.hpp"
#include "Container.hpp"
#include "Swatch.hpp"
#include <math.h>

using namespace cv;
using namespace std;

LumMap getMap_HistogramEqualize(Mat& img,Swatch swatch);
LumMap getMap_HistogramMatching(Mat& src,Swatch srcSwatch, Mat& target,Swatch targetSwatch);
LumMap getMap_LumRemapping(Mat& src,Swatch srcSwatch, Mat& target,Swatch targetSwatch);