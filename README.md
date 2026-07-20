# ESP32 Morse Code Trainer

An interactive Morse Code Trainer built using the ESP32 microcontroller. This project converts user-entered text into Morse code and displays it using LEDs, while allowing speed and brightness adjustments through a potentiometer and LDR.

## Features

- 🔴 Red LED represents DOT (.)
- 🟢 Green LED represents DASH (-)
- 🟡 Yellow LED indicates message playback
- 🎛️ Adjustable Morse speed using a potentiometer
- 💡 Automatic LED brightness adjustment using an LDR
- 🔘 Push button to enter a new message
- 🔊 Buzzer alerts for invalid input
- 🔠 Supports A–Z letters and spaces
- 💻 Serial Monitor interface for text input

## Hardware Components

- ESP32 Development Board
- Red LED
- Green LED
- Yellow LED
- Push Button
- Potentiometer
- LDR
- Passive/Active Buzzer
- Breadboard
- Jumper Wires

## GPIO Connections

| Component | GPIO |
|-----------|------|
| Red LED | 18 |
| Yellow LED | 19 |
| Green LED | 21 |
| Buzzer | 22 |
| Push Button | 25 |
| LDR | 32 |
| Potentiometer | 33 |

## How It Works

1. Press the push button.
2. Enter a message in the Serial Monitor.
3. The input is validated.
4. Valid text is converted into Morse code.
5. LEDs blink according to Morse timing.
6. Potentiometer adjusts playback speed.
7. LDR adjusts LED brightness automatically.
8. Invalid input triggers the buzzer.

## Technologies Used

- Arduino IDE
- ESP32
- Embedded C++
- Serial Communication

## Future Improvements

- OLED/LCD Display
- Bluetooth text input
- Wi-Fi control
- Audio Morse playback
- Store frequently used messages

## Author

**Srinithi J**
