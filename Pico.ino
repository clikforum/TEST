// ---- INIZIO di Codice Pico ---- NON ELIMINARE
// ---- INIZIO di Codice Pico ---- NON ELIMINARE
// NON ELIMINARE NESSUN COMMENTO
#include <Wire.h>
#include <Adafruit_MCP23X17.h>

#define MCP23017_ADDR 0x20        // Mcp_1 Address
#define PICO_I2C_SLAVE_ADDR 0x08  // Indirizzo I2C del Pico come slave

#define INT_MCP_1_PIN 2    // Pin per l'interrupt Di Mcp_1 connesso a INT B
#define RPI_INT_PIN 21     // Pin per segnalare interrupt al Raspberry Pi
#define GUI_ACTIVE_PIN 20  // Pin per rilevare lo stato attivo della GUI
#define LED_GUI_PIN 16     // Pin per il LED che indica la GUI attiva
#define RESET_SEND_MCP 19  // Pin per il reset dell'MCP23017

#define BUTTON_STOP 13      // su Mcp_1
#define BUTTON_START 12     // su Mcp_1
#define BUTTON_FORWARD 11   // su Mcp_1
#define BUTTON_BACKWARD 10  // su Mcp_1
#define BUTTON_FF 9         // su Mcp_1
#define BUTTON_RW 8         // su Mcp_1

#define LED_STOP 15     // su Mcp_1
#define LED_START 14    // su Mcp_1
#define LED_FORWARD 7   // su Mcp_1
#define LED_BACKWARD 4  // su Mcp_1
#define LED_FF 5        // su Mcp_1
#define LED_RW 6        // su Mcp_1

Adafruit_MCP23X17 mcp_1;
volatile bool buttonChanged = false;           // Flag per indicare un cambiamento nei pulsanti (volatile per interrupt)
byte activeCommand = 0;                        // Variabile per il comando attivo da inviare alla GUI
volatile byte lastCommand = 0;                 // Ultimo comando elaborato (volatile per interrupt)
unsigned long lastInterruptTime = 0;           // Timestamp dell'ultimo interrupt per il debounce
const unsigned long debounceInterval = 50;     // NON ELIMINARE // NON  cabiare questo  valore  // Intervallo di debounce in millisecondi
const unsigned long debounceConfirmDelay = 5;  // NON ELIMINARE // NON  cabiare questo  valore  // Ritardo per confermare il debounce
bool lastGuiState = false;                     // Stato precedente della GUI per rilevare cambiamenti

#define HWPANEL_REGISTER 1    // Comando per registrare il pannello
#define HWPANEL_START_STOP 2  // Comando per avviare o fermare la scansione

// NON ELIMINARE // Funzione per gestire l'interrupt dell'MCP23017
void handleMcpInterrupt() {
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTime > debounceInterval) {
    buttonChanged = true;  // Imposta il flag per indicare un cambiamento nei pulsanti
    lastInterruptTime = currentTime;
  }
}

// NON ELIMINARE // Funzione chiamata quando il Pico riceve dati I2C dalla GUI
void receiveEvent(int numBytes) {
  if (Wire1.available()) {
    byte state = Wire1.read();
    mcp_1.digitalWrite(LED_START, state);
    Serial.print("Stato ricevuto dalla GUI: ");
    Serial.println(state);
  }
}

// NON ELIMINARE // Funzione chiamata quando la GUI richiede dati al Pico
void requestEvent() {
  Wire1.write(activeCommand);      // Invia il comando attivo alla GUI
  digitalWrite(RPI_INT_PIN, LOW);  // Riporta LOW il pin di interrupt per il Raspberry Pi
  Serial.print("Comando inviato alla GUI: ");
  Serial.println(activeCommand);
}

// ---- INIZIO SETUP ---- NON ELIMINARE
// NON ELIMINARE // Funzione di configurazione iniziale del Pico
void setup() {
  Serial.begin(115200);
  Serial.println("DEBUG: Inizio setup Pico");

  Wire.begin();  // Inizializza il Pico come master I2C
  Wire.setClock(400000);
  Serial.println("DEBUG: I2C master inizializzato");

  pinMode(RESET_SEND_MCP, OUTPUT);
  digitalWrite(RESET_SEND_MCP, LOW);
  delay(10);
  digitalWrite(RESET_SEND_MCP, HIGH);
  Serial.println("DEBUG: MCP_RESET completato");

  if (!mcp_1.begin_I2C(MCP23017_ADDR, &Wire)) {
    Serial.println("DEBUG: Errore inizializzazione MCP23017");
    while (1)
      ;  // Blocca il programma se l'inizializzazione fallisce
  }
  Serial.println("DEBUG: MCP23017 inizializzato");

  mcp_1.pinMode(BUTTON_STOP, INPUT_PULLUP);
  mcp_1.pinMode(LED_STOP, OUTPUT);
  mcp_1.pinMode(BUTTON_START, INPUT_PULLUP);
  mcp_1.pinMode(LED_START, OUTPUT);
  mcp_1.pinMode(BUTTON_FORWARD, INPUT_PULLUP);
  mcp_1.pinMode(LED_FORWARD, OUTPUT);
  mcp_1.pinMode(BUTTON_BACKWARD, INPUT_PULLUP);
  mcp_1.pinMode(LED_BACKWARD, OUTPUT);
  mcp_1.pinMode(BUTTON_FF, INPUT_PULLUP);
  mcp_1.pinMode(LED_FF, OUTPUT);
  mcp_1.pinMode(BUTTON_RW, INPUT_PULLUP);
  mcp_1.pinMode(LED_RW, OUTPUT);
  Serial.println("DEBUG: Pin MCP23017 configurati");

  mcp_1.digitalWrite(LED_STOP, HIGH);  // Accende il LED Stop all'avvio
  mcp_1.digitalWrite(LED_START, LOW);
  mcp_1.digitalWrite(LED_FORWARD, LOW);
  mcp_1.digitalWrite(LED_BACKWARD, LOW);
  mcp_1.digitalWrite(LED_FF, LOW);
  mcp_1.digitalWrite(LED_RW, LOW);
  Serial.println("DEBUG: Stato iniziale LED impostato");

  mcp_1.setupInterruptPin(BUTTON_START, CHANGE);  // Configura l'interrupt sul pulsante Start per ogni cambiamento
  mcp_1.setupInterrupts(true, false, LOW);        // MIRROR ATTIVO // Abilita gli interrupt dell'MCP, speculari, attivi su LOW
  pinMode(INT_MCP_1_PIN, INPUT);                  // Configura il pin Pico di interrupt proveniente dall'MCP come input
  attachInterrupt(digitalPinToInterrupt(INT_MCP_1_PIN), handleMcpInterrupt, FALLING);
  Serial.println("DEBUG: Interrupt MCP23017 configurati");

  pinMode(RPI_INT_PIN, OUTPUT);
  digitalWrite(RPI_INT_PIN, LOW);
  pinMode(GUI_ACTIVE_PIN, INPUT_PULLDOWN);
  pinMode(LED_GUI_PIN, OUTPUT);
  digitalWrite(LED_GUI_PIN, LOW);
  Serial.println("DEBUG: Pin Pico configurati");

  Serial.println("DEBUG: Inizializzo I2C slave");
  Wire1.begin(PICO_I2C_SLAVE_ADDR);  // Inizializza il Pico come slave I2C con indirizzo specifico
  Wire1.setClock(400000);
  Wire1.onReceive(receiveEvent);
  Wire1.onRequest(requestEvent);
  Serial.println("DEBUG: I2C slave inizializzato");

  delay(100);
  Serial.println("DEBUG: Pico avviato, invio HWPANEL_REGISTER");
  activeCommand = HWPANEL_REGISTER;
  digitalWrite(RPI_INT_PIN, HIGH);  // Segnala al Raspberry Pi l'invio del comando
  Serial.println("DEBUG: Fine setup Pico");
}
// ---- FINE SETUP ---- NON ELIMINARE

// ---- INIZIO loop ---- NON ELIMINARE
// NON ELIMINARE // Ciclo principale del Pico
void loop() {
  if (buttonChanged) {  // Controlla se un pulsante è stato premuto (flag impostato dall'interrupt)
    unsigned long confirmTime = millis();
    delay(debounceConfirmDelay);
    if (mcp_1.digitalRead(BUTTON_START) == LOW && (millis() - confirmTime) < debounceInterval) {
      activeCommand = HWPANEL_START_STOP;  // Imposta il comando Start/Stop
      digitalWrite(RPI_INT_PIN, HIGH);     // Segnala al Raspberry Pi l'invio del comando
      Serial.println("DEBUG: Pulsante Start premuto, invio HWPANEL_START_STOP");
    }
    buttonChanged = false;  // Resetta il flag di cambiamento
  }

  bool currentGuiState = digitalRead(GUI_ACTIVE_PIN);  // Legge lo stato attuale della GUI
  if (currentGuiState != lastGuiState) {               // Controlla se lo stato della GUI è cambiato
    if (currentGuiState == HIGH) {                     // Se la GUI è attiva
      digitalWrite(LED_GUI_PIN, HIGH);                 // Accende il LED verde
      Serial.println("DEBUG: LED_GUI acceso (GUI attiva)");
    } else {
      digitalWrite(LED_GUI_PIN, LOW);
      Serial.println("DEBUG: LED_GUI spento (GUI non attiva)");
    }
    lastGuiState = currentGuiState;  // Aggiorna lo stato precedente della GUI
  }

  delay(10);  // NON ELIMINARE // Pausa di 10 ms per migliorare la reattività
}
// ---- FINE loop ---- NON ELIMINARE

// ---- FINE di Codice Pico ---- NON ELIMINARE
