# 🎵 Detect Notes & Chords

Application en **C++** permettant de détecter les **notes musicales** et les **accords** dans un fichier audio grâce à une analyse spectrale FFT.

---

## 📖 À propos
Ce projet est inspiré du principe que trouver un accords et une notes et souvent difficile donc pour simplifier et pour que les enfants s'y intéresse..  
Il analyse un fichier audio (WAV, MP3, FLAC…) et affiche :
- Les **notes détectées** (solo).
- Les **accords reconnus** (si plusieurs notes sont présentes en même temps).

---

## ⚙️ Technologies utilisées
- **C++** : langage principal.
- **FFTW** : bibliothèque pour la transformation de Fourier rapide (FFT).
- **libsndfile** : lecture de fichiers audio (WAV, FLAC, AIFF…).
- **GitHub Pages** : pour publier la documentation du projet.

---

## 🚀 Installation

### Linux / Termux
```bash
pkg install clang fftw libsndfile
