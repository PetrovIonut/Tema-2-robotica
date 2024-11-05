  Type Racer game in Arduino
  
  Task description : Create a game similar to TypeRacer, using a RGB Led to signal if user inputed text is correct or wrong.
  Components used : 
          Arduino UNO (ATmega328P microcontroller)
          1x LED RGB to signal if user inputed text is correct or not)
          2x Buttons (for start/stop and difficulty select)
          5x Resistors (3x 220/330 ohm, 2x 1000 ohm)
          Breadbord
          Wires
          
  Technical description :
RGB LED - Status indicator:

In the sleep state, the LED will be white.
When the start button is pressed, the LED will flash for 3 seconds, indicating a countdown until the start of the round.
During a round: The LED will be green if the text entered is correct and will turn red if there is an error.

Start/Stop button:

Sleep Mode: If the game is paused, pressing the button initiates a new round after a 3-second countdown.
During a round: If the round is active, pressing the button will stop it immediately.

Difficulty button:

The difficulty button controls the speed at which words appear and can only be used in idle mode.
With each press, the difficulty changes by cycling between: (Easy, Medium, Hard).
When changing the difficulty, a message is sent via serial: "Easy/Medium/Hard mode on!".

Word Generation:

A word dictionary will be created.
During a round, the words will be displayed in the terminal in a random order.
If the current word was spelled correctly, a new word will be displayed immediately. If not, a new word will appear after the time interval set by the difficulty.

Other observations:

The allotted time for a round is 30 seconds.
At the end of each round, the terminal will display how many words were spelled correctly.

  Functionality :

The game is in sleep mode. The RGB LED is white.
Choose the difficulty of the game using the difficulty button, and "Easy/Medium/Hard mode on!" will appear in the terminal.
The start/stop button is pressed.
The LED will flash for 3 seconds, and the terminal will display the countdown: 3, 2, 1.
The LED turns green and words to type begin to appear.
When typed correctly, the next word appears immediately. If the word is not typed within the time set by the difficulty, a new word will appear.
A mistake causes the LED to turn red. To correct the word, the BackSpace key will be used.
After 30 seconds, the round ends, and the terminal will display the score: the total number of correctly written words.
The game can be stopped at any time with the start/stop button.

  Pictures with setup:
  

  ![IMG20241105065842](https://github.com/user-attachments/assets/b7a8f79b-e325-4de0-bbd2-708af21e524e)

  ![IMG20241105065848](https://github.com/user-attachments/assets/3496e5a0-bc15-458e-b42d-f28aa4bb8592)

  Video with setup functionality: 
  https://youtu.be/u2vzYdlzlso
  
