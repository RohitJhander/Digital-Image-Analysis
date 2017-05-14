
#include <opencv2/opencv.hpp>

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace cv;


#define L 7 // Maximum levels
// #define K 2 // Kappa
#define MAXF 1000000000.0

float K = 0.2;

Point NBRS9[] = {
	Point(-1,-1),
	Point(-1, 0),
	Point(-1, 1),

	Point(0, 0),
	Point(0, 1),
	Point(0,-1),

	Point(1, 0),
	Point(1, 1),
	Point(1,-1)
};

Point NBRS25[] = {
	Point(0,-1),
	Point(0,-2),

	Point(-1,-2),
	Point(-1,-1),
	Point(-1, 0),
	Point(-1, 1),	
	Point(-1, 2),

	Point(-2,-2),
	Point(-2,-1),
	Point(-2, 0),
	Point(-2, 1),
	Point(-2, 2)
};

Mat A ;
Mat A_;
Mat B ;
Mat B_;

vector<Mat> pyramidA (L);
vector<Mat> pyramidB (L);
vector<Mat> pyramidA_(L);
vector<Mat> pyramidB_(L);
vector<Mat> s(L);

bool inside(const Point& p, const Mat& image){
	return (p.x>=0 && p.x < image.cols && p.y>=0 && p.y < image.rows);
}

void ComputeGaussianPyramid(Mat A, vector<Mat> *pyramids){
	Mat dst=A.clone();
	for(int l=L-1;l>=0;l--){
		pyramids->at(l) = dst.clone();
		pyrDown(dst, dst, Size(dst.cols/2, dst.rows/2));
	}
}
int norm(Vec3b A, Vec3b B){
	int x = int(A.val[0])-int(B.val[0]);
	return (x*x);
}
float distance(Point p, Point q, int l){
	int i;
	int dist=0,count=0;
	Point pr,qr;

	if(inside(p,pyramidA.at(l)) && inside(q,pyramidB.at(l))) {
		dist += norm(pyramidA .at(l).at<Vec3b>(p),pyramidB .at(l).at<Vec3b>(q));
		count++;
	}

	for(i=0;i<12;i++){
		pr = NBRS25[i]+p;
		qr = NBRS25[i]+q;
		if(inside(pr,pyramidA.at(l)) && inside(qr,pyramidB.at(l))){
			dist += norm(pyramidA .at(l).at<Vec3b>(pr),pyramidB .at(l).at<Vec3b>(qr));
			dist += norm(pyramidA_.at(l).at<Vec3b>(pr),pyramidB_.at(l).at<Vec3b>(qr));
			count++;
		}
		pr = -NBRS25[i]+p;
		qr = -NBRS25[i]+q;
		if(inside(pr,pyramidA.at(l)) && inside(qr,pyramidB.at(l))){
			dist += norm(pyramidA .at(l).at<Vec3b>(pr),pyramidB .at(l).at<Vec3b>(qr));
			count++;
		}
	}

	if(l>0){
		l--;
		p.x/=2; p.y/=2;
		q.x/=2; q.y/=2;
		for(i=0;i<9;i++){
			pr = NBRS9[i]+p;
			qr = NBRS9[i]+q;
			if(inside(pr,pyramidA.at(l)) && inside(qr,pyramidB.at(l))) {
				dist += norm(pyramidA .at(l).at<Vec3b>(pr),pyramidB .at(l).at<Vec3b>(qr));
				dist += norm(pyramidA_.at(l).at<Vec3b>(pr),pyramidB_.at(l).at<Vec3b>(qr));
				count++;
			}
		}
	}
	
	if(count==0) {
		return MAXF;
	} else {
		return float(dist)/(count);
	}
}
Point BestApproximationMatch(int l, Point q){
	int j,k,W,H;
	float mini=-1,dist;
	Point p,argmin;
	int aadfasdf;
	W = pyramidA.at(l).cols;
	H = pyramidA.at(l).rows;

	for(j=0;j<W;j++){
		for(k=0;k<H;k++){
			p = Point(j,k);
			dist = distance(p,q,l);
			if(mini==-1 || dist<mini){				
				mini = dist;
				aadfasdf = j+k;
				argmin = Point(j,k);
			}
		}
	}
	return argmin;
}
Point BestCoherenceMatch(int l, Point q){
	int i;
	float mini=-1,dist=-1;
	Point r,p,argmin;
	for(i=0;i<12;i++){
		r = q + NBRS25[i];
		if(inside(r,pyramidB_.at(l))) {
			p = Point(s.at(l).at<Vec2i>(r).val[0],s.at(l).at<Vec2i>(r).val[1]); // s[r]
			p = p - NBRS25[i]; // s[r] + (q-r)
			dist = distance(p,q,l);
		}
		if(mini==-1 || dist<mini){
			mini = dist;
			argmin = p;
		}
	}

	return argmin;
}
Point BestMatch(int l, Point q) {
	Point p_aprox = BestApproximationMatch(l,q);
	// cout << "p_aprox:" << p_aprox << endl;
	Point p_coher = BestCoherenceMatch(l,q);
	// cout << "p_coher:" << p_coher << endl;
	float d_aprox = float(distance(p_aprox, q, l));
	float d_coher = float(distance(p_coher, q, l));
	if(d_coher <= d_aprox*(1+K*pow(2.0,l-L))){
		return p_coher;
	} else {
		return p_aprox;
	}
}
void CreateImageAnalogy() {
	int l,j,k;
	int W,H;
	Point p,q;

	cvtColor(A , A , CV_BGR2Lab);
	cvtColor(A_, A_, CV_BGR2Lab);
	cvtColor(B , B , CV_BGR2Lab);

	cout << A.type() << endl;

	ComputeGaussianPyramid(A ,&pyramidA );
	ComputeGaussianPyramid(B ,&pyramidB );
	ComputeGaussianPyramid(A_,&pyramidA_);

	for(l=0;l<L;l++){
		pyramidB_.at(l) = pyramidB.at(l).clone();
		s[l] = Mat(pyramidB[l].size(), CV_32SC2);
		W = pyramidB_.at(l).cols;
		H = pyramidB_.at(l).rows;

		for(j=0;j<W;j++){
			for(k=0;k<H;k++){
				q = Point(j,k);
				cout << "l,p,q:" << l << p << q << endl;
				p = BestMatch(l,q);
				pyramidB_.at(l).at<Vec3b>(q).val[0] = pyramidA_[l].at<Vec3b>(p).val[0];
				s.at(l).at<Vec2i>(q).val[0] = p.x;
				s.at(l).at<Vec2i>(q).val[1] = p.y;
			}
		}
	}

	B_ = pyramidB_.at(L-1).clone();

	cvtColor(B_, B_, CV_Lab2BGR);
}

int main(int argc, char** argv ) {
	if ( argc < 4) {
		printf("usage: %s <A> <A'> <B>\n", argv[0]);
		return -1;
	}
	A  = imread(argv[1], 1);
	A_ = imread(argv[2], 1);
	B  = imread(argv[3], 1);

	if (argc > 4) K = stof(argv[4]);


	if (!A.data || !B.data || !A_.data) {
		printf("No image data \n");
		return -1;
	}

	CreateImageAnalogy();
	cvtColor(B , B , CV_BGR2Lab);
	imwrite("Output.jpg", B_);
	return 0;
}
