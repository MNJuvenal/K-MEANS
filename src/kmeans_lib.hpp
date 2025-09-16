#pragma once
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <limits>
#include <cmath>

namespace KMeansLib {

using Vector = std::vector<double>;
using Matrix = std::vector<Vector>;

// Distance euclidienne au carré (plus rapide)
inline double distanceSquared(const Vector& a, const Vector& b) {
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        double diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sum;
}

// Distance euclidienne
inline double distance(const Vector& a, const Vector& b) {
    return std::sqrt(distanceSquared(a, b));
}

// Trouve les centroïdes les plus proches pour chaque point
std::vector<int> findClosestCentroids(const Matrix& points, const Matrix& centroids) {
    std::vector<int> assignments(points.size());
    
    for (size_t i = 0; i < points.size(); ++i) {
        double bestDistance = std::numeric_limits<double>::infinity();
        int bestCentroid = 0;
        
        for (size_t j = 0; j < centroids.size(); ++j) {
            double dist = distanceSquared(points[i], centroids[j]);
            if (dist < bestDistance) {
                bestDistance = dist;
                bestCentroid = static_cast<int>(j);
            }
        }
        assignments[i] = bestCentroid;
    }
    return assignments;
}

// Calcule les nouveaux centroïdes basés sur les assignations
Matrix computeCentroids(const Matrix& points, const std::vector<int>& assignments, int k) {
    if (points.empty()) return Matrix();
    
    int dimensions = static_cast<int>(points[0].size());
    Matrix centroids(k, Vector(dimensions, 0.0));
    std::vector<int> counts(k, 0);
    
    // Somme des points pour chaque centroïde
    for (size_t i = 0; i < points.size(); ++i) {
        int cluster = assignments[i];
        counts[cluster]++;
        for (int d = 0; d < dimensions; ++d) {
            centroids[cluster][d] += points[i][d];
        }
    }
    
    // Moyenne pour chaque centroïde
    for (int c = 0; c < k; ++c) {
        if (counts[c] > 0) {
            for (int d = 0; d < dimensions; ++d) {
                centroids[c][d] /= counts[c];
            }
        }
    }
    
    return centroids;
}

// Initialise les centroïdes aléatoirement
Matrix initializeCentroids(const Matrix& points, int k, uint64_t seed = 0) {
    if (seed == 0) {
        seed = std::random_device{}();
    }
    
    std::vector<int> indices(points.size());
    std::iota(indices.begin(), indices.end(), 0);
    
    std::mt19937_64 rng(seed);
    std::shuffle(indices.begin(), indices.end(), rng);
    
    Matrix centroids(k);
    for (int i = 0; i < k && i < static_cast<int>(points.size()); ++i) {
        centroids[i] = points[indices[i]];
    }
    
    return centroids;
}

// Algorithme K-means complet
struct KMeansResult {
    Matrix centroids;
    std::vector<int> assignments;
    int iterations;
    double finalCost;
};

KMeansResult kmeans(const Matrix& points, int k, int maxIterations = 100, uint64_t seed = 0) {
    if (points.empty() || k <= 0) {
        return {Matrix(), std::vector<int>(), 0, 0.0};
    }
    
    Matrix centroids = initializeCentroids(points, k, seed);
    std::vector<int> assignments;
    
    for (int iter = 0; iter < maxIterations; ++iter) {
        // Assignation des points aux centroïdes
        std::vector<int> newAssignments = findClosestCentroids(points, centroids);
        
        // Vérifier la convergence
        if (iter > 0 && newAssignments == assignments) {
            assignments = newAssignments;
            break;
        }
        
        assignments = newAssignments;
        
        // Recalculer les centroïdes
        Matrix newCentroids = computeCentroids(points, assignments, k);
        centroids = newCentroids;
    }
    
    // Calculer le coût final
    double cost = 0.0;
    for (size_t i = 0; i < points.size(); ++i) {
        cost += distanceSquared(points[i], centroids[assignments[i]]);
    }
    
    return {centroids, assignments, maxIterations, cost};
}

} // namespace KMeansLib