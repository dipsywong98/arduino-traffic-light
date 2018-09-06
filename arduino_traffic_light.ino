int state = 0;   //0: R|G, 1: R|Y, 2:R|R, 3: G|R, 4: GB|R, 5: R|R, 6: R|RY
uint32_t duration[] = {10000,1000,500,5000,5000,500,1000};
uint32_t next_state_time = 0;

//for blinking
bool green_flag = 0;
uint32_t green_next_blink = 0;

#define P_RED 7
#define P_GREEN 8
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

void setup() {
  // put your setup code here, to run once:
  pinMode(0,INPUT);
  pinMode(1,OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(P_RED, OUTPUT);
  pinMode(P_GREEN, OUTPUT);
  pinMode(T_RED,OUTPUT);
  pinMode(T_YELLOW,OUTPUT);
  pinMode(T_GREEN,OUTPUT);
  Serial.begin(9600);
}

void package(byte buf[], int len){
  Serial.println("hi");
  blinking = true;
  if(buf[0]==0x04||len>6){
    Serial.println("xxxxxxxxxxxxxxxxxx");
  }
  init_auto_detect=true;
}

void SendRead(byte addr){
  Serial.write(0xAA);
  Serial.write(0x02);
  Serial.write(0x04);
  Serial.write(addr);
}

void SendAutoDetect(){
  Serial.write(0xAA);
  Serial.write(0x04);
  Serial.write(0x95);
  Serial.write(0xFF);
  Serial.write(0x14);
  Serial.write(0x02);
}

void HandleSerial(){
  while(Serial.available()){
    byte ch = Serial.read();
//    blinking = true;
    char xd[20];
    sprintf(xd,"0x%x",ch);
    Serial.print(xd);
    if (!pkg_start && ch == 0xAA) {
      pkg_start = true;
      total_len = 0;
    } else if (pkg_start && total_len == 0) {
      total_len = ch;
      curr_len = 0;
      if(curr_len >= 100){ //wrong package length
        pkg_start = false;
      }
    } else if (pkg_start && curr_len < total_len ) {
      buf[curr_len++] = ch;
      if(curr_len == total_len){
        package(buf,curr_len);
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

void SetLight(bool pr, bool pg, bool tr, bool ty, bool tg){
    digitalWrite(P_RED,pr);
    digitalWrite(P_GREEN,pg);
    digitalWrite(T_RED,tr);
    digitalWrite(T_YELLOW,ty);
    digitalWrite(T_GREEN,tg);
}

void TrafficLight(){
  switch(state){
    case 0:
    SetLight(1,0,0,0,1);
    break;
    
    case 1:
    SetLight(1,0,0,1,0);
    break;
    
    case 2:
    SetLight(1,0,1,0,0);
    break;
    
    case 3:
    SetLight(0,1,1,0,0);
    break;
    
    case 4:
    if(millis()>green_next_blink){
      green_flag = !green_flag;
      green_next_blink = millis()+250;
    }
    SetLight(0,green_flag,1,0,0);
    break;
    
    case 5:
    SetLight(1,0,1,0,0);
    break;
    
    case 6:
    SetLight(1,0,1,1,0);
    break;
    
    default:
    SetLight(0,0,0,0,0);
  }

  if(millis()>next_state_time){
    state++;
    state%=7;
    next_state_time = millis()+duration[state];
    Serial.println(state);
  }
}

void loop() {  
  //if(Serial.available())digitalWrite(LED_BUILTIN,HIGH);
//  HandleSerial();
  if(millis()>next_blink){
    digitalWrite(LED_BUILTIN,flag = !flag);
    next_blink = millis()+250;
    Serial.println(next_state_time);
  }
//  if(!init_auto_detect)SendAutoDetect();
//  SendRead(4);
  TrafficLight();
}
