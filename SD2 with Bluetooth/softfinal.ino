#include <EEPROM.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>


int state_bt=1;
int relPin;
int stare=0;
byte  COD[10];
byte  AUX[10];
int k=0;
String codacces="*123456#";
String codpairing="*654321#";
//nfc
#define RST_PIN   9     // Configurable, see typical pin layout above
#define SS_PIN    10    // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
#define NEW_UID {0xDE, 0xAD, 0xBE, 0xEF}
MFRC522::MIFARE_Key key;
//lcd
LiquidCrystal_I2C  lcd(0x27,16,2);

//TASTATURA
const byte numRows= 4; //number of rows on the keypad
const byte numCols= 4; //number of columns on the keypad

//keymap defines the key pressed according to the row and columns just as appears on the keypad
char keymap[numRows][numCols]= 
{
{'1', '2', '3', 'A'}, 
{'4', '5', '6', 'B'}, 
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};

//Code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = {2,3,4,5}; //Rows 0 to 3
byte colPins[numCols]= {A0,7,8,9}; //Columns 0 to 3

//initializes an instance of the Keypad class
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);


void setup() {
   pinMode(A0,OUTPUT);
   digitalWrite(A0,HIGH);
   pinMode(A3,OUTPUT);
   digitalWrite(A3,HIGH);
   pinMode(A1,OUTPUT);
   digitalWrite(A1,HIGH);
   pinMode(A2,OUTPUT);
   digitalWrite(A2,LOW);
   pinMode(6,OUTPUT);
   digitalWrite(6,HIGH);
   //nfc
  Serial.begin(9600);  // Initialize serial communications with the PC
  Serial.println("What did you say?:");
  while (!Serial);     // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card
 
    for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

    lcd.init();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.clear();
    lcd.print("BLOCKED");

}

void  citireNFC(){
  for (byte i =0; i<(mfrc522.uid.size); i++) {
    COD[i]=mfrc522.uid.uidByte[i];
  } 
  Serial.print("COD");
  Serial.print(COD[0]);
  Serial.print(COD[1]);
  Serial.print(COD[2]);
  Serial.print(COD[3]);
 
}

void pairNFC(){
  Serial.println("COD in pair");
  Serial.print(COD[0]);
  Serial.print(COD[1]);
  Serial.print(COD[2]);
  Serial.print(COD[3]);
   long  r=0;
   int c=0;
  for(int i=1;i<=EEPROM.read(0);i++){
    switch(i%4){
          case 1 :{AUX[0]=EEPROM.read(i); break;}
          case 2 :{AUX[1]=EEPROM.read(i); break;}
          case 3 :{AUX[2]=EEPROM.read(i); break;}
          case 0 :{AUX[3]=EEPROM.read(i); break;}
    }
    if((i)%4==0)
       {Serial.println(r);
        if( AUX[0]==COD[0] && AUX[1]==COD[1] && AUX[2]==COD[2] && AUX[3]==COD[3] ){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("CODE ALREADY IN");
          lcd.setCursor(0,1);
          lcd.print("SYSTEM");
          delay(2000);
           c=1;
          break;}    
      }
      }
  if(c==0){int ttt=EEPROM.read(0);
              Serial.println("CODE PAIRED");
              Serial.print(COD[0]);
              Serial.print(COD[1]);
              Serial.print(COD[2]);
              Serial.print(COD[3]);

          EEPROM.write(ttt+1,COD[0]);
          EEPROM.write(ttt+2,COD[1]);
          EEPROM.write(ttt+3,COD[2]);
          EEPROM.write(ttt+4,COD[3]);

          ttt=ttt+4;
          Serial.println("ttt");
          Serial.println(ttt);
          EEPROM.write(0,0); 
          EEPROM.write(0,ttt);    
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("CODE PAIRED");
          delay(2000);}   
  }
  

boolean validareNFC(){
   boolean c=false;
      for(int i=1;i<=EEPROM.read(0);i++){
          switch(i%4){
          case 1 :{AUX[0]=EEPROM.read(i); break;}
          case 2 :{AUX[1]=EEPROM.read(i); break;}
          case 3 :{AUX[2]=EEPROM.read(i); break;}
          case 0 :{AUX[3]=EEPROM.read(i); break;}
        }
    if((i)%4==0)
       {
        if( AUX[0]==COD[0] && AUX[1]==COD[1] && AUX[2]==COD[2] && AUX[3]==COD[3])
          c=true;
         }}       
   return c;      
}

int comparareCOD(String a)
 { 
  if(a.equals(codacces))
     return 1;
  else if(a.equals(codpairing)) return 2;
  else return 0;   
 }


String iaCOD(char x)
{ char vec[10];
  vec[0]=x;
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print('X');
  for(int i=1;i<8;i++)
     {vec[i]=myKeypad.waitForKey();
      lcd.print('X');}
  vec[8]=NULL;
  String str(vec);
  return str;
}   



void loop() {
  
  //Start BT autentification
   if(Serial.available())
  {
  char c=Serial.read();
  switch (state_bt) {
  
  case 1:
  if(c=='*') state_bt=2;
  else state_bt=1;
  break;
  
  case 2:
  if(c=='1') state_bt=3;
  else state_bt=1;
  break;
  
  case 3:
  if(c=='2') state_bt=4;
  else state_bt=1;
  break;
  
  case 4:
  if(c=='3') state_bt=5;
  else state_bt=1;
  break;
  
  case 5:
  if(c=='4') state_bt=6;
  else state_bt=1;
  break;
  
  case 6:
  if(c=='5') state_bt=7;
  else state_bt=1;
  break;
  
  case 7:
  if(c=='6') state_bt=8;
  else state_bt=1;
  break;
  
  case 8:
  if(c=='#') state_bt=9;
  else state_bt=1;
  break;
  
  case 9:

  lcd.init();   
  lcd.backlight();
  lcd.print("OPEN");
  digitalWrite(6,LOW);
  delay(5000);
  digitalWrite(6,HIGH);
  lcd.init();   
  lcd.backlight();
  lcd.print("BLOCKED");
  state_bt=1;
  break;
  
  default:
  
  break;
  }
  }
  
  
  //////////END BT Autentification
        
      switch(stare){
      case 0: {
        mfrc522.PCD_Init();
        if (  mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial() ){
        
        citireNFC();
        if(validareNFC())  
           {stare=1;
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("VALID NFC CODE");
            delay(1000);
            return;    
           }
        else{
             lcd.clear();
             lcd.setCursor(0,0);
             lcd.print("INVALID NFC CODE");
             delay(1000);
             lcd.setCursor(0,0);
             lcd.clear();
             lcd.print("BLOCKED");
             return;
            }
        }
      char c=myKeypad.getKey();
      if(c != NO_KEY){
        
      String codcurent=iaCOD(c);
      int A=comparareCOD(codcurent);
      if(A==0)
      {lcd.clear();
       lcd.print("INVALID CODE");
       delay(2000);
        lcd.setCursor(0,0);
        lcd.clear();
        lcd.print("BLOCKED");
       return;}
      if(A==1)
      {lcd.setCursor(0,0);
       lcd.clear();
       lcd.print("VALID CODE");
       delay(2000);
       stare = 1;
       return;}
      if(A==2);
       {stare=2;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Pairing...");
          delay(2000);
        return;}
      } break;
       }
      case 1:{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("UNLOCKED");
        digitalWrite(A3,LOW);
        digitalWrite(A1,LOW);
        digitalWrite(A2,HIGH);
        //tone(6,3000,5010);
        digitalWrite(6,LOW);
        delay(5000);
        digitalWrite(6,HIGH);
        digitalWrite(A3,HIGH);
        digitalWrite(A1,HIGH);
        digitalWrite(A2,LOW);
        stare=0;
        lcd.setCursor(0,0);
        lcd.clear();
        lcd.print("BLOCKED");
       
       
      
        return;
        }
        case 2:{
          
          mfrc522.PCD_Init();
          if (  mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial() ){
          citireNFC();
          pairNFC();
          stare=0;
          delay(2000);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("BLOCKED");   }
                    
       
        
          
         
          break;
          
        }
         
       }

}
