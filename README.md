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

### Menu **Amis**
Dans le menu **Amis**, vous pouvez effectuer les actions suivantes :
   - **Ajouter un ami** : Envoyer une demande d'ami à un autre joueur.
   - **Consulter la liste d'amis** : Voir tous vos amis actuels.
   - **Consulter la liste des demandes d'amis** : Voir les demandes d'amis reçues et les accepter ou les refuser.
   - **Chat privé avec un ami** : Envoyer des messages privés à un ami.

### Menu **Teams**
Dans le menu **Teams**, vous pouvez :
   - **Créer une équipe** : Créer une nouvelle équipe et devenir son propriétaire.
   - **Rejoindre une équipe** : Rejoindre une équipe existante en acceptant une invitation.
   - **Consulter la liste des invitations** : Voir les invitations reçues pour rejoindre une équipe.
   - **Chat dans une room** : Discuter avec les membres de l'équipe dans une salle de discussion dédiée.
   - **Gérer une équipe** : Si vous êtes membre, administrateur ou propriétaire d'une équipe, vous pouvez accéder à des options spécifiques :
     - **Membre** : Voir les informations de l'équipe.
     - **Administrateur** : Ajouter ou retirer des membres, gérer les invitations, etc.

### Menu **Ranking**
Dans le menu **Ranking**, vous pouvez consulter :
   - **Classement global** : Voir le classement de tous les joueurs en fonction de leur score total.

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
    - Ajuster les paramètres de la partie avant son début (vitesse, nombre maximum de joueurs, etc.).

### Modes de jeu

#### **Duel**
Dans le mode **Duel**, les joueurs s'affrontent directement. Un système de malus est mis en place :
   - Les malus sont envoyés à l'adversaire en fonction d'une échelle de combos. Plus le joueur réalise de combos, plus les malus envoyés sont importants.
   - Les malus peuvent inclure des lignes supplémentaires ajoutées au bas du plateau de l'adversaire.

#### **Classic**
Dans le mode **Classic**, les joueurs jouent de manière compétitive, mais avec une mécanique différente pour les malus :
   - Les malus sont envoyés comme dans le mode **Duel**, mais leur destinataire est choisi aléatoirement par le serveur.
   - Cependant, le joueur peut également sélectionner manuellement un destinataire pour envoyer un malus spécifique.

#### **Royal**
Le mode **Royal** introduit un système de malus et de bonus :
   - **Malus** : Les malus affectent les autres joueurs et incluent des effets tels que :
     - Accélérer la vitesse des tétriminos d'un autre joueur.
     - Ajouter des lignes supplémentaires au plateau d'un autre joueur.
   - **Bonus** : Les bonus sont des avantages pour le joueur, tels que :
     - Recevoir un tétrimino composé d'un seul carré.
     - Ralentir la vitesse de descente des tétriminos.

11. Pendant la partie, utilisez les commandes clavier suivantes :
    - `Flèche gauche` : Déplacer le tétrimino vers la gauche.
    - `Flèche droite` : Déplacer le tétrimino vers la droite.
    - `Maintenir la flèche bas` : Accélérer la descente du tétrimino.
    - `Flèche haut` : Tourner le tétrimino de 90 degrés dans le sens des aiguilles d'une montre.

12. Amusez-vous bien !

## Membres de l'équipe

- **Ismail Rhabouqi** : 584567
- **Chris BADI BUDU** : 569082
- **Anthony Van Der Wilt** : 569315
- **ikram BOUTALEB** : 536046
- **Mohamed BOUTALEB** : 566181
- **Oumaima Hamdach** : 568078
- **Valentin Dias** : 593394