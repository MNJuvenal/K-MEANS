#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

using VecD = vector<double>;
using MatD = vector<VecD>;

static inline double l2(const VecD& a, const VecD& b) {
    double s = 0.0;
    for (size_t i = 0; i < a.size(); ++i) { double d = a[i] - b[i]; s += d*d; }
    return sqrt(s);
}

vector<int> find_closest_centroids(const MatD& X, const MatD& C) {
    vector<int> idx(X.size());
    for (size_t i = 0; i < X.size(); ++i) {
        double best = numeric_limits<double>::infinity();
        int bestj = 0;
        for (size_t j = 0; j < C.size(); ++j) {
            double d = l2(X[i], C[j]);
            if (d < best) { best = d; bestj = (int)j; }
        }
        idx[i] = bestj;
    }
    return idx;
}

MatD compute_centroids(const MatD& X, const vector<int>& idx, int K) {
    const int n = (int)X[0].size();
    MatD C(K, VecD(n, 0.0));
    vector<int> cnt(K, 0);
    for (size_t i = 0; i < X.size(); ++i) {
        int k = idx[i]; cnt[k]++;
        for (int t = 0; t < n; ++t) C[k][t] += X[i][t];
    }
    for (int k = 0; k < K; ++k) if (cnt[k]) for (int t = 0; t < n; ++t) C[k][t] /= cnt[k];
    return C;
}

MatD kmeans_init_centroids(const MatD& X, int K, uint64_t seed = random_device{}()) {
    vector<int> perm(X.size()); iota(perm.begin(), perm.end(), 0);
    mt19937_64 rng(seed); shuffle(perm.begin(), perm.end(), rng);
    MatD C(K);
    for (int k = 0; k < K; ++k) C[k] = X[perm[k]];
    return C;
}

pair<MatD, vector<int>> run_kmeans(const MatD& X, int K, int max_iters = 10, uint64_t seed = random_device{}()) {
    MatD C = kmeans_init_centroids(X, K, seed);
    vector<int> idx(X.size(), 0);
    for (int it = 0; it < max_iters; ++it) {
        idx = find_closest_centroids(X, C);
        MatD newC = compute_centroids(X, idx, K);
        for (int k = 0; k < K; ++k) {
            bool empty = all_of(newC[k].begin(), newC[k].end(), [](double v){return v==0.0;});
            if (empty) newC[k] = C[k];
        }
        C.swap(newC);
    }
    return {C, idx};
}

int main(int argc, char** argv) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <input_image> <output_image> <K> [iters=10]\n";
        return 1;
    }
    string inPath = argv[1], outPath = argv[2];
    int K = stoi(argv[3]);
    int iters = (argc >= 5 ? stoi(argv[4]) : 10);

    Mat img = imread(inPath, IMREAD_COLOR);
    if (img.empty()) { cerr << "Cannot read image: " << inPath << "\n"; return 1; }
    img.convertTo(img, CV_32FC3, 1.0/255.0);

    int rows = img.rows, cols = img.cols;
    MatD X; X.reserve(rows*cols);
    for (int r=0;r<rows;r++) {
        const Vec3f* p=img.ptr<Vec3f>(r);
        for(int c=0;c<cols;c++){Vec3f bgr=p[c];X.push_back({(double)bgr[2],(double)bgr[1],(double)bgr[0]});}
    }
    auto [C, idx] = run_kmeans(X, K, iters);

    Mat out(rows, cols, CV_32FC3);
    size_t t=0;
    for (int r=0;r<rows;r++){Vec3f* p=out.ptr<Vec3f>(r);
        for (int c=0;c<cols;c++,t++){auto& cc=C[idx[t]];p[c]=Vec3f((float)cc[2],(float)cc[1],(float)cc[0]);}}
    out.convertTo(out, CV_8UC3,255.0);
    imwrite(outPath,out);
    cout << "Saved compressed image to: " << outPath << "\n";
    return 0;
}
