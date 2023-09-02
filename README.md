# OTP-Hardware-Token-Device
 
This project involved developing an OTP (one-time password) token device with the Proteus Design Suite and Atmel Studio app and physically with the required hardware devices using an ATmega32 microcontroller.

The key components of the system include:

- LCD display with 16x2 characters to show menus, messages, and one-time passwords.

- Matrix keypad for user input and navigation.

- Custom PCB with microcontroller, LCD, keypad, and other components soldered on.

The system has multiple menus:

- Generate One-Time Password: User enters a PIN code, which is validated against the stored PIN. If correct, a 9-digit one-time password is randomly generated and displayed. This can be used for authentication.

- Change PIN Code: User enters current PIN code, which is validated. If correct, user can enter a new PIN code which is stored.

- Disable Card: Allows user to permanently disable the device if needed.

- Turn Display On/Off: To save power, display can be turned off, and turned back on with a keypress.

An ATmega32 microcontroller runs the main program loop, handles user input, controls the LCD display, generates random passwords, and manages other logic. Some key functions I have implemented include:

- Custom LCD driver functions for initialization, sending commands and data, going to specific positions, etc.

- Keypad scanning routine to detect button presses.

- Password verification functions.

- Random password generation using srand and rand functions.

- Menu system and overall program flow control.

- Timer interrupt to turn display off after set timeout.
