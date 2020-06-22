/*
  Veiculo Eletrico BREletrico  
  inicio  23/11/2017  LCD e Temperatura
  Upgrade 05/02/2018 Partindo para medicao de Corrente e Tensao Modbus
  26/08/2018 - Colocando o display no painel
             - Instalando medidor de velocidade no pino 2 (Interrupcao)
  08/09/2018 - Odometro para calibrar o velocimetro 
             - Gravacao em EEPROM
  13/09/2018 - Calibracao Odometro. 210-170=40 pulsos Distancia 8.3metros           
               1 pulso = 8.30 / 40 = 0,2075 metro
             
     
  Pinos do LCD 
  // enable = white = 37
  // r/w = gray = 35
  // rs - blue = 33 

  LCD Based on Universal 8bit Graphics Library, http://code.google.com/p/u8glib/
  Copyright (c) 2012, olikraus@gmail.com

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
*/


#include "U8glib.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "MsTimer2.h"

char versao[10]="03oct18";

/* int sensorPin_tensao_24v = A12; 
int sensorPin_tensao_12v = A13; 
int sensorPin_corr_24v   = A14; //A12; 
int sensorPin_corr_12v   = A15; //i1 = A13; 
*/

int sensorPin_tensao_12v = A15; // amarela  
int sensorPin_tensao_24v = A14; // verde   Calibracao 25.1 V = 857 unidades
int sensorPin_corr_24v   = A13; // laranja Calibracao 0-1023    -5 to 5 
int sensorPin_corr_12v   = A12; //i1 = A13; 
float f;

const int velocidade_pin = 2;


#define sensortemperatura_1_Pin 45
#define sensortemperatura_2_Pin 47
#define sensortemperatura_3_Pin 49
#define sensortemperatura_4_Pin 51
#define sensortemperatura_5_Pin 53

OneWire oneWire1(sensortemperatura_1_Pin);
OneWire oneWire2(sensortemperatura_2_Pin);
OneWire oneWire3(sensortemperatura_3_Pin);
OneWire oneWire4(sensortemperatura_4_Pin);
OneWire oneWire5(sensortemperatura_5_Pin);

DallasTemperature SensorTemperatura_1(&oneWire1);
DallasTemperature SensorTemperatura_2(&oneWire2);
DallasTemperature SensorTemperatura_3(&oneWire3);
DallasTemperature SensorTemperatura_4(&oneWire4);
DallasTemperature SensorTemperatura_5(&oneWire5);

#define PumpPin 53

U8GLIB_ST7920_128X64_4X u8g(37, 35, 33);  
// enable = white = 37
// r/w = gray = 35
// rs - blue = 33

int Temperatura_1_Int;
int Temperatura_2_Int;
int Temperatura_3_Int;
int Temperatura_4_Int;
int Temperatura_5_Int;
   
int Tensao12vInt;
int Tensao24vInt;
int Corrente12vInt;
int Corrente24vInt;

int Velocidade_Int;
int Odometro_Int;

char Temperatura_1_Str[10]; 
char Temperatura_2_Str[10]; 
char Temperatura_3_Str[10]; 
char Temperatura_4_Str[10]; 
char Temperatura_5_Str[10]; 

char Tensao12vStr[10];
char Tensao24vStr[10];
char Corrente12vStr[10];
char Corrente24vStr[10];

char Velocidade_Str[10];
char Odometro_Str[10]; 

void le_temperatura(void)
{ 
 SensorTemperatura_1.requestTemperatures();
 Temperatura_1_Int = SensorTemperatura_1.getTempCByIndex(0);
 dtostrf(Temperatura_1_Int, 3, 0, Temperatura_1_Str);    

 SensorTemperatura_2.requestTemperatures();
 Temperatura_2_Int = SensorTemperatura_2.getTempCByIndex(0);
 dtostrf(Temperatura_2_Int, 3, 0, Temperatura_2_Str);    

 SensorTemperatura_3.requestTemperatures();
 Temperatura_3_Int = SensorTemperatura_3.getTempCByIndex(0);
 dtostrf(Temperatura_3_Int, 3, 0, Temperatura_3_Str);    

 SensorTemperatura_4.requestTemperatures();
 Temperatura_4_Int = SensorTemperatura_4.getTempCByIndex(0);
 dtostrf(Temperatura_4_Int, 3, 0, Temperatura_4_Str);    

 SensorTemperatura_5.requestTemperatures();
 Temperatura_5_Int = SensorTemperatura_5.getTempCByIndex(0);
 dtostrf(Temperatura_5_Int, 3, 0, Temperatura_5_Str);    
}

void le_corrente_tensao(void)
{
 Tensao12vInt=analogRead(sensorPin_tensao_12v);    dtostrf(Tensao12vInt, 4, 0, Tensao12vStr);
 Corrente12vInt=analogRead(sensorPin_corr_12v);  dtostrf(Corrente12vInt, 4, 0, Corrente12vStr);
  
 Tensao24vInt=analogRead(sensorPin_tensao_24v);  
 Corrente24vInt=analogRead(sensorPin_corr_24v);  

 // Calibracao 

 f=map(Tensao24vInt,0,1023,0,299)/10;
 dtostrf(f,4,1,Tensao24vStr);  
 // dtostrf(Tensao24vInt, 4, 0, Tensao24vStr);

 f=map(Corrente24vInt,0,1023,-50,50)/10;
 dtostrf(f,4,1,Corrente24vStr);
 // dtostrf(Corrente24vInt, 4, 0, Corrente24vStr);

 // if (digitalRead(velocidade_pin)==HIGH) Velocidade_Int = 1; else Velocidade_Int = 0;  // usado para calibrar
 dtostrf(Velocidade_Int, 4, 0, Velocidade_Str);
 dtostrf(Odometro_Int, 4, 0, Odometro_Str);
 
}

void draw(void) {
 u8g.setFont(u8g_font_unifont);
 u8g.setScale2x2(); 
 u8g.drawStr( 0, 10 , "Vel="); u8g.drawStr(30, 10, Velocidade_Str);    
 u8g.undoScale();
 u8g.drawStr( 0, 36 , "T3=     T4=    ");  u8g.drawStr(24, 36, Temperatura_3_Str); u8g.drawStr(88, 36, Temperatura_4_Str);
 u8g.drawStr( 0, 50 , "T5=    Dis=    ");  u8g.drawStr(24, 50, Temperatura_5_Str); u8g.drawStr(88, 50, Odometro_Str);
 u8g.drawStr( 0, 62 , "V =     I =    ");  u8g.drawStr(24, 62, Tensao24vStr);      u8g.drawStr(88, 62, Corrente24vStr);
}


void manda_serial(void){
/* Serial.print("T1 =");Serial.print(Temperatura_1_Int); Serial.print("; ");
 Serial.print("T2 =");Serial.print(Temperatura_2_Int); Serial.print("; ");
 Serial.print("T3 =");Serial.print(Temperatura_3_Int); Serial.print("; ");
 Serial.print("T4 =");Serial.print(Temperatura_4_Int); Serial.print("; ");
 Serial.print("T5 =");Serial.print(Temperatura_5_Int); Serial.print("; ");
 Serial.print("V12=");  */
 
// Serial.print(Tensao12vInt);      Serial.print(" ");
// Serial.print("V24=");

// Serial.print(Tensao24vInt);      Serial.print(" ");
// Serial.print("I12=");

// Serial.print(Corrente12vInt);    Serial.print(" ");
// Serial.print("I24=");

// Serial.print(Corrente24vInt);    Serial.print(" ");

/* Serial.print("Vel=");Serial.print(Velocidade_Str);    Serial.print("; ");
 Serial.print("Dis=");Serial.print(Odometro_Str);*/


 Serial.print(Tensao24vStr); Serial.print(" "); Serial.print(Corrente24vStr);
 Serial.println(" ");
}

void setup(void) { 
  pinMode(PumpPin,OUTPUT);
  pinMode(velocidade_pin,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(velocidade_pin), conta_pulsos, CHANGE);  

  MsTimer2::set(1000, BaseDeTempo);
  MsTimer2::start();
  
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
  SensorTemperatura_1.begin();
  SensorTemperatura_2.begin();
  SensorTemperatura_3.begin();
  SensorTemperatura_4.begin(); 
  SensorTemperatura_5.begin();
  SensorTemperatura_1.setResolution(8);
  SensorTemperatura_2.setResolution(8);
  SensorTemperatura_3.setResolution(8);
  SensorTemperatura_4.setResolution(8);
  SensorTemperatura_5.setResolution(8);
  Serial.begin(9600);
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_unifont);
    u8g.drawStr(20, 20, "BREletrico");      
    u8g.drawStr(30, 54, versao);
  } while (u8g.nextPage());
  delay(3000);  
}

void conta_pulsos() {
  Velocidade_Int++;
  Odometro_Int++;
}

void BaseDeTempo(void)
{
 Velocidade_Int=0;
}

void loop(void) {
  le_temperatura();
  le_corrente_tensao();
  manda_serial();
  u8g.firstPage();  
  do {
//    draw_int_tensao_corrente();
//    draw_int_temperatura();
      draw();   
  } while( u8g.nextPage() );
  // rebuild the picture after some delay
  delay(250);
}

