# Pong

Pong v.1.0

## Description

Pong is a two-dimensional sports game that simulates table tennis. The player controls an in-game paddle by moving it vertically across the left or right side of the screen. They can compete against another player controlling a second paddle on the opposing side. Players use the paddles to hit a ball back and forth. The goal is for each player to reach ten points before the opponent; points are earned when one fails to return the ball to the other.

## Links

Github Game repository: https://github.com/youis11/Multiplayer-Networks-Game

### Contributors

Lluís Moreu Farran [youis11](https://github.com/youis11)

Marc Gálvez Llorens [optus23](https://github.com/optus23)

### Contributions

#### Lluís Moreu: 
UDP Virtual connection 
  : COMPLETELY ARCHIEVED.

World state replication 
  : ARCHIEVED with BUGS related to update serialization that Marc helped to fix creating a Map of actions and then a couple of bugs where left that I FIXED where the create and update states needed different serialization functions.

Reliability on top of UDP
  : COMPLETELY ARCHIEVED I changed the redundant sending because even if it was working well, I needed to change it for the Delivery manager implementation because I was sending different packets that could be in one.
___

#### Marc Gálvez
World state replication
  :FIXING some BUGS relating to update serialization, created a Map to help updating the actions because with a vector we were deleting the action so on the update it couldn't find it anymore
Redundant sending of input packets
  :COMPLETELY ARCHIEVED
Everything Gameplay related including art
  :COMPLETELY ARCHIEVED, Barriers movement and collision, Scores, Wall, Goal, Reset Game Behaviors.
  :Audio in Server and Audio in Replication reproduce.
  :Accept a certain number of players (delimited to two players in the game, if another player wants to enter, the menu screen will kick out him, the menu will automatically distribute the type of player (1 or 2) to fill the game correctly).
  :Handle players join/leave events (if one of the two players exists the game, his score and the ball must disappear. If another player enters the game. The match will continue).

## Controls

- Debug:

  - For debug options the ImGui Window is on the bottom left corner, you just need to move it up.

### Player controls:

If a controller is connected, doesn't accept keyboard inputs.

- Controller:

  - Joystick: MOVE UP & DOWN

  - Any button: SPEED UP

- Keyboard:

  - W, S: MOVE UP & DOWN

  - ARROW KEYS: SPEED UP

## Instructions to execute

1.- Download the last release on https://github.com/youis11/Multiplayer-Networks-Game/releases

2.- Unzip the file

3.- Execute Networks.exe

3.1.- Open ONE window for the server clicking on Start Server.

3.2.- Open TWO windows for each players, adding a name with only letters and clicking on Connect to Server. If you play locally leave the other parameters as they are, if you want to play online you should open ports on your router and the player who is connecting to you should enter your IP address.

~~~
!Important - This game is only playable for PC
~~~
