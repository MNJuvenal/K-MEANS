# 🚀 Guide de Contribution

Merci de votre intérêt pour contribuer au projet K-means C++ Pipeline ! Ce guide vous aidera à démarrer.

## 🎯 Types de Contributions

### 🐛 Rapports de Bugs
- Utilisez les **GitHub Issues** avec le tag `bug`
- Incluez votre OS, version de compilateur, et version OpenCV
- Fournissez un exemple reproductible minimal

### ✨ Nouvelles Fonctionnalités
- Ouvrez d'abord une **Issue** pour discuter de l'idée
- Respectez l'architecture modulaire existante
- Ajoutez des tests et documentation

### 📚 Documentation
- Améliorations du README
- Commentaires de code plus clairs
- Exemples d'utilisation

## 🛠️ Setup Développement

### 1. Fork & Clone
```bash
git clone https://github.com/VOTRE-USERNAME/kmeans-cpp-pipeline.git
cd kmeans-cpp-pipeline
git remote add upstream https://github.com/ORIGINAL-OWNER/kmeans-cpp-pipeline.git
```

### 2. Environnement de Développement
```bash
# Ubuntu/Debian
sudo apt install cmake g++ libopencv-dev clang-format

# Build debug
mkdir build-dev && cd build-dev
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-Wall -Wextra"
make -j$(nproc)
```

### 3. Tests
```bash
# Tester tous les outils
./kmeans_simple
./kmeans_visual_2d 3 5
./kmeans_image ../test_image.jpg test_output.jpg 8
```

## 📋 Standards de Code

### Style C++
- **C++17** standard
- **CamelCase** pour les classes : `KMeansResult`
- **snake_case** pour les fonctions : `compute_centroids()`
- **UPPER_CASE** pour les constantes : `MAX_ITERATIONS`

### Formatage
```bash
# Auto-format avec clang-format
find src -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
```

### Exemple de fonction bien documentée
```cpp
/**
 * Calcule les nouveaux centroïdes pour chaque cluster
 * 
 * @param points Ensemble de tous les points de données
 * @param assignments Assignation cluster pour chaque point
 * @param K Nombre de clusters
 * @return Nouveaux centroïdes calculés
 * 
 * COMPLEXITÉ: O(n) où n = nombre de points
 * 
 * ALGORITHME:
 * 1. Pour chaque cluster k:
 *    a. Collecter tous les points assignés à k
 *    b. Calculer la moyenne de ces points
 *    c. Cette moyenne devient le nouveau centroïde
 */
std::vector<Point> compute_centroids(
    const std::vector<Point>& points,
    const std::vector<int>& assignments, 
    int K
);
```

## 🔄 Workflow Git

### 1. Créer une branche
```bash
git checkout -b feature/nom-descriptif
# ou
git checkout -b bugfix/probleme-specifique
# ou  
git checkout -b docs/amelioration-readme
```

### 2. Développement
```bash
# Faire vos modifications
git add .
git commit -m "feat: ajouter visualisation 3D interactive

- Nouvelle classe KMeans3DVisualizer
- Support rotation/zoom souris  
- Export animations GIF
- Tests unitaires inclus

Closes #42"
```

### 3. Synchronisation
```bash
# Récupérer les derniers changements
git fetch upstream
git rebase upstream/main

# Push votre branche
git push origin feature/nom-descriptif
```

### 4. Pull Request
- **Titre clair** : "feat: ajouter support K-means 3D"
- **Description détaillée** : Que fait le changement ? Pourquoi ?
- **Screenshots/GIFs** pour les changements visuels
- **Tests** : Tous les tests passent ?
- **Breaking changes** : Y en a-t-il ?

## 📋 Checklist PR

- [ ] Code compilé sans warnings
- [ ] Tests ajoutés/mis à jour
- [ ] Documentation mise à jour
- [ ] Respecte le style de code
- [ ] Pas de conflits avec main
- [ ] Description PR complète

## 🎨 Idées de Contributions

### Faciles (good first issue)
- [ ] Ajouter support fichiers BMP
- [ ] Améliorer messages d'erreur
- [ ] Ajouter plus d'exemples dans README
- [ ] Corriger typos dans commentaires

### Moyennes
- [ ] Mode batch pour traiter plusieurs images
- [ ] Sauvegarde/chargement états de clustering
- [ ] Interface graphique Qt/GTK
- [ ] Support points 3D dans visualiseur

### Avancées
- [ ] K-means++ pour initialisation intelligente
- [ ] Clustering hiérarchique
- [ ] Support GPU avec CUDA
- [ ] API Python avec pybind11

## 🏷️ Convention Commits

Utilisez la convention [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

**Types** :
- `feat`: Nouvelle fonctionnalité
- `fix`: Correction de bug  
- `docs`: Changements documentation
- `style`: Formatage (pas de changement logique)
- `refactor`: Refactoring code
- `test`: Ajouter/modifier tests
- `chore`: Maintenance (build, CI, etc.)

**Exemples** :
```
feat(visual): ajouter pause/résumé animation
fix(image): corriger fuite mémoire lors traitement
docs(readme): améliorer section installation  
refactor(core): extraire logique distance dans utils
```

## 🤝 Code de Conduite

- Soyez respectueux et bienveillant
- Accueillez les nouveaux contributeurs
- Focalisez sur le code, pas sur la personne
- Aidez les autres à apprendre

## 🆘 Besoin d'Aide ?

- **Issues** : Pour questions techniques
- **Discussions** : Pour idées générales
- **Email** : [maintainer@example.com] pour sujets privés

---

Merci de contribuer ! 🎉 Chaque contribution, petite ou grande, est appréciée.
