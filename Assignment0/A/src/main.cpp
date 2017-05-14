#include "Colorize.hpp"
#include "Swatch.hpp"
#include "UI.hpp"
#include "LuminanceUtils.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;


int main(int argc, char** argv){
	if( argc != 3)
    {
     cout <<" Usage: colorize <colored> <grayscale>" << endl;
     return -1;
    }

    Mat src1,target1,target2,src2;
    src1 = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    target1 = imread(argv[2], CV_LOAD_IMAGE_COLOR);
    target2 = target1.clone();
    src2 = src1.clone();
    vector<pair<Swatch,Swatch> > swatches =  getSwatches(src1,target1);
    
    namedWindow( "Colored_Hist_Matching", WINDOW_AUTOSIZE );
    Colorize(src1,target1, swatches,getMap_HistogramMatching,"Colored_Hist_Matching");
    imshow( "Colored_Hist_Matching", target1 );
       
    namedWindow( "Colored_Lum_Remapping", WINDOW_AUTOSIZE );
    Colorize(src2,target2, swatches,getMap_LumRemapping,"Colored_Lum_Remapping");
    imshow( "Colored_Lum_Remapping", target2 );   
    
    waitKey(0);
}