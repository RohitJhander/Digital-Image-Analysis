#include "LuminanceUtils.hpp"

Swatch alligneSwatch(Size size,Swatch swatch){

}

Container insertImageData(Mat& img,Swatch swatch){
    Container c;
    int nChannels = img.channels();
    for(int j = swatch.getBot(); j <= swatch.getTop(); j++){
        uchar* row  = img.ptr<uchar>(j);
        for(int i = swatch.getLeft(); i<=swatch.getRight(); i++){
            c.insert(row[i*nChannels]);
        }
    }
    return c;
}

LumMap getMap_HistogramEqualize(Mat& img,Swatch swatch){
    CV_Assert(img.depth() == CV_8U);

    Container imgData = insertImageData(img,swatch);

    vector<int> CDF = imgData.getCDF(256);
    
    LumMap map;
    float n = imgData.getSize();
    float minSigCDF = 0;
    
    for(int i=0;i!=256;i++){
        if(CDF[i] != 0){
            minSigCDF = CDF[i];
            break;
        }
    }
    
    for(int i=0;i!=256;i++){
        float val = std::max(float(0),((float(CDF[i])-minSigCDF)/(n - minSigCDF))*255);
        map.setLum(i,((int)val));
    }  

    return map; 
}

LumMap getMap_HistogramMatching(Mat& src,Swatch srcSwatch, Mat& target,Swatch targetSwatch){
    LumMap finMap;
    LumMap srcMap = getMap_HistogramEqualize(src,srcSwatch);
    LumMap targetMap = getMap_HistogramEqualize(target,targetSwatch);
    targetMap.fill();
    targetMap = targetMap.inverse();

    for(int i=0;i!=256;i++){
        finMap.setLum(i,targetMap.getLum(srcMap.getLum(i)));
    }
    return finMap;
}

LumMap getMap_LumRemapping(Mat& src,Swatch srcSwatch, Mat& target,Swatch targetSwatch){
    CV_Assert(src.depth() == CV_8U);
    CV_Assert(target.depth() == CV_8U);

    Container srcData = insertImageData(src,srcSwatch);
    Container targetData = insertImageData(target,targetSwatch);

    float srcMean = srcData.getMean();
    float srcSD = srcData.getSD();
    float targetMean = targetData.getMean();
    float targetSD = targetData.getSD();

    LumMap map;

    for(int i=0;i!=256;i++){
        float val = (((float)i)-srcMean)*(targetSD/srcSD) + targetMean;
        map.setLum(i,((int)val));
    }  

    return map;  
}
