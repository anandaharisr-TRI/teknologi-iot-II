const int buttonPin = 4;
const int ledPin = 5;

int buttonState = 0;
int lastButtonState = LOW;
int ledState = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  
  if (buttonState == HIGH && lastButtonState == LOW) {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);

    if (ledState == HIGH) {
     Serial.println("Tombol ditekan! -> LED ON");
    } else {
     Serial.println("Tombol ditekan! -> LED OFF");
    }

    delay(200);
  }

  lastButtonState = buttonState;
}