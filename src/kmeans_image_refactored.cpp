#include <opencv2/opencv.hpp>
#include <iostream>
#include "kmeans_lib.hpp"

using namespace std;
using namespace cv;
using namespace KMeansLib;

// Convertit une image OpenCV en matrice de points pour K-means
Matrix imageToPoints(const Mat& image) {
    Matrix points;
    points.reserve(image.rows * image.cols);
    
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            Vec3b pixel = image.at<Vec3b>(i, j);
            points.push_back({
                static_cast<double>(pixel[0]), // B
                static_cast<double>(pixel[1]), // G
                static_cast<double>(pixel[2])  // R
            });
        }
    }
    return points;
}

// Convertit les points K-means en image OpenCV
Mat pointsToImage(const Matrix& centroids, const vector<int>& assignments, 
                  int rows, int cols) {
    Mat result(rows, cols, CV_8UC3);
    
    int pointIndex = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int cluster = assignments[pointIndex++];
            const auto& centroid = centroids[cluster];
            
            result.at<Vec3b>(i, j) = Vec3b(
                static_cast<uchar>(round(centroid[0])), // B
                static_cast<uchar>(round(centroid[1])), // G
                static_cast<uchar>(round(centroid[2]))  // R
            );
        }
    }
    return result;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <input_image> <output_image> <K> [max_iterations]\n";
        cerr << "  K: number of colors (clusters)\n";
        cerr << "  max_iterations: maximum iterations (default: 20)\n";
        return 1;
    }
    
    string inputPath = argv[1];
    string outputPath = argv[2];
    int k = stoi(argv[3]);
    int maxIterations = (argc >= 5) ? stoi(argv[4]) : 20;
    
    // Charger l'image
    Mat image = imread(inputPath, IMREAD_COLOR);
    if (image.empty()) {
        cerr << "Erreur: Impossible de charger l'image " << inputPath << "\n";
        return 1;
    }
    
    cout << "Image chargée: " << image.cols << "x" << image.rows 
         << " (" << (image.rows * image.cols) << " pixels)\n";
    cout << "Compression avec K=" << k << " couleurs...\n";
    
    // Convertir l'image en points 3D (RGB)
    Matrix points = imageToPoints(image);
    
    // Exécuter K-means
    auto result = kmeans(points, k, maxIterations);
    
    cout << "K-means terminé après " << result.iterations << " iterations\n";
    cout << "Coût final: " << result.finalCost << "\n";
    
    // Reconstruire l'image avec les couleurs quantifiées
    Mat compressedImage = pointsToImage(result.centroids, result.assignments,
                                       image.rows, image.cols);
    
    // Sauvegarder l'image compressée
    if (!imwrite(outputPath, compressedImage)) {
        cerr << "Erreur: Impossible de sauvegarder l'image " << outputPath << "\n";
        return 1;
    }
    
    cout << "Image compressée sauvegardée: " << outputPath << "\n";
    
    // Afficher les couleurs finales
    cout << "\nCouleurs finales (BGR):\n";
    for (size_t i = 0; i < result.centroids.size(); ++i) {
        const auto& c = result.centroids[i];
        cout << "  Couleur " << i << ": (" 
             << static_cast<int>(c[0]) << ", "
             << static_cast<int>(c[1]) << ", "
             << static_cast<int>(c[2]) << ")\n";
    }
    
    return 0;
}