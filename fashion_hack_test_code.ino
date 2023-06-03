//https://script.google.com/macros/s/AKfycbzRj0qxF0F3NgyxLmUK4xFoadyVUGI4PVfJ-fWjkyUW9I3Vf-slXA98Z_EmOFBZpl_UBg/exec
//https://script.google.com/macros/s/AKfycbzRj0qxF0F3NgyxLmUK4xFoadyVUGI4PVfJ-fWjkyUW9I3Vf-slXA98Z_EmOFBZpl_UBg/exec?temperature="25"&humidity="93"

//AKfycbxMbHLJkpAHwmWW2p7_Hhbg9Ad9qzlTeWMehj343DD51gZ3umN8C0DPedi2sKuroZJlBA
#define BLYNK_TEMPLATE_ID "TMPLVq4_Qpgg"
#define BLYNK_TEMPLATE_NAME "MACHINE DOWNTIME"
#define BLYNK_AUTH_TOKEN "C5UnuPk0rkUzpniK7tUyvcDFKThdZSNk"
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <UrlEncode.h>
#define SENSOR_PIN A0
#define vibr_Pin D5
#define DHTTYPE DHT11   // DHT 11
#define dht_dpin D1
#define BLYNK_PRINT Serial
#define BLYNK_TIMEOUT_MS 5000
#include <WiFiClientSecure.h>
DHT dht(dht_dpin, DHTTYPE);
char auth[] = "C5UnuPk0rkUzpniK7tUyvcDFKThdZSNk";
const char* ssid = "TECNO";
const char* pass = "1234567890";

const int sampleWindow = 50;
unsigned int sample;
float t;
long measurement;
int db;


String phoneNumber = "+919003826710";
String apiKey = "6376256";

// ----------------------------------------Host & httpsPort
const char* host = "script.google.com";
const int httpsPort = 443;
//----------------------------------------

WiFiClientSecure client; //--> Create a WiFiClientSecure object.

String GAS_ID = "AKfycbxMbHLJkpAHwmWW2p7_Hhbg9Ad9qzlTeWMehj343DD51gZ3umN8C0DPedi2sKuroZJlBA"; //--> spreadsheet script ID

//============================================================================== void setup


void sendMessage(String message) {

  // Data to send with HTTP POST
  String url = "http://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);
  WiFiClient client;
  HTTPClient http;
  http.begin(client, url);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  // Send HTTP POST request
  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200) {
    Serial.print("Message sent successfully");
  }
  else {
    //    Serial.println("Error sending the message");
    //    Serial.print("HTTP response code: ");
    //    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();
}


BlynkTimer timer;

void setup() {

  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);

  WiFi.begin(ssid, pass);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.begin(9600);
  dht.begin();
  pinMode (SENSOR_PIN, INPUT);
  pinMode(vibr_Pin, INPUT); //set vibr_Pin input for measurment
  Serial.println("Humidity and temperature \n\n");
  Serial.println("==========================================");
  delay(700);
  client.setInsecure();

}
void temp_data()
{
  float h = dht.readHumidity();
  t = dht.readTemperature();
  Serial.print("Current humidity = ");
  Serial.print(h);
  Serial.print("%  ");
  Serial.print("temperature = ");
  Serial.print(t);
  Serial.println("C  ");
  delay(800);
}
void loop() {
  unsigned long startMillis = millis();  // Start of sample window
  float peakToPeak = 0;  // peak-to-peak level
  unsigned int signalMax = 0;  //minimum value
  unsigned int signalMin = 1024;  //maximum value
  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(SENSOR_PIN);  //get reading from microphone
    if (sample < 1024)  // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;  // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  Serial.print("peakToPeak : ");
  Serial.println(peakToPeak);
  db = map(peakToPeak, 20, 900, 49.5, 90);  //calibrate for deciBels;
  Serial.print("DBLevel:");
  Serial.println(db);
  measurement = TP_init();
  delay(50);
  Serial.print("Vibration = ");
  Serial.println(measurement);
  if (measurement > 1000) {
  }
  else {
  }

  temp_data();
  Machine_downtime();

  notify();
  Blynk.run();
  timer.run();

//  float t1 = random(1,10);
//  float t2 = random(20,30);
//  Serial.print("Current humidity = ");
//  Serial.print(t1);
//  Serial.print("%  ");
//  Serial.print("temperature = ");
//  Serial.print(t2);
//  Serial.println("C  ");

  sendData(t,measurement); //--> Calls the sendData Subroutine

}
long TP_init() {
  delay(10);
  measurement = pulseIn (vibr_Pin, HIGH); //wait for the pin to get HIGH and returns measurement
  return measurement;
}

//============================================================================== void sendData
// Subroutine for sending data to Google Sheets
void sendData(float tem, long hum) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);

  //----------------------------------------Connect to Google host
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  //----------------------------------------

  //----------------------------------------Processing data and sending data
  String string_temperature =  String(tem);
  // String string_temperature =  String(tem, DEC);
  String string_humidity =  String(hum, DEC);
  String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + string_temperature + "&humidity=" + string_humidity;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  //----------------------------------------

  //----------------------------------------Checking whether the data was sent successfully or not
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  //if (line.startsWith("{\"state\":\"success\"")) {
  if (line.startsWith("242")) {
    Serial.println("esp8266 Data Sent successfully");
  } else {
    Serial.println("esp8266 has failed to send data");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  //----------------------------------------
}


void Machine_downtime()
{

  if ((t > 20.00) && (2 == measurement <= 50.00) && (db > 40))
  {
    Serial.println("Switch is on machine idle");
    sendMessage("Switch is on Machine idle");
  }

  else if ((t > 20.00) && (measurement >= 51.00) && (db > 40))
  {
    Serial.println("Switch is on machine running ");
    sendMessage("Switch is on Machine running");
  }

  else if ((t > 20.00) && (measurement == 0) && (db > 40))
  {
    Serial.println("Machine stopped");
    sendMessage("Machine Stopped,Please check it fast");
  }
}


void notify()
{

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V2, measurement);
  Blynk.virtualWrite(V3, db);


}
