#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

static Mat hstackSameHeight(const Mat& left, const Mat& right, int targetH = -1) {
    if (left.empty() || right.empty()) return Mat();
    if (targetH <= 0) targetH = max(left.rows, right.rows);
    auto resizeH=[&](const Mat& img){if(img.rows==targetH) return img;double s=targetH/(double)img.rows;Mat o;resize(img,o,Size(int(img.cols*s),targetH),0,0,INTER_AREA);return o;};
    Mat L=resizeH(left),R=resizeH(right),out;hconcat(L,R,out);return out;
}

int main(int argc,char** argv){
    if(argc<3){cerr<<"Usage: "<<argv[0]<<" <original_img> <compressed_img> [title]\n";return 1;}
    string pathA=argv[1],pathB=argv[2],title=(argc>=4?argv[3]:"Original | Compressed");
    Mat A=imread(pathA),B=imread(pathB);if(A.empty()||B.empty()){cerr<<"Erreur lecture images\n";return 1;}
    Mat vis=hstackSameHeight(A,B);putText(vis,"Original",Point(20,40),FONT_HERSHEY_SIMPLEX,1.0,Scalar(255,255,255),2);
    putText(vis,"Compressed",Point(vis.cols/2+20,40),FONT_HERSHEY_SIMPLEX,1.0,Scalar(255,255,255),2);
    imshow(title,vis);waitKey(0);return 0;
}
