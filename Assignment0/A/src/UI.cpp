#include "UI.hpp"

int drag = 0;

Point p1, p2;
Rect rect;
Vec3b color;

int done = 0;

void mouseHandler(int event, int x, int y, int flags, void* param)
{
	if(done)
		return;
    if (event == CV_EVENT_LBUTTONDOWN && !drag){
        p1 = Point(x, y);
        drag = 1;
    }    
    if (event == CV_EVENT_MOUSEMOVE && drag){
        p2 = Point(x, y);
        rect = Rect(p1.x,p1.y,x-p1.x,y-p1.y);
    }
    if (event == CV_EVENT_LBUTTONUP && drag){
        p2 = Point(x, y);
        rect = Rect(p1.x,p1.y,x-p1.x,y-p1.y);
        drag = 0;
        done = 1;
    }
}

void drawRectangles(Mat& src,Mat& target, 
					vector<pair<pair<Rect,Rect> ,Vec3b> >& rectangles, 
					pair<Rect,Rect> buff, Vec3b buffClr){
	Mat src2 = src.clone(), target2 = target.clone();
	for(int i=0;i!=rectangles.size();i++){
		Rect srcRect = rectangles[i].first.first;
		Rect targetRect = rectangles[i].first.second;
		Vec3b color = rectangles[i].second;
		rectangle(src2, srcRect, CV_RGB(color[0],color[1],color[2]), 3, 8, 0);
		rectangle(target2, targetRect, CV_RGB(color[0],color[1],color[2]), 3, 8, 0);
	}
	rectangle(src2, buff.first, CV_RGB(buffClr[0],buffClr[1],buffClr[2]), 3, 8, 0);
	rectangle(target2, buff.second , CV_RGB(buffClr[0],buffClr[1],buffClr[2]), 3, 8, 0);
	imshow( "Source", src2 );
	imshow( "Target", target2 );
}

vector<pair<Swatch,Swatch> > getSwatchAndAnimate(vector<pair<pair<Rect,Rect> ,Vec3b> >& rectangles,
						Mat& src,Mat& target){
	vector<pair<Swatch,Swatch> > swatches;
	String name = "Source";
	Rect srcRect,targetRect;
	Swatch srcSwatch,targetSwatch;
	Rect* currRect = &srcRect;
	rect = Rect();
	done = 0;
	drag = 0;
	color = Vec3b(rand()%256,rand()%256,rand()%256);
	while(1){
		*currRect = rect;
		drawRectangles(src,target,rectangles,make_pair(srcRect,targetRect),color);
		cvSetMouseCallback(name.c_str(), mouseHandler, NULL);
		if(done){
			if(name == "Source"){
				name = "Target";
				currRect = &targetRect;
				done = 0;
				drag = 0;
				rect = Rect();
				srcSwatch.includePoint(srcRect.x,srcRect.y);
				srcSwatch.includePoint(srcRect.x+srcRect.width-1,srcRect.y+srcRect.height-1);
			}
			else if(name == "Target"){

				rectangles.push_back(make_pair(make_pair(srcRect,targetRect),color));
				targetSwatch.includePoint(targetRect.x,targetRect.y);
				targetSwatch.includePoint(targetRect.x+targetRect.width-1,targetRect.y+targetRect.height-1);
				swatches.push_back(make_pair(srcSwatch,targetSwatch));
				name = "Source";
				currRect = &srcRect;
				done = 0;
				drag = 0;
				rect = Rect();
				srcRect  = Rect();
				targetRect = Rect();
				srcSwatch.clear();
				targetSwatch.clear();
				color = Vec3b(rand()%256,rand()%256,rand()%256);
			}
		}
		if(waitKey(10)>0&&!drag&&name=="Source"){

			return swatches;
		}
	}
}


vector<pair<Swatch,Swatch> > getSwatches(Mat& src,Mat& target){
	vector<pair<Swatch,Swatch> > swatches;
	vector<pair<pair<Rect,Rect> ,Vec3b> >  rectangles;
	namedWindow( "Source", WINDOW_AUTOSIZE );
    imshow( "Source", src );
    namedWindow( "Target", WINDOW_AUTOSIZE );
    imshow( "Target", target );
	
	swatches = getSwatchAndAnimate(rectangles,src,target);

	return swatches;
}
