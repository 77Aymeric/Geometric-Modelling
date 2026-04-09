# MeshViewer — TP1

**Auteur :** Aymeric Letellier  
**École :** ESIEE Paris | Filière Informatique et Applications (FI)  
**Adresse :** 2, boulevard Blaise Pascal, Cité Descartes, 93162 Noisy-le-Grand Cedex, France

---

## Description

MeshViewer est un visualiseur de maillages 3D développé en C++ avec OpenGL. Il supporte le chargement de fichiers `.obj`, la manipulation de maillages via une structure de demi-arêtes (half-edge), et diverses opérations géométriques.

## Fonctionnalités

- Chargement et affichage de maillages `.obj`
- Structure de données demi-arête (half-edge)
- Subdivision de Catmull-Clark et Loop
- Triangulation de faces
- Contraction d'arêtes et de faces
- Découpe, inflation, lissage de maillages
- Affichage wireframe, normales, silhouette, plis
- Sélection interactive de sommets, arêtes et faces
- Annulation (undo)
- Export du maillage modifié

## Dépendances

- OpenGL
- GLUT (FreeGLUT sur Linux, framework GLUT sur macOS)
- GLEW (inclus dans `external/`)
- GLM (inclus dans `external/`)
- CMake ≥ 3.10
- C++17

## Compilation

```bash
mkdir build && cd build
cmake ..
make
```

## Utilisation

```bash
./MeshViewer
```

Un menu contextuel (clic droit) permet d'accéder aux différentes fonctionnalités.

### Fichiers `.obj` fournis

| Fichier                | Description         |
|------------------------|---------------------|
| `cube.obj`             | Cube simple         |
| `cylinder.obj`         | Cylindre            |
| `apple.obj`            | Pomme               |
| `dolphin.obj`          | Dauphin             |
| `hand.obj`             | Main                |
| `shark.objtri.obj`     | Requin (triangulé)  |
| `beethovan.objtri.obj` | Beethoven (triangulé)|
| `Head.objtri.obj`      | Tête (triangulée)   |

## Structure du projet

```
MeshViewerCMake/
├── main.cpp              # Point d'entrée, rendu OpenGL et gestion des événements
├── myMesh.{h,cpp}        # Structure principale du maillage
├── myHalfedge.{h,cpp}    # Demi-arête
├── myFace.{h,cpp}        # Face
├── myVertex.{h,cpp}      # Sommet
├── myPoint3D.{h,cpp}     # Point 3D
├── myVector3D.{h,cpp}    # Vecteur 3D
├── helperFunctions.h     # Fonctions utilitaires
├── shaders/              # Shaders GLSL
├── external/             # Bibliothèques tierces (GLEW, GLM)
├── screenshots/          # Captures d'écran
└── CMakeLists.txt        # Configuration CMake
```
