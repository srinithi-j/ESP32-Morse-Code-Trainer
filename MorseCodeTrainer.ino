/* Full Morse trainer sketch
   - DOT = RED LED (GPIO18)
   - DASH = GREEN LED (GPIO21)
   - STATUS = YELLOW LED (GPIO19) ON while playing
   - button -> GPIO25 (press to enter input)
   - pot -> GPIO33 (speed)
   - LDR -> GPIO32 (brightness)
   - buzzer -> GPIO22 (error tone)
   - Set PASSIVE_BUZZER true if you have a passive buzzer (needs tone generation)
*/

#include <Arduino.h>

const bool PASSIVE_BUZZER = true; // <-- set true for passive buzzers, false for active buzzers

const int red_ledpin    = 18;
const int yellow_ledpin = 19;
const int green_ledpin  = 21;

const int buttonpin = 25;
const int potpin    = 33;
const int ldrpin    = 32;
const int buzzerpin = 22;

unsigned long dotUnit = 250;
String userMessage = "";

// Morse map A-Z
String morseMap[26] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
  "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
  "..-", "...-", ".--", "-..-", "-.--", "--.."
};

// ----------------- Helpers -----------------

// Read LDR and map to 0..255 brightness (invert: dark->bright)
int readLdrBrightness() {
  int v = analogRead(ldrpin); // 0..4095
  int b = map(v, 0, 4095, 255, 50); // dark -> 255, bright -> 50
  b = constrain(b, 0, 255);

  // print LDR raw and mapped brightness
  Serial.print("LDR Value: ");
  Serial.print(v);
  Serial.print("  | Brightness Value: ");
  Serial.println(b);

  return b;
}

// software-PWM blink for totalMs using brightness 0..255
void pwmBlink(int pin, unsigned long totalMs, int brightness) {
  unsigned long minOn = 20; // ms
  unsigned long onTime = (unsigned long)((uint32_t)brightness * (uint32_t)totalMs / 255u);
  if (onTime < minOn && brightness > 0) onTime = minOn;
  if (onTime > totalMs) onTime = totalMs;

  digitalWrite(pin, HIGH);
  delay(onTime);
  digitalWrite(pin, LOW);

  if (totalMs > onTime) delay(totalMs - onTime);
}

// Passive-buzzer tone generator (simple blocking tone)
void toneOnPin(int pin, unsigned int freq, unsigned long ms) {
  unsigned long start = millis();
  unsigned long period_us = 1000000UL / freq;
  while (millis() - start < ms) {
    digitalWrite(pin, HIGH);
    delayMicroseconds(period_us / 2);
    digitalWrite(pin, LOW);
    delayMicroseconds(period_us / 2);
  }
}

// buzzer error tone (3 short beeps). Works for active and passive buzzers.
void playErrorTone() {
  Serial.println("ERROR: Invalid Input -> Buzzer Produces Sound");
  if (PASSIVE_BUZZER) {
    // passive: generate tone frequency pulses
    toneOnPin(buzzerpin, 2000, 120);
    delay(100);
    toneOnPin(buzzerpin, 2000, 120);
    delay(100);
    toneOnPin(buzzerpin, 2000, 120);
  } else {
    // active: simple HIGH/LOW pulses
    const int beepMs = 1000;
    const int pauseMs = 250;
    for (int i = 0; i < 3; ++i) {
      digitalWrite(buzzerpin, HIGH);
      delay(beepMs);
      digitalWrite(buzzerpin, LOW);
      delay(pauseMs);
    }
  }
}

// Check string contains only letters (A-Z or a-z) and spaces
bool isAlphaAndSpaceOnly(const String &s) {
  if (s.length() == 0) return false;
  for (unsigned int i = 0; i < s.length(); ++i) {
    char c = s[i];
    if (c == ' ') continue;
    if (c >= 'A' && c <= 'Z') continue;
    if (c >= 'a' && c <= 'z') continue;
    return false; // found non-letter, non-space
  }
  return true;
}

// ----------------- Morse primitives -----------------

// DOT = RED LED blink (brightness from LDR)
void dot() {
  int bright = readLdrBrightness();
  pwmBlink(red_ledpin, dotUnit, bright);
  delay(dotUnit); // inter-element gap
}

// DASH = GREEN LED blink
void dash() {
  int bright = readLdrBrightness();
  pwmBlink(green_ledpin, dotUnit * 3, bright);
  delay(dotUnit); // inter-element gap
}

// play one letter code (string of '.' and '-')
void playLetter(const String &code) {
  for (unsigned int i = 0; i < code.length(); ++i) {
    if (code[i] == '.') dot();
    else if (code[i] == '-') dash();
  }
  delay(dotUnit * 2); // gap after letter
}

// play a whole message (uses morseMap for letters)
void playMessage(const String &msg) {
  digitalWrite(yellow_ledpin, HIGH); // playing indicator
  Serial.print("Playing message: ");
  Serial.println(msg);

  for (unsigned int i = 0; i < msg.length(); ++i) {
    char ch = msg[i];
    if (ch >= 'A' && ch <= 'Z') {
      String code = morseMap[ch - 'A'];
      Serial.print(ch); Serial.print(" -> "); Serial.println(code);
      playLetter(code);
    } else if (ch == ' ') {
      // word gap
      delay(dotUnit * 7);
    }
  }

  Serial.println("Done.\n");
  digitalWrite(yellow_ledpin, LOW);
}

// ----------------- Setup & Loop -----------------

void setup() {
  Serial.begin(115200);

  pinMode(red_ledpin, OUTPUT);
  pinMode(yellow_ledpin, OUTPUT);
  pinMode(green_ledpin, OUTPUT);

  pinMode(buttonpin, INPUT_PULLUP);
  pinMode(potpin, INPUT);
  pinMode(ldrpin, INPUT);

  pinMode(buzzerpin, OUTPUT);
  digitalWrite(buzzerpin, LOW);

  digitalWrite(red_ledpin, LOW);
  digitalWrite(yellow_ledpin, LOW);
  digitalWrite(green_ledpin, LOW);

  Serial.println();
  Serial.println("=== Morse Trainer Ready ===");
  Serial.println("Press the button to enter a new Morse message.");
  Serial.println("Input rule: only alphabet letters (A-Z) and spaces allowed.");
  Serial.print("Buzzer mode: ");
  Serial.println(PASSIVE_BUZZER ? "PASSIVE (tone generation)" : "ACTIVE (simple HIGH)");
  Serial.println();
}

int lastBtn = HIGH;

void loop() {
  // speed control using pot: dotUnit range 150..900 ms
  int pot = analogRead(potpin);
  dotUnit = map(pot, 0, 4095, 150, 900);

  int btn = digitalRead(buttonpin);

  // detect button press (falling edge)
  if (btn == LOW && lastBtn == HIGH) {
    delay(50); // debounce
    Serial.println("\nEnter a new Morse message (letters and spaces only):");

    // wait for serial input
    while (!Serial.available()) delay(10);
    userMessage = Serial.readStringUntil('\n');
    userMessage.trim();

    Serial.print("You entered: ");
    Serial.println(userMessage);

    // validate: only letters and spaces
    if (!isAlphaAndSpaceOnly(userMessage)) {
      Serial.println("ERROR: Input contains invalid characters (digits/punctuation/etc.) or is empty.");
      playErrorTone();
    } else {
      // convert to uppercase for playback
      String upperMsg = userMessage;
      upperMsg.toUpperCase();
      playMessage(upperMsg);
    }
  }

  lastBtn = btn;
}
