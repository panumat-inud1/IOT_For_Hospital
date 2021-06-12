#include <ESP8266WiFi.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2 //กำหนดขาที่จะเชื่อมต่อ Sensor D4

#include <BlynkSimpleEsp8266.h> //useradmin : hospitalkhelang@gmail.com password : Hp123456
BlynkTimer timer;
#define BLYNK_PRINT Serial

#include <TridentTD_LineNotify.h> //LINE

#define LINE_TOKEN  "3YKSC1NibTMWTHvrybFeoO8vVhNoEnSVa9QFBNh0kaH"

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int lampPin = 12;  //D6 หลอดไฟ
int buttonPin = 13; //D7 พัดลม
int switchPin = 14; //D5 สวิตซ์ปุ่มกด หลอดไฟ
int switchPin1 = 15; //D8 สวิตซ์ปุ่มกด พัดลม

//const int lampPin = 12;  //D6

const char* ssid = "KLNR-STAFF"; //connetWifi username : WIFI KLNR-STAFF password :   
const char* password = "";
char auth[] = "XxZzzIeAE0p5KfmcO7_Bs2MKE7Nl67VF";// XxZzzIeAE0p5KfmcO7_Bs2MKE7Nl67VF   Name : KLNR Hosp , Q5paVZNb5rK393Ot_vmO0M7ICsmPP8mi  Name : Hosp_Test

int lamp;
int button;
int lampButton = 0;
int lampButton1 = 0;
int led;
int fan;
float Temperture, SliderMax, SliderMin;


void setup() {
  pinMode(switchPin, INPUT);
  pinMode(switchPin1, INPUT);

  pinMode(lampPin, OUTPUT);
  pinMode(buttonPin, OUTPUT);

  digitalWrite(lampPin, HIGH);
  digitalWrite(buttonPin, HIGH);

  digitalWrite(switchPin, LOW);
  digitalWrite(switchPin1, LOW);

  Serial.begin(115200);
  Serial.println(LINE.getVersion());

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  sensors.begin();
  Blynk.config(auth);
  timer.setInterval(100000L, sendSensor);
   timer.setInterval(1000L, senMinMaxTemp);
  LINE.setToken(LINE_TOKEN); 
}

unsigned long last_time = 0;  // swithc button
unsigned long last_time1 = 0;  // Blynk
unsigned long last_time2 = 0;  // Blynk

void loop() {

  Blynk.run();
  timer.run();

  lampButton = digitalRead(switchPin); // MICROSW
  lampButton1 = digitalRead(switchPin1); // MICROSW


  if (lamp == 1 || lampButton == HIGH  ) {
    if ( millis() - last_time > 300) {
      last_time = millis();
      if (led == 0) {
        led = 1;
        digitalWrite(lampPin, LOW);
        Blynk.virtualWrite(V2, 255);// LED
      } else {
        led = 0;
        digitalWrite(lampPin, HIGH);
        Blynk.virtualWrite(V2, 0);
      }
    }
  }

  if (button == 1 || lampButton1 == HIGH) {
    if ( millis() - last_time > 250) {
      last_time = millis();
      if (fan == 0) {
        fan = 1;
        digitalWrite(buttonPin, LOW);
        Blynk.virtualWrite(V5, 255);// LED
      } else {
        fan = 0;
        digitalWrite(buttonPin, HIGH);
        Blynk.virtualWrite(V5, 0);
      }
    }
  }


//  Serial.print(" หลอดไฟ : ");
//  Serial.print(lamp);
//  Serial.print(" พัดลม : ");
//  Serial.print(button);
//  Serial.print(" led_lamp : ");
//  Serial.print(led);
//  Serial.print("  Led_ fan : ");
//  Serial.print(fan);
//  Serial.print("  สวิตซ์หลอดไฟ : ");
//  Serial.print(lampButton);
//  Serial.print("  สวิตซ์พัดลม : ");
//  Serial.print(lampButton1);
//  Serial.print("  SliderMax : ");
//  Serial.print(SliderMax);
//  Serial.print("  SliderMin : ");
//  Serial.println(SliderMin);

//      Serial.print(" Temperture : ");
//      Serial.println(Temperture);

}

//-------------------END LOOP----------------------


BLYNK_WRITE(V0)
{
  lamp = param.asInt();
}

BLYNK_WRITE(V4)
{
  button = param.asInt();
}
BLYNK_WRITE(V6)
{
  SliderMin = param.asFloat();
}
BLYNK_WRITE(V7)
{
  SliderMax = param.asFloat();
}

void sendSensor()
{ readTp();
  
  if (Temperture >= SliderMax) {

    LINE.notify("อุณหภูมิตอนนี้ :" +  String(Temperture,1) + "c เกินกว่าค่าที่กำหนด !!"); 

  }
  if (Temperture < SliderMin) {
    LINE.notify("อุณหภูมิตอนนี้ :" + String(Temperture,1) + "c ตำกว่าค่าที่กำหนด !!");
    
  }
  
  Blynk.virtualWrite(V3, Temperture); //อุณหภูมิ
}

void senMinMaxTemp() {
  Blynk.virtualWrite(V8, String(SliderMin,1) + '-' + String(SliderMax,1));
}

float readTp() {

  sensors.requestTemperatures();
  Temperture = sensors.getTempCByIndex(0);
  return Temperture;
}
