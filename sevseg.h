class SevSeg{
public:
  SevSeg(byte pins[]){
    memcpy(this->pins,pins,sizeof(pins)*8);
    for(int i=0; i<8; i++){
      pinMode(pins[i], OUTPUT);
      digitalWrite(pins[i],1);
    }
  }
  void SetPin(byte id, bool flag = 0){
    digitalWrite(pins[id],flag);
  }
  void ShowDigit(uint8_t digit){
//    uint8_t mask = disp[digit];
    uint8_t mask = 0;
    switch(digit){
      case 0: mask = 0b11111100; break;
      case 1: mask = 0b01100000; break;
      case 2: mask = 0b11011010; break;
      case 3: mask = 0b11110010; break;
      case 4: mask = 0b01100110; break;
      case 5: mask = 0b10110110; break;
      case 6: mask = 0b10111110; break;
      case 7: mask = 0b11100000; break;
      case 8: mask = 0b11111110; break;
      case 9: mask = 0b11110110; break;
      default: mask = 0b00000000; 
    }
    for(int i = 7; i>=0; i--){
      if(mask&1){
        digitalWrite(pins[i],LOW);
      }else{
        digitalWrite(pins[i],HIGH);
      }
      mask>>=1;
    }
  }
private:
  byte pins[8];
  const uint8_t disp[10] = {
    0b11111100,//0
    0b01100000,//1
    0b11011010,//2
    0b11110010,//3
    0b01100110,//4
    0b10110110,//5
    0b10111110,//6
    0b11100000,//7
    0b11111110,//8
    0b11110110 //9
  };
};

