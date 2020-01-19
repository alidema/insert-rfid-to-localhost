#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <ArduinoHttpClient.h>
#include <SPI.h>
#include <Ethernet.h>
#include <pitches.h>

#define SS_PIN 53
#define RST_PIN 44
const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 7; 
const int pin_d5 = 6; 
const int pin_d6 = 5; 
const int pin_d7 = 3; 
String statement;
String a;
String state;
int greenLed = 30;
int redLed = 31;
int buzzer = 24;
int port = 80;
String response;
String getReq; 

byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
IPAddress ip(192,168,0,106); //IP address for your arduino.
char server[] = "192.168.0.105"; //IP address of your computer.

//Instancat
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);
EthernetClient ethr;
HttpClient client = HttpClient(ethr, server, port);

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void setup(){
  pinMode(buzzer, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode (pin_RS, OUTPUT);
  pinMode (pin_EN, OUTPUT);
  pinMode (pin_d4, OUTPUT);
  pinMode (pin_d5, OUTPUT);
  pinMode (pin_d6, OUTPUT);
  pinMode (pin_d7, OUTPUT);

  //inicializimet
  Serial.begin(9600); // Initialize serial communications with the PC
  SPI.begin();      // Init SPI bus
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  lcd.begin(16,2);
  Ethernet.begin(mac, ip);
  Serial.println(Ethernet.localIP());
  mfrc522.PCD_Init();

  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(24, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(24);
  }
  digitalWrite(greenLed, HIGH);
  delay(1000);
  digitalWrite(redLed, HIGH);
  delay(1000);
  digitalWrite(greenLed, LOW);
  digitalWrite(redLed, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan your Tag");
}
void loop(){
  readRFID();
  delay(3000);
}
void readRFID(){
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
      return;
    }
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;
    }
    a = mfrc522.GET_ID(&(mfrc522.uid));
    lcd.setCursor(5, 0);
    lcd.clear();
    lcd.print(a);
    getReq = "";
    getRequest();
    if(statement == "True"){
          ethr.print("GET /temaDiplomes/insertAttendance.php?"); //GET request to write data to the database.
          ethr.print("request=");
          ethr.print(a);
          ethr.read();
          Serial.println(a);
          ethr.println(" HTTP/1.1"); 
          ethr.println("Host: 192.168.0.105"); 
          ethr.println("Connection: close"); 
          ethr.println(); 
          ethr.println(); 
          ethr.stop();
          lcd.clear();
          lcd.setCursor(5, 0);
          lcd.clear();
          lcd.print("Welcome");
          digitalWrite(greenLed, HIGH);
          tone(24, 311, 100); 
          delay(2000);
          digitalWrite(greenLed, LOW);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Scan your Tag");
    }else{
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("UID tag not founded");
        digitalWrite(redLed, HIGH);
        tone(30, 100, 100); 
        delay(2000);
        digitalWrite(redLed, LOW);
        lcd.clear();
        lcd.print("Scan your Tag");
    }
}
void getRequest() {
  //Serial.println("Making GET Request");
  getReq.concat("/temaDiplomes/getRfid.php?tag=");
  getReq.concat(a);
  client.get(getReq);

  // read the status code and body of the response
  //statusCode = client.responseStatusCode();
  response = client.responseBody();
  statement = response;
}
