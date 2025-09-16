#include <bits/stdc++.h>
using namespace std;

using Vec = vector<double>;
using Mat = vector<Vec>;

static inline double l2(const Vec& a, const Vec& b) {
    double s = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        double d = a[i] - b[i];
        s += d * d;
    }
    return sqrt(s);
}

vector<int> find_closest_centroids(const Mat& X, const Mat& centroids) {
    const int m = (int)X.size();
    vector<int> idx(m, 0);
    for (int i = 0; i < m; ++i) {
        double best = numeric_limits<double>::infinity();
        int bestj = 0;
        for (int j = 0; j < (int)centroids.size(); ++j) {
            double d = l2(X[i], centroids[j]);
            if (d < best) { best = d; bestj = j; }
        }
        idx[i] = bestj;
    }
    return idx;
}

Mat compute_centroids(const Mat& X, const vector<int>& idx, int K) {
    const int n = (int)X[0].size();
    Mat centroids(K, Vec(n, 0.0));
    vector<int> counts(K, 0);
    for (int i = 0; i < (int)X.size(); ++i) {
        int k = idx[i];
        counts[k]++;
        for (int t = 0; t < n; ++t) centroids[k][t] += X[i][t];
    }
    for (int k = 0; k < K; ++k)
        if (counts[k]) for (int t = 0; t < n; ++t) centroids[k][t] /= counts[k];
    return centroids;
}

Mat kmeans_init_centroids(const Mat& X, int K, uint64_t seed = random_device{}()) {
    vector<int> idx(X.size());
    iota(idx.begin(), idx.end(), 0);
    mt19937_64 rng(seed);
    shuffle(idx.begin(), idx.end(), rng);
    Mat C(K);
    for (int k = 0; k < K; ++k) C[k] = X[idx[k]];
    return C;
}

struct KMeansResult {
    Mat centroids;
    vector<int> idx;
};

KMeansResult run_kmeans(const Mat& X, int K, int max_iters = 10, uint64_t seed = random_device{}()) {
    Mat C = kmeans_init_centroids(X, K, seed);
    vector<int> idx(X.size(), 0);
    for (int it = 0; it < max_iters; ++it) {
        idx = find_closest_centroids(X, C);
        Mat newC = compute_centroids(X, idx, K);
        for (int k = 0; k < K; ++k) {
            bool empty = all_of(newC[k].begin(), newC[k].end(), [](double v){return v==0.0;});
            if (empty) newC[k] = C[k];
        }
        C.swap(newC);
    }
    return {C, idx};
}

int main() {
    Mat X = {{1.0, 1.0}, {1.5, 2.0}, {3.0, 4.0}, {5.0, 7.0}, {3.5, 5.0}, {4.5, 5.0}, {3.5, 4.5}};
    int K = 2, max_iters = 10;
    auto res = run_kmeans(X, K, max_iters);
    cout << "Centroids:\n";
    for (auto& c : res.centroids) {
        for (auto v : c) cout << v << ' ';
        cout << '\n';
    }
    cout << "Assignments:\n";
    for (auto v : res.idx) cout << v << ' ';
    cout << '\n';
}
