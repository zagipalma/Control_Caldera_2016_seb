//Control Caldera 2016

//Librerias LCD
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#define I2C_ADDR    0x27
LiquidCrystal_I2C             lcd(I2C_ADDR,2, 1, 0, 4, 5, 6, 7);

//Librerias Temperatura
#include <OneWire.h>
#include <DallasTemperature.h>

//Definicion Pines Control Reles

#define RELE1 2    // Marcha/Paro Caldera                
#define RELE2 3    // Bomba Recirculacion ACS                   
#define RELE3 4    // Bomba Recirculacion CAL                 
#define RELE4 5    // Valvula 3 Vias

//Definicion Pines Estados

#define Termostato_ACS  6    // Demanda por Temperatura de ACS                
#define Termostato_CAL  7    // Demanda por Temperatura de CAL
#define Electrovalvula  8    // Estado de la Electrovalvula
#define Verano_Invierno 9    // Modo de Funcionamiento Verano / Invierno

//Definicion Temperatura
/*-----( Declare Constants )-----*/
#define ONE_WIRE_BUS 10 /*-(Connect to Pin 10 )-*/
/*-----( Declare objects )-----*/
/* Set up a oneWire instance to communicate with any OneWire device*/
OneWire ourWire(ONE_WIRE_BUS);
/* Tell Dallas Temperature Library to use oneWire Library */
DallasTemperature sensors(&ourWire);

//Definicion Control LLAMA
int isFlamePin = 6;  // This is our input pin
int isFlame = HIGH;  // HIGH MEANS NO FLAME

//Definicion Estado Temperatura
int t = 0;    // Temperatura
int swt = 0;  // Switch Temperatura
int swtt = 0; // Switch Select cambio Temperatura

//Funciones
void temptexto()
{
  lcd.clear();
  lcd.setCursor (0,1);
  lcd.print("C  ");
  lcd.setCursor (9,1);
  lcd.print("R  ");
}
void tempdatos()
{ 
  sensors.requestTemperatures(); // Send the command to get temperatures
  lcd.setCursor (2,1);
  lcd.print(sensors.getTempCByIndex(0));
  lcd.setCursor (11,1);
  lcd.print(sensors.getTempCByIndex(1));
}
void cargainicial()
{
// Inicio del Proceso :
// Colocacion Caracteres en Pantalla LCD
      lcd.clear();
      lcd.setCursor (1,0);
      lcd.print("INICIO PROCESO");
      lcd.setCursor (0,1);
      lcd.print("B 0"); //Bomba Recirculacion
      lcd.setCursor (4,1);
      lcd.print("R 0"); //Resistencia
      lcd.setCursor (8,1);
      lcd.print("V 0"); //Ventilador
      lcd.setCursor (13,1);
      lcd.print("S 0"); //Sinfin
      delay(6000);  
// Se enciende el rele de la bomba de Recirculacion
      digitalWrite(RELE1,LOW);
      lcd.setCursor (0,0);
      lcd.print("BOMBA RECIRCULA."); 
      lcd.setCursor (0,1);
      lcd.print("B 1");
// Se espera 2 segundos      
      delay(2000);
// Se enciende el rele de la Resistencia
      digitalWrite(RELE2,LOW);          
      lcd.setCursor (0,0);
      lcd.print("RESISTENCIA   ON"); 
      lcd.setCursor (4,1);
      lcd.print("R 1");
// Se espera 2 segundos      
      delay(2000);
// Se enciende el rele de Carga Inicial   
      digitalWrite(RELE4,LOW);           
      lcd.setCursor (0,0);
      lcd.print("CARGA === PELLET"); 
      lcd.setCursor (13,1);
      lcd.print("S 1");
// Se espera 6 segundos
      delay(6000);                         
// Se apaga el rele de Carga Inicial
      digitalWrite(RELE4,HIGH);          
      lcd.setCursor (0,0);
      lcd.print("PAUSA === PELLET"); 
      lcd.setCursor (13,1);
      lcd.print("S 0");
// Se espera 5 segundos
      delay(5000);                        
// Se enciende el rele de Carga Inicial 
      digitalWrite(RELE4,LOW);           
      lcd.setCursor (0,0);
      lcd.print("CARGA === PELLET"); 
      lcd.setCursor (13,1);
      lcd.print("S 1");
// Se espera 6 segundos
      delay(6000);                           
// Se apaga el rele de Carga Inicial
      digitalWrite(RELE4,HIGH);          
      lcd.setCursor (13,1);
      lcd.print("S 0");

// Se espera 5 minuto. Antes 2,5 minutos
      lcd.setCursor (0,0);
      lcd.print("CAL. RESIST.    "); 
      for (int i = 300; i >=0; i--)
      { 
        lcd.setCursor (13,0); lcd.print("   ");
        lcd.setCursor (13,0); lcd.print(i);
          if(i==180) 
            { // Se enciende el rele de la Turbina de Aire 
              digitalWrite(RELE3,LOW); 
              lcd.setCursor (8,1); 
              lcd.print("V 1");
            }  
        delay(1000);
      }                     
// Se Apaga el rele de la Resistencia
      digitalWrite(RELE2,HIGH);          
      lcd.setCursor (0,0);
      lcd.print("RESISTENCIA  OFF"); 
      lcd.setCursor (4,1);
      lcd.print("R 0");
// Se enciende el rele de la Turbina de Aire
      //delay(2000);
      //digitalWrite(RELE3,LOW);          
      //lcd.setCursor (0,0);
      //lcd.print("TURBINA AIRE  ON"); 
      //lcd.setCursor (8,1); 
      //lcd.print("V 1");
      //delay(2000);
      //lcd.setCursor (0,0);
      //lcd.print("DETECTAR LLAMA ?"); 
}
void carganormal()
{
// Se enciende el rele de Carga
      digitalWrite(RELE4,LOW);             
      lcd.setCursor (0,0); lcd.print("Carga Pellet    ");
// Se espera 5 segundos
      for (int i = 3; i >=0; i--)
      { lcd.setCursor (13,0); lcd.print("   ");
        lcd.setCursor (14,0); lcd.print(i);
        delay(500);
        tempdatos();
      }
// Se apaga el rele de Carga Inicial      
      digitalWrite(RELE4,HIGH);
      lcd.setCursor (0,0); lcd.print("Pausa Pellet    ");      
// Se espera 25 segundos
      for (int i = 25; i >=0; i--)
      { 
        lcd.setCursor (13,0); lcd.print("   ");
        lcd.setCursor (14,0); lcd.print(i);
        delay(500);
        tempdatos();
      }
}  
void setup()
   {
// Inicializar el display con 16 caraceres 2 lineas
      lcd.begin (16,2);    
      lcd.setBacklightPin(3,POSITIVE);
      lcd.setBacklight(HIGH);
      lcd.home ();
//Inicializar Sensores Temperatura
      sensors.begin();
// Se configuran los pines RELES como salidas
      pinMode(RELE1, OUTPUT);       
      pinMode(RELE2, OUTPUT);
      pinMode(RELE3, OUTPUT);
      pinMode(RELE4, OUTPUT);
// Se configura los parametros PIN para el control de la LLAMA
      pinMode(isFlamePin, INPUT);
// Se colocan los estados de los Reles a HIGH (apagados)
      digitalWrite(RELE1,HIGH);
      digitalWrite(RELE2,HIGH);
      digitalWrite(RELE3,HIGH);
      digitalWrite(RELE4,HIGH);
// Se envia mensaje al LCD.
      lcd.backlight();
      lcd.setCursor (0,0);
      lcd.print("Caldera   Pellet");
      delay(1000);
      lcd.setCursor (0,1);
      lcd.print("Zagi Palma  2016");
      delay(2000);
// Colocacion Caracteres en Pantalla LCD
      lcd.clear();
      lcd.setCursor (1,0);
      lcd.print("INICIO PROCESO");
      lcd.setCursor (0,1);
      lcd.print("B 0"); //Bomba Recirculacion
      lcd.setCursor (4,1);
      lcd.print("R 0"); //Resistencia
      lcd.setCursor (8,1);
      lcd.print("V 0"); //Ventilador
      lcd.setCursor (13,1);
      lcd.print("S 0"); //Sinfin
      delay(3000);  
}
//Bucle de puesta en marcha quemador y alimentacion pellet
void loop()
{
//   tempdatos();
   sensors.requestTemperatures();
   t = (sensors.getTempCByIndex(0)); 
    if(t<70 && swt==0) { swtt = 1; }  //Caldera Fria. Arranque Inicial
    if(t<70 && swt==1) { swtt = 2; }  //Calentando hasta temperatura consigna alta (70º)
    if(t>=70 && swt==1) { swtt = 3; } //Caliente. Ha llegado a la temperatura de consigna alta (70º)
    if(((t>50)&&(t<=65))&&(swt==2)) { swtt = 4; } //Esperando. Espera la temperatura de consigna baja (50º)
    if(t<=50 && swt==2) { swtt = 5; } // Detectado 45º o menos y empieza de nuevo la rutina.
   switch(swtt)
   {
      case(1):
        cargainicial();
        swt=1;
        temptexto();
        lcd.setCursor (0,0);
        lcd.print("ESPERANDO  LLAMA");
        break;
      case(2):
        sensors.requestTemperatures(); tempdatos();
        if(digitalRead(isFlamePin) == LOW)
          {
            digitalWrite(RELE2,HIGH); //rele resistencia
            lcd.setCursor (0,0);
            lcd.print("                ");
            carganormal();
            lcd.setCursor (0,0);
            lcd.print("ESPERANDO  LLAMA");
          } 
        break;
      case(3):
            swt=2;
            lcd.setCursor (0,0); lcd.print("TURB. OFF EN    ");
      // Se apaga el rele de la Turbina de Aire
      // Se espera 1 minutos
            for (int i = 60; i >=0; i--)
            { 
            lcd.setCursor (13,0); lcd.print("   ");
            lcd.setCursor (13,0); lcd.print(i);
            delay(800);
            tempdatos();
            }
            digitalWrite(RELE3,HIGH);
            delay(2000);
            lcd.setCursor (0,0); lcd.print("ESPERANDO 50 Grd");          
        break;
      case(4):
            delay(1000);
            tempdatos();
        break;
      case(5):
            swt=0;
        break;
   }
}
