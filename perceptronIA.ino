#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define GREEN 3
#define RED   5
#define BLUE  6

// ⏱️ Configuração dos tempos (em milissegundos)
int timeInit   = 2000; // tempo da cor inicial
int timeWeight = 1500; // tempo de cada peso/bias
int timeMeta   = 4000; // tempo da cor meta

float inputs[3];
float hidden1[3];
float hidden2[3];
float output;
float w1[3][3];
float w2[3][3];
float w3[3];
float bias1[3], bias2[3], bias3;

float sigmoid(float x) {
  return 1.0 / (1.0 + exp(-x));
}

void initWeights() {
  for (int i=0; i<3; i++) {
    bias1[i] = random(-100,100)/100.0;
    bias2[i] = random(-100,100)/100.0;
    w3[i] = random(-100,100)/100.0;
  }
  bias3 = random(-100,100)/100.0;
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      w1[i][j] = random(-100,100)/100.0;
      w2[i][j] = random(-100,100)/100.0;
    }
  }
}

float runNN(float r, float g, float b) {
  inputs[0]=r; inputs[1]=g; inputs[2]=b;

  for (int i=0; i<3; i++) {
    hidden1[i] = sigmoid(inputs[0]*w1[0][i] + inputs[1]*w1[1][i] + inputs[2]*w1[2][i] + bias1[i]);
  }
  for (int i=0; i<3; i++) {
    hidden2[i] = sigmoid(hidden1[0]*w2[0][i] + hidden1[1]*w2[1][i] + hidden1[2]*w2[2][i] + bias2[i]);
  }
  output = sigmoid(hidden2[0]*w3[0] + hidden2[1]*w3[1] + hidden2[2]*w3[2] + bias3);
  return output;
}

void setColor(int r, int g, int b) {
  analogWrite(RED, r);
  analogWrite(GREEN, g);
  analogWrite(BLUE, b);
}

void showLCD(String msg1, String msg2) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(msg1);
  lcd.setCursor(0,1);
  lcd.print(msg2);
}

String colorName(int r, int g, int b) {
  if (r>200 && g<50 && b<50) return "RED";
  if (g>200 && r<50 && b<50) return "GRN";
  if (b>200 && r<50 && g<50) return "BLU";
  if (r>200 && g>200 && b<50) return "YEL";
  if (g>200 && b>200 && r<50) return "CYN";
  if (r>200 && g>200 && b>200) return "WHT";
  if (r>200 && b>150 && g<80) return "PNK"; // rosa
  if (r>150 && b>150 && g<80) return "PUR"; // roxo
  return "UNK";
}

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  setColor(0,0,0);
  initWeights();
}

void loop() {
  int r = random(0,256);
  int g = random(0,256);
  int b = random(0,256);
  setColor(r,g,b);

  float out = runNN(r/255.0,g/255.0,b/255.0);

  int metaR = (int)(out*255);
  int metaG = (int)(hidden2[1]*255);
  int metaB = (int)(hidden2[2]*255);

  // 🔄 Ciclo automático com tempos configuráveis
  showLCD("Init:"+colorName(r,g,b),"R:"+String(r)+" G:"+String(g));
  delay(timeInit);

  for (int i=0; i<3; i++) {
    showLCD("W1["+String(i)+"]:"+String(w1[0][i],1),"B1:"+String(bias1[i],1));
    delay(timeWeight);
  }
  for (int i=0; i<3; i++) {
    showLCD("W2["+String(i)+"]:"+String(w2[0][i],1),"B2:"+String(bias2[i],1));
    delay(timeWeight);
  }
  showLCD("W3:"+String(w3[0],1),"B3:"+String(bias3,1));
  delay(timeWeight);

  setColor(metaR,metaG,metaB);
  showLCD("Meta:"+colorName(metaR,metaG,metaB),"Out:"+String(out,2));
  delay(timeMeta);
}

