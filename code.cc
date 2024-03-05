#include <LiquidCrystal.h>
float temp_vout;   
float temp; 
float voltage;

int gas_sensor_port = A1;
int gas_sensor_value = 0;

float rain;
const int triggerPin = 10;
const int echoPin = 9;
long duration;

float V_wind = 0;
float Windspeedfloat;
int Windspeedint;

LiquidCrystal lcd(8, 2, 6, 5, 4, 3); 
String ssid     = "Simulator Wifi";  
String password = "";   
String host     = "api.thingspeak.com"; 
const int httpPort   = 80;
String url     = "/update?api_key=O1WB76S0Z5G24Y0O&field1="; 


void setupESP8266(void)    
{
  
  Serial.begin(115200);   
  Serial.println("AT");   
  delay(10);              
  
  if (Serial.find("OK"))
    Serial.println("ESP8266 OK!!!");  
  Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\""); 
  delay(10);                         
  
  if (Serial.find("OK"))
    Serial.println("Connected to WiFi!!!");     
 
 
  Serial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + httpPort);
  delay(50);                                   
  if (Serial.find("OK")) 
   Serial.println("ESP8266 Connected to server!!!") ;
}


void send_data(void) 
{
  
  String httpPacket = "GET " + url + String(temp) + "&field2=" + String(gas_sensor_value) + "&field3=" + String(rain) + "&field4=" + String(Windspeedfloat) + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";
  int length = httpPacket.length();
 
  Serial.print("AT+CIPSEND=");
  Serial.println(length);
  delay(10);       
  Serial.print(httpPacket);
  delay(10);       
  if (Serial.find("SEND OK\r\n"))
    Serial.println("ESP8266 sends data to the server");
}

void setup() 
{
  pinMode(A1, INPUT);      
  pinMode(7, OUTPUT);     
  pinMode(A0, INPUT);     
  pinMode(A2, INPUT);     
  setupESP8266();
  lcd.begin(16,2);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
}

void loop() 
{
 
  temp_vout = analogRead(A0);
  voltage = temp_vout * 0.0048828125; //convert analog value between 0 to 1023 with 5000mV/5V ADC 
  temp = (voltage - 0.5) * 100.0;
  Serial.println("Current temperature: " + String(temp));
 

  gas_sensor_value = analogRead(gas_sensor_port);
  Serial.println("Gas sensor value: " + String(gas_sensor_value));
  if (gas_sensor_value > 200)
  {
    tone(7,523,1000);
  }
  
  
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  rain = 0.01723 * duration;
  delay(10);
  lcd.setCursor(0,0); 
  lcd.print("Rainfall: "); 
  lcd.print(rain); 
  Serial.println("Rainfall: " + String(rain));
  delay(10);
  

  float V_wind = analogRead(A2) * (5.0 / 1023.0);  

  Windspeedint = (V_wind - 0.4) * 10 * 2.025 * 2.237;      
  Windspeedfloat = (V_wind - 0.4) * 10 * 2.025 * 2.237; 
  lcd.setCursor(0,1);        
  lcd.print("Wind speed");
  lcd.print(" ");
  if (V_wind < 0.4)
  {         
    lcd.print("0");
  }     	 
  else 
  {				      
    lcd.print(Windspeedint);
  }
  lcd.print("MPH");
 
  Serial.print("Wind Speed: ");       
  if (Windspeedfloat <= 0)
  {            
    Serial.print("0.0");}				
  else{									 
    Serial.print(Windspeedfloat);}	
  Serial.println(" MPH");				
  delay(100);
  
  Serial.print("Anemometer Voltage: ");
  if (V_wind > 2){						
    Serial.println("Out of range!");
  }   
  else if (V_wind < 0.4)                
  {				
    Serial.println("Out of range!");   
  }
  else{									 
    Serial.print(V_wind);				
    Serial.println(" V");}				
  
  
  send_data();
  delay(1000);      
}
