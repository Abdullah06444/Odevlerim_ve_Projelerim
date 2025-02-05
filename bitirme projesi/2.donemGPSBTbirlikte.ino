#include <TinyGPS++.h> // Library for GPS sensor
#include <SoftwareSerial.h>
#include <Wire.h>
#include <dht11.h> // Library for Temp-Humi sensor
#include <LiquidCrystal_I2C.h> // for working of the LCD screen

// Sensör ve Motorların Arduino'ya bağladığımız pinlerini tanımlıyoruz.
#define MotorR1 6
#define MotorR2 7

#define MotorL1 5
#define MotorL2 4

#define echoPin 10 //Ultrasonik sensörün echo pini Arduino'nun 12.pinine
#define trigPin 11 //Ultrasonik sensörün trig pini Arduino'nun 13.pinine tanımlandı.

static const int TXPin = 9, RXPin = 8;
static const uint32_t GPSBaud = 9600; // GPS sensörünün çalıştığı baudrate

// The TinyGPS++ pbject
TinyGPSPlus gps; // This is the GPS object that will pretty much do all the grunt work with the NMEA data

// The serial connection to the GPS device
SoftwareSerial BT(0,1);//TX,RX respectively
SoftwareSerial ss(TXPin, RXPin);

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD panelin bağlantı pinleri.
dht11 DHT11;

int dly=200;
int acc=1000;
int m,k;
String readvoice;
long timee, distance;

void setup() {
  
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  BT.begin(9600);
  ss.begin(GPSBaud);
  
  lcd.begin(/*16, 2*/); // (row, column)
  DHT11.attach(13); //dht 11 in s bacağının bağlantı pini

  // Sensör ve Motorların Giriş-Çıkış bilgilerini belirtiyoruz.
  pinMode(MotorR1, OUTPUT); 
  pinMode(MotorR2, OUTPUT);

  pinMode(MotorL1, OUTPUT);
  pinMode(MotorL2, OUTPUT);

  // ultrasonik sensör Trig pininden ses dalgaları gönderdiği için OUTPUT (Çıkış),
  // bu dalgaları Echo pini ile geri aldığı için INPUT (Giriş) olarak tanımlanır.
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);

  lcd.home();
  lcd.print("  ABDULLAH GUR  ");
  lcd.setCursor(0,8);
  lcd.print("YUNUS CIFTCILER ");

  delay(500);
  lcd.clear();
  delay(100);
}



void theforward(){  // Robotun ileri yönde hareketi için fonksiyon tanımlıyoruz.

  analogWrite(MotorR1, acc); // Sağ motorun ileri hareketi aktif
  analogWrite(MotorR2, 0); // Sağ motorun geri hareketi pasif

  analogWrite(MotorL1, acc); // Sol motorun ileri hareketi aktif
  analogWrite(MotorL2, 0); // Sol motorun geri hareketi pasif
}

void theback(){  // Robotun ileri yönde hareketi için fonksiyon tanımlıyoruz.

  analogWrite(MotorR1, 0); // Sağ motorun ileri hareketi aktif
  analogWrite(MotorR2, acc); // Sağ motorun geri hareketi pasif

  analogWrite(MotorL1, 0); // Sol motorun ileri hareketi aktif
  analogWrite(MotorL2, acc); // Sol motorun geri hareketi pasif
}

void theleft(){  // Robotun ileri yönde hareketi için fonksiyon tanımlıyoruz.

  analogWrite(MotorR1, acc); // Sağ motorun ileri hareketi aktif
  analogWrite(MotorR2, 0); // Sağ motorun geri hareketi pasif

  analogWrite(MotorL1, 0); // Sol motorun ileri hareketi aktif
  analogWrite(MotorL2, 0); // Sol motorun geri hareketi pasif
}

void theright(){  // Robotun ileri yönde hareketi için fonksiyon tanımlıyoruz.

  analogWrite(MotorR1, 0); // Sağ motorun ileri hareketi aktif
  analogWrite(MotorR2, 0); // Sağ motorun geri hareketi pasif

  analogWrite(MotorL1, acc); // Sol motorun ileri hareketi aktif
  analogWrite(MotorL2, 0); // Sol motorun geri hareketi pasif
}

void thestop(){  // Robotun ileri yönde hareketi için fonksiyon tanımlıyoruz.

  analogWrite(MotorR1, 0);
  analogWrite(MotorR2, 0);
  
  analogWrite(MotorL1, 0);
  analogWrite(MotorL2, 0);
}

void tempe(){

  DHT11.read();
  float temp = DHT11.temperature;
  double lat=gps.location.lat();
  double lng=gps.location.lng();
  if(gps.location.isValid())//This will pretty much be fired all the time anyway but will at least reduce it to only after a package of NMEA data comes in
  {

    Serial.print(temp);
    Serial.print(",");
  }
}

void humi(){

  DHT11.read();
  float humi = DHT11.humidity;
  double lat=gps.location.lat();
  double lng=gps.location.lng();
  if(gps.location.isValid())//This will pretty much be fired all the time anyway but will at least reduce it to only after a package of NMEA data comes in
  {
    Serial.print(humi);
    Serial.print(",");
  }
}

void myLocation() {

  if(gps.location.isValid())//This will pretty much be fired all the time anyway but will at least reduce it to only after a package of NMEA data comes in
  {

    double lat=gps.location.lat();
    double lng=gps.location.lng();
    //Get the latest info from the gps object which it derived from the data sent by the GPS unit
    
    Serial.print(lat, 6);
    Serial.print(",");
    Serial.println(lng, 6);

    lcd.setCursor(0, 0);
    lcd.print("Lati:");
    lcd.print(lat, 6);
    lcd.print("   ");
    lcd.setCursor(0, 1);
    lcd.print("Long:");
    lcd.print(lng, 6);
    lcd.print("   ");
    
  }
  else// if(gps.charsProcessed() < 10)
  {

    lcd.setCursor(0,0);
    lcd.print("  No GPS data:  ");
    lcd.setCursor(0,1);
    lcd.print("  check wiring  ");   
  }

}

// This custom version of delay() ensures that the gps object is being "fed".
static void smartDelayss(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    
    //Serial.println("ss delay");
    ss.listen();
    while (ss.available())
      gps.encode(ss.read());
  }while (millis() - start < ms);
}

static void smartDelayBT(unsigned long ms)
{
  unsigned long start = millis();
  do
  {

    //Serial.println("BT delay");
    BT.listen();
    if(BT.available())
    {
      readvoice = "";
      m=1;      
    }
    while (BT.available() && m==1){  //Check if there is an available byte to read
      delay(10); //Delay added to make thing stable
      char c = BT.read(); //Conduct a serial read
      readvoice += c; //build the string- "forward", "reverse", "left" and "right"
    }
    m=0;
  }while (millis() - start < ms);

}


String temp,temp2;

void loop() {

    smartDelayBT(dly);

    if (readvoice.length() > 0) {
      
    if(readvoice == "temperature"){
    
      DHT11.read();
      byte val = DHT11.temperature;//map(DHT11.temperature,0,0,0,0);
      Serial.write(val);
      
      temp = readvoice;
    }

    if(readvoice == "humidity"){
    
      DHT11.read();
      byte val = DHT11.humidity;//map(DHT11.humidity,0,0,0,0);
      Serial.write(val);
            
      temp = readvoice;
    }

    //Google Visualization da göstermek için
    smartDelayss(dly); 
      
    if(readvoice == "tempMap"){

      Serial.begin(115200);
      if(k==0)
      {
      Serial.println();
      Serial.print("N,"); // Temperature or Humidity
      Serial.print("Latitude,");
      Serial.println("Longitude");
      k=1;
      }
      tempe();
      myLocation();
      
      Serial.begin(9600);
      if(temp == "temperature")
      {
      
      DHT11.read();
      byte val = map(DHT11.temperature,0,128,0,127);
      Serial.write(val);
      }
      else if(temp == "humidity")
      {
      
      DHT11.read();
      byte val = map(DHT11.humidity,0,128,0,127);
      Serial.write(val);          
      }
      
      temp2 = readvoice;
    }

      
    if(readvoice == "humMap"){

      Serial.begin(115200);
      if(k==0)
      {
      Serial.println();
      Serial.print("N,"); // Temperature or Humidity
      Serial.print("Latitude,");
      Serial.println("Longitude");
      k=1;
      }
      humi();
      myLocation();
      
      Serial.begin(9600);
      if(temp == "temperature")
      {
      
      DHT11.read();
      byte val = map(DHT11.temperature,0,128,0,127);
      Serial.write(val);
      }
      else if(temp == "humidity")
      {
      
      DHT11.read();
      byte val = map(DHT11.humidity,0,128,0,127);
      Serial.write(val);          
      }
      
      temp2 = readvoice;
    }

    if(readvoice == "forward"){
    
      digitalWrite(trigPin, LOW);
      delayMicroseconds(5);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      timee = pulseIn(echoPin, HIGH);
      distance = timee / 29.1 / 2;
      Serial.println(distance);
      
      if(distance < 15){
      
        theback();
        delay(300);
        theright();
        delay(150);
      }
      if(distance >= 15)
        theforward();
      
      
      
      if(temp == "temperature")
      {
      
        DHT11.read();
        byte val = map(DHT11.temperature,0,128,0,127);
        Serial.write(val);
      }
      else if(temp == "humidity")
      {
      
        DHT11.read();
        byte val = map(DHT11.humidity,0,128,0,127);
        Serial.write(val);          
      }
      
      
      if(temp2 == "tempMap")
      {
      
        Serial.begin(115200);
        if(k==0)
        {
          Serial.println();
          Serial.print("N,"); // Temperature or Humidity
          Serial.print("Latitude,");
          Serial.println("Longitude");
          k=1;
        }
        tempe();
        myLocation();
        Serial.begin(9600);        
      }
      else if(temp2 == "humMap")
      {
      
        Serial.begin(115200);
        if(k==0)
        {
          Serial.println();
          Serial.print("N,"); // Temperature or Humidity
          Serial.print("Latitude,");
          Serial.println("Longitude");
          k=1;
        }
        humi();
        myLocation();          
        Serial.begin(9600);        
      }
      
    }

    if(readvoice == "left"){

      theleft();

      if(temp == "temperature")
      {
      
        DHT11.read();
        byte val = map(DHT11.temperature,0,128,0,127);
        Serial.write(val);
      }
      else if(temp == "humidity")
      {
      
        DHT11.read();
        byte val = map(DHT11.humidity,0,128,0,127);
        Serial.write(val);          
      }
      
      
      if(temp2 == "tempMap")
      {
      
        Serial.begin(115200);
        if(k==0)
        {
          Serial.println();
          Serial.print("N,"); // Temperature or Humidity
          Serial.print("Latitude,");
          Serial.println("Longitude");
          k=1;
        }
        tempe();
        myLocation();
        Serial.begin(9600);        
      }
      else if(temp2 == "humMap")
      {
      
        Serial.begin(115200);
        if(k==0)
        {
          Serial.println();
          Serial.print("N,"); // Temperature or Humidity
          Serial.print("Latitude,");
          Serial.println("Longitude");
          k=1;
        }
        humi();
        myLocation();          
        Serial.begin(9600);        
      }   
          
    }

    if(readvoice == "right"){
    
      theright();

      if(temp == "temperature")
      {
      
        DHT11.read();
        byte val = map(DHT11.temperature,0,128,0,127);
        Serial.write(val);
      }
      else if(temp == "humidity")
      {
      
        DHT11.read();
        byte val = map(DHT11.humidity,0,128,0,127);
        Serial.write(val);          
      }
      
      
      if(temp2 == "tempMap")
      {
      
        Serial.begin(115200);
        if(k==0)
        {
          Serial.println();
          Serial.print("N,"); // Temperature or Humidity
          Serial.print("Latitude,");
          Serial.println("Longitude");
          k=1;
        }
        tempe();
        myLocation();
        Serial.begin(9600);        
      }
      else if(temp2 == "humMap")
      {
      
        Serial.begin(115200);
        if(k==0)
        {
          Serial.println();
          Serial.print("N,"); // Temperature or Humidity
          Serial.print("Latitude,");
          Serial.println("Longitude");
          k=1;
        }
        humi();
        myLocation();          
        Serial.begin(9600);        
      }
      
    }
    
    if(readvoice == "back"){
    
      theback();

      if(temp == "temperature")
      {
      
        DHT11.read();
        byte val = map(DHT11.temperature,0,128,0,127);
        Serial.write(val);
      }
      else if(temp == "humidity")
      {
      
        DHT11.read();
        byte val = map(DHT11.humidity,0,128,0,127);
        Serial.write(val);          
      }
      
      
      if(temp2 == "tempMap")
      {
      
        Serial.begin(115200);
        if(k==0)
        {
          Serial.println();
          Serial.print("N,"); // Temperature or Humidity
          Serial.print("Latitude,");
          Serial.println("Longitude");
          k=1;
        }
        tempe();
        myLocation();
        Serial.begin(9600);        
      }
      else if(temp2 == "humMap")
      {
      
        Serial.begin(115200);
        if(k==0)
        {
          Serial.println();
          Serial.print("N,"); // Temperature or Humidity
          Serial.print("Latitude,");
          Serial.println("Longitude");
          k=1;
        }
        humi();
        myLocation();          
        Serial.begin(9600);        
      }
      
    }
    
    if(readvoice == "stop"){
    
      thestop();

      if(temp == "temperature")
      {
      
        DHT11.read();
        byte val = map(DHT11.temperature,0,128,0,127);
        Serial.write(val);
      }
      else if(temp == "humidity")
      {
      
        DHT11.read();
        byte val = map(DHT11.humidity,0,128,0,127);
        Serial.write(val);          
      }
      
      
      if(temp2 == "tempMap")
      {
      
        Serial.begin(115200);
        if(k==0)
        {
          Serial.println();
          Serial.print("N,"); // Temperature or Humidity
          Serial.print("Latitude,");
          Serial.println("Longitude");
          k=1;
        }
        tempe();
        myLocation();
        Serial.begin(9600);        
      }
      else if(temp2 == "humMap")
      {
      
        Serial.begin(115200);
        if(k==0)
        {
          Serial.println();
          Serial.print("N,"); // Temperature or Humidity
          Serial.print("Latitude,");
          Serial.println("Longitude");
          k=1;
        }
        humi();
        myLocation();          
        Serial.begin(9600);        
      }    
        
    }
  }
}
