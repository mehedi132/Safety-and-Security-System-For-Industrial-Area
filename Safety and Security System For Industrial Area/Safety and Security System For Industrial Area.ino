#include <Servo.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
SoftwareSerial mySerial(9, 10);

byte tx = 1;

Servo door;

const int TriggerPin = 40;
const int EchoPin = 38;
int LEDU = 24;
int LEDV = 30;
int LEDF = 32;
int LEDG = 34;

long Duration;
int Distance;

const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {44,45,46,47};
byte colPins[COLS] = {51,52,53};
const int exFan = 14;
const int buzzer = 6;
int doorPos = 90;

Keypad keypad = Keypad( makeKeymap (keys), rowPins, colPins, ROWS, COLS);

int gas = 0; //set initial tempC 0 for all MQ 3
int smk = 0; //set initial tempC 0 for all MQ 2
int a=0;

const int SensorPin1 = A8; //fire input sensor pin
const int SensorPin2 = A9;
const int VIBRATION = A13;
String textForSMS;


void setup(){
  
  door.attach(7);
  door.write(doorPos);
  
  pinMode(exFan, OUTPUT);

  lcd.begin(16, 2);
  delay(100);
  pinMode(tx, OUTPUT);
  
  pinMode(buzzer, OUTPUT);
  pinMode(SensorPin1, INPUT);
  pinMode(SensorPin2, INPUT);
  pinMode(VIBRATION, INPUT);

  pinMode(LEDU, OUTPUT);
  pinMode(LEDV, OUTPUT);
  pinMode(LEDF, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);

  mySerial.begin(9600);
  Serial.begin(9600); //Start the serial connection with the computer
}

int cnt=0;
int lcdInit = 1;
int mode = 0;
char pass[4]={'*','1','4','9'};
char input[4];
char star[4]="";


void loop(){
  
  if(lcdInit){
    lcd.setCursor(0,0);
    lcd.clear();
    lcd.print("Loading..");
    delay(100);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("CSE-3216 Project");
    delay(200);
    lcd.setCursor(0,1);
    lcd.print("Safety and Security");
    delay(200);
    lcd.setCursor(0,3);
    lcd.print("System for Industry");
    delay(200);
    lcdInit = 0;
    lcd.clear();
  }
  
  if(mode==0){    
    lcd.setCursor(0,0);
    //lcd.clear();
    lcd.print("Enter Password: ");    

    char c = keypad.getKey();
    Serial.print(c);
    
    if(c){
      star[cnt]='*';
      lcd.setCursor(0,1);
      lcd.print(star);
      input[cnt]=c;
      cnt++;

      if(cnt == 4){
        if(matchPassword()){
          
          lcd.setCursor(0,0);
          lcd.clear();
          lcd.print("Welcome!!");
          
          for(int i=2; i<90; i+=2){
            door.write(doorPos+i);
            delay(15);
          }
                    
          mode = 1;  
                  
        }else{
          
          printWrongPasswordMessage();
          buzzerAlert();
          printTryAgainMessage();
          cnt = 0;
          clearInputAndStar();
          lcd.setCursor(0,1);
          lcd.clear();
          
        }
        Serial.println();
      }      
    }
   }
   else{
    
    int gas = analogRead(SensorPin2);
    int Smk = analogRead(SensorPin1);
    gas = analogRead(SensorPin2); //read the value from the LM35 sensor
    gas = (5.0 * gas * 100.0) / 1024.0; //convert the analog data to temperature
    smk = analogRead(SensorPin1); //read the value from the MQ 2 sensor
    smk = (5.0 * smk * 100.0) / 1024.0; //convert the analog data to temperature
    int vib = analogRead(VIBRATION);
    vib = (5.0 * vib * 100.0) / 1024.0;
  
    delay(50);
   

    if (gas >= 120 ){
      gasfound();   
    }
    else{
      // digitalWrite(exFan, LOW);
      a=0;
      lcd.clear();
      digitalWrite(LEDG, LOW);
      lcd.setCursor(0, 0);
    
      lcd.print("  NO GAS   ");
      delay(100);
      digitalWrite(exFan, LOW);
    }
  
    if( smk >= 120){
      smokefound();
    }
    else{
      a=0;
      lcd.clear();
      digitalWrite(LEDF, LOW);
      lcd.setCursor(0, 0);    
      
      lcd.print("  NO FIRE   ");
      delay(100);
    }

    if (vib >= 120 ){
      vibincrease();
    }
    else{
      vibnormal();
    }
  
    digitalWrite(TriggerPin, LOW);      // Clear Trigger Pin
    delay(2);
    
    digitalWrite(TriggerPin, HIGH);
    delay(10);                          // Set Trigger Pin on HIGH state for 10 micro seconds  
    
    digitalWrite(TriggerPin, LOW);
    
    Duration = pulseIn(EchoPin, HIGH);  // Reads the Echo Pin, returns the sound wave travel time in microseconds
    
    Distance = Duration*0.034/2;
  
    Serial.print("Distance: ");
    Serial.println(Distance);
  
    if(Distance < 500){    
      digitalWrite(LEDU, HIGH);  //LED ON
          
      lcd.clear();  
      lcd.setCursor(0, 0);
      lcd.print("Danger Area!(");
      lcd.print(Distance/100);
      lcd.print("M)");
      delay(500);
      lcd.setCursor(0, 1);
      lcd.print("Step Away!");   
      delay(500);   
      lcd.clear();  
       
      //LED OFF 
      delay(100);
    } 
    else{
      lcd.clear();  
      lcd.setCursor(0, 0);      
      digitalWrite(LEDU, LOW); 
      lcd.print("You're in safe");
       delay(300); 
      lcd.setCursor(0, 1);
      lcd.print("distance.(");
      lcd.print(Distance/100);
      lcd.print("M)");
    }  
  }
}



bool matchPassword(){
  for(int i=0;i<4;i++){
    if(input[i] != pass[i]){
      return false;
    }
  }
  return true;
}

void printWrongPasswordMessage(){
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print("Wrong Password!!");          
}


void printTryAgainMessage(){
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print("Try Again.");
  delay(1000);
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print("Enter Password: ");
}


void clearInputAndStar(){
  for(int i=0;i<4;i++){
    star[i]='\0';
    input[i]='\0';
  }
}


void buzzerAlert(){
  for(int i=0;i<3;i++){
    tone(buzzer,500); 
    delay(200);        
    noTone(buzzer);    
    delay(200);
  }
}


void gasfound(){
  a=1;
  digitalWrite(LEDG, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("THERE IS GAS LECKAGE");
  delay(100);
  lcd.setCursor(0, 2);
  lcd.print(" NOT SAFE HERE ");
  delay(100);
  digitalWrite(exFan, HIGH);
  buzzerAlert();
}


void smokefound(){  
  a=1;
  digitalWrite(LEDF, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  THERE IS FIRE ");
  delay(100);
  buzzerAlert();
  lcd.setCursor(0, 1);
  lcd.print(" NOT SAFE HERE ");
  delay(100);
  lcd.clear();
  lcd.print("Sending SMS...");
  delay(100);
 
  Serial.print("FIRE ALERT! Please Be Informed that Fire has Occured!\r");

  delay(200);   
  Serial.println();
}


void vibincrease(){
  a=1;
  digitalWrite(LEDV, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("VIBRATION INCREASE");
  delay(100);
  lcd.setCursor(0, 1);
  lcd.print(" NOT SAFE HERE ");
  delay(100);  
  buzzerAlert();
}


void vibnormal(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("VIBRATION IS NORMAL");
  delay(100);
  digitalWrite(LEDV, LOW);
}
  
  
