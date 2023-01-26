installation on Visual Studio
you need to clone the files and add to visual studio. then add the SDL libraries like this link :
https://lazyfoo.net/tutorials/SDL/01_hello_SDL/windows/msvc2019/index.php 
I summurazied it here for you to make it easy: 
1- this is x64 project
2-Go to Configuration Properties -> VC++ Directories -> Include Directories -> Edit. And then add the include directory from the SDL development folder we extracted. the include folder provided in clone file in header/include. 
just add it to above section in properties
3- Go to Configuration Properties -> Linker -> Additional Dependencies -> Edit. then add two entry like this , SDL2.lib; SDLmain.lib;
4- Go to Configuration Properties -> VC++ Directories -> Library Directories -> Edit , then add the lib\x64 files that provided. you can find x32 files in the provided zip file . 
5- You need to add the \lib\x64 to your Environmental path on your

For Visual Studio, File-> New -> project From ExistingCode. then add the folder you cloned into. then create a project. it should work fine I tested few times and works well.


# SnakeGame

This is a modified game based on the recommended game by Udacity. the core of the game is not changed like the rendering and Snake functions. the simple game put a random food, player should go towrds the food and take it,
gets one point and then her speed and size will increase. it is the simple rule of the game. But i added some items and applied the rubric for implementation of these new items:
1- I added three enemy snake moving around the board. their size and speed is fixde and not changed. 
2- every 4 seconds a the enemy snakes update their direction towards the player to hit her
3- if the head of enemy hits the body of player, we deduct one point from player
4- if the player hits her body, game is over
5- if player hits and pass the enemy body nothing happens
6- if the player score become negative, the game is over

So it was the new rules and items I added to the game, now the rubric:

##Loops, Functions, I/O
1- The project demonstrates an understanding of C++ functions and control structures:
too many functions are added to the program like: in game.h line 40-45, CreateEnemies(), RunEnemies(), ....
in each function control and iterational structures are used


##Object Oriented Programming, color.h
2-The project uses Object Oriented Programming techniques: 
the main provided file is classified well but I added one separate class too. there is a Color.h struct which has functions, overloaded constructor and attributes members which describe a (R,G,B,A) color system. 
before in the main code, the colors were hard-coded, but with my version it is flexible and object oriented based now. 
3- Classes use appropriate access specifiers for class members: it is struct and all are public 
4- Classes abstract implementation details from their interfaces: the name of the pre-defined functions are clear, as the name of the color such as Red,Blue,...
5 -Overloaded functions allow the same function to operate on different parameters: the two constructor is overloaded

##Memory Management
6- The project makes use of references in function declarations: 
I have both types:  Points_Are_Equal(SDL_Point& p1, SDL_Point& p2) and  RefreshScreen(....)  in game.h is call by reference and isDirectionParallel(Snake::Direction d1, Snake::Direction d2) is call by value.
7- The project uses smart pointers instead of raw pointers:  in main.cpp line 20 , I used a shared pointer for game object. 

##Concurrency
8- The project uses multithreading: there are two thraed in this program. the main thread of the program and in main.cpp line 27, the second thread is responsible to run the AI of enemies, when this thread run, the other thread 
is waited on its oncditional variable
9- A mutex or lock is used in the project: mutex is used in two functions: game.cpp, UpdateEnemies(), line 216 and in game.cpp, Run() , line 40 ; explained well as comment inside the function what they are tyring to do;
Also unique_lock is used for mutex 
10- A condition variable is used in the project: in two above funtions, a conditional variable which is shared for game object is used and workes well. i could protect the race for two threads well.
