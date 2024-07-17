void connectWiFi() {
  Serial.print("SSID에 연결 시도 중: ");
  Serial.print(ssid);
  Serial.print(" ");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) { // 와이파이 연결 시도
    Serial.print(".");
    delay(5000);
  }
  Serial.println();

  Serial.println("네트워크에 연결되었습니다.");
  Serial.println();
}

void connectMQTT() {
  Serial.print("MQTT 브로커에 연결 시도 중: ");
  Serial.print(broker);
  Serial.println(" ");

  while (!mqttClient.connect(broker, 8883)) { // MQTT 브로커에 연결 시도
    Serial.print(".");
    delay(5000);
  }
  Serial.println();

  Serial.println("MQTT 브로커에 연결되었습니다.");
  Serial.println();

  mqttClient.subscribe(AWS_IOT_SUBSCRIBE_TOPIC); // 주제 구독
}