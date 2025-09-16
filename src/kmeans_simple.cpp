#include <iostream>
#include "kmeans_lib.hpp"

using namespace std;
using namespace KMeansLib;

int main() {
    // Données d'exemple
    Matrix points = {
        {1.0, 1.0}, 
        {1.5, 2.0}, 
        {3.0, 4.0}, 
        {5.0, 7.0}, 
        {3.5, 5.0}, 
        {4.5, 5.0}, 
        {3.5, 4.5}
    };
    
    int k = 2;
    auto result = kmeans(points, k, 10);
    
    cout << "Centroids:\n";
    for (size_t i = 0; i < result.centroids.size(); ++i) {
        for (size_t j = 0; j < result.centroids[i].size(); ++j) {
            cout << result.centroids[i][j];
            if (j < result.centroids[i].size() - 1) cout << " ";
        }
        cout << "\n";
    }
    
    cout << "Assignments:\n";
    for (size_t i = 0; i < result.assignments.size(); ++i) {
        cout << result.assignments[i];
        if (i < result.assignments.size() - 1) cout << " ";
    }
    cout << "\n";
    
    cout << "Final cost: " << result.finalCost << "\n";
    
    return 0;
}