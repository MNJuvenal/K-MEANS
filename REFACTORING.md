# Architecture Refactorisée K-means

## 🏗️ Structure du Projet

### **Bibliothèque Commune : `kmeans_lib.hpp`**
Bibliothèque header-only contenant :
- **Types** : `Vector`, `Matrix`, `KMeansResult`
- **Algorithme complet** : `kmeans()` 
- **Fonctions utilitaires** : `distance()`, `findClosestCentroids()`, etc.
- **Namespace** : `KMeansLib` pour éviter les conflits

### **Avantages de cette Approche**
✅ **DRY (Don't Repeat Yourself)** : Code K-means écrit une seule fois
✅ **Maintenance simplifiée** : Bug fixé = fixé partout
✅ **Performance** : Header-only = inlining automatique
✅ **Modularité** : Chaque outil garde sa spécialité
✅ **Extensibilité** : Facile d'ajouter de nouveaux outils

## 🛠️ Outils Disponibles

### **Versions Originales (legacy)**
- `kmeans` : Version basique avec code dupliqué
- `kmeans_image` : Compression d'images (code dupliqué)
- `kmeans_visual_2d` : Visualiseur 2D (code dupliqué)
- `view_side_by_side` : Comparateur d'images (pas K-means)
- `kmeans_pipeline` : Pipeline complet (code dupliqué)

### **Versions Refactorisées (recommandées)**
- `kmeans_simple` : Version basique utilisant `kmeans_lib.hpp`
- `kmeans_image_refactored` : Compression d'images avec bibliothèque commune

## 🚀 Utilisation

### Compilation
```bash
cd build
cmake .. -DBUILD_REFACTORED=ON
make -j$(nproc)
```

### Test Simple
```bash
./kmeans_simple
```

### Compression d'Image
```bash
./kmeans_image_refactored input.jpg output.jpg 16 50
# 16 couleurs, 50 itérations max
```

## 📈 Prochaines Étapes

Pour compléter la refactorisation :
1. **`kmeans_visual_2d_refactored.cpp`** : Visualiseur utilisant la lib commune
2. **`kmeans_pipeline_refactored.cpp`** : Pipeline utilisant la lib commune
3. **Tests unitaires** : Validation de la bibliothèque
4. **Benchmarks** : Comparaison performances original vs refactorisé

## 🎯 Code Example

```cpp
#include "kmeans_lib.hpp"
using namespace KMeansLib;

Matrix points = {{1,2}, {3,4}, {5,6}};
auto result = kmeans(points, 2, 100);
// result.centroids, result.assignments, result.finalCost
```

Cette architecture respecte les bonnes pratiques :
- Séparation des responsabilités
- Réutilisabilité du code
- Facilité de maintenance
- Performance optimale
