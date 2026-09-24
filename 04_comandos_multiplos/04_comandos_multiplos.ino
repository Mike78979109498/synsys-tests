#define LED_PIN 2

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.println("READY");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.length() == 0) {
      return;
    }
    cmd.toUpperCase();

    if (cmd == "PING") {
      Serial.println("PONG");
    } else if (cmd == "LED_ON") {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("OK:LED_ON");
    } else if (cmd == "LED_OFF") {
      digitalWrite(LED_PIN, LOW);
      Serial.println("OK:LED_OFF");
    } else if (cmd == "STATUS") {
      Serial.print("STATUS:LED=");
      Serial.println(digitalRead(LED_PIN) ? "ON" : "OFF");
    } else {
      Serial.print("ERR:UNKNOWN_CMD:");
      Serial.println(cmd);
    }
  }
}
