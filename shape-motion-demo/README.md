# Ping Pong Game

This directory contains:
* code that implements a msp430 game  
* time and shape libraries provided generously by the proffesor and TA's

After being hired by ACME, the toy I was in charged of developing
is a ping pong game toy.

It was designed with visual paddles and a playing ball

The game a classic ping pong game that allows each player to deflect the ball in order to
not award the opposite player a point

The ball will bounce of the walls and paddles and only count points if the wall
behind the padlle is hit

First player to reach a total of 3 points wins the game!


* Assembly state machine switches the frequency that is played when a player wins a point or wins the game.

* The assembly was produced by using the following command

$ gcc -O2 -S assemblyMain.c

To delete binaries:
~~~
$ make clean
~~~
 
To compile:

(Inside the project directory we run the makefile to make sure that our
timer libraries and shape libraries are initialized.
~~~

$ make all
~~~

To test it, select the shape-motion-demo directoy and run:
~~~
$ make load
~~~
