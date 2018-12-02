/////////////////////////////////////////////////
// Bibliotecas

#include <SoftwareSerial.h>
#include <Servo.h>
#include <Ultrasonic.h>
#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>

/////////////////////////////////////////////////
// Definindo pinos (digitais) para atuadores

#define ledWarning  7  // D7
#define pinoFT      9  // D9  ~ pwm
#define pinoLDR     10 // D10 ~ pwm
#define pinServo    11 // D11 ~ pwm
#define pinBuzzer   12 // D12
#define pinBtnPanic 4 // D4
#define pinBtn      8 // D8

// Definindo pinos para sensores

#define pinTRIGGER  2  // D2 - Arduino pin tied to trigger pin on the ultrasonic sensor.
#define pinECHO     5  // D5 ~ pwm - Arduino pin tied to echo pin on the ultrasonic sensor.

// Alguns pinos têm capacidades especiais. 
// Por exemplo, os pinos digitais 3, 5, 6, 9, 10 e 11, no Arduino Nano, 
// permitem gerar saída analógica usando PWM.

/////////////////////////////////////////////////
// Parâmetros de controle

#define minDistance 5
#define maxDistance 250
// Maximum distance we want to ping for (in centimeters).
// Maximum sensor distance is rated at 400-500cm. 

#define intFrequence 10
// Wait 50ms between pings (about 20 pings/sec). 
// 29ms should be the shortest delay between pings.

// Medidas em centimetros
float longe = 100;
float medio = 60;
float perto = 30;

// Servo position in degrees
int servoAngleMax = 90;
int servoAngleInit = 40;

// BUZZER
// * frequência: a frequência da nota em Hertz 
// * duração: a duração da nota em milissegundos (parâmetro opcional)
// Nota  =  Frequência (Hz)
// Dó  - C4 = 261.63
// Ré  - D4 = 293.66
// Mi  - E4 = 329.63
// Fá  - F4 = 349.23
// Sol - G4 = 392.00
// Lá  - A4 = 440.00
// Si  - B4 = 493.88
// Dó  - C5 = 523.25

float buzzDuration = 100;
float buzzFrequence = 261.63;

bool standby = false;

/////////////////////////////////////////////////
// Instânciando sensores e atuadores

// Instância o sensor de distância com pinos PWN
Ultrasonic ultrasonic(pinTRIGGER, pinECHO);

// Instância servo motor
Servo servo;

/////////////////////////////////////////////////
// ThreadController that will controll all threads
ThreadController conthrol = ThreadController();

// Thread to control servo and ultrasonic sensor (not pointer)
Thread ultrasonicReading = Thread();

// Thread to control infrared sensor (not pointer)
Thread infraredReading = Thread();

// Thread to control actions buttons (not pointer)
Thread ButtonReading = Thread();

// Servo controller
int direcao = 1;
int posicao = servoAngleInit;

// Callback for ultrasonicReading
void ultrasonicCallback() {

  if (standby == false) {
  
    //for(int i = servoAngleInit; i <= servoAngleMax; i++) {
      //servo.write(i);
      //objectWarning(medianDistance());
      //delay(intFrequence);
    //}
  
    // Repeats the previous lines from 120 to 20 degrees
    //for(int i = servoAngleMax; i > servoAngleInit; i--) {
      //servo.write(i);
      //objectWarning(medianDistance());
      //delay(intFrequence);
    //}
  
    if (posicao >= servoAngleMax){
      direcao = 0;
    } else if (posicao <= servoAngleInit){
      direcao = 1;
    }
  
    if (direcao == 1){
      posicao = posicao + 1;
    } else {
      posicao = posicao - 1;
    }
    
    servo.write(posicao);
    objectWarning(medianDistance());

  }

  //Serial.print("Ultrasonic. I'm running on: ");
  //Serial.println(millis());
}

// Callback
void infraredCallback(){

  if (standby == false) {

    // Armazenar o valor lido do LDR
    //int lum1 = analogRead(pinoLDR);
    // Valor lido pelo fototransistor do sensor Infrared
    int infr1 = digitalRead(pinoFT);
    delay(25);
    
    //int lum2 = analogRead(pinoLDR);
    int infr2 = digitalRead(pinoFT);
    delay(25);
    
    //int lum3 = analogRead(pinoLDR);
    int infr3 = digitalRead(pinoFT);
    delay(25);
    
    //int lum4 = analogRead(pinoLDR);
    int infr4 = digitalRead(pinoFT);
  
    //int lumTotal = lum1 + lum2 + lum3 + lum4;
    int infTotal = infr1 + infr2 + infr3 + infr4;
  
    //float lumMedia = lumTotal / 4;
    float infMedia = infTotal / 4;
        
    //Serial.print("Luminosidade: "); // exibindo no console a resistencia do sensor LDR
    //Serial.println(lumMedia); // exibindo no console a resistencia do sensor LDR
  
    //Serial.print("Infrared: ");
    //Serial.println(infMedia);
  
    // Verifica se o objeto foi detectado 
    if (infMedia != 1)
    {
      //Serial.println("Objeto: Detectado");
      Serial.println("Infrared:true");
    }

  }

  //Serial.print("Infrared. I'm running on: ");
  //Serial.println(millis());
}

// Callback
void ActionButtonCallback(){

  // Botões de ação - Pânico
  if (digitalRead(pinBtnPanic) == 1){
    Serial.print("PanicButton:true");
    Standby(true);
    tone(pinBuzzer, buzzFrequence, buzzDuration * 5);
  }

  // Botões de ação - Reiniciar
  if (digitalRead(pinBtn) == 1){
    Serial.print("RebootButton:true");
    Standby(false);
    noTone(pinBuzzer);
  }
}

/////////////////////////////////////////////////
// Usar o sensor ultrassonico para calcular a distância de objetos. Usando média de três medidas

float medianDistance() {

  float d1 = ultrasonic.distanceRead();
  delay(5);

  float d2 = ultrasonic.distanceRead();
  delay(5);

  float d3 = ultrasonic.distanceRead();
  delay(5);

  float d4 = ultrasonic.distanceRead();

  float total = d1 + d2 + d3 + d4;
  float distance = total / 4;

  //Serial.print("Distancia em cm: ");
  //Serial.println(distance);

  return distance;
}

void objectWarning(float distance) {

  // Desligar o alerta
  noTone(pinBuzzer);

  // Se zero não houve retorno. Se maior que o distância máxima controlada não é necessário avisar
  if (distance > minDistance && distance < maxDistance) {
    
    if (distance < perto) {
      
      tone(pinBuzzer, buzzFrequence, buzzDuration);

      Serial.println("Ultrasonic:perto");

      float distanceCheck = medianDistance();
    
      if (distance > minDistance && distance < maxDistance && distance < perto) {
        
        delay(intFrequence);

        distanceCheck = medianDistance();

        if (distance > minDistance && distance < maxDistance && distance < perto) {
          delay(intFrequence);
        }
      }
      
    } else if (distance < medio) {
      
      tone(pinBuzzer, buzzFrequence, buzzDuration);

      Serial.println("Ultrasonic:medio");

      float distanceCheck = medianDistance();
    
      if (distance > minDistance && distance < maxDistance && distance < medio) {
        
        delay(intFrequence);
      }
      
    } else if (distance < longe) {
      
      tone(pinBuzzer, buzzFrequence, buzzDuration / 2);
      
      Serial.println("Ultrasonic:longe");
    }
  }
}

/////////////////////////////////////////////////
// Mover o servo motor, suavemente, para a posição desejada

void moveTo(int servoAngle) {
  
  int currentPosition = servo.read();

  //Serial.print("Posição: ");
  //Serial.println(currentPosition);
  
  if (currentPosition > servoAngle) {
    for (int i = currentPosition; i > servoAngle; i--) {
      servo.write(i);
      delay(intFrequence / 2);
    }
  } else if (currentPosition < servoAngle) {
    for (int i = currentPosition; i <= servoAngle; i++){
      servo.write(i);
      delay(intFrequence / 2);
    }
  }
}

/////////////////////////////////////////////////
// Ativar ou desativar funções

void Standby(bool action) {
  
  if (action == true){
    standby = true;
    Serial.println("Standbying...");  // exbindo somente para testes
    //digitalWrite(ledWarning, LOW); // desligar o LED
    //noTone(pinBuzzer); // Desligar o alerta
    //moveTo(servoAngleMax);
    
  } else if (action == false){
    standby = false;
    Serial.println("Restarting...");  // exbindo somente para testes
  }
}

/////////////////////////////////////////////////
// Controle de entrada e saída de comandos pelo Bluetooth

void handleSerial() {

  //while (Serial.available() > 0) {
  if (Serial.available() > 0) {
    
    char incomingChar = Serial.read();

    switch (incomingChar) {
      case '+':
        Standby(false);
        break;
      case '-':
        Standby(true);
        break;
    }
  }
}

/////////////////////////////////////////////////
// Esta função "setup" roda uma vez quando a placa e ligada ou resetada

void setup() {
  
  Serial.begin(9600);

  pinMode(pinoLDR, INPUT); // declarando pino de entrada
  pinMode(ledWarning, OUTPUT); // declarando pino de saída
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinoFT, INPUT); //Pino ligado ao coletor do fototransistor
  pinMode(pinBtnPanic, INPUT);
  pinMode(pinBtn, INPUT);

  // Iniciar motor servo e reposicionar
  servo.attach(pinServo);

  moveTo(servoAngleInit);

  // Configure threads
  ultrasonicReading.onRun(ultrasonicCallback);
  ultrasonicReading.setInterval(5);

  infraredReading.onRun(infraredCallback);
  infraredReading.setInterval(100);

  ButtonReading.onRun(ActionButtonCallback);
  ButtonReading.setInterval(100);

  // Adds both threads to the controller
  conthrol.add(&ultrasonicReading);
  conthrol.add(&infraredReading); // & to pass the pointer to it
  conthrol.add(&ButtonReading); // & to pass the pointer to it
}

/////////////////////////////////////////////////
// Função que se repete infinitamente

void loop() {

  // Run ThreadController
  // this will check every thread inside ThreadController,
  // if it should run. If yes, he will run it;
  conthrol.run();

  handleSerial();

  if (standby == false) {
    digitalWrite(ledWarning, HIGH); // ligar o LED
  } else {
    digitalWrite(ledWarning, LOW); // desligar o LED
  }
}
