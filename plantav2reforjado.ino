#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
 
const byte rowsCount = 4;
const byte columsCount = 4;

int ALV = 2;
int MOTOMAMI = 3;    // Input3 conectada al pin 1 -> bomba
int SENSUALON = A3;

int DEBUG = 13;

int ideal = 0;
int sensual;
int margen = 20;
int margenSuperior;
int margenInferior;

bool bandera = 1;
bool flagMenu = 1; // Bandera
bool flagErase = 1;

char key;
char numPad[3];
char subChar[3];
int longitud = 0;

const byte rowPins[rowsCount] = { 11, 10, 9, 8 };
const byte columnPins[columsCount] = { 7, 6, 5, 4 };

char keys[rowsCount][columsCount] = {
   { '1','2','3', 'A' },
   { '4','5','6', 'B' },
   { '7','8','9', 'C' },
   { '*','0','#', 'D' }
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, columnPins, rowsCount, columsCount);
LiquidCrystal_I2C lcd(0x27,16,2); 

void menu1(){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor: ");
    lcd.print(sensual);
    lcd.setCursor(0, 1);
    lcd.print("Valor ideal: ");
    lcd.print(ideal);
    delay(200);
}

void menu2(){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ingrese");
    lcd.setCursor(0, 1);
    lcd.print("valor: ");
    for(int i = 0; i < longitud; i++)
      lcd.print(numPad[i]);
    delay(200);
}

void menu3(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("No puedes agregar mas caracteres");
  delay(2000);

}

void setup() {

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  ideal = analogRead(SENSUALON);

  pinMode(MOTOMAMI, OUTPUT);
  pinMode(ALV, OUTPUT);

  pinMode(DEBUG, OUTPUT);

}

void loop() {

  key = keypad.getKey();
  sensual = analogRead(SENSUALON);

  Serial.println(sensual);

  if (key) {
    if(key == '#'){
      flagMenu = !flagMenu;
    }
  }
  if(!flagMenu){
    if(flagErase){
      for(int i = longitud; i < 3; i++){
        numPad[i] = 0;
      }
      flagErase = 0;
    }
    ideal = atoi(numPad);
    longitud = 0;
    menu1();
  }
  else{
    flagErase = 1;
    if(key >= 48 && key <= 57){
      if(longitud+1 > 3){
        menu3();
      }
      else{
        numPad[longitud] = key;
        longitud++;
      }
    }
    if(key == '*'){
      numPad[longitud-1] = 0;
      longitud--;
    }
      menu2();
  }

  margenSuperior = ideal + margen;
  margenInferior = ideal - margen;

  if(bandera){
    if(sensual > ideal){
      Serial.println(1);
      digitalWrite(MOTOMAMI, LOW);
      digitalWrite(ALV, HIGH);
    }
    else if(sensual < ideal){
      Serial.println("2");
      digitalWrite(MOTOMAMI, HIGH);
      digitalWrite(ALV, LOW);
    }
    else bandera = 0;
  }
  else{
    if(sensual < margenSuperior && sensual > margenInferior){
      Serial.println("3");
      digitalWrite(MOTOMAMI, LOW);
      digitalWrite(ALV, LOW);
    }
    else if(sensual > margenSuperior){
      Serial.println("4");
      digitalWrite(MOTOMAMI, LOW);
      digitalWrite(ALV, HIGH);
      bandera = 1;
    }
    else if(sensual < margenInferior){
      Serial.println("5");
      digitalWrite(MOTOMAMI, HIGH);
      digitalWrite(ALV, LOW);
      bandera = 1;
    }
  }

  //digitalWrite(MOTOMAMI, HIGH);
  //digitalWrite(ALV, HIGH);
  //digitalWrite(DEBUG, HIGH);

  //delay(2000);

  //digitalWrite(MOTOMAMI, LOW);
  //digitalWrite(ALV, LOW);
  //digitalWrite(DEBUG, LOW);
  //delay(2000);'
  delay(50);

}
