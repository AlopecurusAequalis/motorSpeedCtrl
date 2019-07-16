#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

float pGain = 0.6;
float iGain = 0;
float integratedError = 0;

void setup(){
  lcd.begin(16, 2);
  lcd.noCursor();
  lcd.print("Check disp");
  Serial.begin(9600);
}
#define btnRIGHT  0   // RIGHT ボタン
#define btnUP     1   // UP ボタン
#define btnDOWN   2   // DOWN ボタン
#define btnLEFT   3   // LEFT ボタン
#define btnSELECT 4   // SELECT ボタン
#define btnNONE   5   // 該当なし

int read_LCD_buttons(){
  int adc_key_in = analogRead(0);            // キー入力アナログ値の取得
  if (adc_key_in > 1000) return btnNONE; // 入力値の範囲をチェック
   // V1.0 の時のレベルチェック
  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 195)  return btnUP;
  if (adc_key_in < 380)  return btnDOWN;
  if (adc_key_in < 555)  return btnLEFT;
  if (adc_key_in < 790)  return btnSELECT;
  return btnNONE;  // 範囲外の時
}

void loop(){
  int reference = 300;
  int nowOut = analogRead(1);
  int error = reference - nowOut;
  // anti wind-up
  if (abs(integratedError) > 30000){
    integratedError = 30000*integratedError/abs(integratedError);
  }
  integratedError = integratedError + error*0.5;
  int input = error*pGain + iGain*integratedError;
  if (abs(input) > 255){
    input = (int) input*input/abs(input);
  }
  analogWrite(3, input);
  int lcd_key;
  if (1){
    lcd_key = read_LCD_buttons();
    if (lcd_key == btnUP){
      pGain = pGain + 0.01;
    }else if (lcd_key == btnDOWN){
      pGain = pGain - 0.01;
    }else if (lcd_key == btnLEFT){
      iGain = 0;
    }else if (lcd_key == btnRIGHT){
      iGain = 0.1;
      integratedError = 0;
    }

    lcd.setCursor(0, 0);
    lcd.print("e="); lcd.print(error); lcd.print(",");
    lcd.print("u="); lcd.print(input); lcd.print(",");
    lcd.setCursor(0, 1);
    lcd.print("pG="); lcd.print(pGain); lcd.print(",");
    lcd.print(integratedError);

    Serial.print(300);
    Serial.print(",");
    Serial.print(nowOut);
    Serial.print(",");
    // 積分が始まったとわかりやすくするためのグラフを表示するため
    Serial.println(iGain * 1000);
  }
  delay(50);
}
