//#include <EEPROM.h>

unsigned char pin[12];

unsigned char PD_4 = 4;
unsigned char PB_1 = 9;

unsigned char PB_0 = 8;
unsigned char PD_7 = 7;

unsigned char PC_4 = 18;
unsigned char PC_5 = 19;

unsigned char PD_6 = 6;
unsigned char PD_5 = 5;

unsigned char PC_0 = 14;
unsigned char PC_1 = 15;

unsigned char PC_2 = 16;
unsigned char PC_3 = 17;

unsigned char PC_7 = 2;

void setPinMode(unsigned char *inputPin,int inNum,unsigned char *outputPin,int outNum)
{
  int i;
  for(i = 0; i < inNum; i++)
  {
    pinMode(inputPin[i],INPUT);
  }
  for(i = 0; i < outNum; i++)
  {
    pinMode(outputPin[i],OUTPUT);
  }
}

void digitalWritePin(unsigned char *pin,int pinNum,int level)
{
    int i;
    for(i = 0; i< pinNum; i++)
    {
      digitalWrite(pin[i],level);
    }
}

void digitalReadPin(unsigned char *pin,int pinNum, unsigned char *readValue)
{
  int i;
  for(i = 0; i < pinNum; i++)
  {
      readValue[i] = digitalRead(pin[i]);
  }
}

int TestIO()
{
  unsigned char IODat[6] = {   0,0,0,0,0,0  };
  int ADDat = 0;
  int errorFlag = 0;
  int i;
  
  pin[0] = PD_4;
  pin[1] = PB_1;
  pin[2] = PB_0;
  pin[3] = PD_7;
  pin[4] = PC_7;
  pin[5] = PC_0;

  pin[6] = PC_4;
  pin[7] = PC_5;
  pin[8] = PD_6;
  pin[9] = PD_5;
  pin[10] = PC_3;
  pin[11] = PC_2;

  //Serial.begin(9600);
  pinMode(PC_1,OUTPUT);
  
  //test pin[0-5] in, and pin[6-11] out
  setPinMode(pin,6,pin+6,6);  
  digitalWritePin(pin+6,6,HIGH);
  digitalReadPin(pin,6,IODat);
  for(i = 0; i < 6; i++)
  {
      if(0 == IODat[i])
      {
         errorFlag = 1;
      }
  }
  /*
  if(1 == IODat[0]){    Serial.println("\r\nPD4,PB1 in out OK!");  }
  else{    Serial.println("PD4,PB1 in out ERROR!");  errorFlag = 1;}

  if(1 == IODat[1]){    Serial.println("PB0,PD7 in out OK!");  }
  else{    Serial.println("PB0,PD7 in out ERROR!");  errorFlag = 1;}

  if(1 == IODat[2]){    Serial.println("PC4,PC5 in out OK!");  }
  else{    Serial.println("PC4,PC5 in out ERROR!");  errorFlag = 1;}

  if(1 == IODat[3]){    Serial.println("PD6,PD5 in out OK!");  }
  else{    Serial.println("PD6,PD5 in out ERROR!");  errorFlag = 1;}

  if(1 == IODat[4]){    Serial.println("PC0,PC1 in out OK!");  }
  else{    Serial.println("PC0,PC1 in out ERROR!");  errorFlag = 1;}

  if(1 == IODat[5]){    Serial.println("PC2,PC3 in out OK!");  }
  else{    Serial.println("PC2,PC3 in out ERROR!");  errorFlag = 1;}
  */
  //test pin[0-5] out, and pin[6-11] in, and ADC7 read
  setPinMode(pin+6,6,pin,6);
  digitalWritePin(pin,6,HIGH);
  digitalReadPin(pin+6,6,IODat);
  
  //ADDat = analogRead(ADC_7);
/*
  if(1 == IODat[0]){    Serial.println("\r\nPD4,PB1 out in OK!");  }
  else{    Serial.println("PD4,PB1 out in ERROR!");  errorFlag = 1;}

  if(1 == IODat[1]){    Serial.println("PB0,PD7 out in OK!");  }
  else{    Serial.println("PB0,PD7 out in ERROR!");  errorFlag = 1;}

  if(1 == IODat[2]){    Serial.println("PC4,PC5 out in OK!");  }
  else{    Serial.println("PC4,PC5 out in ERROR!");  errorFlag = 1;}

  if(1 == IODat[3]){    Serial.println("PD6,PD5 out in OK!");  }
  else{    Serial.println("PD6,PD5 out in ERROR!");  errorFlag = 1;}

  if(1 == IODat[4]){    Serial.println("PC0,PC1 out in OK!");  }
  else{    Serial.println("PC0,PC1 out in ERROR!");  errorFlag = 1;}

  if(1 == IODat[5]){    Serial.println("PC2,PC3 out in OK!");  }
  else{    Serial.println("PC2,PC3 out in ERROR!");  errorFlag = 1;}

  if(ADDat > 500)  {    Serial.println("ADC7 out in OK!");    }
  else  {      Serial.println("ADC7 out in ERROR!");   errorFlag = 1;}
  */
  for(i = 0; i < 6; i++)
  {
      if(0 == IODat[i])
      {
         errorFlag = 1;
      }
  }
  if(ADDat < 500)
  {
      errorFlag = 1;
  }
  if(1 == errorFlag){
    for(i = 0; i < 5; i++)
    digitalWrite(PC_1,LOW);
    delay(1000);
    digitalWrite(PC_1,HIGH);
  }
  else{
    digitalWrite(PC_1,LOW);
  }
  return errorFlag;
}


