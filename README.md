# MeshViewer — TP Géométrie Algorithmique

**Auteur :** Aymeric Letellier  
**École :** ESIEE Paris | Filière Informatique et Applications (FI)

Visualiseur de maillages 3D en C++ / OpenGL, basé sur une structure de demi-arêtes (half-edge).

---

## Compilation et lancement

```bash
./build.sh
./build/MeshViewer
```

Menu contextuel (clic droit) pour les opérations. Raccourci : **`s`** pour lancer une simplification.

---

## Fonctionnalités implémentées

### Chargement de maillage (`readFile`)

Lecture de fichiers `.obj` et construction de la structure half-edge (sommets, demi-arêtes, faces, twins).

| | |
|---|---|
| ![readfile_cube](screenshots/readfile_cube.png) | ![readfile_dauphin](screenshots/readfile_dauphin.png) |
| Cube (6 faces) | Dauphin (108 faces) |

![readfile_main](screenshots/readfile_main.png)

---

### Normales (`computeNormals`)

Calcul des normales par face et par sommet. Affichage des vecteurs normaux possible via le menu Draw.

![normales_main](screenshots/normales_affichees_main.png)

---

### Silhouette

Détection des arêtes de silhouette selon l'orientation des faces par rapport à la caméra.

| | |
|---|---|
| ![silhouette_dauphin](screenshots/silhouette_dauphin.png) | ![silhouette_pomme](screenshots/silhouette_normales_pomme.png) |

---

### Triangulation

Trois méthodes implémentées, avec gestion des faces concaves et des polygones à trous.

**Fan (éventail)** — octogone :

![triangulation_fan](screenshots/triangulation_fan_octogone.png)

**Barycentre** — octogone :

![triangulation_barycentre](screenshots/triangulation_barycentre_octogone.png)

**Ear clipping** — formes concaves :

| | |
|---|---|
| ![ear_clipping](screenshots/triangulation_ear_clipping.png) | ![ear_clipping_concave](screenshots/triangulation_ear_clipping_concave.png) |

**Polygones à trous** :

| | |
|---|---|
| ![trous_1](screenshots/triangulation_polygone_trous_1.png) | ![trous_2](screenshots/triangulation_polygone_trous_2.png) |

Triangulation appliquée sur un maillage complet (dauphin) :

![triangulation_dauphin](screenshots/triangulation_dauphin_wireframe.png)

---

### Tests half-edge (`checkMesh`)

Vérification de la cohérence de la structure après chargement et après les opérations de modification.

![half_edge_test](screenshots/half_edge_test.png)

---

### Surface de révolution (`generateRevolution`)

Génération procédurale d'un maillage par rotation d'un profil 2D.

| | |
|---|---|
| ![revolution](screenshots/surface_revolution.png) | ![revolution_wireframe](screenshots/surface_revolution_wireframe.png) |

![revolution_vue2](screenshots/surface_revolution_vue2.png)

---

### Simplification par edge collapse (`simplify`)

À chaque appel, collapse de l'arête la plus courte du maillage. Fonctionne sur maillages triangulés et non triangulés (quads, n-gons).

![simplification](screenshots/mesh_simplification.png)

---

### Subdivision Catmull-Clark (`subdivisionCatmullClark`)

Subdivision itérative : cube de départ puis 3 passes successives.

| | |
|---|---|
| ![cc1](screenshots/catmull_clark_1.png) | ![cc2](screenshots/catmull_clark_2.png) |
| Cube initial (6 faces) | 1ère subdivision (24 faces) |

| | |
|---|---|
| ![cc3](screenshots/catmull_clark_3.png) | ![cc4](screenshots/catmull_clark_4.png) |
| 2ème subdivision (96 faces) | 3ème subdivision (384 faces) |

---

## Use of AI

I used AI occasionally during the project, mainly when I was stuck on a bug or wanted to sanity-check an approach. The most concrete case was re-setting up the build on a virtual machine: the app had stopped working on my Mac due to OpenGL compatibility issues, and AI helped me get CMake / FreeGLUT / GLEW running again on the VM.

AI also helped on parts of the implementation when I needed a second opinion or got stuck integrating something into the half-edge structure. I cannot give an exact list of every interaction, as the use was sporadic and spread over time.
