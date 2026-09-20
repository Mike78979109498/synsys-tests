void setup() {
  Serial.begin(115200);
}

void loop() {
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();

    if (comando == "LER") {
      int valorSimulado = random(20, 35);
      Serial.println(valorSimulado);
    }
  }
}
