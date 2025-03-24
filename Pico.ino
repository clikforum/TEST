// ---- INIZIO di Codice Pico ---- NON ELIMINARE
// NON ELIMINARE // Inizio del codice per il Pico, include le librerie e definisce il comportamento del pannello hardware

#include <Wire.h>  // NON ELIMINARE // Libreria per la comunicazione I2C
#include <Adafruit_MCP23X17.h>  // NON ELIMINARE // Libreria per gestire l'espansore I/O MCP23017

#define MCP23017_ADDR 0x20  // NON ELIMINARE // Indirizzo I2C dell'MCP23017
#define PICO_I2C_SLAVE_ADDR 0x08  // NON ELIMINARE // Indirizzo I2C del Pico come slave

#define MCP_INT_PIN 2  // NON ELIMINARE // Pin per l'interrupt dell'MCP23017
#define RPI_INT_PIN 21  // NON ELIMINARE // Pin per segnalare interrupt al Raspberry Pi
#define GUI_ACTIVE_PIN 20  // NON ELIMINARE // Pin per rilevare lo stato attivo della GUI
#define LED_GUI_PIN 16  // NON ELIMINARE // Pin per il LED che indica la GUI attiva
#define MCP_RESET_PIN 19  // NON ELIMINARE // Pin per il reset dell'MCP23017

#define BUTTON_STOP 14  // NON ELIMINARE // Pin del pulsante Stop sull'MCP23017
#define LED_STOP 15  // NON ELIMINARE // Pin del LED Stop sull'MCP23017
#define BUTTON_START 12  // NON ELIMINARE // Pin del pulsante Start sull'MCP23017
#define LED_START 13  // NON ELIMINARE // Pin del LED Start sull'MCP23017
#define BUTTON_FORWARD 10  // NON ELIMINARE // Pin del pulsante Forward sull'MCP23017
#define LED_FORWARD 11  // NON ELIMINARE // Pin del LED Forward sull'MCP23017
#define BUTTON_BACKWARD 6  // NON ELIMINARE // Pin del pulsante Backward sull'MCP23017
#define LED_BACKWARD 7  // NON ELIMINARE // Pin del LED Backward sull'MCP23017
#define BUTTON_FF 8  // NON ELIMINARE // Pin del pulsante Fast Forward sull'MCP23017
#define LED_FF 9  // NON ELIMINARE // Pin del LED Fast Forward sull'MCP23017
#define BUTTON_RW 4  // NON ELIMINARE // Pin del pulsante Rewind sull'MCP23017
#define LED_RW 5  // NON ELIMINARE // Pin del LED Rewind sull'MCP23017

Adafruit_MCP23X17 mcp_1;  // NON ELIMINARE // Oggetto per controllare l'MCP23017
volatile bool buttonChanged = false;  // NON ELIMINARE // Flag per indicare un cambiamento nei pulsanti (volatile per interrupt)
byte activeCommand = 0;  // NON ELIMINARE // Variabile per il comando attivo da inviare alla GUI
volatile byte lastCommand = 0;  // NON ELIMINARE // Ultimo comando elaborato (volatile per interrupt)
unsigned long lastInterruptTime = 0;  // NON ELIMINARE // Timestamp dell'ultimo interrupt per il debounce
const unsigned long debounceInterval = 50;  // NON ELIMINARE // NON  cabiare questo  valore  // Intervallo di debounce in millisecondi
const unsigned long debounceConfirmDelay = 5;  // NON ELIMINARE // NON  cabiare questo  valore  // Ritardo per confermare il debounce
bool lastGuiState = false;  // NON ELIMINARE // Stato precedente della GUI per rilevare cambiamenti

#define HWPANEL_REGISTER 1  // NON ELIMINARE // Comando per registrare il pannello
#define HWPANEL_START_STOP 2  // NON ELIMINARE // Comando per avviare o fermare la scansione

// NON ELIMINARE // Funzione per gestire l'interrupt dell'MCP23017
void handleMcpInterrupt() {
  unsigned long currentTime = millis();  // NON ELIMINARE // Ottiene il tempo corrente in millisecondi
  if (currentTime - lastInterruptTime > debounceInterval) {  // NON ELIMINARE // Controlla se è passato abbastanza tempo dal'ultimo interrupt
    buttonChanged = true;  // NON ELIMINARE // Imposta il flag per indicare un cambiamento nei pulsanti
    lastInterruptTime = currentTime;  // NON ELIMINARE // Aggiorna il timestamp dell'ultimo interrupt
  }
}

// NON ELIMINARE // Funzione chiamata quando il Pico riceve dati I2C dalla GUI
void receiveEvent(int numBytes) {
  if (Wire1.available()) {  // NON ELIMINARE // Controlla se ci sono dati da leggere sull'I2C
    byte state = Wire1.read();  // NON ELIMINARE // Legge lo stato inviato dalla GUI (0 = Stop, 1 = Start)
    mcp_1.digitalWrite(LED_START, state);  // NON ELIMINARE // Imposta il LED Start in base allo stato ricevuto
    Serial.print("Stato ricevuto dalla GUI: ");  // NON ELIMINARE // Stampa un messaggio di debug
    Serial.println(state);  // NON ELIMINARE // Stampa lo stato ricevuto
  }
}

// NON ELIMINARE // Funzione chiamata quando la GUI richiede dati al Pico
void requestEvent() {
  Wire1.write(activeCommand);  // NON ELIMINARE // Invia il comando attivo alla GUI
  digitalWrite(RPI_INT_PIN, LOW);  // NON ELIMINARE // Riporta basso il pin di interrupt per il Raspberry Pi
  Serial.print("Comando inviato alla GUI: ");  // NON ELIMINARE // Stampa un messaggio di debug
  Serial.println(activeCommand);  // NON ELIMINARE // Stampa il comando inviato
}

// ---- INIZIO SETUP ---- NON ELIMINARE
// NON ELIMINARE // Funzione di configurazione iniziale del Pico
void setup() {
  Serial.begin(115200);  // NON ELIMINARE // Inizia la comunicazione seriale a 115200 baud
  Serial.println("DEBUG: Inizio setup Pico");  // NON ELIMINARE // Stampa un messaggio di debug per l'avvio

  Wire.begin();  // NON ELIMINARE // Inizializza il Pico come master I2C
  Wire.setClock(400000);  // NON ELIMINARE // Imposta la velocità dell'I2C a 400 kHz
  Serial.println("DEBUG: I2C master inizializzato");  // NON ELIMINARE // Stampa un messaggio di debug

  pinMode(MCP_RESET_PIN, OUTPUT);  // NON ELIMINARE // Configura il pin di reset dell'MCP come output
  digitalWrite(MCP_RESET_PIN, LOW);  // NON ELIMINARE // Porta basso il pin di reset
  delay(10);  // NON ELIMINARE // Aspetta 10 ms per il reset
  digitalWrite(MCP_RESET_PIN, HIGH);  // NON ELIMINARE // Porta alto il pin di reset
  Serial.println("DEBUG: MCP_RESET completato");  // NON ELIMINARE // Stampa un messaggio di debug

  if (!mcp_1.begin_I2C(MCP23017_ADDR, &Wire)) {  // NON ELIMINARE // Inizializza l'MCP23017 sull'I2C
    Serial.println("DEBUG: Errore inizializzazione MCP23017");  // NON ELIMINARE // Stampa un messaggio di errore
    while (1);  // NON ELIMINARE // Blocca il programma se l'inizializzazione fallisce
  }
  Serial.println("DEBUG: MCP23017 inizializzato");  // NON ELIMINARE // Stampa un messaggio di debug

  mcp_1.pinMode(BUTTON_STOP, INPUT_PULLUP);  // NON ELIMINARE // Configura il pulsante Stop come input con pull-up
  mcp_1.pinMode(LED_STOP, OUTPUT);  // NON ELIMINARE // Configura il LED Stop come output
  mcp_1.pinMode(BUTTON_START, INPUT_PULLUP);  // NON ELIMINARE // Configura il pulsante Start come input con pull-up
  mcp_1.pinMode(LED_START, OUTPUT);  // NON ELIMINARE // Configura il LED Start come output
  mcp_1.pinMode(BUTTON_FORWARD, INPUT_PULLUP);  // NON ELIMINARE // Configura il pulsante Forward come input con pull-up
  mcp_1.pinMode(LED_FORWARD, OUTPUT);  // NON ELIMINARE // Configura il LED Forward come output
  mcp_1.pinMode(BUTTON_BACKWARD, INPUT_PULLUP);  // NON ELIMINARE // Configura il pulsante Backward come input con pull-up
  mcp_1.pinMode(LED_BACKWARD, OUTPUT);  // NON ELIMINARE // Configura il LED Backward come output
  mcp_1.pinMode(BUTTON_FF, INPUT_PULLUP);  // NON ELIMINARE // Configura il pulsante Fast Forward come input con pull-up
  mcp_1.pinMode(LED_FF, OUTPUT);  // NON ELIMINARE // Configura il LED Fast Forward come output
  mcp_1.pinMode(BUTTON_RW, INPUT_PULLUP);  // NON ELIMINARE // Configura il pulsante Rewind come input con pull-up
  mcp_1.pinMode(LED_RW, OUTPUT);  // NON ELIMINARE // Configura il LED Rewind come output
  Serial.println("DEBUG: Pin MCP23017 configurati");  // NON ELIMINARE // Stampa un messaggio di debug

  mcp_1.digitalWrite(LED_STOP, HIGH);  // NON ELIMINARE // Accende il LED Stop all'avvio
  mcp_1.digitalWrite(LED_START, LOW);  // NON ELIMINARE // Spegne il LED Start all'avvio
  mcp_1.digitalWrite(LED_FORWARD, LOW);  // NON ELIMINARE // Spegne il LED Forward all'avvio
  mcp_1.digitalWrite(LED_BACKWARD, LOW);  // NON ELIMINARE // Spegne il LED Backward all'avvio
  mcp_1.digitalWrite(LED_FF, LOW);  // NON ELIMINARE // Spegne il LED Fast Forward all'avvio
  mcp_1.digitalWrite(LED_RW, LOW);  // NON ELIMINARE // Spegne il LED Rewind all'avvio
  Serial.println("DEBUG: Stato iniziale LED impostato");  // NON ELIMINARE // Stampa un messaggio di debug

  mcp_1.setupInterruptPin(BUTTON_START, CHANGE);  // NON ELIMINARE // Configura l'interrupt sul pulsante Start per ogni cambiamento
  mcp_1.setupInterrupts(true, false, LOW);  // NON ELIMINARE // Abilita gli interrupt dell'MCP, speculari, attivi su LOW
  pinMode(MCP_INT_PIN, INPUT);  // NON ELIMINARE // Configura il pin di interrupt dell'MCP come input
  attachInterrupt(digitalPinToInterrupt(MCP_INT_PIN), handleMcpInterrupt, FALLING);  // NON ELIMINARE // Associa la funzione di interrupt al pin
  Serial.println("DEBUG: Interrupt MCP23017 configurati");  // NON ELIMINARE // Stampa un messaggio di debug

  pinMode(RPI_INT_PIN, OUTPUT);  // NON ELIMINARE // Configura il pin di interrupt per il Raspberry Pi come output
  digitalWrite(RPI_INT_PIN, LOW);  // NON ELIMINARE // Imposta il pin di interrupt basso inizialmente
  pinMode(GUI_ACTIVE_PIN, INPUT_PULLDOWN);  // NON ELIMINARE // Configura il pin della GUI come input con pull-down
  pinMode(LED_GUI_PIN, OUTPUT);  // NON ELIMINARE // Configura il pin del LED GUI come output
  digitalWrite(LED_GUI_PIN, LOW);  // NON ELIMINARE // Spegne il LED GUI inizialmente
  Serial.println("DEBUG: Pin Pico configurati");  // NON ELIMINARE // Stampa un messaggio di debug

  Serial.println("DEBUG: Inizializzo I2C slave");  // NON ELIMINARE // Stampa un messaggio di debug
  Wire1.begin(PICO_I2C_SLAVE_ADDR);  // NON ELIMINARE // Inizializza il Pico come slave I2C con indirizzo specifico
  Wire1.setClock(400000);  // NON ELIMINARE // Imposta la velocità dell'I2C slave a 400 kHz
  Wire1.onReceive(receiveEvent);  // NON ELIMINARE // Associa la funzione per ricevere dati I2C
  Wire1.onRequest(requestEvent);  // NON ELIMINARE // Associa la funzione per inviare dati I2C su richiesta
  Serial.println("DEBUG: I2C slave inizializzato");  // NON ELIMINARE // Stampa un messaggio di debug

  delay(100);  // NON ELIMINARE // Aspetta 100 ms per stabilizzare l'I2C
  Serial.println("DEBUG: Pico avviato, invio HWPANEL_REGISTER");  // NON ELIMINARE // Stampa un messaggio di debug
  activeCommand = HWPANEL_REGISTER;  // NON ELIMINARE // Imposta il comando iniziale di registrazione
  digitalWrite(RPI_INT_PIN, HIGH);  // NON ELIMINARE // Segnala al Raspberry Pi l'invio del comando
  Serial.println("DEBUG: Fine setup Pico");  // NON ELIMINARE // Stampa un messaggio di debug
}
// ---- FINE SETUP ---- NON ELIMINARE

// ---- INIZIO loop ---- NON ELIMINARE
// NON ELIMINARE // Ciclo principale del Pico
void loop() {
  if (buttonChanged) {  // NON ELIMINARE // Controlla se un pulsante è stato premuto (flag impostato dall'interrupt)
    unsigned long confirmTime = millis();  // NON ELIMINARE // Ottiene il tempo corrente per il debounce
    delay(debounceConfirmDelay);  // NON ELIMINARE // Aspetta un breve ritardo per confermare il debounce
    if (mcp_1.digitalRead(BUTTON_START) == LOW && (millis() - confirmTime) < debounceInterval) {  // NON ELIMINARE // Verifica se il pulsante Start è premuto
      activeCommand = HWPANEL_START_STOP;  // NON ELIMINARE // Imposta il comando Start/Stop
      digitalWrite(RPI_INT_PIN, HIGH);  // NON ELIMINARE // Segnala al Raspberry Pi l'invio del comando
      Serial.println("DEBUG: Pulsante Start premuto, invio HWPANEL_START_STOP");  // NON ELIMINARE // Stampa un messaggio di debug
    }
    buttonChanged = false;  // NON ELIMINARE // Resetta il flag di cambiamento
  }
  
  bool currentGuiState = digitalRead(GUI_ACTIVE_PIN);  // NON ELIMINARE // Legge lo stato attuale della GUI
  if (currentGuiState != lastGuiState) {  // NON ELIMINARE // Controlla se lo stato della GUI è cambiato
    if (currentGuiState == HIGH) {  // NON ELIMINARE // Se la GUI è attiva
      digitalWrite(LED_GUI_PIN, HIGH);  // NON ELIMINARE // Accende il LED verde
      Serial.println("DEBUG: LED_GUI acceso (GUI attiva)");  // NON ELIMINARE // Stampa un messaggio di debug
    } else {  // NON ELIMINARE // Se la GUI non è attiva
      digitalWrite(LED_GUI_PIN, LOW);   // NON ELIMINARE // Spegne il LED verde
      Serial.println("DEBUG: LED_GUI spento (GUI non attiva)");  // NON ELIMINARE // Stampa un messaggio di debug
    }
    lastGuiState = currentGuiState;  // NON ELIMINARE // Aggiorna lo stato precedente della GUI
  }
  
  delay(10);  // NON ELIMINARE // Pausa di 10 ms per migliorare la reattività
}
// ---- FINE loop ---- NON ELIMINARE

// ---- FINE di Codice Pico ---- NON ELIMINARE