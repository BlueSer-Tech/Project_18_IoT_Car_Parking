/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6czed06qr"
#define BLYNK_TEMPLATE_NAME "CarParking"
#define BLYNK_AUTH_TOKEN "f7CS0nNpE4wB-dAVLU7FcB3aK9A4PdXr"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#define SS_PIN 4  // Pin SS/RST của MFRC522 kết nối với chân 5 trên ESP8266
#define SS_2_PIN 13
#define RST_PIN1 15
#define RST_PIN2 5
#define ir_enter1 34
#define ir_enter2 35
#define ir_exit1 17
#define ir_exit2 16
#define ir_car1 32
#define ir_car2 33
#define ir_car3 25
#define ir_car4 26
#define ir_car5 27
LiquidCrystal_I2C lcd(0x27, 20, 4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
WiFiUDP u;
NTPClient n(u, "1.asia.pool.ntp.org", 7 * 3600);
MFRC522 mfrc522(SS_PIN, RST_PIN1);  // Tạo đối tượng MFRC522
MFRC522 mfrc522_2(SS_2_PIN, RST_PIN2);
Servo servoEnter;                  // Đối tượng servo cho cửa vào
Servo servoExit;                   // Đối tượng servo cho cửa ra
String ss = "";
String ss1 = "";
String s1 = "";
String s2 = "";
String s3 = "";

String s4 = "";
String s5 = "";
String h;
String m;
String s;
String data_now = "";
String data_last = "";
unsigned long thoigian_vao;
unsigned long thoigian_ra;
unsigned long a[10];

int S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0;
int temp = 0;
int flag1 = 0, flag2 = 0;
int slot = 5;

BlynkTimer timer;

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void sendSensor()
{
    if(digitalRead(ir_car1)==0)
    {
        Blynk.virtualWrite(V1, 1);
    }
    else
    {
        Blynk.virtualWrite(V1, 0);
    }

    if(digitalRead(ir_car2)==0)
    {
        Blynk.virtualWrite(V2, 1);
    }
    else
    {
        Blynk.virtualWrite(V2, 0);
    }
    if(digitalRead(ir_car3)==0)
    {
        Blynk.virtualWrite(V3, 1);
    }
    else
    {
        Blynk.virtualWrite(V3, 0);
    }

    if(digitalRead(ir_car4)==0)
    {
        Blynk.virtualWrite(V4, 1);
    }
    else
    {
        Blynk.virtualWrite(V4 , 0);
    }

        if(digitalRead(ir_car5)==0)
    {
        Blynk.virtualWrite(V5, 1);
    }
    else
    {
        Blynk.virtualWrite(V5 , 0);
    }

}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  
  WiFi.disconnect();

  WiFi.begin("Mastermind", "2001zone");
  Serial.println("");
  Serial.println("Connecting");
  while ((!(WiFi.status() == WL_CONNECTED))) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Dia chi IP: ");
  Serial.println(WiFi.localIP());
  n.begin();

  SPI.begin();         // Khởi tạo giao tiếp SPI
  mfrc522.PCD_Init();  // Khởi tạo MFRC522
  mfrc522_2.PCD_Init();
  servoEnter.attach(12);
  servoExit.attach(14);
  // servoEnter.write(0);
  // servoExit.write(0);
  // initialize digital pins as input.
  pinMode(ir_car1, INPUT);
  pinMode(ir_car2, INPUT);
  pinMode(ir_car3, INPUT);
  pinMode(ir_car4, INPUT);
  pinMode(ir_car5, INPUT);

  pinMode(ir_enter1, INPUT);
  pinMode(ir_exit1, INPUT);
  pinMode(ir_enter2, INPUT);
  pinMode(ir_exit2, INPUT);

  lcd.setCursor(0, 0);
  lcd.print("   Welcome to   ");
  lcd.setCursor(0, 1);
  lcd.print("    Smart Car   ");
  lcd.setCursor(0, 2);
  lcd.print(" Parking System ");
  delay(2000);
  lcd.clear();

  Blynk.begin(BLYNK_AUTH_TOKEN, "Mastermind", "2001zone");
  timer.setInterval(1000L, sendSensor);

  int total = temp + S1 + S2 + S3 + S4 + S5;
  slot = slot - total ;
}

void loop() {
  n.update();
  h = (String)n.getHours();
  m = (String)n.getMinutes();
  s = (String)n.getSeconds();
  Blynk.run();
  timer.run();
  
  Read_Sensor();

  
  if (slot > 0 && slot < 6) {
    Serial.println("Have slot");
    Serial.println(slot);
    lcd.setCursor(0, 0);
    lcd.print("     Have slot : ");
    lcd.setCursor(17, 0);
    lcd.print(slot);
    lcd.print("  ");
  } else{
    lcd.setCursor(0, 0);
    lcd.print("     Full slot");
    Serial.println("full slot");
  }

  if (S1 == 1) {
    Serial.println("S1:Fill  ");
    lcd.setCursor(0, 1);
    lcd.print("S1:Fill    ");
  } else {
    Serial.println("S1:Empty");
    lcd.setCursor(0, 1);
    lcd.print("S1:Empty   ");
  }
  if (S2 == 1) {
    Serial.println("S2:Fill  ");
    lcd.setCursor(0, 2);
    lcd.print("S2:Fill    ");
  } else {
    Serial.println("S2:Empty");
    lcd.setCursor(0, 2);
    lcd.print("S2:Empty   ");
  }
  if (S3 == 1) {
    Serial.println("S3:Fill ");
    lcd.setCursor(0, 3);
    lcd.print("S3:Fill ");
  } else {
    Serial.println("S3:Empty");
    lcd.setCursor(0, 3);
    lcd.print("S3:Empty  ");
  }
  if (S4 == 1) {
    Serial.println("S4:Fill ");
    lcd.setCursor(11, 1);
    lcd.print("S4:Fill ");
  } else {
    Serial.println("S4:Empty");
    lcd.setCursor(11, 1);
    lcd.print("S4:Empty");
  }
  if (S5 == 1) {
    Serial.println("S5:Fill ");
    lcd.setCursor(11, 2);
    lcd.print("S5:Fill ");
  } else {
    Serial.println("S5:Empty");
    lcd.setCursor(11, 2);
    lcd.print("S5:Empty");
  }
  gate();
  // RFID();
  // RFID2();
  // if (!mfrc522.PICC_IsNewCardPresent())
  //   return;

  // // Select one of the cards
  // if (!mfrc522.PICC_ReadCardSerial())
  //   return;
  // String assign = "";
  // byte letter;
  // for (int i = 0; i < mfrc522.uid.size; i++) {
  //   Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
  //   Serial.print(mfrc522.uid.uidByte[i], HEX);
  //   assign.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
  //   assign.concat(String(mfrc522.uid.uidByte[i], HEX));
  // }
  // Serial.println();
  // assign.toUpperCase();
  // ss = assign.substring(1);
  // //Serial.println(ss);
  // Serial.println();
  if (isIdScanned()==0) {
    data_now = h + ":" + m + ":" + s;
    Serial.print("Thoi gian vao: ");
    Serial.println(data_now);
    thoigian_vao = millis();
    Serial.print("ID mới quét: ");
    if (s1 == "") {
      s1 = ss;
      a[0] = thoigian_vao;
      //Serial.println(thoigian_vao);
      Serial.println(s1);
    } else if (s2 == "") {
      s2 = ss;
      a[1] = thoigian_vao;
      Serial.println(s2);
    } else if (s3 == "") {
      s3 = ss;
      a[2] = thoigian_vao;
      Serial.println(s3);
    } else if (s4 == "") {
      s4 = ss;
      a[3] = thoigian_vao;
      Serial.println(s4);
    } else if (s5 == "") {
      s5 = ss;
      a[4] = thoigian_vao;
      Serial.println(s5);
    }
    ss="";
    slot--;
    servoEnter.write(0);
  } 
  else if(isIdScanned()==1){
    // Hiển thị thông báo ID đã quét trước đó
    data_last = h + ":" + m + ":" + s;
    Serial.print("Thoi gian ra: ");
    Serial.println(data_last);
    Serial.print("ID đã được quét trước đó: ");
    
    Serial.println(ss1);
    if (s1 == ss1 && ss1 !="") {
      Serial.println(s1);
      s1 = "";
      thoigian_ra = millis() - a[0];
      a[0] = 0;
    } else if (s2 == ss1 && ss1 != "") {
      Serial.println(s2);
      s2 = "";
      thoigian_ra = millis() - a[1];
      a[1] = 0;
    } else if (s3 == ss1 && ss1 != "") {
      Serial.println(s3);
      s3 = "";
      thoigian_ra = millis() - a[2];
      a[2] = 0;
    } else if (s4 == ss1 && ss1 != "") {
      Serial.println(s4);
      s4 = "";
      thoigian_ra = millis() - a[3];
      a[3] = 0;
    } else if (s5 == ss1 && ss1 != "") {
      Serial.println(s5);
      s5 = "";
      thoigian_ra = millis() - a[4];
      a[4] = 0;
    }
    // if(ss=="") slot++;
    ss1="";
    slot++;
    Serial.println(thoigian_ra);
    int tien = sum();
    Serial.println(tien);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Your Parking Fee : ");
    lcd.setCursor(6,1);
    lcd.print(tien);
    delay(3000);
    servoExit.write(0);

  }
  else {Serial.println("");}
  // int tien = sum();
  // Serial.println(tien);
  delay(1000);
}
void RFID(){
  
  if (!mfrc522.PICC_IsNewCardPresent())
    return;

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
    return;
  String assign = "";
  byte letter;
  for (int i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    assign.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    assign.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  assign.toUpperCase();
  ss = assign.substring(1);
  //Serial.println(ss);
  Serial.println();
  flag1 = 1;
  // Serial.println(flag1);
}
void RFID2(){
  if (!mfrc522_2.PICC_IsNewCardPresent())
    return;

  // Select one of the cards
  if (!mfrc522_2.PICC_ReadCardSerial())
    return;
  String assign = "";
  byte letter;
  for (int i = 0; i < mfrc522_2.uid.size; i++) {
    Serial.print(mfrc522_2.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522_2.uid.uidByte[i], HEX);
    assign.concat(String(mfrc522_2.uid.uidByte[i] < 0x10 ? " 0" : " "));
    assign.concat(String(mfrc522_2.uid.uidByte[i], HEX));
  }
  Serial.println();
  assign.toUpperCase();
  ss1 = assign.substring(1);
  //Serial.println(ss);
  Serial.println();
  flag2=1;
  // Serial.println(flag2);
}
void Read_Sensor() {
  S1 = 0; S2 = 0; S3 = 0; S4 = 0; S5 = 0;
  if (digitalRead(ir_car1) == 0) {
    S1 = 1;
  }
  if (digitalRead(ir_car2) == 0) {
    S2 = 1;
  }
  if (digitalRead(ir_car3) == 0) {
    S3 = 1;
  }
  if (digitalRead(ir_car4) == 0) {
    S4 = 1;
  }
  if (digitalRead(ir_car5) == 0) {
    S5 = 1;
  }
}
void gate() {
  // Serial.println(flag1);
  // Serial.println(flag2);
  if (digitalRead(ir_enter1) == 0 && flag1 == 0) {
    if (slot > 0 && slot < 6) {
      
      
      if (flag2 == 0) {
        //servoEnter.write(0);
        
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Car arrive at ");
        lcd.setCursor(0, 1);
        lcd.print("the entane gate");
        lcd.setCursor(0, 2);
        lcd.print("Open the gate");
        delay(2000);
        Serial.println("car arrive at the entrance gate");
        Serial.println("open the door");
        RFID();
        
      }
    }
  }

  if (digitalRead(ir_exit1) == 0 && flag2 == 0) {
    
    
    if (flag1 == 0) {
      //servoExit.write(0);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Car arrive at ");
      lcd.setCursor(0, 1);
      lcd.print("the exit gate");
      lcd.setCursor(0, 2);
      lcd.print("Open the gate");
      delay(2000);
      Serial.println("car arrive at the exit gate");
      Serial.println("open the door");
      RFID2();
    }
  }

  if (digitalRead(ir_enter2) == 0 && flag1 == 1) {
    flag1 = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Car has entered");
    lcd.setCursor(0, 1);
    lcd.print("close the gate");
    delay(2000);
    Serial.println("Car has entered the gate");
    Serial.println("close the door");
    servoEnter.write(90);
  }

  if (digitalRead(ir_exit2) == 0 && flag2 == 1) {
    flag2 = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Car exited the gate");
    lcd.setCursor(0, 1);
    lcd.print("close the gate");
    delay(2000);
    Serial.println("Car exited the gate");
    Serial.println("close the door");
    servoExit.write(90);
  }
}

int isIdScanned() {
  if (ss !="" || ss1 != ""){
    Serial.println("ss: ");
    Serial.println(ss);
    Serial.println("s1: ");
    Serial.println(s1);
    if(ss == s1 || ss == s2 || ss == s3 || ss == s4 || ss == s5 || slot == 0){
      Serial.println("1");
      return 1;
    }  // ID đã tồn tại trong mảng
    else if(ss1 == s1 || ss1 == s2 || ss1 == s3 || ss1 == s4 || ss1 == s5) {
      Serial.println("0");
      return 0;
    } 
  // return false;  // ID chưa tồn tại trong mảng
  }
  return 2;
}

void openDoorEnter() {
  // Mở cửa vào
  servoEnter.write(90);
  delay(2000);
  // Đóng cửa vào
  servoEnter.write(0);
  delay(1000);
}

void openDoorExit() {
  // Mở cửa ra
  servoExit.write(90);
  delay(2000);
  // Đóng cửa ra
  servoExit.write(0);
  delay(1000);
}

int sum(){
  Serial.println(thoigian_ra);
  if(thoigian_ra < 7200000){
    return 5000;
  }
  else if(thoigian_ra >= 7200000 && thoigian_ra < 10800000){
    return 10000;
  }
  else return 20000;

}