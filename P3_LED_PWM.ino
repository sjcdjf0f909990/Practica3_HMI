#include <Controllino.h>            // Librería específica para el hardware Controllino
#include "Stone_HMI_Define.h"      // Define elementos y funciones para trabajar con HMI Stone
#include "Procesar_HMI.h"          // Librería personalizada para procesar comandos HMI

// Pines PWM conectados a los LEDs
const int led1 = CONTROLLINO_D0;
const int led2 = CONTROLLINO_D1;

// Entradas digitales conectadas a los botones físicos
const int boton1 = CONTROLLINO_I16;
const int boton2 = CONTROLLINO_I17;

// Variables para almacenar el estado actual (encendido/apagado) de cada LED
bool estado_led1 = false;
bool estado_led2 = false;

// Variables para manejo de antirrebote
bool last_boton1 = LOW;
bool last_boton2 = LOW;
unsigned long t_last1 = 0;
unsigned long t_last2 = 0;
const unsigned long t_antirrebote = 10;  // Tiempo mínimo entre lecturas válidas

void setup() {
  Serial.begin(115200);   // Monitor serial para depuración
  Serial2.begin(115200);  // Comunicación con la HMI por puerto serial 2

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(boton1, INPUT);
  pinMode(boton2, INPUT);

  HMI_init();  // Inicializa el sistema de comunicación con la HMI

  // Inicializa ambos Spin Box de la interfaz con valor 0
  Stone_HMI_Set_Value("spin_box", "spin_box1", NULL, 0);
  Stone_HMI_Set_Value("spin_box", "spin_box2", NULL, 0);
}

void loop() {
  unsigned long t_now = millis();  // Tiempo actual para antirrebote

  // Lectura con antirrebote del botón 1
  bool btn1 = digitalRead(boton1);
  if (btn1 != last_boton1 && (t_now - t_last1) > t_antirrebote) {
    t_last1 = t_now;
    if (btn1 == HIGH) {
      estado_led1 = !estado_led1;  // Alterna el estado del LED 1
    }
  }
  last_boton1 = btn1;

  // Lectura con antirrebote del botón 2
  bool btn2 = digitalRead(boton2);
  if (btn2 != last_boton2 && (t_now - t_last2) > t_antirrebote) {
    t_last2 = t_now;
    if (btn2 == HIGH) {
      estado_led2 = !estado_led2;  // Alterna el estado del LED 2
    }
  }
  last_boton2 = btn2;

  // Lectura del valor de los Spin Box desde la interfaz HMI (rango 0–100)
  float duty1 = HMI_get_value("spin_box", "spin_box1");
  float duty2 = HMI_get_value("spin_box", "spin_box2");

  // Aplicación de PWM al LED 1 si está habilitado
  if (estado_led1 && duty1 >= 0 && duty1 <= 100) {
    int pwm1 = map(duty1, 0, 100, 0, 255);  // Escala de 0–100 a 0–255
    analogWrite(led1, pwm1);
  } else {
    analogWrite(led1, 0);
  }

  // Aplicación de PWM al LED 2 si está habilitado
  if (estado_led2 && duty2 >= 0 && duty2 <= 100) {
    int pwm2 = map(duty2, 0, 100, 0, 255);
    analogWrite(led2, pwm2);
  } else {
    analogWrite(led2, 0);
  }

  // Impresión por consola del valor PWM actual de ambos LEDs
  Serial.print("LED1: ");
  Serial.print("| PWM1: ");
  Serial.print(duty1);
  Serial.print("%\t");

  Serial.print("LED2: ");
  Serial.print("| PWM2: ");
  Serial.print(duty2);
  Serial.println("%");
}
