#include <Arduino.h>
#include <BleKeyboard.h>
#include <EEPROM.h>
int buttonr = 0;         // current state of the button
int lastbuttonr = 0;     // previous state of the button
int buttonl = 0;         // current state of the button
int lastbuttonl = 0;     // previous state of the button
int buttonm = 0;
int lastbuttonm = 0;
int interval=120000;
int modes = 0;
int pic = 0;
#define BUTTON_PIN_BITMASK 0x2002000
#define Buttonr 13
#define Buttonl 25
#define Buttonm 23
uint8_t leftsetting[] ={0xD8,0xD7};
uint8_t rightsetting[] ={0xD7,0xD8};
BleKeyboard bleKeyboard("Manga buttoner","Magical tools",100);
unsigned long previousMillis=0;
void buttonmodes(int which,bool Leftright);
void setup() {
  EEPROM.begin(24);
  setCpuFrequencyMhz(80);
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH);
  pinMode(Buttonl,INPUT_PULLUP);
  pinMode(Buttonr,INPUT_PULLUP);
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
      //esp_deep_sleep_start();
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
