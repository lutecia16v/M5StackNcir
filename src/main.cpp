#include <Arduino.h>
#include <M5Stack.h>

#include <Adafruit_MLX90614.h>

#define NOTE_Cd1 277
#define NOTE_D1 294
#define NOTE_E1 330
#define NOTE_F1 349
#define NOTE_G1 392
#define NOTE_DH2 661

int music[] = {
  NOTE_E1, NOTE_E1, NOTE_E1,
  NOTE_E1, NOTE_E1, NOTE_E1,
  NOTE_E1, NOTE_G1, NOTE_Cd1, NOTE_D1, NOTE_E1,
  NOTE_F1, NOTE_F1, NOTE_F1, NOTE_F1,
  NOTE_F1, NOTE_E1, NOTE_E1,
  NOTE_E1, NOTE_D1, NOTE_D1, NOTE_E1, NOTE_D1,
  NOTE_G1
};

#define MEASUREMENT_INTERVAL  1000
#define BUTTON_INTERVAL 200
#define LINE_MARGIN  50

#define MODE_DEACTIVE 0
#define MODE_SETTING 1
#define MODE_ACTIVE 2

double ambTemp = 0.0;
double objTemp = 0.0;
double targetTemp = 0.0;

unsigned long cur_t;
unsigned long btn_t;

int mode;

bool bFlag = false;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {
  m5.begin();
  Serial.begin(9600);

  m5.Lcd.setTextSize(4);

  mlx.begin();  

  cur_t = millis();
  btn_t = millis();

  mode = MODE_DEACTIVE;
}

void getData(){
  ambTemp = mlx.readAmbientTempC();
  objTemp = mlx.readObjectTempC();
}

void displayData(){
  size_t pos = 10;
  m5.Lcd.clearDisplay();
  m5.Lcd.setCursor(20,pos);
  m5.Lcd.println("[A]: " + String(ambTemp));
  pos += LINE_MARGIN;

  m5.Lcd.setCursor(20,pos);
  m5.Lcd.println("[O]: " + String(objTemp));
  pos += LINE_MARGIN;

  m5.Lcd.setCursor(20,pos);
  switch(mode){
    case MODE_ACTIVE:
      m5.Lcd.println("[T]: " + String(targetTemp));
      break;

    case MODE_SETTING:
      m5.Lcd.println(">>>: " + String(targetTemp));
      break;

    case MODE_DEACTIVE:
      m5.Lcd.println("---: " + String(targetTemp));
      break;
  }
}

void updateTarget(){
  size_t pos = 10 + LINE_MARGIN * 2;
  m5.Lcd.setCursor(20, pos);
  m5.Lcd.println(">>>: " + String(targetTemp));
}

void loop() {
  m5.update();
  if(m5.BtnA.isPressed()){
    if(bFlag == false){
      bFlag = true;
      switch(mode){
        case MODE_DEACTIVE:
          targetTemp = objTemp;
          updateTarget();
          mode = MODE_SETTING;
          break;

        case MODE_SETTING:
          mode = MODE_ACTIVE;
          break;

        case MODE_ACTIVE:
          mode = MODE_DEACTIVE;
          break;
      }
    }
  }
  if(m5.BtnA.isReleased()){
    if(bFlag == true){
      bFlag = false;
    }
  }

  if(millis() - btn_t >= BUTTON_INTERVAL){ // Doesn't take into account for overflow.
    btn_t = millis();
    if(mode == MODE_SETTING){
      if(m5.BtnB.isPressed()){
        targetTemp += 1.0;
        updateTarget();
      }

      if(m5.BtnC.isPressed()){
        targetTemp -= 1.0;
        updateTarget();
      }
    }
  }

  if(millis() - cur_t >= MEASUREMENT_INTERVAL){ // Doesn't take into account for overflow.
    cur_t = millis();
    getData();
    displayData();

    if(mode == MODE_ACTIVE){
      if(objTemp <= targetTemp ){
        M5.Speaker.beep();
      }
    }
  }
}

