void servo_on() {
	angle = ON_DEGREE;
  servo.write(ON_DEGREE);
  delay(300);
  servo.write(90);
  delay(1000);

  mqttClient.beginMessage(AWS_IOT_PUBLISH_TOPIC);   // 켜기 상태를 발행
  mqttClient.print(snd_payload_on);
  mqttClient.endMessage();
}

void servo_off() {
	angle = OFF_DEGREE;
  servo.write(OFF_DEGREE);
  delay(300);
  servo.write(90);
  delay(1000);

  mqttClient.beginMessage(AWS_IOT_PUBLISH_TOPIC);   // 끄기 상태를 발행
  mqttClient.print(snd_payload_off);
  mqttClient.endMessage();
}