#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <UrlEncode.h>
#include <ESP8266WiFi.h>       

//Define the digital pins of the electronic components
#define GREEN D1
#define RED D5
#define BUZZER D6

//Threshold to trigger the alarm (0-100 range)
#define THRESHOLD 85

//Information on Wi-Fi connection
const char* ssid     = "SSID";        
const char* password = "PASSWORD";     
String phoneNumber = "PHONE-NUMBER";
String apiKey = "API-KEY";

//Variable to store the MQ-2 analog signal
int sensorGas;

void whatsAppMessage(String message);

void setup() {
  //Configuring the digital pins to OUTPUT mode
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  
  Serial.begin(115200);
  // Begins the Wi-Fi connection
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  // Waits to the connection to be established
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  //Confirms the connection success
  Serial.print("Connected to the Wi-Fi network with the following IP: ");
  Serial.println(WiFi.localIP());

  // Sends a WhatsApp message to confirm the connection with the message bot
  whatsAppMessage("Connection established");
}

void loop() { 
  // sensorGas variable stores the 10-byte value from the A0 pin receiving the analog signal provided by the MQ-2 sensor
  sensorGas = analogRead(A0);
  //Conversion of the 10-byte range number to a percentual value
  sensorGas = map(sensorGas, 0, 1023, 0, 100);
  
  Serial.println(sensorGas);

  if (sensorGas > THRESHOLD){
    //Dangerous gas levels detected (above the threshold)
    
    //Green LED Off
    digitalWrite(green, LOW);
    //Red LED blinks every 0.5 second
    digitalWrite(red, HIGH);
    delay(500);
    digitalWrite(red, LOW);
    delay(500);
    //Buzzer emits a 1000Hz sound
    tone(buzzer, 1000);
    //Continous alert messages are sent to the WhatsApp number registered 
    whatsAppMessage("ALERT - POSSIBLE GAS LEAKAGE DETECTED");
  }
  else{
    //No dangerous gas levels detected (under the threshold)
  
    //Green LED turned on
    digitalWrite(green, HIGH);
    //Red LED turned off
    digitalWrite(red, LOW);
    //Buzzer turned off
    noTone(buzzer);

  }
}

void whatsAppMessage(String message){

  String url = "http://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);
  WiFiClient client;    
  HTTPClient http;
  http.begin(client, url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
// Send POST request to URL provided by the CallMeBot API
  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200){
    Serial.println("Alert sent");
  }
  else{
    Serial.print("Error ");
    Serial.println(httpResponseCode);
  }
  http.end();
}
