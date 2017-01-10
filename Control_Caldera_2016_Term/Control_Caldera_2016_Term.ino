//Control Caldera SEB 2016 Termostato Ambiente

//Librerias LCD
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#define I2C_ADDR    0x27
LiquidCrystal_I2C             lcd(I2C_ADDR,2, 1, 0, 4, 5, 6, 7);

//Librerias Temperatura
#include <OneWire.h>
#include <DallasTemperature.h>

//Definicion Pines Control Reles

#define RELE1 2    // Bomba Recirculacion                
#define RELE2 3    // Resistencia                   
#define RELE3 4    // Turbina                   
#define RELE4 5    // Sinfin - Pellet

//Definicion  Sensores de Temperatura
#define ONE_WIRE_BUS 10
OneWire ourWire(ONE_WIRE_BUS);
DallasTemperature sensors(&ourWire);

//Definicion Control LLAMA
int isFlamePin = 6; 
int isFlame = HIGH; 

//Definicion Estado Temperatura
int t = 0;    // Temperatura
int swt = 0;  // Switch Temperatura
int swtt = 0; // Switch Select cambio Temperatura
int term = 7; // Termostato Ambiente Nest en Pin 7
int activo = 0; // Estado del caldeo

//Definicion Control servo motor turbina
Servo miServo;

void setup()
   {
// Se configura el Servo en el pin 9
      miServo.attach(9);
// Se configura PIN 7 para el control del Termostato y se activa como apagado
      pinMode(term, INPUT);
      digitalWrite(term, HIGH);
// Se configura PIN 6 para el control de la LLAMA
      pinMode(isFlamePin, INPUT);

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
      lcd.print("ZAGI PALMA  2016");      
      delay(2000);
}
//Bucle de puesta en marcha quemador y alimentacion pellet
void loop()
{
//   tempdatos();
   sensors.requestTemperatures();
   t = (sensors.getTempCByIndex(0)); 
    if(t<70 && swt==0) { swtt = 1; activo = 1; }  //Caldera Fria. Arranque Inicial
    if(t<70 && swt==1) { swtt = 2; activo = 1; }  //Calentando hasta temperatura consigna alta (70º)
    if(t>=70 && swt==1) { swtt = 3; activo =1; } //Caliente. Ha llegado a la temperatura de consigna alta (70º)
    if(((t>50)&&(t<=65))&&(swt==2)) { swtt = 4; activo =1; } //Esperando. Espera la temperatura de consigna baja (50º)
    if(t<=50 && swt==2) { swtt = 5; activo = 1; } // Detectado 50º o menos y empieza de nuevo la rutina.
    if (digitalRead(term) == HIGH) {swtt = 6;} //Termostato en OFF

   switch(swtt)
   {
      case(1):
// Control estado Termostato      
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
            miServo.write(175);
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
      case(6):
//      lcd.clear();
            lcd.setCursor (0,0);
            lcd.print(" TERMOSTATO OFF ");
            temptexto();
            delay(1000);
            tempdatos();
            if (activo == 1) //Si termostato pasa a OFF y la caldera esta activa.
            {
// Se enciende la turbina para limpieza
      digitalWrite(RELE3,LOW); 
      lcd.setCursor (0,0);
      lcd.print(" TERMOSTATO OFF ");
      lcd.setCursor (0,1);
      lcd.print("LIMPIEZA CALDERA"); 
//      lcd.setCursor (8,1); 
//      lcd.print("T 1");
      miServo.write(175);
// Se espera 1 MINUTO      
      delay(60000);
      miServo.write(15);
// Se espera 1/2 MINUTO      
      delay(30000);
      digitalWrite(RELE3,HIGH);
      delay(4000);
      miServo.write(6);      
//      lcd.setCursor (8,1); 
//      lcd.print("T 0");
// Se apaga el rele de la bomba de Recirculacion despues de 3 minutos
           lcd.setCursor (0,1); lcd.print("RECC. OFF EN    ");
      // Se apaga el rele de la bomba de recirculacion
      // Se espera 1 minutos
            for (int i = 180; i >=0; i--)
            { 
            lcd.setCursor (13,1); lcd.print("   ");
            lcd.setCursor (13,1); lcd.print(i);
            }
            digitalWrite(RELE1,HIGH);
            activo = 0; swt = 0; swtt = 0;
            }
            if (digitalRead(term) == LOW) {swt = 0;} //Termostato en ON
        break;
   }
}

//Funciones
void temptexto()
{
  //lcd.clear();
  lcd.setCursor (0,1);
  lcd.print("C ");
  lcd.setCursor (8,1);
  lcd.print(" R ");
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
      lcd.print("T 0"); //Turbina
      lcd.setCursor (13,1);
      lcd.print("P 0"); //Sinfin - Aporte Pellet
      delay(6000);  
// Se enciende el rele de la bomba de Recirculacion
      digitalWrite(RELE1,LOW);
      lcd.setCursor (0,0);
      lcd.print("BOMBA RECIRCULA."); 
      lcd.setCursor (0,1);
      lcd.print("B 1");
// Se espera 2 segundos      
      delay(2000);
// Se enciende la turbina para limpieza
      digitalWrite(RELE3,LOW); 
      lcd.setCursor (0,0);
      lcd.print("LIMPIEZA CALDERA"); 
      lcd.setCursor (8,1); 
      lcd.print("T 1");
      miServo.write(175);
// Se espera 1 MINUTO      
      delay(60000);
      miServo.write(15);
// Se espera 1/2 MINUTO      
      delay(30000);
      digitalWrite(RELE3,HIGH);
      delay(4000);
      miServo.write(6);      
      lcd.setCursor (8,1); 
      lcd.print("T 0");

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
      lcd.print("P 1");
// Se espera 6 segundos
      delay(6000);                         
// Se apaga el rele de Carga Inicial
      digitalWrite(RELE4,HIGH);          
      lcd.setCursor (0,0);
      lcd.print("PAUSA === PELLET"); 
      lcd.setCursor (13,1);
      lcd.print("P 0");
// Se espera 5 segundos
      delay(5000);                        
// Se enciende el rele de Carga Inicial 
      digitalWrite(RELE4,LOW);           
      lcd.setCursor (0,0);
      lcd.print("CARGA === PELLET"); 
      lcd.setCursor (13,1);
      lcd.print("P 1");
// Se espera 6 segundos
      delay(6000);                           
// Se apaga el rele de Carga Inicial
      digitalWrite(RELE4,HIGH);          
      lcd.setCursor (13,1);
      lcd.print("P 0");
// Se espera 200 segundos. 
      lcd.setCursor (0,0);
      lcd.print("CAL. RESIST.    "); 
      for (int i = 200; i >=0; i--)
      { 
        lcd.setCursor (13,0); lcd.print("   ");
        lcd.setCursor (13,0); lcd.print(i);
          if(i==120) 
            { // Se enciende el rele de la Turbina de Aire 
              miServo.write(10
              );
              digitalWrite(RELE3,LOW); 
              lcd.setCursor (8,1); 
              lcd.print("T 1");
            }  
        delay(1000);
      }                     
// Se Apaga el rele de la Resistencia
      digitalWrite(RELE2,HIGH);          
      lcd.setCursor (0,0);
      lcd.print("RESISTENCIA  OFF"); 
      lcd.setCursor (4,1);
      lcd.print("R 0");
}
void carganormal()
{
// Se enciende el rele de Carga
      digitalWrite(RELE4,LOW);             
      lcd.setCursor (0,0); lcd.print("Carga Pellet    ");
// Se espera 4 segundos
      for (int i = 3;  i >=0; i--)
      { lcd.setCursor (13,0); lcd.print("   ");
        lcd.setCursor (14,0); lcd.print(i);
        delay(500);
        tempdatos();
      }
// Se apaga el rele de Carga Inicial      
      digitalWrite(RELE4,HIGH);
      lcd.setCursor (0,0); lcd.print("Pausa Pellet    ");      
// Se espera 25 segundos
      for (int i = 24; i >=0; i--)
      { 
        lcd.setCursor (13,0); lcd.print("   ");
        lcd.setCursor (14,0); lcd.print(i);
        delay(500);
        tempdatos();
      }
}  

