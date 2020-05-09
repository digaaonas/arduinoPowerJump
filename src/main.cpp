#include <ESP8266WiFi.h>
#include <EmonLib.h>

EnergyMonitor emon1;

const char* ssid = "YOUR SSID";
const char* password = "YOUR PASSWORD";
int timeConnection = 0;

const char* host = "YOUR SERVER ADDRES";

float dataPower = 0.00;

int nColetaData;

void sendData(){

  if(WiFi.status() == WL_DISCONNECTED){
    ESP.restart();
  }

  WiFiClient client;

  Serial.println("[Sending a data]");

  if(!client.connect(host, 2222)){
    Serial.println("Not Connected :(");

    return;
  }

  Serial.println("connected");
  
  digitalWrite(5, LOW);

  digitalWrite(15, HIGH);

  client.print(String("GET /dataPower") + " HTTP/1.1\r\n" + "Data: " + dataPower + "\r\n" + "NumberColetaData: " + nColetaData + "\r\n\r\n");

  String response = client.readString();

  if(response.indexOf("Cicle Complete!") != -1){

  Serial.println("Cicle Complete");

  digitalWrite(15, LOW);

  dataPower = 0.00;

  nColetaData = 0;
  
  Serial.println();

  }

  Serial.println("Client Stop");
  client.flush();
  client.stop();

}

void readData(){
  dataPower += emon1.calcIrms(1480);

  nColetaData += 1;

  if(nColetaData >= 50 && nColetaData < 60){ 
    sendData();

  }else if(nColetaData >= 60){
    ESP.restart();
  }

  digitalWrite(5, LOW);

}

void setup(){

  Serial.begin(115200);
  Serial.println();

  pinMode(12, OUTPUT); //LED WIFI CONNECTED D6
  pinMode(14, OUTPUT); //LED CURRENT ON D5
  pinMode(5, OUTPUT); //LED READ DATA D1
  pinMode(15, OUTPUT); //LED SEND DATA D8
  pinMode(A0, INPUT); // A0

  //Pino, calibracao - Cur Const= Ratio/BurdenR. 2000/25 = 80
  emon1.current(A0, 80);

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);

    digitalWrite(12, HIGH);

    delay(500);

    digitalWrite(12, LOW);

    Serial.print("."),

    timeConnection += 1;

    if(timeConnection >= 10){
      
      ESP.restart();

    }

  }

  Serial.println(" connected");

  timeConnection = 0;

  digitalWrite(12, HIGH);

}

void loop(){

  if(WiFi.status() == WL_DISCONNECTED){
    ESP.restart();
  }

  readData();

  if(emon1.calcIrms(1480) > 0.15){
    digitalWrite(14, HIGH);
  }else{
    digitalWrite(14, LOW);
  }

  delay(1000);
  Serial.println(emon1.calcIrms(1480));


  digitalWrite(5, HIGH);

}