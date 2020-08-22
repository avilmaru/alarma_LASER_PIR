// ---------------------------------------------------------------------------
// Ejemplo alarma con PIR y laser con módulos Grove:  - v1.0 - 20/08/2020
//
// AUTOR:
// Creado por Angel Villanueva - @avilmaru
//
// LINKS:
// Blog: http://www.mecatronicalab.es
//
//
// HISTORICO:
// 20/08/2020 v1.0 - Release inicial.
//
// ---------------------------------------------------------------------------

#include <Wire.h>
#include "rgb_lcd.h"
#include <Digital_Light_TSL2561.h>

const int TIEMPO_VERIFICACION = 60;

// Red led button
const int PIN_RED_LED_BUTTON = 5;
const int PIN_RED_BUTTON = 6;

// Blue led button
const int PIN_BLUE_LED_BUTTON = 7;
const int PIN_BLUE_BUTTON = 8;

const int PIN_PIR_SENSOR = 2;
const int PIN_BUZZER = 3;

String estadoBotonRojo = "OFF";
String estadoBotonAzul = "OFF";

unsigned int valueSensor;

rgb_lcd lcd;

void setup() {
  
  pinMode(PIN_RED_LED_BUTTON, OUTPUT);
  pinMode(PIN_RED_BUTTON, INPUT);
  
  pinMode(PIN_BLUE_LED_BUTTON, OUTPUT);
  pinMode(PIN_BLUE_BUTTON, INPUT);
  
  pinMode(PIN_PIR_SENSOR, INPUT);
  pinMode(PIN_BUZZER, OUTPUT); 

  Wire.begin();
  TSL2561.init();
  
  lcd.begin(16, 2);
  lcd.clear();  
  lcd.setRGB(255, 255, 255);  
  lcd.setCursor(3,0);
  lcd.print("LASER / PIR");
  lcd.setCursor(6,1);
  lcd.print("Alarm");   
 
  delay(2000);
  laserTest();
  mostrarEstadoAlarmas();
  delay(1000);
    
}

void loop() {

  lecturaPulsadorRojo();
  lecturaPulsadorAzul();

  // Sensor Laser 
  if ((estadoBotonRojo == "ON") || (estadoBotonAzul == "ON"))
  {
    valueSensor = TSL2561.readFSpecLuminosity();
    if (valueSensor < 65535)
      alarma("LASER");
  }

  // Sensor PIR
  if (estadoBotonAzul == "ON")
  {
    if (digitalRead(PIN_PIR_SENSOR))
      alarma("PIR");
  }

 
}

void mostrarEstadoAlarmas()
{
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("LASER Alarm: ");

    if ((estadoBotonRojo == "ON") || (estadoBotonAzul == "ON"))
    {
      lcd.print("ON"); 
      lcd.setRGB(0, 255, 0);
    }
    else
    {
      lcd.print("OFF"); 
      lcd.setRGB(255, 255, 255);
    }
     
    lcd.setCursor(0,1);  
    lcd.print("PIR Alarm  : ");

    if (estadoBotonAzul == "ON")
      lcd.print("ON");
    else
      lcd.print("OFF");
}

void alarma(String sensor)
{

  lcd.clear();  
  lcd.setRGB(255, 0, 0);  
  lcd.setCursor(0,0);
  lcd.print("Alarm ! ! ! !");  
  lcd.setCursor(0,1);
  lcd.print(sensor); 
  lcd.print(" sensor");   
 
  String alarma = "ON";
  bool estado = 0;
  
  do {

    if (estado == 1)
      estado = 0;
    else
      estado = 1;

   digitalWrite(PIN_BUZZER, estado);  
      
   if (estadoBotonRojo == "ON")
   {
     digitalWrite(PIN_RED_LED_BUTTON,estado); 
     lecturaPulsadorRojo();
     alarma = estadoBotonRojo;
   }
   else if (estadoBotonAzul == "ON")
   {
     digitalWrite(PIN_BLUE_LED_BUTTON,estado); 
     lecturaPulsadorAzul();
     alarma = estadoBotonAzul;
   }

  } while (alarma == "ON");

  estadoBotonAzul = "OFF";
  estadoBotonRojo = "OFF";
  digitalWrite(PIN_BUZZER, LOW);  
  mostrarEstadoAlarmas();
  delay(500);
  
}


void sonidoBoton() 
{
    
  digitalWrite(PIN_BUZZER, HIGH);
  delay(30);
  digitalWrite(PIN_BUZZER, LOW);
  
}

void activarAlarma(String botonPulsado) 
{

  int PIN;
  
  if (botonPulsado == "ROJO")
  {
     PIN = PIN_RED_LED_BUTTON; 
     digitalWrite(PIN_BLUE_LED_BUTTON,LOW);
     estadoBotonAzul == "OFF";
  }
  else if (botonPulsado == "AZUL")
  {
    PIN = PIN_BLUE_LED_BUTTON; 
    digitalWrite(PIN_RED_LED_BUTTON,LOW);
    estadoBotonRojo == "OFF";
  }
    
  lcd.setRGB(255, 255, 255);  
  
  for (int i = 10; i > 0; i--) 
  {

    lcd.clear();  
    lcd.setCursor(0,0);
    lcd.print("Alarm active in:");  
    lcd.setCursor(8,1);
    lcd.print(i);  
    
    digitalWrite(PIN_BUZZER, HIGH);
    digitalWrite(PIN,HIGH); 
    if (i<5)
      delay(250);
    else
      delay(500);
   
    digitalWrite(PIN_BUZZER, LOW);
    digitalWrite(PIN,LOW); 
    
    if (i<5)
      delay(250);
    else
      delay(500);
   }  
   
}



void laserTest()
{

  lcd.clear();  
  lcd.setRGB(255, 255, 0);  
  lcd.setCursor(0,0);
  lcd.print("LASER pointer");  

  String pulsacion = "KO";
  bool estado = 0;
  
  do {

    if (estado == 1)
      estado = 0;
    else
      estado = 1;

    valueSensor = TSL2561.readFSpecLuminosity();
    if (valueSensor == 65535)
    {
      lcd.setRGB(0, 255, 0);  
      lcd.setCursor(0,1);
      lcd.print("TEST: OK"); 
      digitalWrite(PIN_RED_LED_BUTTON,estado); 
      
      int lecturaPULSADOR = lecturaPin(PIN_RED_BUTTON);
      if (lecturaPULSADOR == LOW)  //  default on LED BUTTON is high
        pulsacion = "OK";
    }else
    {
      lcd.setRGB(255, 0, 0);  
      lcd.setCursor(0,1);
      lcd.print("TEST: KO"); 
      digitalWrite(PIN_RED_LED_BUTTON,LOW); 
      pulsacion ="KO";
    }
      
  } while ((valueSensor != 65535) || (pulsacion != "OK"));

   digitalWrite(PIN_RED_LED_BUTTON,LOW); 
   sonidoBoton();
   
}


void lecturaPulsadorAzul()
{
  
  // Lectura del pulsador (PIR)
  int lecturaPULSADOR = lecturaPin(PIN_BLUE_BUTTON);
  if (lecturaPULSADOR == LOW)  //  default on LED BUTTON is high
  {
    
    sonidoBoton();
    if (estadoBotonAzul == "OFF"){
      digitalWrite(PIN_BLUE_LED_BUTTON,HIGH);
      estadoBotonAzul = "ON"; 
      activarAlarma("AZUL");
    } else {
      digitalWrite(PIN_BLUE_LED_BUTTON,LOW);
      estadoBotonAzul ="OFF";  
    }
   
    mostrarEstadoAlarmas();
   
  }
  
}


void lecturaPulsadorRojo()
{
  // Lectura del pulsador
  int lecturaPULSADOR = lecturaPin(PIN_RED_BUTTON);
  if (lecturaPULSADOR == LOW)  //  default on LED BUTTON is high
  {
   
   sonidoBoton();
   if (estadoBotonRojo == "OFF"){
      digitalWrite(PIN_RED_LED_BUTTON,HIGH);
      estadoBotonRojo = "ON";
      activarAlarma("ROJO");
    } else {
      digitalWrite(PIN_RED_LED_BUTTON,LOW);
      estadoBotonRojo ="OFF";  
    }
     
    mostrarEstadoAlarmas();
     
  }

}
  
boolean lecturaPin(int pin)
{
    boolean estado;
    boolean estadoAnterior;
    
    estadoAnterior = digitalRead(pin);  
    for(int i=0; i < TIEMPO_VERIFICACION; i++)
    {
      delay(1);                       // esperar 1 milisegundo
      estado = digitalRead(pin);     
      if( estado != estadoAnterior)  // si el estado cambia -> resetear el contador y guardar el estado
      {
        i = 0; 
        estadoAnterior = estado;
      }
    }
    
    // si hemos llegado aqui el estado se ha mantenido estable (no ha cambiado) por un periodo de tiempo y damos por válida la lectura
    return estado;
}
