# 🔢 Sort Visualizer (SDL2)

![preview](https://github.com/user-attachments/assets/10c2b588-a3b9-480b-83e8-58616f3997a2)

Un visualiseur de tri en C utilisant **SDL2**.  
Affiche différents algorithmes de tri en temps réel avec animations et couleurs dynamiques.

---

## ✨ Features

- 🎨 Visualisation en barres en temps réel
- 🔁 Changement automatique d’algorithme
- ⏱️ Restart automatique après chaque tri
- 🧠 Plusieurs algorithmes implémentés :
  - Bogo Sort
  - Bubble Sort
  - Selection Sort
  - Insertion Sort
  - Cocktail Sort
  - Gnome Sort
  - Stalin Sort (😈)
  - Thanos Sort (💀)
  - Gamble Sort (🎲)
- 🎯 Bouton **SKIP / RESTART**
- 🌙 Mode **sleep** (éteint l’écran, idéal Raspberry Pi)

---

## 🛠️ Compilation

```bash
gcc main.c -o sort -lSDL2 -lSDL2_ttf
```

---

## ▶️ Lancer

```bash
./sort
```

---

## 🎮 Contrôles

| Action | Description |
|--------|------------|
| Click bouton gauche | Skip / Restart |
| Click bouton droit | Sleep mode |
| ESC | Quitter |

---

## 🧠 Fonctionnement

Chaque algorithme est structuré avec :

```c
typedef struct {
    char name[32];
    int size;
    int fps;
    void (*init)(int*);
    void (*step)(int*, int* done, int* size);
    void (*get_color)(int index, int size, Uint8*, Uint8*, Uint8*);
} Algo;
```

---

## 🎨 Couleurs

- 🔵 Bleu → éléments triés  
- 🔴 Rouge → éléments en cours  
- 🟢 Vert → minimum (selection sort)  
- 🟡 Jaune → animation de fin  

---

## ⚡ Particularités

- Certains tris sont volontairement absurdes :
  - **Stalin Sort** → supprime les éléments non triés
  - **Thanos Sort** → supprime la moitié au hasard
  - **Gamble Sort** → swaps aléatoires

---

## 📦 Dépendances

- SDL2  
- SDL2_ttf  

Installation (Ubuntu/Debian) :

```bash
sudo apt install libsdl2-dev libsdl2-ttf-dev
```

---

## 💡 Idées d’amélioration

- Ajouter QuickSort / MergeSort
- Mode comparaison de plusieurs tris
- Graphiques de performance
- Interface menu

---

## 📜 Licence

Libre d’utilisation / modification.


