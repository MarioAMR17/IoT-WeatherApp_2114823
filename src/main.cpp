#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include "secrets.h"

FirebaseData fbdo;
FirebaseData firebaseData;
FirebaseJson updateSensorValue;
FirebaseJson json;

int gpioVal=0;
int adcVal=0;
 
String pathLedControl = "/ESP32IoTApp/Led_Control/estadoLC";
String pathDAC="/ESP32IoTApp/DAC";
String pathLedRGB="/ESP32IoTApp/LedRGB/color";
 
String estadoLC = "";
String DAC = " ";
String color=" ";
 
#define BUTTON_PIN  12  // Pin GPIO donde está conectado el botón
#define LED_Control  33
#define RED 27
#define GREEN 14
#define BLUE 26
#define DAC1 25 // Define DAC1 pin (GPIO25)

volatile bool has_interrupted = false;
void IRAM_ATTR handleButtonPress(void);

void setup() {

  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  ////////configuracion de terminales///////////
        pinMode(BUTTON_PIN, INPUT_PULLDOWN);  
        pinMode(LED_Control, OUTPUT);
        pinMode(RED, OUTPUT);
        pinMode(GREEN, OUTPUT);
        pinMode(BLUE, OUTPUT);
 
        digitalWrite(LED_Control, LOW);
        digitalWrite(RED, LOW);
        digitalWrite(GREEN, LOW);
        digitalWrite(BLUE, LOW);
      // Configura la interrupción
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, RISING);
}

void loop()
{
  if(has_interrupted)
  {
    Serial.println("¡Botón presionado!");
    gpioVal=!gpioVal;  // Se establece la variable a true cuando se detecta una interrupción
    has_interrupted = false;
    updateSensorValue.add("GPIO",gpioVal);
    updateSensorValue.add("ESP32IoTApp",json);
    Firebase.RTDB.updateNode(&fbdo, "/ESP32IoTApp", &updateSensorValue);
    Serial.print("GPIO:");
    Serial.println(gpioVal);
  }
  ////////////////////////////////////////////////
  adcVal = analogRead(34);
  Serial.print("ADC:");
  Serial.println(analogRead(34));
 
  updateSensorValue.add("ADC",adcVal);
  updateSensorValue.add("ESP32IoTApp",json);
  Firebase.RTDB.updateNode(&fbdo, "/ESP32IoTApp", &updateSensorValue);
//////////////////////////////////////////////////
///LECTURA DESDE FIREBASE////////////////////////
/////////////////////////////////////////////////
  Firebase.get(firebaseData, pathLedControl);
  estadoLC = firebaseData.stringData();
  Serial.print("Led Control:");
  Serial.println(estadoLC);
  if (estadoLC.equals("Apagado"))
  {
    digitalWrite(LED_Control, LOW);
 
  }else if (estadoLC.equals("Encendido"))
  {
    digitalWrite(LED_Control, HIGH);
 
  }
////////////////////////LED RGB///////////////////////////////
   Firebase.get(firebaseData, pathLedRGB);
  color = firebaseData.stringData();
  //CONVERTIR EL STRING A UNA OPCION QUE ENCIENDA EL COLOR CORRESPONDIENTE
  Serial.println(color);
    if (color.equals("Apagado"))
  {
      digitalWrite(RED, LOW);
      digitalWrite(GREEN, LOW);
      digitalWrite(BLUE, LOW);
      Serial.print("RGB color: ");
      Serial.println(color);
     
  }else if (color.equals("Rojo"))
  {
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW);
    Serial.print("RGB color: ");
    Serial.println(color);
  }else if (color.equals("Verde"))
  {
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, LOW);
    Serial.print("RGB color: ");
    Serial.println(color);
  }else if(color.equals("Azul"))
  {
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, HIGH);
    Serial.print("RGB color: ");
    Serial.println(color);
  }
////////////////////DAC////////////////////////
  ////////////////////DAC////////////////////////
  Firebase.get(firebaseData, pathDAC);
  DAC = firebaseData.stringData();
  //dar de alta el valor en el CANAL DAC
  dacWrite(DAC1, DAC.toInt());
  Serial.print("DAC: ");
  Serial.println(DAC);
}

void IRAM_ATTR handleButtonPress(void)
{
   has_interrupted = true;
 
}