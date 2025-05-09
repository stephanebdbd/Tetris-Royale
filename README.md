# Tetris Royale

## Table des matières
- [Introduction](#introduction)
- [Fonctionnalités](#fonctionnalités)
- [Prérequis](#prérequis)
- [Installation](#installation)
- [Utilisation](#utilisation)
- [Membres de l'équipe](#membres-de-léquipe)
- [Licence](#licence)

## Introduction
Ce projet est un jeu de Tetris réalisé par notre équipe dans le cadre du cours INFO-F209. Le jeu est conçu pour reproduire l'expérience classique de Tetris, avec un gameplay interactif, un système de score, et différents niveaux de difficulté. Il est développé en C++ en suivant des pratiques de la programmation orientée objet.

## Fonctionnalités
- Mécaniques de jeu classiques de Tetris.
- Système d'authentification utilisateur (nom d'utilisateur et mot de passe).
- Mises à jour en temps réel avec déplacement automatique des tétrominos.
- Contrôles au clavier pour le gameplay.
- Représentation interactive du plateau de jeu dans le terminal ou via une interface graphique.

## Prérequis
Avant de compiler et d'exécuter le projet, assurez-vous que les bibliothèques suivantes sont installées sur votre système :
- **SFML** (Simple and Fast Multimedia Library) pour l'interface graphique.
- **ncurses** pour l'affichage dans le terminal.
- **SQLite3** pour la gestion de la base de données.
- **pthread** pour la gestion des threads.

### Commandes pour installer les bibliothèques nécessaires (Linux)
```bash
sudo apt update
sudo apt install libsfml-dev libncurses5-dev libsqlite3-dev g++ make


## Utilisation
1. Compilez le code source avec le Makefile situé dans le dossier `build` :
   ```bash
   cd build
   make
   ```

2. Lancez le serveur avec l'une des commandes suivantes dans le dossier `src/server` :
   ```bash
   ./serveur
   ```
   ou
   ```bash
   make run
   ```

3. Lancez le client avec l'une des commandes suivantes dans le dossier `src/client` :
   ```bash
   ./client
   ```
   ou
   ```bash
   make run
   ```

4. Après avoir lancé le client, choisissez le mode d'affichage :
   - Tapez `1` pour l'affichage dans le terminal.
   - Tapez `2` pour l'interface graphique.

5. Entrez votre nom d'utilisateur et votre mot de passe pour vous connecter. Si vous n'avez pas de compte, créez-en un nouveau.

6. Une fois connecté, vous accédez au menu principal du jeu.

7. Dans le menu principal, choisissez une option parmi les suivantes :
   - **Play** : Jouer une partie.
   - **Amis** : Gérer votre liste d'amis.
   - **Chat** : Discuter avec d'autres joueurs.
   - **Teams** : Gérer ou rejoindre une équipe.
   - **Ranking** : Consulter le classement.

8. Dans le menu **Play**, vous pouvez :
   - Créer une nouvelle partie.
   - Rejoindre une partie existante.

9. Si vous choisissez de créer une nouvelle partie, sélectionnez le mode de jeu parmi les suivants :
   - **Endless** : Mode infini.
   - **Duel** : Mode duel.
   - **Classic** : Mode classique.
   - **Royal** : Mode royal.

10. Pour les modes **Duel**, **Classic**, et **Royal**, vous accédez à un lobby où vous pouvez :
    - Inviter vos amis.
    - Ajuster les paramètres de la partie avant son début.

11. Pendant la partie, utilisez les commandes clavier suivantes :
    - `Flèche gauche` : Déplacer le tétromino vers la gauche.
    - `Flèche droite` : Déplacer le tétromino vers la droite.
    - `Maintenir la flèche bas` : Accélérer la descente du tétromino.
    - `Flèche haut` : Tourner le tétromino de 90 degrés dans le sens des aiguilles d'une montre.
    - `ESC` : Quitter le jeu.

12. Amusez-vous bien !


## Membres de l'équipe

- **Ismail Rhabouqi** : 584567
- **Chris BADI BUDU** : 569082
- **Anthony Van Der Wilt** : 569315
- **ikram BOUTALEB** : 536046
- **Mohamed BOUTALEB** : 566181
- **Oumaima Hamdach** : 568078
- **Rayan Rabeh** : 576232
- **Valentin Dias** : 593394