void publishMessage() {
  Serial.println("메시지를 발행합니다");

  mqttClient.beginMessage(AWS_IOT_PUBLISH_TOPIC); // 메시지 시작
  mqttClient.print("스위치를 끄거나 키세요 ");
  mqttClient.print(millis());
  mqttClient.endMessage(); // 메시지 끝
}

void onMessageReceived(int messageSize) {   //페이로드 시리얼 프린트
  payload_received = 1;
  Serial.print("주제 '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("'의 메시지를 받았습니다, 길이 ");
  Serial.print(messageSize);
  Serial.println(" 바이트:");

  i = 0;
  while (mqttClient.available()) {
    payload[i] = (char)mqttClient.read();
    i++;
  }
  Serial.print(payload);

  Serial.println();

  Serial.println();
}

unsigned long getTime() {
  return WiFi.getTime(); // 현재 시간 가져오기
}

void emptyString() {
  i = 0;		//문자열관리 변수
  while(i < 512) {
    payload[i] = '\0';
    i++;
  }
}