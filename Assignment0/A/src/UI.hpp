#include <opencv2/opencv.hpp>
#include "Swatch.hpp"
#include <vector>

using namespace cv;
using namespace std;

vector<pair<Swatch,Swatch> > getSwatches(Mat& src,Mat& target);