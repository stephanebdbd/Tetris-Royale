# Tetris Royale

## Table of Contents

* [Introduction](https://www.google.com/search?q=%23introduction)
* [Features](https://www.google.com/search?q=%23features)
* [Prerequisites](https://www.google.com/search?q=%23prerequisites)
* [Installation](https://www.google.com/search?q=%23installation)
* [Usage](https://www.google.com/search?q=%23usage)
* [Team Members](https://www.google.com/search?q=%23team-members)
* [License](https://www.google.com/search?q=%23license)

## Introduction

This project is a Tetris game developed by our team for the **INFO-F209** course. We are proud to announce that this project received a grade of **17/20**.

The game is designed to replicate the classic Tetris experience, featuring interactive gameplay, a scoring system, and various difficulty levels. It is developed in C++ following Object-Oriented Programming (OOP) practices.

## Features

* Classic Tetris gameplay mechanics.
* User authentication system (username and password).
* Real-time updates with automatic tetromino movement.
* Keyboard controls for gameplay.
* Interactive game board representation available in both the terminal and a Graphical User Interface (GUI).

## Prerequisites

Before compiling and running the project, ensure the following libraries are installed on your system:

* **SFML** (Simple and Fast Multimedia Library) for the GUI.
* **ncurses** for terminal display.
* **SQLite3** for database management.
* **pthread** for thread management.

### Commands to install necessary libraries (Linux)

```bash
sudo apt update
sudo apt install libsfml-dev libncurses5-dev libsqlite3-dev g++ make

```

## Usage

1. Compile the source code using the Makefile located in the `build` folder:
```bash
cd build
make

```


2. Start the server using one of the following commands in the `src/server` folder:
```bash
./serveur

```


or
```bash
make run

```


3. Start the client using one of the following commands in the `src/client` folder:
```bash
./client

```


or
```bash
make run

```


4. After launching the client, choose your display mode:
* Type `1` for Terminal display.
* Type `2` for GUI display.


5. Enter your username and password to log in. If you do not have an account, create a new one.
6. Once logged in, you will access the main game menu.
7. In the main menu, choose one of the following options:
* **Play**: Start a game.
* **Friends**: Manage your friends list.
* **Chat**: Chat with other players.
* **Teams**: Manage or join a team.
* **Ranking**: View the leaderboard.



### **Friends** Menu

In the **Friends** menu, you can:

* **Add a friend**: Send a friend request to another player.
* **View friends list**: See all your current friends.
* **View friend requests**: See received requests and accept or decline them.
* **Private chat**: Send private messages to a friend.

### **Teams** Menu

In the **Teams** menu, you can:

* **Create a team**: Create a new team and become its owner.
* **Join a team**: Join an existing team by accepting an invitation.
* **View invitations**: See invitations received to join a team.
* **Room Chat**: Chat with team members in a dedicated chat room.
* **Manage a team**: If you are a member, admin, or owner, you can access specific options:
* **Member**: View team information.
* **Admin**: Add or remove members, manage invitations, etc.



### **Ranking** Menu

In the **Ranking** menu, you can view:

* **Global Ranking**: See the ranking of all players based on their total score.

8. In the **Play** menu, you can:
* Create a new game.
* Join an existing game.


9. If you choose to create a new game, select one of the following game modes:
* **Endless**: Infinite mode.
* **Duel**: Duel mode.
* **Classic**: Classic mode.
* **Royal**: Royal mode.


10. For **Duel**, **Classic**, and **Royal** modes, you will enter a lobby where you can:
* Invite friends.
* Adjust game settings before starting (speed, max number of players, etc.).



### Game Modes

#### **Duel**

In **Duel** mode, players compete directly against each other. A penalty (malus) system is in place:

* Penalties are sent to the opponent based on a combo scale. The more combos a player achieves, the heavier the penalties sent.
* Penalties can include adding extra lines to the bottom of the opponent's board.

#### **Classic**

In **Classic** mode, players play competitively but with a different penalty mechanic:

* Penalties are sent just like in **Duel** mode, but the target is chosen randomly by the server.
* However, the player can also manually select a target to send a specific penalty to.

#### **Royal**

**Royal** mode introduces a system of both penalties and bonuses:

* **Malus (Penalties)**: These affect other players and include effects such as:
* Increasing the speed of another player's tetrominoes.
* Inverting another player's controls.
* Blocking another player's controls.
* Deleting a 2x2 zone from another player's board.
* Blacking out another player's screen.


* **Bonus**: These are advantages for the player, such as:
* Transforming the next pieces into 1x1 blocks.
* Temporarily slowing down the falling pieces.



11. During the game, use the following keyboard controls:
* `Left Arrow`: Move tetromino left.
* `Right Arrow`: Move tetromino right.
* `Hold Down Arrow`: Accelerate the descent (Soft drop).
* `Up Arrow`: Rotate the tetromino 90 degrees clockwise.


12. Have fun!

## Team Members

* **Ismail Rhabouqi**
* **Chris BADI BUDU**
* **Anthony Van Der Wilt**
* **ikram BOUTALEB**
* **Mohamed BOUTALEB**
* **Oumaima Hamdach**
* **Valentin Dias**

## License
