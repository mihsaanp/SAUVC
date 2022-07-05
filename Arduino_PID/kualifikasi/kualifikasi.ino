#include <Servo.h>
Servo FR;
Servo FL;
Servo BR;
Servo BL;
Servo CR;
Servo CL;
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
float sum_error,erorAwal, koreksiP, koreksiI,erorAkhir, koreksiD, koreksiTotal,FKiri , FKanan, BKiri, BKanan, Kiri, Kanan, P, I, D, SET_POINT;

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
  FR.attach(8,1000,2000);
  FL.attach(9,1000,2000);
  BR.attach(10,1000,2000);
  BL.attach(11,1000,2000);
  CR.attach(12,1000,2000);
  CL.attach(13,1000,2000);
  FR.writeMicroseconds(1500);
  FL.writeMicroseconds(1500);
  BR.writeMicroseconds(1500);
  BL.writeMicroseconds(1500);
  CR.writeMicroseconds(1500);
  CL.writeMicroseconds(1500);
  delay(1000);
  P = 1.5;
  I = 0.00;
  D = 10;
  SET_POINT = 0.00;
  FKiri = 1550;
  FKanan = 1550;
  FKiri = 1450;
  FKanan = 1450;
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
    pivot();
 //  Serial.print(koreksiTotal);
 //  Serial.print("|");
 //  Serial.println(hadap);
 //  delay(10000);
    
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
  hadap1 = abs(hadap);
  if (hadap1 > 0.3){
  FKiri = FKiri - koreksiTotal;
  FKanan = FKanan + koreksiTotal;
  BKiri = BKiri + koreksiTotal;
  BKanan = BKanan - koreksiTotal;
  if(FKiri <= 1500) {FKiri = 1500;}
  if(FKiri >= 1600) {FKiri = 1600;}
  if(FKanan <= 1500) {FKanan = 1500;}
  if(FKanan >= 1600) {FKanan = 1600;}
  if(BKiri >= 1500) {BKiri = 1500;}
  if(BKiri <= 1400) {BKiri = 1400;}
  if(BKanan >= 1500) {BKanan = 1500;}
  if(BKanan <= 1400) {BKanan = 1400;}
  } 
  else {
    FKiri = 1550;
    FKanan = 1550;
    BKiri = 1450;
    BKanan = 1450;
  }
  //Serial.print(hadap);
 // Serial.print(" | ");
//Serial.print(BKiri);
  //Serial.print(" | ");
  //Serial.println(BKanan);
  //delay(100);
  FR.writeMicroseconds(FKanan);
  FL.writeMicroseconds(FKiri);
  BR.writeMicroseconds(BKanan);
  BL.writeMicroseconds(BKiri);
  CR.writeMicroseconds(1500);
  CL.writeMicroseconds(1500);  
  delay(10000);     
  }

void pivot(){

  hitung_PID(hadap);
  hadap1 = abs(hadap);
  if (hadap1 > 5.0){
  FKiri = FKiri + koreksiTotal;
  FKanan = FKanan - koreksiTotal;
  BKiri = BKiri + koreksiTotal;
  BKanan = BKanan - koreksiTotal;
  if(FKiri <= 1400) {FKiri = 1400;}
  if(FKiri >= 1600) {FKiri = 1600;}
  if(FKanan <= 1400) {FKanan = 1400;}
  if(FKanan >= 1600) {FKanan = 1600;}
  if(BKiri >= 1500) {BKiri = 1500;}
  if(BKiri <= 1400) {BKiri = 1400;}
  if(BKanan >= 1500) {BKanan = 1500;}
  if(BKanan <= 1400) {BKanan = 1400;}
  } 
  else {
    FKiri = 1500;
    FKanan = 1500;
    BKiri = 1500;
    BKanan = 1500;
  }
  Serial.print(hadap);
  Serial.print(" | ");
  Serial.print(FKiri);
  Serial.print(" | ");
  Serial.println(FKanan);
  delay(100);
  //FR.writeMicroseconds(FKanan);
  //FL.writeMicroseconds(FKiri);
  //BR.writeMicroseconds(BKanan);
  //BL.writeMicroseconds(BKiri);
  //CR.writeMicroseconds(1500);
  //CL.writeMicroseconds(1500);  
  //delay(10000);     
  }

void kanan(){
  Kiri = 1600;
  Kanan = 1600;
  SET_POINT = 90;
  hitung_PID(hadap);

  }
