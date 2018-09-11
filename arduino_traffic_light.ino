#include "sevseg.h"
int state = 0;
#define PEDESTRIAN_RED_DURATION 7000
#define PEDESTRIAN_NORMAL_GREEN_DURATION 5000
#define PEDESTRIAN_BLINK_GREEN_DURATION 4000
#define PEDESTRIAN_TOTAL_GREEN_DURATION (PEDESTRIAN_NORMAL_GREEN_DURATION + PEDESTRIAN_BLINK_GREEN_DURATION)
#define NFC_ELONGATE_GREEN_DURATION 3000
#define TRAFFIC_GREEN_MIN_TIME 0    //haven't implement

//0: R|G, 1: R|Y, 2:R|R, 3: G|R, 4: GB|R, 5: R|R, 6: R|RY
uint32_t duration[] = {
  PEDESTRIAN_RED_DURATION,          //0: pedestrian red and traffic green
  1000,                             //1: traffic to yellow
  500,                              //2: both red
  PEDESTRIAN_NORMAL_GREEN_DURATION, //3: pedestrian normal green
  PEDESTRIAN_BLINK_GREEN_DURATION,  //4: pedestrian blink green
  500,                              //5: both red
  1000                              //6: traffic yellow and red
};
uint32_t next_state_time = 0;

bool tapped_card = false;

//for blinking
bool green_flag = 0;
uint32_t green_next_blink = 0;

#define P_RED 11
#define P_GREEN 10
#define T_RED 13
#define T_YELLOW 12
#define T_GREEN 11


bool pkg_start = false;
int curr_len = 0;
int total_len = 0;
byte buf[100];
bool blinking = false;
bool flag = false;
uint32_t next_blink = 0;
bool init_auto_detect = false;

SevSeg *ps0 = NULL;
SevSeg *ps1 = NULL;

void setup() {
  // put your setup code here, to run once:
  pinMode(0, INPUT);
  pinMode(1, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(P_RED, OUTPUT);
  pinMode(P_GREEN, OUTPUT);
  pinMode(T_RED, OUTPUT);
  pinMode(T_YELLOW, OUTPUT);
  pinMode(T_GREEN, OUTPUT);
  Serial.begin(9600);
  byte pins0[8] = {3,2,A2,A1,A0,4,5};
  ps0 = new SevSeg(pins0);
  byte pins1[8] = {7,6,A5,A4,A3,8,9};
  ps1 = new SevSeg(pins1);
}

void SendRead(byte addr) {
  Serial.write(0xAA);
  Serial.write(0x02);
  Serial.write(0x04);
  Serial.write(addr);
}

void SendAutoDetect() {
  Serial.write(0xAA);
  Serial.write(0x04);
  Serial.write(0x95);
  Serial.write(0xFF);
  Serial.write(0x14);
  Serial.write(0x02);
}

void HandleSerial() {
  while (Serial.available()) {
    byte ch = Serial.read();
    //    blinking = true;
    char xd[20];
    sprintf(xd, "0x%x", ch);
    Serial.print(xd);
    if (!pkg_start && ch == 0xAA) {
      pkg_start = true;
      total_len = 0;
    } else if (pkg_start && total_len == 0) {
      total_len = ch;
      curr_len = 0;
      if (curr_len >= 100) { //wrong package length
        pkg_start = false;
      }
    } else if (pkg_start && curr_len < total_len ) {
      buf[curr_len++] = ch;
      if (curr_len == total_len) {
        package(buf, curr_len);
        pkg_start = false;
        total_len = 0;
        curr_len = 0;
      }
    } else {
      pkg_start = false;
      total_len = 0;
      curr_len = 0;
      Serial.print("destroy\n");
    }
  }
}

void SetLight(bool pr, bool pg, bool tr, bool ty, bool tg) {
  digitalWrite(P_RED, pr);
  digitalWrite(P_GREEN, pg);
  digitalWrite(T_RED, tr);
  digitalWrite(T_YELLOW, ty);
  digitalWrite(T_GREEN, tg);
}

void SetState(int state_img) {
  state = state_img;
  next_state_time = millis() + duration[state];
  if(state==3 && tapped_card){
    next_state_time += NFC_ELONGATE_GREEN_DURATION;
    tapped_card = false;
  }
}

void package(byte buf[], int len) {
  //  Serial.println("hi");
  blinking = true;
  if (buf[0] == 0x04 || len > 6) {
    //    Serial.println("xxxxxxxxxxxxxxxxxx");
  }
  if (init_auto_detect && state == 0) {
    tapped_card = true;
//    SetState(1);
  }
  init_auto_detect = true;
}

void TrafficLight() {
  switch (state) {
    //0: pedestrian red and traffic green
    case 0:
      SetLight(1, 0, 0, 0, 1);
      break;

    //1: traffic to yellow
    case 1:
      SetLight(1, 0, 0, 1, 0);
      break;

    //2: both red
    case 2:
      SetLight(1, 0, 1, 0, 0);
      break;

    //3: pedestrian normal green
    case 3:
      SetLight(0, 1, 1, 0, 0);
      ShowNum((next_state_time - millis() + duration[4])/1000);
      break;

    //4: pedestrian blink green
    case 4:
      if (millis() > green_next_blink) {
        green_flag = !green_flag;
        green_next_blink = millis() + 250;
      }
      SetLight(0, green_flag, 1, 0, 0);
      ShowNum((next_state_time - millis())/1000);
      break;

    //5: both red
    case 5:
      ShowNum(0);
      SetLight(1, 0, 1, 0, 0);
      break;

    //6: traffic yellow and red
    case 6:
      SetLight(1, 0, 1, 1, 0);
      break;

    default:
      SetLight(0, 0, 0, 0, 0);
  }

  if (millis() > next_state_time) {
    SetState((state + 1) % 7);
  }
}

void ShowNum(uint8_t num){
  ps0->ShowDigit(num%10);
  ps1->ShowDigit(num/10%10);
}

void loop() {
  HandleSerial();
  if (millis() > next_blink) {
    digitalWrite(LED_BUILTIN, flag = !flag);
    next_blink = millis() + 250;
    int num = millis()/1000%10;
    char xd[10];
    sprintf(xd,"show %d",num);
//    Serial.println(num);
//    ps0->ShowDigit(num);
//    ps1->ShowDigit(num);
  }
//  if (!init_auto_detect) {
//    SendAutoDetect();
//    Serial.println("send auto detect");
//  }
  TrafficLight();
}
