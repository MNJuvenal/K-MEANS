#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // Créer une image de test 200x200 avec différentes couleurs
    Mat image(200, 200, CV_8UC3, Scalar(255, 255, 255)); // Blanc
    
    // Zone rouge
    rectangle(image, Rect(0, 0, 100, 100), Scalar(0, 0, 255), FILLED);
    
    // Zone verte  
    rectangle(image, Rect(100, 0, 100, 100), Scalar(0, 255, 0), FILLED);
    
    // Zone bleue
    rectangle(image, Rect(0, 100, 100, 100), Scalar(255, 0, 0), FILLED);
    
    // Zone jaune
    rectangle(image, Rect(100, 100, 100, 100), Scalar(0, 255, 255), FILLED);
    
    // Sauvegarder
    if (imwrite("test_image.jpg", image)) {
        cout << "Image de test créée: test_image.jpg (200x200, 4 couleurs)" << endl;
        cout << "Dimensions: " << image.cols << "x" << image.rows << endl;
    } else {
        cerr << "Erreur lors de la création de l'image" << endl;
        return 1;
    }
    
    return 0;
}
