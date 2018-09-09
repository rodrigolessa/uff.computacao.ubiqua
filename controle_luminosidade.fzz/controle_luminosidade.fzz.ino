include <Javino.h>
Javino j;

int pinoLDR = A0; // pino de entrada do sensor de limunosidade
int pinoLED = 2; // pino de saída do LED vermelho
int pinoTemp = A1; // pino de entrada do sensor de temperatura
int lum = 0; // armazenar o valor lido do LDR
int temp = 20; // temperatura inicial em ºC
int tempWarning = 40; // temperatura superior ao normal
int tempDanger = 60; // temperatura de incêndio
int i = 0;

void setup() {
  pinMode(pinoLDR, INPUT); // declarando pino de entrada
  pinMode(pinoLED, OUTPUT); // declarando pino de saída
  Serial.begin(9600);
}

void loop() {
  if (j.availablemsg()){
    actions(j.getmsg());
  }

  //delay(1200); // Aguardar x millisegundos até a póxima verificação
}

void actions(String msg){
  
  // Verificando luminosidade
  if (msg == "readLum"){
    
    lum = 0;
    
    for (i = 0; i < 3; i = i + 1){
      lum = lum + analogRead(pinoLDR); // lendo o sensor
      delay(200);
    }
  
    lum = lum / 3;
    
    Serial.println(lum); // exibindo no console a resistencia do sensor LDR
  
    if (lum > 800){
      digitalWrite(pinoLED, LOW); // muito claro, desligar o LED
    }
    if (lum < 400){
      digitalWrite(pinoLED, HIGH); // ligar o LED
    }
  }

  // Verificando temperatura
  if (msg == "readLum"){

    temp = analogRead(pinoTemp); // lendo temperatura
    temp = temp * 0.488; // convertendo °C 
  
    Serial.print(temp);
    //Serial.println(" °C");  // exbindo somente para testes
  }
  
}
