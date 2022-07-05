#include <Servo.h>
Servo ESC;
Servo ESC2;
//#include <SoftwareSerial.h>

//static const int RXPin = 2, TXPin = 3;
//SoftwareSerial Seriall(RXPin, TXPin);

//#define SET_POIN 0.00
#define MIN 1220
#define MAX 1500
//#define P 1.4329
//#define I 0.00
//#define D 603.98901

String dataInIMU;
String dataIMU[10];
int i;
boolean parsing = false;
float hadap, hadap1;
//unsigned long waktuSekarang,waktuAkhir,waktuSelisih;
float sum_error,erorAwal, koreksiP, koreksiI,erorAkhir, koreksiD, koreksiTotal,Kiri ,Kanan, P, I, D, SET_POINT;

void setup(){
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial.print("Calibrating IMU\n");
  Serial.print("Please Wait....\n");

  dataInIMU="";
// Correction mode, Kalibrasi roll pitch. tahan beberapa detik sebelum melakukan kalibrasi.
  Serial2.write(0xA5);
  Serial2.write(0x54);

  delay(2000); // jeda sebelum kalibrasi heading
// Correction mode, Kalibrasi kompas. Kompas akan diset ke 0 ketika perintah ini dikirim.
  Serial2.write(0xA5);
  Serial2.write(0x55);

  delay(100); // jeda sebelum konfigurasi output
// Kirim data sensor secara realtime dalam bentuk ASCII.
  Serial2.write(0xA5);
  Serial2.write(0x53);

  delay(1000); 
  ESC.attach(9,1000,2000);
  ESC2.attach(10,1000,2000);
  ESC.writeMicroseconds(1475);
  ESC2.writeMicroseconds(1470);
  delay(1000);
  P = 2;
  I = 0.00;
  D = 603.98901;
  SET_POINT = 0.00;
  Kiri = 1600;
  Kanan = 1600;
}

void loop(){
  if(Serial2.available() > 0){
    char inChar = (char)Serial2.read();
    dataInIMU += inChar;
    if (inChar == '\n'){
      parsing = true;
    }
  }

  if (parsing) {
    parsingData();
    parsing = false;
    dataInIMU = "";
    maju();
    Serial.print(koreksiTotal);
    Serial.print("|");
    Serial.println(hadap);
 //   delay(10000);
    
    }
 // Serial.print("P: ");
 // Serial.print(P);
 // Serial.print("I: ");
 // Serial.print(I);
 // Serial.print("D: ");
 // Serial.println(D);
 // delay(100);
  
  if(Serial.available()>0) // if there is data comming
  {
    char command = Serial.read(); // read string until meet newline character

    if(command == 's')
    {
     P = P + 1;
    // Serial.print("Nilai P: ");
    // Serial.print(P);
    }
    else
    if(command == 'a')
    {
     P = P - 1;
    // Serial.print("Nilai P: ");
    // Serial.print(P);
    }
    if(command == 'f')
    {
     D = D + 1;
   // Serial.print("Nilai D: ");
     //Serial.print(D);
    }
    else
    if(command == 'd')
    {
     D = D - 1;
    // Serial.print("Nilai D: ");
    // Serial.print(D);
    }
  }
  
}

void parsingData(){
  int j = 0;

  dataIMU[j] = "";
  //proses parsing data.
  for (i = 1; i < dataInIMU.length(); i++) {
    //pengecekan tiap karakter dengan karakter (#) (,) dan (=).
    if ((dataInIMU[i] == '#') || (dataInIMU[i] == ',') || (dataInIMU[i] == '='))
    {
      //increment variable j, digunakan untuk merubah index array penampung
      j++;
      dataIMU[j] = ""; //inisialisasi variable array dataIMU[j]
    }
    else
    {
      //proses tampung data saat pengecekan karakter selesai.
      dataIMU[j] = dataIMU[j] + dataInIMU[i];
    }
  }

  hadap = dataIMU[1].toFloat();
  if (hadap > 180 ){
    float selisih = 180 - hadap;
    hadap = (180 - selisih)*(-1);
  }


}
void setnol(){
  Serial2.write(0xA5);
  Serial2.write(0x55);
  delay(100); 
  
  Serial2.write(0xA5);
  Serial2.write(0x53);
  delay(1000);
}
float hitung_PID(float x)
{
  sum_error = sum_error + erorAwal;
  erorAwal = SET_POINT - x;
  koreksiP = P*erorAwal;
  koreksiI = sum_error;
  koreksiD = (erorAwal-erorAkhir)/10;
  erorAkhir = erorAwal;

  koreksiTotal = koreksiP + I*koreksiI + D*koreksiD;

  if(koreksiTotal <= -200) {koreksiTotal = -200;}
  if(koreksiTotal >= 200) {koreksiTotal = 200;}
  return koreksiTotal;
}
void maju(){

  hitung_PID(hadap);
  if (koreksiTotal>10){
    Kiri = Kiri + koreksiTotal;
    Kanan = Kanan - koreksiTotal;
    batascw();
    ESC.writeMicroseconds(Kiri);
    ESC2.writeMicroseconds(Kanan);  
    delay(10);     
  }
  if (koreksiTotal<10){
    Kiri = Kiri + koreksiTotal;
    Kanan = Kanan - koreksiTotal;
    batascw();
    ESC.writeMicroseconds(Kiri);
    ESC2.writeMicroseconds(Kanan);    
    delay(10);
  }
  else{
    Kiri = 1600;
    Kanan = 1600;
  }
}

void kanan(){
  Kiri = 1600;
  Kanan = 1600;
  SET_POINT = 90;
  hitung_PID(hadap);
  if (koreksiTotal>90){
    Kiri = Kiri - koreksiTotal;
    Kanan = Kanan - koreksiTotal;
    ESC.writeMicroseconds(Kiri);
    ESC2.writeMicroseconds(Kanan);  
    delay(2000);  
  }
  if (koreksiTotal<0){
    Kiri = Kiri + koreksiTotal;
    Kanan = Kanan + koreksiTotal;
    batascw();
    batasccw();
    ESC.writeMicroseconds(Kiri);
    ESC2.writeMicroseconds(Kanan);   
    delay(2000); 
  }
}

void batascw(){
  if(Kiri <= 1500) {Kiri = 1500;}
  if(Kiri >= 1700) {Kiri = 1700;}
}
void batasccw(){
  if(Kanan <= 1300) {Kanan = 1300;}
  if(Kanan >= 1500) {Kanan = 1500;}
}
