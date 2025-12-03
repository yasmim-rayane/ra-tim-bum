#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/* --- Variáveis globais --- */

// Controle do sensor LDR
const int LIMIAR_ESCURIDAO = 500;  // Valor abaixo do qual considera-se escuro (ajuste conforme necessário)
bool dragaoAtivado = false;

/* Pinos */
// Olhos do dragão
const int OLHO_ESQUERDO = 2;
const int OLHO_DIREITO = 4;

// Ventoinha na boca do dragão
const int PINO_VENTOINHA = 9;

// DFPlayer Mini
const int ARDUINO_RX = 13;  // Arduino TX -> DFPlayer RX
const int ARDUINO_TX = 12;  // Arduino RX -> DFPlayer TX
SoftwareSerial mySerial(ARDUINO_RX, ARDUINO_TX);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;

// Sensor de luminosidade LDR
const int PINO_LDR = A0;

// Luzes do templo
const int PINO_LUZ1_TEMPLO = A1;
const int PINO_LUZ2_TEMPLO = A2;
const int PINO_LUZ3_TEMPLO = A3;
const int PINO_LUZ4_TEMPLO = 7;

// Fita RGB
const int PINO_GREEN_RGB = 6;
const int PINO_RED_RGB = 5;
const int PINO_BLUE_RGB = 3;

// Display LCD I2C (endereço 0x27, 16 colunas, 2 linhas)
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* Protótipo de funções */
void fadeRGB();
void acenderLuzesTemplo();
void verificarLDR();
void ativarDragao();

void setup(){
    /* --- Definição de pinos --- */
    // Olhos do dragão
    pinMode(OLHO_ESQUERDO, OUTPUT);
    pinMode(OLHO_DIREITO, OUTPUT);
    
    // Ventoinha
    pinMode(PINO_VENTOINHA, OUTPUT);
    
    // Sensor LDR (entrada analógica)
    pinMode(PINO_LDR, INPUT);
    
    // Luzes do templo
    pinMode(PINO_LUZ1_TEMPLO, OUTPUT);
    pinMode(PINO_LUZ2_TEMPLO, OUTPUT);
    pinMode(PINO_LUZ3_TEMPLO, OUTPUT);
    pinMode(PINO_LUZ4_TEMPLO, OUTPUT);
    
    // Fita RGB
    pinMode(PINO_GREEN_RGB, OUTPUT);
    pinMode(PINO_RED_RGB, OUTPUT);
    pinMode(PINO_BLUE_RGB, OUTPUT);
    
    // Inicializar todos os componentes desligados
    digitalWrite(OLHO_ESQUERDO, LOW);
    digitalWrite(OLHO_DIREITO, LOW);
    digitalWrite(PINO_VENTOINHA, LOW);
    digitalWrite(PINO_GREEN_RGB, LOW);
    digitalWrite(PINO_RED_RGB, LOW);
    digitalWrite(PINO_BLUE_RGB, LOW);
    
    // Acender luzes do templo
    acenderLuzesTemplo();

    /* Inicialização do Display LCD */
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ra-Tim-Bum");
    lcd.setCursor(0, 1);
    lcd.print("Aguardando...");

    /* Inicialização do DFPlayer */
    Serial.begin(9600);
    mySerial.begin(9600);
    if (!myDFPlayer.begin(mySerial)) {
        Serial.println(F("Aviso: DFPlayer nao inicializado. Audio sera ignorado."));
    } else {
        myDFPlayer.volume(20);
    }
}

void loop(){
    fadeRGB();
    acenderLuzesTemplo();
    verificarLDR();
}

/* --- Definição de funções --- */

void verificarLDR() {
    int valorLDR = analogRead(PINO_LDR);
    
    // Se detectar escuridão e o dragão ainda não foi ativado
    if (valorLDR < LIMIAR_ESCURIDAO && !dragaoAtivado) {
        ativarDragao();
        dragaoAtivado = true;
    }
}

void ativarDragao() {
    // Acender os olhos do dragão
    digitalWrite(OLHO_ESQUERDO, HIGH);
    digitalWrite(OLHO_DIREITO, HIGH);
    
    // Tocar áudio do rugido
    myDFPlayer.play(1);  // Toca o primeiro arquivo de áudio da pasta
    
    // Ligar a ventoinha
    digitalWrite(PINO_VENTOINHA, HIGH);
    
    // Atualizar display LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DRAGAO ATIVADO!");
    lcd.setCursor(0, 1);
    lcd.print("Cuidado!!!");
}

void acenderLuzesTemplo() {
    digitalWrite(PINO_LUZ1_TEMPLO, HIGH);
    digitalWrite(PINO_LUZ2_TEMPLO, HIGH);
    digitalWrite(PINO_LUZ3_TEMPLO, HIGH);
    digitalWrite(PINO_LUZ4_TEMPLO, HIGH);
}

void fadeRGB() {
    static unsigned long ultimoTempo = 0;
    static int corAtual = 0;  // 0=Amarelo, 1=Vermelho, 2=Laranja
    static int brilho = 0;
    static bool aumentando = true;
    
    unsigned long tempoAtual = millis();
    
    // Atualiza o fade a cada 10ms
    if (tempoAtual - ultimoTempo >= 10) {
        ultimoTempo = tempoAtual;
        
        if (aumentando) {
            brilho += 5;
            if (brilho >= 255) {
                brilho = 255;
                aumentando = false;
            }
        } else {
            brilho -= 5;
            if (brilho <= 0) {
                brilho = 0;
                aumentando = true;
                corAtual = (corAtual + 1) % 3;  // Avança para próxima cor (3 cores)
            }
        }
        
        // Define cores baseadas na cor atual
        switch (corAtual) {
            case 0:  // Amarelo (Vermelho + Verde)
                analogWrite(PINO_RED_RGB, brilho);
                analogWrite(PINO_GREEN_RGB, brilho);
                analogWrite(PINO_BLUE_RGB, 0);
                break;
            case 1:  // Vermelho
                analogWrite(PINO_RED_RGB, brilho);
                analogWrite(PINO_GREEN_RGB, 0);
                analogWrite(PINO_BLUE_RGB, 0);
                break;
            case 2:  // Laranja (Vermelho total + Verde parcial)
                analogWrite(PINO_RED_RGB, brilho);
                analogWrite(PINO_GREEN_RGB, brilho / 2);
                analogWrite(PINO_BLUE_RGB, 0);
                break;
        }
    }
}