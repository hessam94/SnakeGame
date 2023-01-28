## installation on Visual Studio  
you need to clone the files and add to visual studio. then add the SDL libraries like this link :  
https://lazyfoo.net/tutorials/SDL/01_hello_SDL/windows/msvc2019/index.php   
I summurazied it here for you to make it easy:   
1. this is x64 project  
2. Go to Configuration Properties -> VC++ Directories -> Include Directories -> Edit. And then add the include directory from the SDL development folder we extracted.   the include folder provided in clone file in header/include.   
just add it to above section in properties  
3. Go to Configuration Properties -> Linker -> Additional Dependencies -> Edit. then add two entry like this , SDL2.lib; SDLmain.lib;  
4. Go to Configuration Properties -> VC++ Directories -> Library Directories -> Edit , then add the lib\x64 files that provided. you can find x32 files in the provided zip file.   
5. You need to add the \lib\x64 to your Environmental path

For Visual Studio, File-> New -> project From ExistingCode. then add the folder you cloned into. then create a project. it should work fine I tested few times and works well.  


# SnakeGame

This is a modified snake game which use the SDL2 library for rendering purposes. I tried to use some C++ functionalities here such as Object Oriented concepts, Multi-Threading, Smart Pointers and Memory Management and intelligent search.   
In this game a random food is put on the board, player should go towards the food and take it,
gets one point and then her speed and size will increase. It is the simple rule of the game. But I added some items:  
1. I added three enemy snake moving around the board. their size and speed is fixde and not changed. 
2. every 4 seconds the enemy snakes update their direction towards the player to hit her.  A more advance searching added which works based on A-Star AI algorithm. 
3. if the head of enemy hits the body of the player, we deduct one point from the player.  
4. if the player hits her body, game is over.  
5. if player hits and pass the enemy's body nothing happens.  
6. if the player's score becomes negative, the game is over.  
