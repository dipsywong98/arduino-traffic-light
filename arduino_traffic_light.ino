bool state = 0;

bool pkg_start = false;
int curr_len = 0;
int total_len = 0;
byte buf[100];
bool blinking = false;
bool flag = false;
int next_blink = 0;
bool init_auto_detect = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(0,INPUT);
  pinMode(1,OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
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

void loop() {  
  //if(Serial.available())digitalWrite(LED_BUILTIN,HIGH);
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
  if(millis()>next_blink && blinking){
    digitalWrite(LED_BUILTIN,flag = !flag);
    next_blink = millis()+250;
  }
  if(!init_auto_detect)SendAutoDetect();
//  SendRead(4);
}
