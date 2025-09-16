// ============================================================================
// PROJET: Visualiseur K-means 2D Interactif
// AUTEUR: Étudiant en Intelligence Artificielle
// DATE: Septembre 2025
// 
// DESCRIPTION: 
// Ce programme implémente et visualise l'algorithme K-means sur des données 2D.
// Il génère des clusters de points colorés et montre comment les centroïdes
// se déplacent à chaque itération pour converger vers les centres des groupes.
//
// FONCTIONNALITÉS:
// - Génération automatique de 8 clusters colorés avec du bruit
// - Animation en temps réel du processus K-means
// - Affichage des trajectoires des centroïdes
// - Interface utilisateur avec contrôles (ESC, SPACE)
// - Support des fichiers CSV pour données personnalisées
// ============================================================================

#include <opencv2/opencv.hpp>  // Bibliothèque OpenCV pour l'affichage graphique
#include <bits/stdc++.h>       // Toutes les bibliothèques standard C++ (pratique mais pas optimal)
using namespace std;           // Pour éviter std:: partout
using namespace cv;            // Pour éviter cv:: partout

// ============================================================================
// DÉFINITION DES TYPES PERSONNALISÉS
// ============================================================================

// Un vecteur 2D représente un point (x, y) dans l'espace
using Vector2D = vector<double>;

// Une matrice de données est un ensemble de points 2D
using MatD = vector<Vector2D>;

// ============================================================================
// FONCTIONS UTILITAIRES POUR L'ALGORITHME K-MEANS
// ============================================================================

/**
 * Calcule la distance euclidienne au carré entre deux points 2D
 * 
 * POURQUOI AU CARRÉ? 
 * - Plus rapide (pas besoin de sqrt())
 * - Préserve l'ordre des distances (important pour trouver le minimum)
 * - Mathématiquement équivalent pour la comparaison
 * 
 * @param a Premier point (x1, y1)
 * @param b Deuxième point (x2, y2)
 * @return Distance au carré: (x2-x1)² + (y2-y1)²
 */
static inline double l2sq(const Vector2D& a, const Vector2D& b){
    double s=0.0; // Somme des différences au carré
    
    // Parcourt les coordonnées (x, y)
    for(size_t i=0;i<a.size();++i){ 
        double d=a[i]-b[i];  // Différence sur cette dimension
        s+=d*d;              // Ajoute le carré à la somme
    } 
    return s; // Retourne la distance euclidienne au carré
}

/**
 * ÉTAPE 1 de K-means: Assigner chaque point au centroïde le plus proche
 * 
 * PRINCIPE:
 * Pour chaque point, on calcule sa distance à tous les centroïdes
 * et on l'assigne au cluster du centroïde le plus proche.
 * 
 * @param X Matrice des points de données (chaque ligne = un point)
 * @param C Matrice des centroïdes actuels (chaque ligne = un centroïde)
 * @return Vecteur d'assignations (idx[i] = cluster du point i)
 */
vector<int> find_closest_centroids(const MatD& X, const MatD& C){
    vector<int> idx(X.size()); // Vecteur pour stocker les assignations
    
    // Pour chaque point dans nos données
    for(size_t i=0;i<X.size();++i){
        double best = numeric_limits<double>::infinity(); // Distance minimale trouvée
        int bj=0; // Index du meilleur centroïde
        
        // Teste la distance à chaque centroïde
        for(size_t j=0;j<C.size();++j){ 
            double d=l2sq(X[i],C[j]); // Distance au carré vers centroïde j
            if(d<best){               // Si c'est la meilleure distance
                best=d;               // Mettre à jour la meilleure distance
                bj=(int)j;            // Mémoriser l'index du centroïde
            }
        }
        idx[i]=bj; // Assigner le point i au centroïde bj
    }
    return idx; // Retourner toutes les assignations
}

/**
 * ÉTAPE 2 de K-means: Recalculer les centroïdes basés sur les assignations
 * 
 * PRINCIPE:
 * Chaque centroïde devient le centre de masse (moyenne) des points
 * qui lui sont assignés. C'est ce qui fait "bouger" les centroïdes
 * vers le centre de leur cluster.
 * 
 * @param X Matrice des points de données
 * @param idx Assignations actuelles (résultat de find_closest_centroids)
 * @param K Nombre de clusters souhaités
 * @return Nouveaux centroïdes calculés
 */
MatD compute_centroids(const MatD& X, const vector<int>& idx, int K){
    int n = (int)X[0].size(); // Nombre de dimensions (2 pour 2D)
    
    // Initialiser les nouveaux centroïdes à zéro
    MatD C(K, Vector2D(n,0.0)); 
    vector<int> cnt(K,0); // Compteur de points par cluster
    
    // PHASE 1: Accumuler les coordonnées des points par cluster
    for(size_t i=0;i<X.size();++i){
        int k=idx[i];  // Cluster assigné au point i
        cnt[k]++;      // Incrémenter le compteur pour ce cluster
        
        // Ajouter les coordonnées du point au centroïde
        for(int t=0;t<n;++t) {
            C[k][t]+=X[i][t]; // Accumulation des coordonnées
        }
    }
    
    // PHASE 2: Diviser par le nombre de points pour obtenir la moyenne
    for(int k=0;k<K;++k) {
        if(cnt[k]) { // Si le cluster a des points assignés
            for(int t=0;t<n;++t) {
                C[k][t]/=cnt[k]; // Moyenne = somme / nombre de points
            }
        }
        // ATTENTION: Si cnt[k] == 0, le centroïde reste à (0,0)
        // Dans un vrai projet, il faudrait gérer ce cas edge
    }
    return C; // Retourner les nouveaux centroïdes
}

/**
 * Initialise les centroïdes de manière aléatoire
 * 
 * STRATÉGIE D'INITIALISATION:
 * Au lieu de placer les centroïdes au hasard, on choisit K points
 * aléatoires parmi nos données. Cette méthode donne généralement
 * de meilleurs résultats que l'initialisation complètement aléatoire.
 * 
 * @param X Données d'entrée
 * @param K Nombre de centroïdes à initialiser
 * @param seed Graine pour la génération aléatoire (0 = aléatoire)
 * @return Centroïdes initialisés
 */
MatD init_centroids(const MatD& X, int K, uint64_t seed=0){
    // Si pas de graine fournie, en générer une aléatoire
    if(seed == 0) seed = std::random_device{}();
    
    // Créer un vecteur d'indices [0, 1, 2, ..., n-1]
    vector<int> perm(X.size()); 
    iota(perm.begin(),perm.end(),0); // Remplit avec 0,1,2,...
    
    // Mélanger les indices de manière aléatoire
    mt19937_64 rng(seed);           // Générateur de nombres aléatoires
    shuffle(perm.begin(),perm.end(),rng); // Mélange aléatoire
    
    // Prendre les K premiers indices mélangés comme centroïdes initiaux
    MatD C(K); 
    for(int k=0;k<K;++k) {
        C[k]=X[perm[k]]; // Copier le point à l'index aléatoire
    }
    return C;
}

// ============================================================================
// STRUCTURES ET FONCTIONS POUR LA VISUALISATION
// ============================================================================

/**
 * Structure pour stocker les limites d'affichage des données 2D
 * Permet de mapper les coordonnées des données vers l'écran
 */
struct MinMax{ 
    double minx, miny, maxx, maxy; 
};

/**
 * Calcule les limites (bounding box) de tous les points
 * 
 * UTILITÉ:
 * Pour afficher les données à l'écran, on doit connaître l'étendue
 * des coordonnées pour faire une projection correcte.
 * 
 * @param X Ensemble des points 2D
 * @return Limites min/max en x et y
 */
MinMax bounds2D(const MatD& X){
    // Initialiser avec les valeurs extrêmes
    MinMax mm{
        numeric_limits<double>::infinity(),    // minx = +∞
        numeric_limits<double>::infinity(),    // miny = +∞
        -numeric_limits<double>::infinity(),   // maxx = -∞
        -numeric_limits<double>::infinity()    // maxy = -∞
    };
    
    // Parcourir tous les points pour trouver les vraies limites
    for(auto& p: X){ 
        mm.minx=min(mm.minx,p[0]); // Minimum en x
        mm.maxx=max(mm.maxx,p[0]); // Maximum en x
        mm.miny=min(mm.miny,p[1]); // Minimum en y
        mm.maxy=max(mm.maxy,p[1]); // Maximum en y
    }
    
    // Éviter les divisions par zéro si tous les points sont alignés
    if(mm.maxx-mm.minx==0) {mm.maxx=mm.minx+1;} // Largeur minimale
    if(mm.maxy-mm.miny==0) {mm.maxy=mm.miny+1;} // Hauteur minimale
    
    return mm;
}

/**
 * Convertit un point 2D en coordonnées d'écran pour l'affichage
 * 
 * TRANSFORMATION MATHÉMATIQUE:
 * Données: (minx, maxx) × (miny, maxy)  →  Écran: (0, W) × (0, H)
 * 
 * Formules de transformation linéaire:
 * x_écran = pad + (x_données - minx) * échelle_x
 * y_écran = H - (pad + (y_données - miny) * échelle_y)  [y inversé!]
 * 
 * @param p Point en coordonnées de données
 * @param mm Limites des données  
 * @param W Largeur de la fenêtre
 * @param H Hauteur de la fenêtre
 * @param pad Marge autour du graphique
 * @return Point en coordonnées d'écran
 */
Point2i toCanvas(const Vector2D& p, const MinMax& mm, int W, int H, int pad=40){
    // Calcul des facteurs d'échelle
    double sx=(W-2*pad)/(mm.maxx-mm.minx);  // Échelle horizontale
    double sy=(H-2*pad)/(mm.maxy-mm.miny);  // Échelle verticale
    
    // Transformation des coordonnées
    double x = pad + (p[0]-mm.minx)*sx;           // Position x à l'écran
    double y = H - (pad + (p[1]-mm.miny)*sy);     // Position y à l'écran (inversée)
    
    // POURQUOI Y INVERSÉ?
    // Dans les maths: y augmente vers le haut
    // À l'écran: y augmente vers le bas
    // Donc on fait H - y pour corriger
    
    return Point2i((int)round(x),(int)round(y));  // Convertir en pixels entiers
}

/**
 * Palette de 30 couleurs distinctes pour visualiser les clusters
 * 
 * DESIGN DE LA PALETTE:
 * - Couleurs vives et contrastées pour une bonne lisibilité
 * - Éviter les couleurs trop similaires (ex: pas deux verts proches)
 * - Format BGR d'OpenCV (Bleu, Vert, Rouge) au lieu de RGB
 * - Assez de couleurs pour gérer jusqu'à 30 clusters différents
 * 
 * @param k Index du cluster (0, 1, 2, ...)
 * @return Couleur BGR pour ce cluster
 */
Scalar colorFromIdx(int k){
    static vector<Scalar> palette = {
        Scalar(230,25,75),    // Rouge vif
        Scalar(60,180,75),    // Vert émeraude  
        Scalar(255,225,25),   // Jaune doré
        Scalar(0,130,200),    // Bleu océan
        Scalar(245,130,48),   // Orange
        Scalar(145,30,180),   // Violet
        Scalar(70,240,240),   // Cyan
        Scalar(240,50,230),   // Magenta
        Scalar(210,245,60),   // Vert lime
        Scalar(250,190,190),  // Rose pâle
        Scalar(0,128,128),    // Sarcelle
        Scalar(230,190,255),  // Lavande
        Scalar(170,110,40),   // Marron
        Scalar(255,250,200),  // Beige
        Scalar(128,0,0),      // Bordeaux
        Scalar(170,255,195),  // Menthe
        Scalar(128,128,0),    // Olive
        Scalar(255,215,180),  // Pêche
        Scalar(0,0,128),      // Bleu marine
        Scalar(128,128,128),  // Gris
        Scalar(255,20,147),   // Rose profond
        Scalar(50,205,50),    // Vert lime
        Scalar(255,140,0),    // Orange sombre
        Scalar(138,43,226),   // Bleu violet
        Scalar(0,255,255),    // Aqua
        Scalar(255,105,180),  // Rose bonbon
        Scalar(34,139,34),    // Vert forêt
        Scalar(255,69,0),     // Rouge orangé
        Scalar(72,61,139),    // Ardoise sombre
        Scalar(255,192,203)   // Rose clair
    };
    
    // Utilise le modulo pour boucler si on a plus de 30 clusters
    // Exemple: cluster 32 aura la même couleur que cluster 2
    return palette[k % palette.size()];
}

/**
 * Dessine une croix ('+') pour marquer la position d'un centroïde
 * 
 * POURQUOI UNE CROIX?
 * - Facile à voir même avec beaucoup de points autour
 * - Indique clairement le centre exact du cluster
 * - Se distingue bien des cercles utilisés pour les points
 * 
 * @param img Image sur laquelle dessiner
 * @param c Centre de la croix
 * @param col Couleur de la croix
 * @param size Demi-longueur des branches
 * @param thickness Épaisseur des lignes
 */
void drawCross(Mat& img, Point c, Scalar col, int size=8, int thickness=2){
    // Ligne horizontale: de (x-size, y) à (x+size, y)
    line(img, Point(c.x-size,c.y), Point(c.x+size,c.y), col, thickness, LINE_AA);
    
    // Ligne verticale: de (x, y-size) à (x, y+size)
    line(img, Point(c.x, c.y-size), Point(c.x, c.y+size), col, thickness, LINE_AA);
    
    // LINE_AA = antialiasing pour des lignes plus lisses
}

// ============================================================================
// FONCTION PRINCIPALE - PROGRAMME INTERACTIF K-MEANS 2D
// ============================================================================

/**
 * Programme principal qui orchestre toute la visualisation K-means
 * 
 * ARCHITECTURE DU PROGRAMME:
 * 1. Analyse des arguments de ligne de commande
 * 2. Génération ou chargement des données
 * 3. Initialisation des centroïdes
 * 4. Boucle principale d'animation K-means
 * 5. Affichage du résultat final
 */
int main(int argc, char** argv){
    // ========================================================================
    // ANALYSE DES PARAMÈTRES D'ENTRÉE
    // ========================================================================
    
    // Dataset 2D avec beaucoup de clusters ou CSV (x,y) optionnel
    // Usage: kmeans_visual_2d [K=8] [iters=15] [csv_path]
    
    int K = (argc>=2? stoi(argv[1]) : 8);      // Nombre de clusters (défaut: 8)
    int iters = (argc>=3? stoi(argv[2]) : 15); // Nombre max d'itérations (défaut: 15)
    string csv = (argc>=4? argv[3] : "");      // Fichier CSV optionnel

    // ========================================================================
    // GÉNÉRATION OU CHARGEMENT DES DONNÉES
    // ========================================================================
    
    MatD X; // Matrice qui contiendra tous nos points 2D
    
    if(csv.empty()){
        // ====================================================================
        // MODE DÉMONSTRATION: Génération de 8 clusters + bruit
        // ====================================================================
        
        cout << "Mode démonstration: génération de " << K << " clusters synthétiques\n";
        
        mt19937 rng(42); // Générateur pseudo-aléatoire avec graine fixe
                        // Graine fixe = résultats reproductibles pour le debug
        
        // Cluster 1: Rouge (en haut à gauche)
        // Distribution normale = points concentrés autour d'un centre avec dispersion
        normal_distribution<double> c1x(2.0,0.8), c1y(6.0,0.6);
        for(int i=0;i<120;++i) X.push_back({c1x(rng), c1y(rng)});
        
        // Cluster 2: Vert (en haut à droite)  
        normal_distribution<double> c2x(8.0,0.7), c2y(6.5,0.5);
        for(int i=0;i<100;++i) X.push_back({c2x(rng), c2y(rng)});
        
        // Cluster 3: Bleu (milieu gauche)
        normal_distribution<double> c3x(1.5,0.6), c3y(3.5,0.8);
        for(int i=0;i<90;++i) X.push_back({c3x(rng), c3y(rng)});
        
        // Cluster 4: Jaune (centre)
        normal_distribution<double> c4x(5.0,0.9), c4y(4.0,0.7);
        for(int i=0;i<110;++i) X.push_back({c4x(rng), c4y(rng)});
        
        // Cluster 5: Magenta (bas droite)
        normal_distribution<double> c5x(8.5,0.5), c5y(1.5,0.6);
        for(int i=0;i<80;++i) X.push_back({c5x(rng), c5y(rng)});
        
        // Cluster 6: Cyan (bas gauche)
        normal_distribution<double> c6x(2.5,0.7), c6y(1.0,0.5);
        for(int i=0;i<85;++i) X.push_back({c6x(rng), c6y(rng)});
        
        // Cluster 7: Orange (milieu droite)
        normal_distribution<double> c7x(9.0,0.6), c7y(4.5,0.8);
        for(int i=0;i<95;++i) X.push_back({c7x(rng), c7y(rng)});
        
        // Cluster 8: Violet (centre haut)
        normal_distribution<double> c8x(5.5,0.5), c8y(7.0,0.4);
        for(int i=0;i<75;++i) X.push_back({c8x(rng), c8y(rng)});
        
        // Points dispersés (bruit) - rend le clustering plus réaliste
        // Distribution uniforme = points éparpillés partout
        uniform_real_distribution<double> noise_x(0.5, 10.0), noise_y(0.5, 8.0);
        for(int i=0;i<45;++i) X.push_back({noise_x(rng), noise_y(rng)});
        
        cout << "Généré " << X.size() << " points répartis en 8 clusters + bruit\n";
        
    } else {
        // ====================================================================
        // MODE FICHIER: Chargement depuis un CSV
        // ====================================================================
        
        cout << "Chargement des données depuis: " << csv << "\n";
        
        ifstream f(csv);
        if(!f){ 
            cerr<<"❌ Erreur: Impossible de lire " << csv << "\n"; 
            return 1; 
        }
        
        string line;
        int lineNum = 0;
        
        // Lecture ligne par ligne du fichier CSV
        while(getline(f,line)){
            lineNum++;
            if(line.empty()) continue; // Ignorer les lignes vides
            
            stringstream ss(line);
            string a,b; // Coordonnées x et y sous forme de chaînes
            
            // Séparer par virgules: "x,y"
            if(!getline(ss,a,',')) continue; // Lire jusqu'à la première virgule
            if(!getline(ss,b,',')) continue; // Lire jusqu'à la deuxième virgule
            
            try {
                // Convertir les chaînes en nombres
                double x = stod(a); // string to double
                double y = stod(b);
                X.push_back({x, y});
            } catch(const exception& e) {
                cerr << "⚠️  Ligne " << lineNum << " ignorée (format invalide): " << line << "\n";
            }
        }
        
        if(X.empty()){ 
            cerr << "❌ Erreur: CSV vide ou aucune donnée valide trouvée.\n"; 
            return 1; 
        }
        
        cout << "✅ Chargé " << X.size() << " points depuis " << csv << "\n";
    }

    auto C = init_centroids(X, K);
    vector<int> idx(X.size(),0);

    const int W=900, H=700;
    MinMax mm = bounds2D(X);

    Mat canvas(H,W,CV_8UC3, Scalar(30,30,30));
    vector<MatD> history; history.push_back(C);

    for(int it=0; it<iters; ++it){
        idx = find_closest_centroids(X, C);
        Mat frame(H,W,CV_8UC3, Scalar(30,30,30));

        // points
        for(size_t i=0;i<X.size();++i){
            Point2i p = toCanvas(X[i], mm, W,H);
            circle(frame, p, 3, colorFromIdx(idx[i]), FILLED, LINE_AA);
        }

        // liens centroides (trajectoire)
        for(size_t h=1; h<history.size(); ++h){
            for(int k=0;k<K;++k){
                Point2i p1 = toCanvas(history[h-1][k], mm,W,H);
                Point2i p2 = toCanvas(history[h][k],   mm,W,H);
                line(frame, p1, p2, Scalar(255,255,255), 1, LINE_AA);
            }
        }

        // centroides courants
        for(int k=0;k<K;++k){
            Point2i pc = toCanvas(C[k], mm,W,H);
            drawCross(frame, pc, Scalar(255,255,255), 8, 2);
        }

        // UI avec plus d'informations
        putText(frame, "Iteration "+to_string(it+1)+"/"+to_string(iters),
                Point(20,40), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255,255,255), 2, LINE_AA);
        putText(frame, "Clusters K="+to_string(K)+" | Points: "+to_string(X.size()), 
                Point(20,80), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(200,200,200), 2, LINE_AA);
        putText(frame, "ESC pour quitter | SPACE pour pause", Point(20,H-40), 
                FONT_HERSHEY_SIMPLEX, 0.6, Scalar(200,200,200), 1, LINE_AA);
        imshow("K-means 2D Visualizer - Multi-Clusters", frame);
        int key = waitKey(1800); // 1.8 secondes
        if(key==27) break; // ESC pour quitter
        if(key==32) waitKey(0); // SPACE pour pause

        // update centroids
        auto newC = compute_centroids(X, idx, K);
        history.push_back(newC);
        C.swap(newC);
    }

    // Affiche la dernière frame jusqu'à touche
    putText(canvas, "Termine - Appuyez sur une touche", Point(20,H-20),
            FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255,255,255), 2, LINE_AA);
    imshow("K-means 2D Visualizer", canvas);
    waitKey(0);
    return 0;
}
