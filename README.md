# GoBhello

Overview:
This project implements the classic game of Othello for two players, allowing them to play on separate Game Boys connected over Wi-Fi. The game features an 8x8 grid, where players take turns making moves to flip their opponent's pieces and dominate the board.

Hardware Requirements:

Two Game Boys (or compatible devices)
Keypad for user input
OLED display for visual feedback
Wi-Fi modules for communication (ESP32 used in this case)
Libraries Used:

Keypad library for handling keypad inputs
Adafruit_GFX and Adafruit_SSD1306 for managing the OLED display
Wi-Fi library for communication over Wi-Fi
Game Rules:

Players use the keypad to navigate the cursor on the 8x8 grid.
Pressing 'A' places a piece on the selected position.
The game follows Othello rules, flipping opponent pieces when surrounded.
The game continues until the board is full or players decide to end.
Networking:

The ESP32 boards act as a server and client, enabling communication over Wi-Fi.
Players connect to a Wi-Fi network created by the server ESP32.
Once both players are connected, the game begins.
Folder Structure:

Server: Contains the code for the Game Boy acting as the server.
Client: Contains the code for the Game Boy acting as the client.
How to Play:

Upload the server code to one Game Boy and the client code to the other.
Power on both Game Boys and connect to the Wi-Fi network created by the server.
Once both players are connected, the game starts automatically.
Use the keypad to navigate and 'A' to make a move.
Enjoy the Othello game on two Game Boys connected over Wi-Fi!
Notes:

Ensure that the Wi-Fi network credentials are correctly set in both server and client codes.
Both Game Boys must be within the Wi-Fi range for seamless communication.
