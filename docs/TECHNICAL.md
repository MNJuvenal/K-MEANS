# 📖 Documentation Technique

## 🏗️ Architecture Détaillée

### Core Library (`kmeans_lib.hpp`)

```cpp
namespace KMeansLib {
    // Types de base
    using Vector = std::vector<double>;
    using Matrix = std::vector<Vector>;
    
    // Structure de résultat
    struct KMeansResult {
        Matrix centroids;           // Centroïdes finaux
        std::vector<int> labels;    // Labels pour chaque point
        int iterations;             // Nombre d'itérations effectuées
        double inertia;            // Somme des distances intra-cluster
    };
    
    // Fonction principale
    KMeansResult kmeans(const Matrix& data, int k, int max_iter = 100);
}
```

### Algorithme K-means Détaillé

#### 1. Initialisation des Centroïdes
```cpp
// Méthode: Sélection aléatoire parmi les points existants
std::vector<Vector> init_centroids(const Matrix& data, int k) {
    std::vector<Vector> centroids;
    std::vector<int> indices(data.size());
    std::iota(indices.begin(), indices.end(), 0);
    
    // Mélange aléatoire
    std::shuffle(indices.begin(), indices.end(), generator);
    
    // Sélectionner k premiers
    for(int i = 0; i < k; ++i) {
        centroids.push_back(data[indices[i]]);
    }
    return centroids;
}
```

#### 2. Assignation aux Clusters
```cpp
// Distance euclidienne optimisée
double distance_squared(const Vector& a, const Vector& b) {
    double sum = 0.0;
    for(size_t i = 0; i < a.size(); ++i) {
        double diff = a[i] - b[i];
        sum += diff * diff;  // Évite sqrt() pour la performance
    }
    return sum;
}

// Assignation efficace
std::vector<int> assign_clusters(const Matrix& data, 
                               const std::vector<Vector>& centroids) {
    std::vector<int> labels(data.size());
    
    for(size_t i = 0; i < data.size(); ++i) {
        double min_dist = std::numeric_limits<double>::max();
        int best_cluster = 0;
        
        for(size_t j = 0; j < centroids.size(); ++j) {
            double dist = distance_squared(data[i], centroids[j]);
            if(dist < min_dist) {
                min_dist = dist;
                best_cluster = j;
            }
        }
        labels[i] = best_cluster;
    }
    return labels;
}
```

#### 3. Mise à Jour des Centroïdes
```cpp
std::vector<Vector> update_centroids(const Matrix& data,
                                   const std::vector<int>& labels,
                                   int k) {
    std::vector<Vector> new_centroids(k);
    std::vector<int> counts(k, 0);
    
    // Initialiser avec des zéros
    for(int i = 0; i < k; ++i) {
        new_centroids[i].resize(data[0].size(), 0.0);
    }
    
    // Accumuler les points par cluster
    for(size_t i = 0; i < data.size(); ++i) {
        int cluster = labels[i];
        counts[cluster]++;
        
        for(size_t d = 0; d < data[i].size(); ++d) {
            new_centroids[cluster][d] += data[i][d];
        }
    }
    
    // Calculer les moyennes
    for(int i = 0; i < k; ++i) {
        if(counts[i] > 0) {
            for(auto& coord : new_centroids[i]) {
                coord /= counts[i];
            }
        }
    }
    
    return new_centroids;
}
```

## 🎨 Visualisation 2D

### Architecture Graphique

```cpp
class KMeansVisualizer2D {
private:
    cv::Mat canvas_;
    MinMaxBounds bounds_;
    ColorPalette colors_;
    
public:
    void drawPoints(const Matrix& points, const std::vector<int>& labels);
    void drawCentroids(const std::vector<Vector>& centroids);
    void drawTrajectories(const std::vector<std::vector<Vector>>& history);
    void animate(int delay_ms = 1000);
};
```

### Système de Coordonnées

```cpp
// Transformation données → pixels
struct MinMaxBounds {
    double min_x, max_x, min_y, max_y;
    
    // Constructeur automatique
    MinMaxBounds(const Matrix& data) {
        min_x = max_x = data[0][0];
        min_y = max_y = data[0][1];
        
        for(const auto& point : data) {
            min_x = std::min(min_x, point[0]);
            max_x = std::max(max_x, point[0]);
            min_y = std::min(min_y, point[1]);
            max_y = std::max(max_y, point[1]);
        }
        
        // Ajouter marge 10%
        double margin_x = (max_x - min_x) * 0.1;
        double margin_y = (max_y - min_y) * 0.1;
        min_x -= margin_x; max_x += margin_x;
        min_y -= margin_y; max_y += margin_y;
    }
};

cv::Point2i toScreenCoords(const Vector& point, 
                          const MinMaxBounds& bounds,
                          int width, int height) {
    // Normalisation [0,1]
    double norm_x = (point[0] - bounds.min_x) / (bounds.max_x - bounds.min_x);
    double norm_y = (point[1] - bounds.min_y) / (bounds.max_y - bounds.min_y);
    
    // Mapping vers pixels (Y inversé pour OpenCV)
    int pixel_x = static_cast<int>(norm_x * width);
    int pixel_y = static_cast<int>((1.0 - norm_y) * height);
    
    return cv::Point2i(pixel_x, pixel_y);
}
```

### Palette de Couleurs

```cpp
class ColorPalette {
    std::vector<cv::Scalar> colors_;
    
public:
    ColorPalette() {
        // 30 couleurs distinctes perceptuellement
        colors_ = {
            cv::Scalar(255, 0, 0),      // Rouge vif
            cv::Scalar(0, 255, 0),      // Vert vif  
            cv::Scalar(0, 0, 255),      // Bleu vif
            cv::Scalar(255, 255, 0),    // Cyan
            cv::Scalar(255, 0, 255),    // Magenta
            cv::Scalar(0, 255, 255),    // Jaune
            cv::Scalar(128, 0, 128),    // Violet
            cv::Scalar(255, 165, 0),    // Orange
            // ... 22 couleurs supplémentaires
        };
    }
    
    cv::Scalar getColor(int cluster_id) const {
        return colors_[cluster_id % colors_.size()];
    }
};
```

## 🖼️ Compression d'Images

### Pipeline de Traitement

```cpp
class ImageCompressor {
public:
    cv::Mat compress(const cv::Mat& input, int k, int max_iter = 50) {
        // 1. Conversion couleur
        cv::Mat lab_image;
        cv::cvtColor(input, lab_image, cv::COLOR_BGR2Lab);
        
        // 2. Extraction pixels comme vecteurs 3D
        Matrix pixels = extractPixels(lab_image);
        
        // 3. Clustering K-means
        auto result = KMeansLib::kmeans(pixels, k, max_iter);
        
        // 4. Reconstruction image
        return reconstructImage(lab_image, result);
    }
    
private:
    Matrix extractPixels(const cv::Mat& image) {
        Matrix pixels;
        pixels.reserve(image.rows * image.cols);
        
        for(int y = 0; y < image.rows; ++y) {
            for(int x = 0; x < image.cols; ++x) {
                cv::Vec3b pixel = image.at<cv::Vec3b>(y, x);
                pixels.push_back({
                    static_cast<double>(pixel[0]),  // L
                    static_cast<double>(pixel[1]),  // a  
                    static_cast<double>(pixel[2])   // b
                });
            }
        }
        return pixels;
    }
    
    cv::Mat reconstructImage(const cv::Mat& original, 
                           const KMeansResult& result) {
        cv::Mat compressed = original.clone();
        
        int pixel_idx = 0;
        for(int y = 0; y < compressed.rows; ++y) {
            for(int x = 0; x < compressed.cols; ++x) {
                int cluster = result.labels[pixel_idx];
                const auto& centroid = result.centroids[cluster];
                
                compressed.at<cv::Vec3b>(y, x) = cv::Vec3b(
                    static_cast<uchar>(centroid[0]),
                    static_cast<uchar>(centroid[1]), 
                    static_cast<uchar>(centroid[2])
                );
                pixel_idx++;
            }
        }
        
        // Reconversion BGR
        cv::Mat bgr_result;
        cv::cvtColor(compressed, bgr_result, cv::COLOR_Lab2BGR);
        return bgr_result;
    }
};
```

## ⚡ Optimisations Performance

### 1. Éviter les Allocations
```cpp
// ❌ Lent - nouvelles allocations
std::vector<double> compute_distances(const Vector& point, 
                                    const std::vector<Vector>& centroids) {
    std::vector<double> distances;
    for(const auto& centroid : centroids) {
        distances.push_back(euclidean_distance(point, centroid));
    }
    return distances;
}

// ✅ Rapide - réutilisation mémoire
void compute_distances(const Vector& point,
                      const std::vector<Vector>& centroids,
                      std::vector<double>& distances_out) {
    distances_out.clear();
    distances_out.reserve(centroids.size());
    
    for(const auto& centroid : centroids) {
        distances_out.push_back(euclidean_distance(point, centroid));
    }
}
```

### 2. SIMD et Vectorisation
```cpp
// Utilisation des instructions vectorielles modernes
#ifdef __AVX2__
#include <immintrin.h>

double distance_avx2(const double* a, const double* b, size_t len) {
    __m256d sum = _mm256_setzero_pd();
    
    for(size_t i = 0; i < len - 3; i += 4) {
        __m256d va = _mm256_load_pd(a + i);
        __m256d vb = _mm256_load_pd(b + i);
        __m256d diff = _mm256_sub_pd(va, vb);
        sum = _mm256_fmadd_pd(diff, diff, sum);
    }
    
    // Réduction horizontale
    double result[4];
    _mm256_store_pd(result, sum);
    return result[0] + result[1] + result[2] + result[3];
}
#endif
```

### 3. Parallélisation OpenMP
```cpp
#pragma omp parallel for
for(size_t i = 0; i < data.size(); ++i) {
    labels[i] = find_closest_centroid(data[i], centroids);
}
```

## 🧪 Tests et Validation

### Tests Unitaires (Header-only)
```cpp
#include "kmeans_lib.hpp"
#include <cassert>
#include <cmath>

void test_distance_calculation() {
    Vector a = {0.0, 0.0};
    Vector b = {3.0, 4.0};
    
    double dist = KMeansLib::euclidean_distance(a, b);
    assert(std::abs(dist - 5.0) < 1e-10);
}

void test_simple_clustering() {
    Matrix data = {
        {1.0, 1.0}, {1.1, 1.1}, {1.2, 0.9},  // Cluster 1
        {5.0, 5.0}, {5.1, 5.2}, {4.9, 5.1}   // Cluster 2
    };
    
    auto result = KMeansLib::kmeans(data, 2, 100);
    
    // Vérifier 2 clusters distincts
    assert(result.centroids.size() == 2);
    assert(result.labels.size() == 6);
    
    // Les 3 premiers points → même cluster
    assert(result.labels[0] == result.labels[1]);
    assert(result.labels[1] == result.labels[2]);
    
    // Les 3 derniers points → autre cluster
    assert(result.labels[3] == result.labels[4]);
    assert(result.labels[4] == result.labels[5]);
    
    // Clusters différents
    assert(result.labels[0] != result.labels[3]);
}
```

## 📊 Métriques de Qualité

### Inertie Intra-Cluster
```cpp
double compute_inertia(const Matrix& data, 
                      const std::vector<Vector>& centroids,
                      const std::vector<int>& labels) {
    double total_inertia = 0.0;
    
    for(size_t i = 0; i < data.size(); ++i) {
        int cluster = labels[i];
        double dist_sq = distance_squared(data[i], centroids[cluster]);
        total_inertia += dist_sq;
    }
    
    return total_inertia;
}
```

### Silhouette Score
```cpp
double silhouette_score(const Matrix& data, const std::vector<int>& labels) {
    double total_score = 0.0;
    int n = data.size();
    
    for(int i = 0; i < n; ++i) {
        double a = mean_intra_cluster_distance(data, labels, i);
        double b = mean_nearest_cluster_distance(data, labels, i);
        
        double silhouette = (b - a) / std::max(a, b);
        total_score += silhouette;
    }
    
    return total_score / n;
}
```
