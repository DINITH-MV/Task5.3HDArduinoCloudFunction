//Required WiFiNINA Library for Arduino from https://github.com/arduino-libraries/WiFiNINA

#include <WiFiNINA.h>
#include "secrets.h"

#define RED_LED 6
#define GREEN_LED 7
#define BLUE_LED 8

String FIREBASE_HOST = "traffic-light-system-3096d-default-rtdb.firebaseio.com";
String FIREBASE_AUTH = "";

String PATH_URL = "/.json";


WiFiSSLClient client;

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  Serial.print("Connecting to Wi-Fi");
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void loop() {
  String responseBody = "";

  if (client.connect(FIREBASE_HOST.c_str(), 443)) {
    client.println("GET " + PATH_URL + " HTTP/1.1");
    client.println("Host: " + FIREBASE_HOST);
    client.println("Connection: close");
    client.println();

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    bool isBody = false;

    while (client.available()) {
      String line = client.readStringUntil('\n');
      line.trim();

      if (line.length() == 0) {
        isBody = true;
        continue;
      }

      if (isBody) {
        responseBody += line;
      }
    }

    Serial.println("LEDS : " + responseBody);

    if (responseBody.indexOf("\"LED_RED\":\"on\"") != -1) {
      digitalWrite(RED_LED, HIGH);
      Serial.println("RED_LED : ON");
    } else {
      digitalWrite(RED_LED, LOW);
      Serial.println("RED_LED : OFF");
    }

    if (responseBody.indexOf("\"LED_GREEN\":\"on\"") != -1) {
      digitalWrite(GREEN_LED, HIGH);
      Serial.println("GREEN_LED : ON");
    } else {
      digitalWrite(GREEN_LED, LOW);
      Serial.println("GREEN_LED : OFF");
    }

    if (responseBody.indexOf("\"LED_BLUE\":\"on\"") != -1) {
      digitalWrite(BLUE_LED, HIGH);
      Serial.println("BLUE_LED : ON");
    } else {
      digitalWrite(BLUE_LED, LOW);
      Serial.println("BLUE_LED : OFF");
    }

    client.stop();
  } else {
    Serial.println(">>> Connection failed.");
  }
}
