#include "Colorize.hpp"

bool** mask;

vector<int> getNeighbourhood(Mat& img, int x, int y){
    vector<int> neighbourhood;
    int nChannels = img.channels();
    if(y-N_GAP < 0){
        y = N_GAP;
    }
    if(y+2 >= img.size().height){
        y = img.size().height-1-N_GAP;
    }
    if(x-N_GAP < 0){
        x = N_GAP;
    }
    if(x+2 >= img.size().width){
        y = img.size().width-1-N_GAP;
    }

    for(int j = -N_GAP; j<= N_GAP ; j++){
        uchar* row  = img.ptr<uchar>(j+y);
        for(int i = -N_GAP; i<=N_GAP ; i++){
             neighbourhood.push_back(row[(x+i)*nChannels]);   
        }
    }
    return neighbourhood;
}

vector<pair<int,int> > getSamples(Swatch swatch, int count){
    vector<pair<int,int> > samples;
    for(int i=0;i!=count;i++){
        int left = swatch.getLeft(), right = swatch.getRight();
        int top = swatch.getTop(), bot = swatch.getBot();

        int x = left + (rand()%(right-left-3)) +2;
        int y = bot + (rand()%(top-bot-3)) +2;
        samples.push_back(make_pair(x,y));
    }
    return samples;
}

float getStatisticValue(Mat& img, int x, int y){
    vector<int> neighbourhood = getNeighbourhood(img,x,y);
    float x1=0,x2=0;
    for(int i = 0; i != neighbourhood.size();i++){
        x1+=neighbourhood[i];
        x2+=neighbourhood[i]*neighbourhood[i];
    }
    x1/=float(neighbourhood.size());
    x2/=float(neighbourhood.size());
    float sd = x2-x1*x1;
    int nChannels = img.channels();
    float val = img.ptr<uchar>(y)[x*nChannels];

    return (0.5*val+0.5*sd);
}

void transferColorByPixel(Mat& src,pair<int,int> from,Mat& target,pair<int,int> to){

    int nChannels = src.channels();
    uchar* srcRow = src.ptr<uchar>(from.second);
    uchar* targetRow = target.ptr<uchar>(to.second);
    targetRow[nChannels*to.first+1]=srcRow[nChannels*from.first+1];
    targetRow[nChannels*to.first+2]=srcRow[nChannels*from.first+2];
}

pair<int,int> getBestSample(float score, map<pair<int,int>, float>& sampleScores){
    pair<int,int> bestPair;
    float bestScore = 10000000000;
    for(map<pair<int,int>, float>::iterator itr = sampleScores.begin();itr!=sampleScores.end();itr++){
        if(abs(score-bestScore)>abs(score-(itr->second))){
            bestPair = itr->first;
            bestScore = itr->second;
        }
    }
    return bestPair;
}

float getL2Dist(vector<int> v1, vector<int> v2){
    float dist = 0;
    for(int i=0;i!=v1.size();i++){
        float diff = v1[i]-v2[i];
        dist += diff*diff;
    }
    dist/=float(v1.size());
    return dist;
}

pair<int,int> getBestTexture(vector<int> tex, map<pair<int,int>,vector<int> >& sampleTex){
    pair<int,int> bestPair;
    float minDist = 1000000000000;
    for( map<pair<int,int>,vector<int> >::iterator itr = sampleTex.begin();itr!=sampleTex.end();itr++){
        float dist = getL2Dist(tex,itr->second);
        if(minDist > dist){
            bestPair = itr->first;
            minDist = dist;
        }
    }
    return bestPair;
}

void colorTransfer(Mat& src,Swatch srcSwatch, Mat& target,Swatch targetSwatch){
    vector<pair<int,int> > samples = getSamples(srcSwatch,SAMPLE_SIZE_TRANSFER);
    map<pair<int,int>, float> sampleScores;
    for(int i=0;i!=samples.size();i++){
        sampleScores[samples[i]] = getStatisticValue(src,samples[i].first,samples[i].second);
    }
    for(int j = targetSwatch.getBot(); j <= targetSwatch.getTop(); j++){
        for(int i = targetSwatch.getLeft(); i<=targetSwatch.getRight(); i++){
            pair<int,int> bestPixel = getBestSample(getStatisticValue(target,i,j),sampleScores);
            transferColorByPixel(src,bestPixel,target,make_pair(i,j));
            mask[i][j] = true;
        }
    }
}

void applyMap(Mat& img, LumMap map, Swatch swatch){
    int nChannels = img.channels();
    for(int j = swatch.getBot(); j <= swatch.getTop(); j++){
        uchar* row  = img.ptr<uchar>(j);
        for(int i = swatch.getLeft(); i<=swatch.getRight(); i++){
            row[i*nChannels] = map.getLum(row[i*nChannels]);
        }
    }
}



void SpreadColorByTexture(Mat& target,map<pair<int,int>,vector<int> >& textureSamples){
    for(int i=0;i!=target.size().width;i++){
        for(int j=0;j!=target.size().height;j++){
            if(!mask[i][j]){
                pair<int,int> bestPixel = getBestTexture(getNeighbourhood(target,i,j),textureSamples);
                transferColorByPixel(target,bestPixel,target,make_pair(i,j));
                mask[i][j] = true;
            }
        }
    }
}

void addTextureSamples(Mat& target,vector<pair<int,int> > samples,map<pair<int,int>,vector<int> >& textureSamples){
    for(int i=0;i!=samples.size();i++){
        textureSamples[samples[i]] = getNeighbourhood(target,samples[i].first,samples[i].second);
    }
}

void setUpMask(Size size){
    mask = new bool*[size.width];
    for(int i=0;i!=size.width;i++){
        mask[i] = new bool[size.height];
        for(int j=0;j!=size.height;j++){
            mask[i][j] = false;
        }
    }
}

void destroyMask(){
    delete[] mask;
}

void Colorize( Mat& src, Mat& target, vector<pair<Swatch,Swatch> > swatches,LumMap(*getMap)(Mat&,Swatch,Mat&,Swatch),string name){
    srand(time(NULL));
    setUpMask(target.size());
    cvtColor(src, src, CV_BGR2Lab);
    cvtColor(target, target, CV_BGR2Lab);
    map<pair<int,int>,vector<int> > textureSamples;

    for(int i=0;i!=swatches.size();i++){
        Swatch srcSwatch = swatches[i].first;
        Swatch targetSwatch = swatches[i].second;

        LumMap lumMap = getMap(src,srcSwatch,target,targetSwatch);
        applyMap(src,lumMap,srcSwatch);
        colorTransfer(src,srcSwatch,target,targetSwatch);
        vector<pair<int,int> > samples = getSamples(targetSwatch,SAMPLE_SIZE_TEXTURE);
        addTextureSamples(target,samples,textureSamples);
    }

    Mat target2 = target.clone();
    
    cvtColor(target2, target2, CV_Lab2BGR);
    imshow( name.c_str(), target2 );

    waitKey(10);

    SpreadColorByTexture(target,textureSamples);
    cvtColor(target, target, CV_Lab2BGR);
    destroyMask();
}




