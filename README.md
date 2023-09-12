# RetroBikeRacer
Little retro race game developed in C++ using the game-engine framework olcPixelGameEngine created by @Javidx9 

This project is based on the project developed by @Javidx9 in ​​his video "Code-It-Yourself! Retro Arcade Racing Game" https://www.youtube.com/watch?v=KkMZI5Jbf18&t=2072s

The following changes have been implemented in this version compared to the original:

-The project has been developed using the olcPixelGameEngine developed by Javidx9 (https://github.com/OneLoneCoder/olcPixelGameEngine) instead of the ConsoleGameEngine also developed by Javidx9 used in the original project.

-Implemented a track generator that will generate a pseudo-random track of the given length

-The game has been modified to work as a game against the clock where the player has to complete the track before the time runs out. For this a starting time of 60 seconds has been set and CheckPoints have been added which add an extra 30 seconds to the counter each time the player passes one.

-GameOver has been added which will end the game every time the time runs out or the player passes the finish line.

-The style of the game has been modified to resemble a Super Hang-On style motorbike game.
