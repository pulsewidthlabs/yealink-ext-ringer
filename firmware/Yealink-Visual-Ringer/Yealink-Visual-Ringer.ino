uint8_t  dLen = 0;
uint8_t dLen_last = 0;

uint32_t data = 0;
bool newData = false;
bool isPoll = false;

byte clk = true;
byte lastVal_SS = false;
byte mySS = false;


void setup() {
 cli();
 
 //EIMSK = (1 << INT0);             // Dissable INT0 Interupt - Pin 3  
 EICRA =  (1 << ISC01); // Register INT0 Interupt for Falling Edge

 digitalWrite(4,LOW);
 pinMode(4,OUTPUT);//
 pinMode(3,INPUT_PULLUP); // CLK INT0
 pinMode(2,INPUT_PULLUP); // Data
 pinMode(1,INPUT_PULLUP); // SS
 Serial.begin(115200);
 sei();
}



ISR(INT0_vect)
{
  data |= digitalRead(2) << dLen++;
  
  if (isPoll) {
    if (dLen == 12) {
      digitalWrite(4,HIGH);
    }
    else if (dLen == 13) {
      digitalWrite(4,LOW);
    }
    else if (dLen == 14) {
      digitalWrite(4,HIGH);
    }
  }
  
}

void loop() {
  mySS = digitalRead(1);
  
  if (!mySS && lastVal_SS) 
  {
    // Zero Data
    data = 0; 
    // Zero data length
    dLen = 0;
    dLen_last = 0;
    // SS Line pulled low - Master About to send Data 
    EIMSK = (1 << INT0); // Enable INT0 Interupt - Pin 3
  } 
  else if (mySS && !lastVal_SS)
  {
    isPoll = false;
    digitalWrite(4,LOW);
    EIMSK = 0; // Data Transmission Stoped - Dissable INT0 Interupt
    
    Serial.print("Data: ");
    Serial.println(data,BIN);
    Serial.print("Data Len: ");
    Serial.println(dLen);
  }

  if (dLen != dLen_last) 
  {
    //New Byte
    dLen_last = dLen;
    if (dLen == 10 && !(data ^ 0b1100001101)) 
    {
      isPoll = true;
      Serial.println("POLL!");
    }
  }

  lastVal_SS = mySS;
  
}
