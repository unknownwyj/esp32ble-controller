#include <Arduino.h>
#include <BleKeyboard.h>
#include <EEPROM.h>
int buttonr = 0;         // current state of the button
int lastbuttonr = 1;     // previous state of the button
int buttonl = 0;         // current state of the button
int lastbuttonl = 1;     // previous state of the button
int buttonm = 0;
int lastbuttonm = 1;
int interval=120000;
RTC_DATA_ATTR int modes = 0;
int pic = 0;

#define BUTTON_PIN_BITMASK 0x402000000
#define Buttonr 34
#define Buttonl 25
#define Buttonm 23
uint8_t leftsetting[] ={0xD8,0xD7};
uint8_t rightsetting[] ={0xD7,0xD8};

BleKeyboard bleKeyboard("Manga buttoner","Magical tools",100);
unsigned long previousMillis=0;
void buttonmodes(int which,bool Leftright);
uint8_t convertinttokey(int keyselect);
String convertinttoString(int keyselect);
void printhelp();
const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data
boolean newData = false;
void recvWithEndMarker();
void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
    
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}
void setup() {
  Serial.begin(230400);
  EEPROM.begin(24);
  setCpuFrequencyMhz(240);
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH);
  pinMode(Buttonl,INPUT);
  pinMode(Buttonr,INPUT);
  pinMode(Buttonm,INPUT_PULLUP);
  /*EEPROM.write(0,KEY_LEFT_ARROW);
  EEPROM.write(1,KEY_RIGHT_ARROW);
  EEPROM.write(2,KEY_RIGHT_ARROW);
  EEPROM.write(3,KEY_LEFT_ARROW);
  EEPROM.commit(); */
  leftsetting[0]=EEPROM.read(0);
  leftsetting[1]=EEPROM.read(1);
  rightsetting[0]=EEPROM.read(2);
  rightsetting[1]=EEPROM.read(3);
  //Serial.begin(9600);
  bleKeyboard.begin();
}

void loop() {
  unsigned long currentMillis = millis();
  if ((unsigned long)(currentMillis - previousMillis) >= interval) {
      // It's time to do something!
      previousMillis = currentMillis;
      Serial.println("Entering Deep Sleep");
      esp_deep_sleep_start();
   }
  buttonl = digitalRead(Buttonl);
  buttonr = digitalRead(Buttonr);
  buttonm = digitalRead(Buttonm);
  if (buttonl != lastbuttonl){
    if(buttonl == HIGH){
      Serial.println("pressed l");
      previousMillis = currentMillis;
      if(bleKeyboard.isConnected()){
        buttonmodes(modes,0);
      }
    }
  }
  if (buttonr != lastbuttonr){
    if(buttonr == HIGH){
      Serial.println("pressed r");
      previousMillis = currentMillis;
      if(bleKeyboard.isConnected()){
        buttonmodes(modes,1);
      }
    }
  }
  if(buttonm != lastbuttonm){
    if(buttonm == HIGH){
      Serial.println("pressed m");
      if (modes > 2){
        modes = 0;
      }
      else {
        modes = modes + 1;
      }
      Serial.print("modes = ");
      Serial.println(modes);
    }
  }
  if (modes == 3){
    recvWithEndMarker();
    if (newData == true) {
        if (receivedChars[0]=='h'&& receivedChars[1]=='e'&& receivedChars[2]=='l' && receivedChars[3]=='p'){
          printhelp();
          newData = false;
        }
        if (receivedChars[0]=='s'&& receivedChars[1]=='h'&& receivedChars[2]=='o' && receivedChars[3]=='w'){
          
          newData = false;
        }
        int leftrightdet = 0;
        int whichkey = 0;
        if (receivedChars[0] == '0'){
          leftrightdet = 0;
        }
        if (receivedChars[0] == '1'){
          leftrightdet = 2;
        }
        if (receivedChars[1] == '1'){
          leftrightdet += 1;
        }
        char in[] = {receivedChars[2],receivedChars[3]};// macam can
        whichkey = atoi(in);
        //valid check ?
        bool valid = true;
        if (receivedChars[0] == '0' || receivedChars[0] == '1'){
        }
        else{
          valid = false;
        }
        if (receivedChars[1] == '0' || receivedChars[1] == '1'){
        }
        else{
          valid = false;
        }
        if (leftrightdet < 0 || leftrightdet >3){
          valid = false;
        }
        if (whichkey < 0 || whichkey > 34){
          valid = false;
        }
        if (valid){
        EEPROM.write(leftrightdet,convertinttokey(whichkey));
        EEPROM.commit();
        if (leftrightdet == 0){
          Serial.print("Left button");
          Serial.print(" Preset 1");
        }
        if (leftrightdet == 1){
          Serial.print("Left button");
          Serial.print(" Preset 2");
        }
        if (leftrightdet == 2){
          Serial.print("Right button");
          Serial.print(" Preset 1");
        }
        if (leftrightdet == 3){
          Serial.print("Right button");
          Serial.print(" Preset 2");
        }
        Serial.print(" Key Changed to ");
        Serial.println(convertinttoString(whichkey));
        leftsetting[0]=EEPROM.read(0); //left preset 1 = 0
        leftsetting[1]=EEPROM.read(1); //left preset 2 = 1
        rightsetting[0]=EEPROM.read(2);//Right preset 1 = 2
        rightsetting[1]=EEPROM.read(3);//Left preset 2 = 3
        //printf("%s button, preset %s Changed to key %s",buttonname.toCharArray(),presetname.toCharArray(),convertinttoString(whichkey).toCharArray());
        }
        else if(!valid){
          Serial.println("Chars Received Invalid Type help for more info");
          Serial.println(receivedChars);
        }
        newData = false;
    }
  }
  delay(60);
  lastbuttonr = buttonr;
  lastbuttonl = buttonl;
  lastbuttonm = buttonm;
}
void buttonmodes(int which,bool Leftright){
  if(Leftright == 0){
    switch (which){
      case 0:
      bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
      break;
      case 1:
      bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
      break;
      case 2:
      bleKeyboard.write(leftsetting[0]);
      break;
      case 3:
      bleKeyboard.write(leftsetting[1]);
      break;
    }
  }
  else if(Leftright == 1) 
  {
    switch(which){
      case 0:
      bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
      break;
      case 1:
      bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
      break;
      case 2:
      bleKeyboard.write(rightsetting[0]);
      break;
      case 3:
      bleKeyboard.write(rightsetting[1]);
      break;
    }
  }
}
void printhelp(){
  Serial.println("Here is the help");
  Serial.println("First Char is to determine left right button");
  Serial.println("0 = left button");
  Serial.println("1 = right button");
  Serial.println("Second Char is to determine Which Preset");
  Serial.println("0 = Preset 1");
  Serial.println("1 = Preset 2");
  Serial.println("Third and forth Char is to determine Which Key To Change");
  Serial.println("00 = LEFT_CTRL");
  Serial.println("01 = LEFT_SHIFT");
  Serial.println("02 = LEFT_ALT");
  Serial.println("03 = LEFT_GUI");
  Serial.println("04 = RIGHT_CTRL");
  Serial.println("05 = RIGHT_SHIFT");
  Serial.println("06 = RIGHT_ALT");
  Serial.println("07 = RIGHT_GUI");
  Serial.println("08 = UP_ARROW");
  Serial.println("09 = DOWN_ARROW");
  Serial.println("10 = LEFT_ARROW");
  Serial.println("11 = RIGHT_ARROW");
  Serial.println("12 = BACKSPACE");
  Serial.println("13 = TAB");
  Serial.println("14 = RETURN");
  Serial.println("15 = ESC");
  Serial.println("16 = INSERT");
  Serial.println("17 = DELETE");
  Serial.println("18 = PAGE_UP");
  Serial.println("19 = PAGE_DOWN");
  Serial.println("20 = HOME");
  Serial.println("21 = END");
  Serial.println("22 = CAPS_LOCK");
  Serial.println("23 = F1");
  Serial.println("24 = F2");
  Serial.println("25 = F3");
  Serial.println("26 = F4");
  Serial.println("27 = F5");
  Serial.println("28 = F6");
  Serial.println("29 = F7");
  Serial.println("30 = F8");
  Serial.println("31 = F9");
  Serial.println("32 = F10");
  Serial.println("33 = F11");
  Serial.println("34 = F12");
  Serial.println("End Of help");
}
uint8_t convertinttokey(int keyselect){
  switch (keyselect){
    case 0:
    return KEY_LEFT_CTRL;
    case 1:
    return KEY_LEFT_SHIFT;
    case 2:
    return KEY_LEFT_ALT;
    case 3:
    return KEY_LEFT_GUI;
    case 4:
    return KEY_RIGHT_CTRL;
    case 5:
    return KEY_RIGHT_SHIFT;
    case 6:
    return KEY_RIGHT_ALT;
    case 7:
    return KEY_RIGHT_GUI;
    case 8:
    return KEY_UP_ARROW;
    case 9:
    return KEY_DOWN_ARROW;
    case 10:
    return KEY_LEFT_ARROW;
    case 11:
    return KEY_RIGHT_ARROW;
    case 12:
    return KEY_BACKSPACE;
    case 13:
    return KEY_TAB;
    case 14:
    return KEY_RETURN;
    case 15:
    return KEY_ESC;
    case 16:
    return KEY_INSERT;
    case 17:
    return KEY_DELETE;
    case 18:
    return KEY_PAGE_UP;
    case 19:
    return KEY_PAGE_DOWN;
    case 20:
    return KEY_HOME;
    case 21:
    return KEY_END;
    case 22:
    return KEY_CAPS_LOCK;
    case 23:
    return KEY_F1;
    case 24:
    return KEY_F2;
    case 25:
    return KEY_F3;
    case 26:
    return KEY_F4;
    case 27:
    return KEY_F5;
    case 28:
    return KEY_F6;
    case 29:
    return KEY_F7;
    case 30:
    return KEY_F8;
    case 31:
    return KEY_F9;
    case 32:
    return KEY_F10;
    case 33:
    return KEY_F11;
    case 34:
    return KEY_F12;
    default:
    return KEY_LEFT_ARROW;
  }
}
String convertinttoString(int keyselect){
  switch (keyselect){
    case 0:
    return "KEY_LEFT_CTRL";
    case 1:
    return "KEY_LEFT_SHIFT";
    case 2:
    return "KEY_LEFT_ALT";
    case 3:
    return "KEY_LEFT_GUI";
    case 4:
    return "KEY_RIGHT_CTRL";
    case 5:
    return "KEY_RIGHT_SHIFT";
    case 6:
    return "KEY_RIGHT_ALT";
    case 7:
    return "KEY_RIGHT_GUI";
    case 8:
    return "KEY_UP_ARROW";
    case 9:
    return "KEY_DOWN_ARROW";
    case 10:
    return "KEY_LEFT_ARROW";
    case 11:
    return "KEY_RIGHT_ARROW";
    case 12:
    return "KEY_BACKSPACE";
    case 13:
    return "KEY_TAB";
    case 14:
    return "KEY_RETURN";
    case 15:
    return "KEY_ESC";
    case 16:
    return "KEY_INSERT";
    case 17:
    return "KEY_DELETE";
    case 18:
    return "KEY_PAGE_UP";
    case 19:
    return "KEY_PAGE_DOWN";
    case 20:
    return "KEY_HOME";
    case 21:
    return "KEY_END";
    case 22:
    return "KEY_CAPS_LOCK";
    case 23:
    return "KEY_F1";
    case 24:
    return "KEY_F2";
    case 25:
    return "KEY_F3";
    case 26:
    return "KEY_F4";
    case 27:
    return "KEY_F5";
    case 28:
    return "KEY_F6";
    case 29:
    return "KEY_F7";
    case 30:
    return "KEY_F8";
    case 31:
    return "KEY_F9";
    case 32:
    return "KEY_F10";
    case 33:
    return "KEY_F11";
    case 34:
    return "KEY_F12";
    default:
    return "ERROR";
  }
}

/*
0 = left
1 = right
0010 = left setting0 key10 ?  
void selecting(){
  switch (pic){
  case 1:
  Serial.println("Button setting");
  Serial.println(">Left Button");
  Serial.println(" Right Button");
  Serial.println(" Save Settings");
  Serial.println("");
  break;
  case 2:
  Serial.println("Button setting");
  Serial.println(" Left Button");
  Serial.println(">Right Button");
  Serial.println(" Save Settings");
  Serial.println("");
  break;
  case 3:
  Serial.println("Button setting");
  Serial.println(" Left Button");
  Serial.println(" Right Button");
  Serial.println(">Save Settings");
  Serial.println("");
  break;
  case 10:
  Serial.println("Which Preset");
  Serial.println(">Preset 1");
  Serial.println(" Preset 2");
  Serial.println(" Back");
  Serial.println("");
  break;
  case 11:
  Serial.println("Which Preset");
  Serial.println(" Preset 1");
  Serial.println(">Preset 2");
  Serial.println(" Back");
  Serial.println("");
  break;
  case 12:
  Serial.println("Which Preset");
  Serial.println(" Preset 1");
  Serial.println(" Preset 2");
  Serial.println(">Back");
  Serial.println("");
  break;
  case 20:
  Serial.println("Which Preset");
  Serial.println(">Preset 1");
  Serial.println(" Preset 2");
  Serial.println(" Back");
  Serial.println("");
  break;
  case 21:
  Serial.println("Which Preset");
  Serial.println(" Preset 1");
  Serial.println(">Preset 2");
  Serial.println(" Back");
  Serial.println("");
  break;
  case 22:
  Serial.println("Which Preset");
  Serial.println(" Preset 1");
  Serial.println(" Preset 2");
  Serial.println(">Back");
  Serial.println("");
  break;
  case 100:
  Serial.println(" UP ARROW");
  Serial.println(" DOWN ARROW");
  Serial.println(" LEFT ARROW");
  Serial.println(" RIGHT ARROW");
  Serial.println(" BACKSPACE");
  Serial.println(" TAB");
  Serial.println(" RETURN");
  Serial.println(" ESC");
  Serial.println(" INSERT");
  Serial.println(" DELETE");
  Serial.println(" PAGE_UP");
  Serial.println(" PAGE_DOWN");
  Serial.println(" HOME");
  Serial.println(" END");
  Serial.println(" CAPS LOCK");
  Serial.println(" LEFT CTRL");
  Serial.println(" LEFT SHIFT");
  Serial.println(" LEFT_ALT");
  Serial.println(" LEFT_GUI");
  Serial.println(" RIGHT_CTRL");
  Serial.println(" RIGHT_SHIFT");
  Serial.println(" RIGHT_ALT");
  Serial.println(" RIGHT_GUI");
  Serial.println(" KEY F");
  }
}
*/