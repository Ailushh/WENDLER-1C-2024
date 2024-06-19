#include <Arduino.h>
#include <IRremote.h>

const uint16_t irSignal[112] = {
//Código para encendido de aire acondicionado marca Surrey
      111,4400,4250,650,1500,650,450,600,1550,600,1550,650,450,
      600,450,650,1500,650,450,650,450,600,1550,600,500,600,450,
      650,1500,650,1550,600,450,600,1550,650,1500,650,450,600,1550,
      650,1500,650,1500,650,1500,650,1500,650,1550,600,450,650,1500,
      650,450,600,500,600,450,650,450,600,450,650,450,650,450,600,1550,
      600,450,650,1550,600,450,650,450,600,450,650,450,650,1500,650,450,
      600,1550,650,450,600,1550,600,1550,600,1550,600,1550,650,5050,4450,
      4250,600,1550,600,450,650,1550,600,1550,600
};

const uint16_t irSignal2[92] = {91,8950,4400,500,600,550,550,650,1550,600,550,550,1650,650,
450,600,600,500,600,550,550,550,1650,600,550,550,550,600,550,550,1650,550,1650,600,1650,550,600,550,
550,550,550,600,550,550,550,550,600,550,550,550,600,550,600,550,550,550,600,550,550,550,550,550,600,550,550,550,
600,550,550,600,550,550,550,600,550,550,600,500,600,550,550,550,650,500,1650,550,600,550,1650,550,550,550
};

const uint16_t irSignal3[112] = {91,8950,4350,550,550,650,500,550,1650,550,550,600,550,550,600,500,600,600,500,650,500,650,1550,550,1650,600,1650,600,550,600,1550,650,1600,600,1600,650,500,600,500,650,500,600,500,650,500,600,500,650,450,650,500,650,500,600,500,650,500,600,500,650,500,600,500,650,500,600,500,650,500,600,500,650,500,600,500,650,500,600,500,650,500,600,500,600,1650,600,1600,650,1550,650,1550,650};

int ac_repeat = 100;
IRsend sender(3); //Pin por el cual vamos a emitir los datos IR

void setup() {
  Serial.begin(9600);
  pinMode(7, INPUT); //GPIO de control 
}
void loop() {

  if (digitalRead(7)==HIGH){
  //for(int i = 0; i < ac_repeat; i++) 
    // {
      sender.sendRaw(irSignal, 112 , 38);//la señal que quiero transmitir, el tamaño del arreglo y en que frecuencia (en kHz)
      Serial.println("Emitiendo encendido: ");
      //Serial.println(i);
      delay(2000);
     //}
  }
  if (digitalRead(7)==LOW){
      sender.sendRaw(irSignal3, 112 , 38);
      Serial.println("Emitiendo apagado: ");
      delay(2000);
  }
 
}
