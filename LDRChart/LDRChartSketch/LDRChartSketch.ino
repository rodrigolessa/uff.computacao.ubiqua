#include <Javino.h>
Javino javino;
String  strMsgLum = "temp=1|lumin=";
int pinoLDR = A0;
int value;

void setup() {
  pinMode(pinoLDR, INPUT); // Set pResistor - A0 pin as an input (optional)
  Serial.begin(9600);
}

void loop() {
  handleSerialCmd();
}

void handleSerialCmd(){
  while (Serial.available() > 0) {
    char incomingCommand = Serial.read();
    switch (incomingCommand){
      case 'l':
        value = analogRead(pinoLDR);
        javino.sendmsg(strMsgLum + value);
        //Serial.println(value); // exibindo no console a resistencia do sensor LDR
        break;
    }
  } 
}
