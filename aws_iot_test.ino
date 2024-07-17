#include <ArduinoBearSSL.h>
#include <ArduinoECCX08.h>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h> // MKR1000의 경우 #include <WiFi101.h>로 변경
#include <ArduinoJson.h>  //적용 준비

#include <SPI.h>   //아두이노 communication 라이브러리
#include <Servo.h> //서보모터 라이브러리
#include <string>

#include "arduino_secrets.h"

#define SERVO_PIN 9  // 서보 모터 신호 핀 9
#define BUTTON_PIN 2  // 택트 스위치의 값을 읽는 핀

#define ON_DEGREE 35    //스위치를 켤 때 각도
#define OFF_DEGREE 165    //스위치를 끌 때 각도

#define AWS_IOT_PUBLISH_TOPIC "PUBLIH TOPI"   //게시하기 위한(전송하기 위한) 주제
#define AWS_IOT_SUBSCRIBE_TOPIC "SUBSCRIBE_TOPIC"    //구독하기 위한(수신받기 위한) 주제


const char ssid[]        = SECRET_SSID;       // 와이파이 SSID
const char pass[]        = SECRET_PASS;       // 와이파이 비밀번호
const char broker[]      = SECRET_BROKER;     // MQTT 브로커 주소
const char* certificate  = SECRET_CERTIFICATE; // SSL/TLS 인증서

WiFiClient    wifiClient;     // TCP 소켓 연결에 사용되는 와이파이 클라이언트
BearSSLClient sslClient(wifiClient); // SSL/TLS 연결에 사용되는 클라이언트, ECC508과 통합됨
MqttClient    mqttClient(sslClient); // MQTT 클라이언트

Servo servo;  // Servo 객체 생성
int angle;    // 각도 변수 선언

unsigned long lastMillis = 0;   //시간을 받아오는 변수 초기화
int i = 0;    //문자열 관리변수

char payload[512];   //받은 페이로드
char snd_payload_on[512];   // 끄기 페이로드
char snd_payload_off[512];    // 켜기 페이로드
int payload_received;    //페이로드 수신여부 확인 변수

void setup() {
  servo.attach(SERVO_PIN);   //서보모터 신호핀 설정 9번
  
  Serial.begin(115200);		//시리얼 통신 시작
  pinMode(BUTTON_PIN, INPUT_PULLUP);  //풀업 저항 사용, BUTTON_PIN의 모드를 설정

  angle = OFF_DEGREE;   //초기 상태를 off 상태로 설정.

  sprintf(snd_payload_on,"{\"state\": { \"reported\": { \"status\": \"on\" } }}");   // 켜기 페이로드 설정
  sprintf(snd_payload_off,"{\"state\": { \"reported\": { \"status\": \"off\" } }}");   // 끄기 페이로드 설정

  if (!ECCX08.begin()) { // ECCX08 시작
    Serial.println("ECCX08이 없습니다!");
    while (1);
  }

  ArduinoBearSSL.onGetTime(getTime); // 시간 가져오기 콜백 설정

  sslClient.setEccSlot(0, certificate); // ECCX08 슬롯 설정

  mqttClient.setId("clientId"); // MQTT 클라이언트 ID 설정

  mqttClient.onMessage(onMessageReceived); // 메시지 수신 콜백 설정

  mqttClient.beginMessage(AWS_IOT_PUBLISH_TOPIC);
  mqttClient.print(snd_payload_off);
  mqttClient.endMessage();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) { // 와이파이 연결 상태 확인
    connectWiFi(); // 와이파이 연결 시도
  }

  if (!mqttClient.connected()) { // MQTT 클라이언트 연결 상태 확인
    connectMQTT(); // MQTT 브로커에 연결
  }

  mqttClient.poll(); // 새로운 MQTT 메시지 폴링 및 keep alive 보내기

  int button_state = digitalRead(BUTTON_PIN);   //BUTTON_PIN을 읽어 button_state에 저장
  
  if(payload_received == 1) { 
    StaticJsonDocument<512> sensor_doc;   // JSON 문서 생성
    DeserializationError error_sensor = deserializeJson(sensor_doc, payload);   // 수신한 페이로드를 JSON으로 변환
    if (error_sensor) {
      Serial.print("json 생성 오류");
    }
    const char *sensor = sensor_doc["state"]["status"];   // 상태 추출    
    Serial.print(sensor);

    if (strcmp(sensor, "on") == 0 && angle == OFF_DEGREE) {   //풀업 저항에 사용 중이므로 low을 때 스위치 작동확인 가능, 꺼져있을 때 킴.
      Serial.print("test");
      servo_on();
      payload_received = 0;
      sensor = " ";
      emptyString();
    } else if (strcmp(sensor, "off") == 0 && angle == ON_DEGREE){    //풀업 저항에 사용 중이므로 low을 때 스위치 작동확인 가능, 꺼져있을 때 킴.
	    servo_off();
      payload_received = 0;
      sensor = " ";
      emptyString();
    }
  }

  if (button_state == LOW && angle == OFF_DEGREE) {   //풀업 저항에 사용 중이므로 low을 때 스위치 작동확인 가능, 꺼져있을 때 킴.
    servo_on();
  } else if (button_state == LOW && angle == ON_DEGREE){    //풀업 저항에 사용 중이므로 low을 때 스위치 작동확인 가능, 꺼져있을 때 킴.
    servo_off();
  }
}