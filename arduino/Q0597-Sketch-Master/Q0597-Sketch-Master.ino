/*
     CÓDIGO:  Q0597-Master
     AUTOR:   BrincandoComIdeias
     LINK:    https://www.youtube.com/brincandocomideias ; https://cursodearduino.net/ ; https://cursoderobotica.net
     COMPRE:  https://www.arducore.com.br/
     SKETCH:  Garagem MQTT - Esp01 Master
     DATA:    11/12/2019
*/

// INCLUSÃO DE BIBLIOTECAS
#include <A2a.h>
#include "config.h"

// DEFINIÇÕES DE PINOS
#define pinPortao1 12
#define pinPortao2 7
#define pinLed    13
#define pinIN1   9
#define pinIN2   10
#define pinSensor 5

// DEFINIÇÕES
#define endereco 0x08
#define tempoAtualizacao 10000
#define tempoConfirmacao 500

#define ABERTO 1
#define FECHADO 0
#define LIVRE 1
#define OCUPADO 0


// INSTANCIANDO OBJETOS
AdafruitIO_Feed *portao = io.feed("portao");
AdafruitIO_Feed *controle = io.feed("controle");
AdafruitIO_Feed *vaga = io.feed("vaga");

A2a arduinoSlave;

// DECLARAÇÃO DE FUNÇÕES
void configuraSlave();
void configuraMQTT();
void fechaPortao();
void abrePortao();
void pararPortao();

bool monitoraVaga(byte pin);

// DECLARAÇÃO DE VARIÁVEIS
unsigned long controleTempo = 0;

bool comandoRecebido = false;
bool estadoPortao = FECHADO;
bool estadoVaga = OCUPADO;

void setup() {
  Serial.begin(9600);
  while (! Serial);

  configuraMQTT();

  arduinoSlave.begin(0, 2);
  configuraSlave();

  Serial.println("Conferindo posição do portão");
  
  portao->get();
  io.run();
  
  if(estadoPortao == FECHADO){
    if(arduinoSlave.digitalWireRead(endereco, pinPortao1)){
      fechaPortao();      
    }
  }else{
    if(arduinoSlave.digitalWireRead(endereco, pinPortao2)){
      abrePortao();      
    }
  }
  
  Serial.println("Portão posicionado");
  
  Serial.println("Fim Setup");
}

void loop() {
  io.run();

  //Monitora o sensor da vaga
  if (millis() > controleTempo + tempoAtualizacao) {
    if (monitoraVaga(pinSensor)) {
      controleTempo = millis();
      vaga->save(estadoVaga);
    }
  }

  //Executa o comando para ABRIR ou FECHAR o portao
  if ( comandoRecebido ) {
    if (estadoPortao == FECHADO) {
      estadoPortao = ABERTO;
      portao->save(estadoPortao);
      io.run();
      
      abrePortao();
      comandoRecebido = false;
    } else {
      estadoPortao = FECHADO;
      portao->save(estadoPortao);
      io.run();
      
      fechaPortao();
      comandoRecebido = false;
    }
  }
}

// IMPLEMENTO DE FUNÇÕES
void controleMQTT(AdafruitIO_Data *data) {
  Serial.print("Controle Recebido <- ");
  Serial.println(data->value());
  comandoRecebido = true;
}

void portaoMQTT(AdafruitIO_Data *estado) {
  Serial.print("Portao Recebido <- ");
  estadoPortao = estado->toBool();
  Serial.println(estado->value());
}

bool monitoraVaga(byte pin) {
  static bool leituraAnt;
  static bool mediaLeitura[5];

  bool leitura = arduinoSlave.digitalWireRead(endereco, pin);
  estadoVaga = leitura;

  if (leitura != leituraAnt) {

    //Faz cinco leituras para filtrar interferencias
    for ( byte i = 0; i < 5 ; i++) {
      mediaLeitura[i] = arduinoSlave.digitalWireRead(endereco, pin);
      delay(tempoConfirmacao);
    }

    byte qtdAnterior = 0;
    byte qtdAtual = 0;

    for ( byte i = 0; i < 5 ; i++) {
      if (mediaLeitura[i] == leituraAnt) {
        qtdAnterior++;
      }
      else {
        qtdAtual++;
      }

    }

    if ( qtdAtual > qtdAnterior) {
      leituraAnt = leitura;
      return 1 ;
    } else {
      return 0;
    }

  } else {
    return 0;
  }

}

void fechaPortao() {
  arduinoSlave.digitalWireWrite(endereco, pinIN1, HIGH);
  arduinoSlave.digitalWireWrite(endereco, pinIN2, LOW);
  while (arduinoSlave.digitalWireRead(endereco, pinPortao1));
  pararPortao();
}

void abrePortao() {
  arduinoSlave.digitalWireWrite(endereco, pinIN1, LOW);
  arduinoSlave.digitalWireWrite(endereco, pinIN2, HIGH);
  while (arduinoSlave.digitalWireRead(endereco, pinPortao2));
  pararPortao();
}

void pararPortao() {
  arduinoSlave.digitalWireWrite(endereco, pinIN1, HIGH);
  arduinoSlave.digitalWireWrite(endereco, pinIN2, HIGH);
}

void configuraSlave() {
  Serial.println("configurando pinMode do Arduino");
  arduinoSlave.pinWireMode(endereco, pinLed, OUTPUT);
  arduinoSlave.pinWireMode(endereco, pinPortao2, INPUT_PULLUP);
  arduinoSlave.pinWireMode(endereco, pinPortao1, INPUT_PULLUP);
  arduinoSlave.pinWireMode(endereco, pinSensor, INPUT);
  arduinoSlave.pinWireMode(endereco, pinIN1, OUTPUT);
  arduinoSlave.pinWireMode(endereco, pinIN2, OUTPUT);
  arduinoSlave.digitalWireWrite(endereco, pinIN1, LOW);
  arduinoSlave.digitalWireWrite(endereco, pinIN2, LOW);
  Serial.println("Slave Configurado!");
}

void configuraMQTT() {
  Serial.print("Conectando ao Adafruit IO");
  io.connect();

  controle->onMessage(controleMQTT);
  portao->onMessage(portaoMQTT);

  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println(io.statusText());
}
