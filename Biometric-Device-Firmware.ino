/*
  THIS FIRMWARE IS NOT SUBJECT TO COPYRIGHT  
  AUTHOR: AMADI COLLINS  
  EMAIL: admin@amadi.tech  
  MOBILE: +254794693472  

  ENDPOINT API: POWERED BY PHP  
  UI: JAVASCRIPT  
  DEVICE FIRMWARE: C++  

  DESCRIPTION:  
  This firmware powers a fingerprint biometric device used for tracking staff attendance in an organization.  

  FUNCTIONALITY:  
  - After booting up and connecting to a WiFi network, the device waits for user interaction.  
  - When the "Clock-In" button is pressed:  
    1. The fingerprint is scanned and compared against stored templates in the DY50-V2 fingerprint sensor.  
    2. If a match is found, the unique fingerprint ID is sent to the API endpoint:  
       `https://amadi.tech/embeddedsys/api/clock-in.php` for processing.  
    3. The fingerprint ID, along with the clock-in time (date, month, year), is stored in the SQL database for further operations.  

  - The same process occurs when the "Clock-Out" button is pressed.  
  - A built-in logic prevents users from clocking in or out more than once per day.  
*/

#include <Adafruit_Fingerprint.h>
#include <WiFi.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WebServer.h>

const int RS = 1;
const int E = 2;
const int D4 = 3;
const int D5 = 4;
const int D6 = 5;
const int D7 = 6;

HTTPClient http;
Adafruit_Fingerprint finger(&Serial2);
WebServer server(2002);
LiquidCrystal my_lcd(RS,E,D4,D5,D6,D7);
StaticJsonDocument<200> doc;

#define SSID "Amadi Smartwaves"
#define PWD "Amadi1234"

String CONFIG = "";
String KEYPAD_INPUT = "";
int LAST_REG_FINGERID = 0;

const int POWER_LED = 7;
const int STATUS_LED = 8;
const int BUZZER = 9;

const int CLK_IN_BTN = 10;
const int CLK_OUT_BTN = 11;

//my keypad config settings
  const byte ROWS = 4;
  const byte COLS = 4;
  
  const char keys[ROWS][COLS] = {{'1','2','3'},{'4','5','6'},{'7','8','9'},{'*','0','#'}};
  
  byte row_pins[ROWS] = {12,13,14};
  byte col_pins[COLS] = {15,16,17,18};
  
  Keypad keypad = Keypad(makeKeyMap(keys),row_pins,col_pins,ROWS,COLS);
  
  String AP_SSID ="Smartwaves AP";
  String AP_PWD = "12345678";


void setup(){
  
  
  //my GPIO config Setup
  pinMode(BUZZER,OUTPUT);
  pinMode(STATUS_LED,OUTPUT);
  pinMode(POWER_LED,OUTPUT);
  pinMode(CLK_IN_BTN,INPUT);
  pinMode(CLK_OUT_BTN,INPUT);
  
  Serial.begin(9600);
  Serial.println("Serial communication Init...");
  
  my_lcd.begin(16,2);
  Serial.println("LCD screen Init...");
  
  LCD_R_ONE("SYSTEM BOOTING ...");
  delay(2500);
  
  LCD_R_ONE("Powered By >>>");
  delay(1500);
  
  LCD_R_TWO("AMADI SMARTWAVES");
  delay(2000);
  
  LCD_R_ONE("WIFI Connecting...");
  
  WiFi.begin(SSID,PWD);
  
  while(WiFi.status() != WL_CONNECTED){
    
    delay(500);
    digitalWrite(STATUS_LED,1);
    delay(1000);
    digitalWrite(STATUS_LED,0);
  }
  
  LCD_R_TWO("WIFI CONNECTED");
  
  if(finger.verifyPassword() != FINGERPRINT_OK){
    
    LCD_R_ONE("Fingerprint Sensor Err");
    
    while(1){
       
       delay(500);
       analogWrite(BUZZER,128);
       delay(1000);
       analogWrite(BUZZER,0);
    }
  }
  
  home_page();
}

void loop(){
   
  my_keypad_func();
   
  if(digitalRead(CLK_IN_BTN) == HIGH){
    
    clock_in();
    
  }else if(digitalRead(CLK_OUT_BTN) == HIGH){
    
    clock_out();
  }
  
}

void LCD_R_ONE(String text){
  
  my_lcd.clear();
  my_lcd.setCursor(0,0);
  
  my_lcd.print(text);
}

void LCD_R_TWO(String text){
  
  my_lcd.clear();
  my_lcd.setCursor(0,1);
  
  my_lcd.print(text);
}

void clock_in(){
  
  LCD_R_ONE("Place finger <<");
  
  while(finger.getImage() != FINGERPRINT_OK){
  }
  
  if(finger.image2Tz() != FINGERPRINT_OK){
    
    LCD_R_ONE("ERR!_OCCURED_RETRY!");
    delay(1500);
    LCD_R_TWO(">>TEMP_CONV_ERROR");
    return;
  }
  
  if(finger.fingerFastSearch() != FINGERPRINT_OK){
    
    LCD_R_ONE("NO MATCH FOUND");
    
    analogWrite(BUZZER,127);
    delay(1000);
    
    analogWrite(BUZZER,0);
    return;
  }
  
  uint8_t user_id = finger.fingerID;
  const char* clock_in_endpoint = "http://amadi.tech/embeddedsys/api/clock-in.php";
  
  http.begin(clock_in_endpoint);
  http.addHeader("Content-type","application/x-www-form-urlencoded");
  
  String data = "user_id="+String(user_id)+"&format=formData&Auth=amadi_smartwaves";
  
 int response_code = http.POST(data);
  
  if(response_code <= 0){
     
     LCD_R_ONE("SERVER POST ERR!");
     return;
  }
  
  String response = http.getString();
  
  DeserializationError error = deserializeJson(doc,response);
  
  if(error){
    LCD_R_ONE("SERVER RES ERR!");
    return;
  }
  
 char* server_res = doc["msg"];
  
  LCD_R_TWO(server_res);
}


void clock_out(){
  
  LCD_R_ONE("Place finger <<");
  
  while(finger.getImage() != FINGERPRINT_OK){
  }
  
  if(finger.image2Tz() != FINGERPRINT_OK){
    
    LCD_R_ONE("ERR!_OCCURED_RETRY!");
    delay(1500);
    LCD_R_TWO(">>TEMP_CONV_ERROR");
    return;
  }
  
  if(finger.fingerFastSearch() != FINGERPRINT_OK){
    
    LCD_R_ONE("NO MATCH FOUND");
    
    analogWrite(BUZZER,127);
    delay(1000);
    
    analogWrite(BUZZER,0);
    return;
  }
  
  const uint8_t user_id = finger.fingerID;
  const char* clock_out_endpoint = "http://amadi.tech/embeddedsys/api/clock-out.php";
  
  http.begin(clock_out_endpoint);
  http.addHeader("Content-type","application/x-www-form-urlencoded");
  
  String data = "user_id="+String(user_id)+"&format=formData&Auth=amadi_smartwaves";
  
   int response_code = http.POST(data);
  
  if(response_code <= 0){
     
     LCD_R_ONE("SERVER POST ERR!");
     return;
  }
  
  String response = http.getString();
  
  DeserializationError error = deserializeJson(doc,response);
  
  if(error){
    LCD_R_ONE("SERVER RES ERR!");
    return;
  }
  
 char* server_res = doc["msg"];
  
  LCD_R_TWO(server_res);
}

void my_keypad_func(){
   
   char key = keypad.getKey();
   
   if(key){
     
     if(key == '*'){
       KEYPAD_INPUT = "";
     }
     
     analogWrite(BUZZER,127);
     delay(500);
     analogWrite(BUZZER,0);
     
     if(KEYPAD_INPUT.length() >= 3){
        
        if(KEYPAD_INPUT == "100"){
          register_fingerprint_func();
        }else if(KEYPAD_INPUT == "123"){
          start_ap_func();
        }else if(KEYPAD_INPUT == "200"){
          start_web_server();
        }else if(KEYPAD_INPUT == "999"){
          print_my_info();
        }else if(KEYPAD_INPUT == "911"){
          reset_esp_board();
        }else{
          LCD_R_ONE("INVALID COMMAND!");
        }
        
     }
     
     LCD_R_ONE(KEYPAD_INPUT);
   }
}

void register_fingerprint_func(){
   
   LCD_R_ONE("ADD NEW FINGER<<");
   delay(2000);
   LCD_R_TWO("Scan Finger now<");
   
   while(finger.getImage() != FINGERPRINT_OK){
     LCD_R_ONE("Scanning fingerprint..");
     delay(1500);
   }
   
   if(finger.image2Tz(1) != FINGERPRINT_OK){
     LCD_R_ONE("TEMPLATE  ERROR!");
     delay(2500);
     LCD_R_TWO("Try again");
     delay(1000);
     LCD_R_ONE("BIOMETRIC SYSTEM");
     return;
   }
   
   LCD_R_TWO("Remove finger<<");
   delay(2000);
   
   LCD_R_ONE("Place same finger");
   
   while(finger.getImage() !=  FINGERPRINT_OK){
     
     LCD_R_ONE("Scanning fingerprint...");
     delay(1000);
   }
   
   if(finger.image2Tz(2) != FINGERPRINT_OK){
     LCD_R_ONE("TEMPLATE  ERROR!");
     delay(2500);
     LCD_R_TWO("Try again");
     delay(1000);
     LCD_R_ONE("BIOMETRIC SYSTEM");
     return;
   }
   
   if(finger.getTemplateCount() != FINGERPRINT_OK){
     LCD_R_ONE("Template count ERR!");
     return;
   }
   
   if(finger.createModel() != FINGERPRINT_OK){
     
     LCD_R_ONE("Finger mismatch!");
     delay(2000);
     LCD_R_TWO("Start again!");
     return;
   }
   
   if(finger.fingerFastSearch() == FINGERPRINT_OK){
     
     LCD_R_ONE("REGISTRATION ERR");
     delay(1500);
     LCD_R_TWO(">>Finger exists!");
     return;
   }
   
   int user_id = finger.fingerID + 1;
   
   if(finger.storeModel(user_id) != FINGERPRINT_OK){
     LCD_R_ONE("STORE FINGER ERR!");
     
     return;
   }
   
   LAST_REG_FINGERID = user_id;
   
     analogWrite(BUZZER,127);
   
   LCD_R_ONE("FINGERPRINT ADDED");
   delay(2500);
   LCD_R_TWO("Finger ID >"+String(user_id));
   analogWrite(BUZZER,0);
   return;
   
}

void print_my_info(){
  
  LCD_R_ONE("POWERED BY <<");
  delay(2000);
  LCD_R_ONE("Amadi Smartwaves");
  delay(2000);
  LCD_R_TWO("Made in Kenya");
  delay(2000);
  LCD_R_ONE("Amadi Collins");
  delay(1500);
  LCD_R_TWO("Biometric v1.0.1");
  return;
}

void start_ap_func(){
  
  WiFi.softAP(AP_SSID,AP_PWD);
  my_lcd.setCursor(2,0);
  my_lcd.print("WiFi HOTSPOT ON *");
  my_lcd.setCursor(0,1);
  my_lcd.print("IP "+WiFi.softAPIP().toString());
  
  delay(4000);
  
  home_page();
  
}

void home_page(){
  
  LCD_R_ONE("BIOMETRIC SYSTEM >>");
  my_lcd.setCursor(0,1);
  my_lcd.print("Amadi Technologies");
}
