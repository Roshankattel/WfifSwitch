#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <BlynkSimpleEsp8266.h>
#include <Bounce2.h>

char auth[] = "55e189d17c08420e880bb02ede676b01";

#define CONNECTION_STATUS_TO_APP V3
#define SENSE_PIN_STATUS_TO_APP V5
#define LED_PIN_STATUS_TO_APP V4

const int blynkConnectionPin = 5;
const int sensePin = 12;  //from motor circut
const int ledPin = 13;
const int modeTriggerPin = 14;
const int buttonPin = 15;

bool blynkButton = LOW;
bool connectionStaus = LOW;
bool ledState = LOW;

Bounce debouncer1 = Bounce();
Bounce debouncer2 = Bounce();

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(modeTriggerPin, INPUT_PULLUP);
  pinMode(sensePin, INPUT_PULLUP);
  pinMode(blynkConnectionPin, OUTPUT);

  debouncer1.attach(buttonPin);
  debouncer1.interval(30);

  debouncer2.attach(sensePin);
  debouncer2.interval(30);

  Serial.begin(115200);
  Serial.println(digitalRead(modeTriggerPin));
  Serial.println("start up");

  if ( digitalRead(modeTriggerPin) == LOW ) {
    Serial.println("Entering setup mode.");
    WiFiManager wifiManager;
    wifiManager.startConfigPortal("preesuSwitch");
  }
  WiFi.mode(WIFI_STA);
  Blynk.config(auth);
}

void loop() {

  debouncer1.update();
  debouncer2.update();

  Blynk.run();

  int buttonPinValue = debouncer1.read();
  int sensePinValue = debouncer2.read();

  if (debouncer1.fell())  {
    ledState = !ledState;

    Serial.print(millis());
    Serial.print("->buttonpressed->");
    if (ledState) {
      Serial.print("ON");
    }
    else {
      Serial.print("OFF");
    }
    Blynk.virtualWrite(LED_PIN_STATUS_TO_APP, ledState);
    Serial.println();
  }
  digitalWrite(ledPin, ledState);

  if (debouncer2.fell() || debouncer2.rose()) {
    Blynk.virtualWrite(SENSE_PIN_STATUS_TO_APP, digitalRead(sensePin));
    Serial.println("Sense state changed");
  }

  if (Blynk.connected()) {
    digitalWrite(blynkConnectionPin, HIGH);
  }
  else {
    digitalWrite(blynkConnectionPin, LOW);
  }
}


BLYNK_WRITE(V1) {
  blynkButton = param.asInt();

  if (blynkButton == 1) {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }
  Blynk.virtualWrite(LED_PIN_STATUS_TO_APP, ledState);
}


BLYNK_APP_CONNECTED() {
  Blynk.virtualWrite(LED_PIN_STATUS_TO_APP, digitalRead(ledPin));
  Blynk.virtualWrite(SENSE_PIN_STATUS_TO_APP, digitalRead(sensePin));
}


BLYNK_CONNECTED() {
  Blynk.virtualWrite(LED_PIN_STATUS_TO_APP, digitalRead(ledPin));
  Blynk.virtualWrite(SENSE_PIN_STATUS_TO_APP, digitalRead(sensePin));
}


BLYNK_READ(CONNECTION_STATUS_TO_APP) {
  Blynk.virtualWrite(CONNECTION_STATUS_TO_APP, connectionStaus);
  connectionStaus = !connectionStaus;
}
