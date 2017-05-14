
#include <opencv2/opencv.hpp>

#include <stdio.h>
#include <iostream>
#include <string>

#include <cmath>

using namespace cv;
using namespace std;

struct Params {
	Mat src;
	Mat dst;
	int sigma;
	int k;
	int p;
	int size;
	int phi;
	int eps;
};

void printParams(Params *params) {
	cout << "sigma: " << float(params->sigma)/100.0
		 << " k: " << float(params->k)/100.0
		 << " p: " << params->p
		 << " size: " << params->size
		 << " phi: " << float(params->phi)/100.0
		 << " eps: " << float(params->eps)/10.0 << endl;
}

string type2str(int type) {
  string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}

Mat applyFilter(Mat src, Mat kernel) {
	Mat dst;
	Point anchor = Point(-1, -1);
	int delta = 0;
	int ddepth = CV_64F;
	filter2D(src, dst, ddepth, kernel, anchor, delta, BORDER_DEFAULT);
	return dst;
}

Mat createKernel(float sigma, float k, float p, int size) {

	Mat kernel1 = getGaussianKernel(size, sigma, CV_64F);
	Mat kernel2 = getGaussianKernel(size, k*sigma, CV_64F);
	Mat kernel = (1.0+p) * (kernel1 * kernel1.t()) - (p) * (kernel2 * kernel2.t());

	return kernel;
}

Mat xDOG(Mat src, float sigma, float k, float p, int size, float phi, float eps) {
	Mat DxKernel = createKernel(sigma, k, p, size);
	Mat DxI = applyFilter(src, DxKernel);

	
	// Threshold
	for (int i = 0; i < DxI.rows; i++) {
		for (int j = 0; j < DxI.cols; j++) {
			double val = DxI.at<double>(i,j);
			DxI.at<double>(i,j) = val <= eps ? 1.0 : (1.0 + tanh(phi * (val - eps)));
		}
	}	

	// Normalize
	Mat dst;
	double maxVal, minVal;
	minMaxIdx(DxI, &minVal, &maxVal);
	DxI.convertTo(dst,CV_8U,255.0/(maxVal-minVal),-255.0/minVal);

	return dst;
}

void xDOG(int x, void* params_) {
	Params *params = (Params *) params_;
	printParams(params);
	params->dst = xDOG(params->src,
		float(params->sigma)/100.0,
		float(params->k)/100.0,
		float(params->p),
		params->size,
		float(params->phi)/10000,
		float(params->eps)/10);

	imshow("Display Image", params->dst);
}

int main(int argc, char** argv ) {
	if ( argc < 2) {
		printf("usage: %s <Image_Path>\n", argv[0]);
		return -1;
	}

	Mat image_src = imread(argv[1], 0);
	
	if ( !image_src.data ) {
		printf("No image data \n");
		return -1;
	} else {
		cout << "Size: " << image_src.size() << endl;
	}

	int sigma = 0;

	Params params;

	params.src = image_src;
	params.sigma = 60;// 110
	params.k = 160;
	params.p = 35; // 25
	params.size = 100;
	params.phi = 230; // 130
	params.eps = 750; // 200

	namedWindow("Original Image", WINDOW_AUTOSIZE);
	imshow("Original Image", image_src);

	namedWindow("Display Image", WINDOW_AUTOSIZE);
	namedWindow("Controller", WINDOW_NORMAL);
	createTrackbar("sigma", "Controller", &(params.sigma), 2000, &(xDOG), (void *) (&params));
	createTrackbar("k", "Controller", &(params.k), 2000, &(xDOG), (void *) (&params));
	createTrackbar("p", "Controller", &(params.p), 2000, &(xDOG), (void *) (&params));
	createTrackbar("phi", "Controller", &(params.phi), 1000, &(xDOG), (void *) (&params));
	createTrackbar("eps", "Controller", &(params.eps), 2500, &(xDOG), (void *) (&params));

	xDOG(0, (void *)(&params));
	int c;
	while((c = waitKey(20)) != 27) {
		if (c == 32) {
			String name = string(argv[1]);
			name = name.substr(0,name.find_last_of(".")) + "_out" + name.substr(name.find_last_of("."),name.length());
			imwrite( name, params.dst);
		}
	}
	return 0;
}
