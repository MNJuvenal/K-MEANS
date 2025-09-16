#include <opencv2/opencv.hpp>
#include <bits/stdc++.h>
using namespace std;
using namespace cv;

using VecD = vector<double>;
using MatD = vector<VecD>;

static inline double l2sq(const VecD& a, const VecD& b){
    double s=0.0; for(size_t i=0;i<a.size();++i){ double d=a[i]-b[i]; s+=d*d; } return s;
}
vector<int> find_closest_centroids(const MatD& X, const MatD& C){
    vector<int> idx(X.size());
    for(size_t i=0;i<X.size();++i){
        double best=numeric_limits<double>::infinity(); int bj=0;
        for(size_t j=0;j<C.size();++j){ double d=l2sq(X[i],C[j]); if(d<best){best=d; bj=(int)j;} }
        idx[i]=bj;
    }
    return idx;
}
MatD compute_centroids(const MatD& X, const vector<int>& idx, int K){
    int n=(int)X[0].size(); MatD C(K, VecD(n,0.0)); vector<int> cnt(K,0);
    for(size_t i=0;i<X.size();++i){ int k=idx[i]; cnt[k]++; for(int t=0;t<n;++t) C[k][t]+=X[i][t]; }
    for(int k=0;k<K;++k) if(cnt[k]) for(int t=0;t<n;++t) C[k][t]/=cnt[k];
    return C;
}
MatD kmeans_init_centroids(const MatD& X, int K, uint64_t seed=std::random_device{}()){
    vector<int> perm(X.size()); iota(perm.begin(),perm.end(),0); mt19937_64 rng(seed);
    shuffle(perm.begin(),perm.end(),rng); MatD C(K); for(int k=0;k<K;++k) C[k]=X[perm[k]]; return C;
}
pair<MatD, vector<int>> run_kmeans(const MatD& X, int K, int max_iters=10){
    MatD C = kmeans_init_centroids(X, K);
    vector<int> idx(X.size(),0);
    for(int it=0; it<max_iters; ++it){
        cout << "K-Means iteration " << it << "/" << (max_iters-1) << "\n";
        idx = find_closest_centroids(X, C);
        C = compute_centroids(X, idx, K);
    }
    return {C, idx};
}

// Palette visualization as a grid image
Mat palette_image(const MatD& C, int tile=40, int cols=8){
    int K = (int)C.size();
    int rows = (K + cols - 1) / cols;
    Mat img(rows*tile, cols*tile, CV_8UC3, Scalar(30,30,30));
    for(int k=0;k<K;++k){
        int r = k / cols, c = k % cols;
        const auto& cc = C[k]; // RGB in [0,1]
        Mat roi = img(Rect(c*tile, r*tile, tile, tile));
        roi.setTo(Scalar(cc[2]*255.0, cc[1]*255.0, cc[0]*255.0));
        putText(img, to_string(k), Point(c*tile+5, r*tile+tile-8), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,255,255), 1, LINE_AA);
    }
    return img;
}

int main(int argc, char** argv){
    if(argc < 5){
        cerr << "Usage: " << argv[0] << " <input_image> <output_image> <K> <iters>\n";
        return 1;
    }
    string inPath = argv[1], outPath = argv[2];
    int K = stoi(argv[3]); int iters = stoi(argv[4]);

    Mat imgBGR = imread(inPath, IMREAD_COLOR);
    if(imgBGR.empty()){ cerr << "Cannot read image: " << inPath << "\n"; return 1; }
    Mat img; imgBGR.convertTo(img, CV_32FC3, 1.0/255.0); // [0,1]

    // Flatten to (m,3) RGB
    int rows = img.rows, cols = img.cols;
    MatD X; X.reserve(rows*cols);
    for(int r=0;r<rows;++r){
        const Vec3f* p = img.ptr<Vec3f>(r);
        for(int c=0;c<cols;++c){ Vec3f bgr = p[c]; X.push_back({(double)bgr[2], (double)bgr[1], (double)bgr[0]}); }
    }

    // Run K-Means
    auto [C, idx] = run_kmeans(X, K, iters);

    // Reconstruct image from centroids
    Mat out(rows, cols, CV_32FC3);
    size_t t=0;
    for(int r=0;r<rows;++r){
        Vec3f* p = out.ptr<Vec3f>(r);
        for(int c=0;c<cols;++c,++t){
            const auto& cc = C[idx[t]]; // RGB
            p[c] = Vec3f((float)cc[2], (float)cc[1], (float)cc[0]); // back to BGR
        }
    }
    Mat out8; out.convertTo(out8, CV_8UC3, 255.0);
    imwrite(outPath, out8);
    cout << "Saved compressed image to: " << outPath << "\n";

    // Side-by-side visualization (like notebook)
    Mat visL = imgBGR; // original uint8
    Mat visR = out8;   // compressed
    int targetH = max(visL.rows, visR.rows);
    auto resizeH=[&](const Mat& m){ if(m.rows==targetH) return m; double s=targetH/(double)m.rows; Mat o; resize(m,o,Size((int)(m.cols*s),targetH),0,0,INTER_AREA); return o; };
    Mat L = resizeH(visL), R = resizeH(visR), side; hconcat(L,R,side);
    putText(side, "Original", Point(20,40), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255,255,255), 2, LINE_AA);
    putText(side, "Compressed (K="+to_string(K)+")", Point(L.cols+20,40), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255,255,255), 2, LINE_AA);

    // Palette window
    Mat pal = palette_image(C, 40, min(8, K));

    namedWindow("K-means Compression - Side by Side", WINDOW_AUTOSIZE);
    imshow("K-means Compression - Side by Side", side);
    namedWindow("Palette (centroids)", WINDOW_AUTOSIZE);
    imshow("Palette (centroids)", pal);
    cout << "Press any key to close windows...\n";
    waitKey(0);
    destroyAllWindows();
    return 0;
}
