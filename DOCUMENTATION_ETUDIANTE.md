# 📚 Documentation Étudiante - Visualiseur K-means 2D

## 🎯 Objectif du Projet
J'ai créé un visualiseur interactif de l'algorithme K-means en 2D qui montre comment les centroïdes se déplacent pour converger vers les centres des clusters de données.

## 🏗️ Architecture du Code

### 📁 Structure Générale
```cpp
// En-tête avec description complète du projet
// Définition des types personnalisés (Vector2D, MatD)
// Fonctions utilitaires pour K-means
// Fonctions de visualisation
// Programme principal avec interface utilisateur
```

### 🧮 Algorithme K-means Implémenté

1. **`l2sq()`** - Calcul de distance euclidienne au carré
   - ✅ Plus rapide (pas de sqrt())
   - ✅ Préserve l'ordre des distances

2. **`find_closest_centroids()`** - Étape 1 de K-means
   - ✅ Assigne chaque point au centroïde le plus proche
   - ✅ Complexité: O(n × k) par itération

3. **`compute_centroids()`** - Étape 2 de K-means  
   - ✅ Recalcule les centroïdes comme moyenne des points assignés
   - ✅ Gère les clusters vides (cas edge)

4. **`init_centroids()`** - Initialisation intelligente
   - ✅ Choisit K points aléatoires parmi les données
   - ✅ Meilleur que l'initialisation complètement aléatoire

### 🎨 Système de Visualisation

1. **Palette de 30 couleurs** - Distingue jusqu'à 30 clusters
2. **Transformation de coordonnées** - Mappe données → écran
3. **Animation fluide** - 800ms entre frames
4. **Interface utilisateur** - ESC (quitter), SPACE (pause)

### 📊 Génération de Données

- **Mode démonstration**: 8 clusters pré-définis + bruit
- **Mode fichier**: Chargement CSV (x,y)
- **800+ points** répartis intelligemment
- **Distributions normales** pour des clusters réalistes

## 🎓 Concepts Pédagogiques Illustrés

### Mathématiques
- Distance euclidienne et ses propriétés
- Moyennes et centres de masse
- Distributions de probabilité (normale, uniforme)
- Transformations linéaires (coordonnées)

### Algorithmique
- Algorithme itératif avec convergence
- Complexité temporelle O(n × k × i)
- Gestion des cas limites (clusters vides)
- Initialisation et optimisation

### Programmation
- Types personnalisés (`using`)
- Fonctions modulaires et réutilisables
- Gestion des erreurs (`try/catch`)
- Interface utilisateur interactive

## 💡 Améliorations Possibles

### Algorithme
- [ ] K-means++ pour meilleure initialisation
- [ ] Critère de convergence automatique
- [ ] Support pour plus de dimensions

### Interface
- [ ] Contrôle de vitesse en temps réel
- [ ] Sauvegarde des animations en GIF
- [ ] Zoom et navigation

### Code
- [ ] Tests unitaires
- [ ] Configuration par fichier JSON
- [ ] Mode batch sans interface

## 🚀 Utilisation

```bash
# Mode par défaut (8 clusters, 15 itérations)
./kmeans_visual_2d

# Personnalisé
./kmeans_visual_2d 5 20        # 5 clusters, 20 itérations
./kmeans_visual_2d 3 10 data.csv  # 3 clusters sur fichier CSV
```

## 📈 Résultats d'Apprentissage

En développant ce projet, j'ai appris :
- ✅ Implémentation complète de K-means
- ✅ Visualisation temps réel avec OpenCV
- ✅ Gestion des entrées utilisateur
- ✅ Architecture modulaire et commentée
- ✅ Debug et optimisation d'algorithmes

---

*Ce projet démontre une compréhension approfondie de l'algorithme K-means, de sa mise en œuvre pratique et de sa visualisation pédagogique.*
