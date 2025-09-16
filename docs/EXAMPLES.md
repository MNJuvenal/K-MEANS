# 🚀 Quick Start Examples

Collection d'exemples rapides pour tester le K-means C++ Pipeline.

## 📊 Données de Test Incluses

Le projet inclut plusieurs datasets de démonstration :

- `test_image.jpg` : Image couleur pour compression (31 KB)
- `test_simple.png` : Image simple pour tests rapides (301 bytes)
- Génération automatique : 735 points répartis en 8 clusters + bruit

## ⚡ Tests Rapides (5 minutes)

### 1. Compilation Express
```bash
cd kmeans-cpp-pipeline
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 2. Test Visualisation 2D 
```bash
# Animation 8 clusters (15 itérations)
./kmeans_visual_2d

# Configuration personnalisée
./kmeans_visual_2d 5 10  # 5 clusters, 10 itérations
```

**Contrôles** : `ESC` pour quitter, `SPACE` pour pause

### 3. Test Compression Image
```bash
# Compression standard (16 couleurs)
./kmeans_image ../test_image.jpg compressed.jpg

# Style artistique (8 couleurs)
./kmeans_image ../test_image.jpg art.jpg 8 50
```

### 4. Comparaison Visuelle
```bash
./view_side_by_side ../test_image.jpg compressed.jpg "Original vs Compressed"
```

### 5. Version Refactorisée
```bash
# Clustering basique optimisé
./kmeans_simple

# Compression images optimisée  
./kmeans_image_refactored ../test_image.jpg optimized.jpg 12
```

## 📈 Exemples Avancés

### Dataset CSV Personnalisé
```bash
# Créer fichier data.csv
echo "1.0,1.0
1.1,1.2
5.0,5.0
5.2,4.8" > data.csv

# Visualiser
./kmeans_visual_2d 2 20 data.csv
```

### Pipeline Complet
```bash
./kmeans_pipeline ../test_image.jpg results/ 16 30
# Génère: results/compressed.jpg, results/clusters.txt, results/stats.json
```

### Batch Processing
```bash
# Traitement multiple images
for img in *.jpg; do
    ./kmeans_image "$img" "compressed_$img" 8 25
done
```

## 🎨 Cas d'Usage Créatifs

### Art Génératif
```bash
# Palette minimaliste (4 couleurs)
./kmeans_image photo.jpg poster.jpg 4 100

# Style rétro (8 couleurs)  
./kmeans_image photo.jpg retro.jpg 8 50
```

### Analyse de Données
```bash
# Clustering données scientifiques
./kmeans_visual_2d 3 50 experiment_data.csv

# Observer convergence lente vs rapide
./kmeans_visual_2d 10 5   # Rapide, peu précis
./kmeans_visual_2d 10 50  # Lent, très précis
```

### Performance Testing
```bash
# Benchmark temps exécution
time ./kmeans_simple

# Test mémoire avec valgrind
valgrind --leak-check=full ./kmeans_simple
```

## 🐛 Dépannage Rapide

### OpenCV non trouvé
```bash
# Ubuntu/Debian
sudo apt install libopencv-dev

# Vérifier installation
pkg-config --exists opencv4 && echo "OpenCV OK" || echo "OpenCV manquant"
```

### Erreur compilation C++17
```bash
# Force C++17
cmake .. -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON
```

### Problème affichage graphique
```bash
# Test environment graphique
echo $DISPLAY

# Fallback : utiliser versions sans GUI
./kmeans_simple     # Pas de visualisation
```

## 📊 Résultats Attendus

### Visualisation 2D
- **Animation fluide** à 1.8s par itération
- **8 clusters colorés** avec trajectoires centroïdes
- **735 points** générés automatiquement
- **Convergence** en ~10-15 itérations

### Compression Image
- **Réduction taille** : 50-80% selon nombre couleurs
- **Qualité visuelle** : Excellente avec 16+ couleurs
- **Temps traitement** : ~2-3s pour image 640×480

### Performance
- **Clustering simple** : <100ms pour 1000 points
- **Image processing** : ~2-3s pour 300K pixels
- **Mémoire** : <50MB même pour gros datasets

## 🎯 Benchmarks de Référence

| Dataset | Points | Clusters | Temps | Mémoire |
|---------|--------|----------|-------|---------|
| Généré 2D | 735 | 8 | ~15 iter × 1.8s | <10MB |
| Image 640×480 | 307,200 | 16 | ~2.3s | ~25MB |
| CSV 1000 pts | 1,000 | 10 | ~0.1s | <5MB |
| Grande image | 1M pixels | 32 | ~8s | ~80MB |

## 🏆 Défis Utilisateurs

### Défi 1: Clustering Parfait
Trouvez les paramètres pour obtenir une convergence en moins de 5 itérations sur le dataset par défaut.

### Défi 2: Art Minimal  
Créez une version artistique d'une photo avec seulement 3 couleurs.

### Défi 3: Performance
Optimisez le code pour traiter une image 4K en moins de 10 secondes.

---

💡 **Astuce** : Commencez par `./kmeans_visual_2d` pour voir l'algorithme en action !
