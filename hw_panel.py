 ---- INIZIO di hw_panel.py ---- NON ELIMINARE
# NON ELIMINARE # Inizio del file che definisce la classe HwPanel per gestire il pannello hardware
"""
****************************************************************************************************************
Class HwPanel
Class to encapsulate hardware panel
****************************************************************************************************************
"""
# NON ELIMINARE # Documentazione originale che descrive la classe HwPanel

import smbus2  # NON ELIMINARE # Libreria per la comunicazione I2C sul Raspberry Pi
from RPi import GPIO  # NON ELIMINARE # Libreria per gestire i pin GPIO del Raspberry Pi
import time  # NON ELIMINARE # Libreria per gestire ritardi e temporizzazioni
import sys  # NON ELIMINARE # Libreria per configurare l'output del sistema
import logging  # NON ELIMINARE # Libreria per il logging dei messaggi

# Forza codifica UTF-8 per evitare crash
sys.stdout.reconfigure(encoding='utf-8')  # NON ELIMINARE # Configura l'output per usare UTF-8 ed evitare errori di codifica
logging.basicConfig(encoding='utf-8')  # NON ELIMINARE # Configura il logging per usare UTF-8

class HwPanel():  # NON ELIMINARE # Definizione della classe HwPanel
    _instance = None  # NON ELIMINARE # Variabile statica per implementare il pattern Singleton
    ExitingApp = False  # NON ELIMINARE # Flag per indicare se l'applicazione sta terminando
    active = ''  # NON ELIMINARE # Variabile per lo stato attivo (non usata nel codice corrente)
    hwpanel_after = None  # NON ELIMINARE # Variabile per gestire eventi temporizzati (non usata nel codice corrente)
    AltScan8Callback = None  # NON ELIMINARE # Riferimento alla funzione di callback per comunicare con Alt_01.py

    HWPANEL_REGISTER = 1  # NON ELIMINARE # Comando per registrare il pannello
    HWPANEL_START_STOP = 2  # NON ELIMINARE # Comando per avviare o fermare la scansione
    HWPANEL_FORWARD = 3  # NON ELIMINARE # Comando per avanzare il film
    HWPANEL_BACKWARD = 4  # NON ELIMINARE # Comando per retrocedere il film
    HWPANEL_FF = 5  # NON ELIMINARE # Comando per avanzare velocemente il film
    HWPANEL_RW = 6  # NON ELIMINARE # Comando per riavvolgere il film
    HWPANEL_FOCUS_VIEW = 7  # NON ELIMINARE # Comando per la vista di messa a fuoco (non implementato)
    HWPANEL_ZOOM_VIEW = 8  # NON ELIMINARE # Comando per la vista zoom (non implementato)
    HWPANEL_ZOOM_VIEW_PLUS = 9  # NON ELIMINARE # Comando per ingrandire la vista (non implementato)
    HWPANEL_ZOOM_VIEW_MINUS = 10  # NON ELIMINARE # Comando per ridurre la vista (non implementato)
    HWPANEL_ZOOM_VIEW_RIGHT = 11  # NON ELIMINARE # Comando per spostare la vista a destra (non implementato)
    HWPANEL_ZOOM_VIEW_LEFT = 12  # NON ELIMINARE # Comando per spostare la vista a sinistra (non implementato)
    HWPANEL_ZOOM_VIEW_UP = 13  # NON ELIMINARE # Comando per spostare la vista in alto (non implementato)
    HWPANEL_ZOOM_VIEW_DOWN = 14  # NON ELIMINARE # Comando per spostare la vista in basso (non implementato)
    HWPANEL_AUTO_EXPOSURE = 15  # NON ELIMINARE # Comando per l'esposizione automatica (non implementato)
    HWPANEL_AUTO_WB = 16  # NON ELIMINARE # Comando per il bilanciamento del bianco automatico (non implementato)
    HWPANEL_AUTOSTOP_ENABLE = 17  # NON ELIMINARE # Comando per abilitare l'autostop (non implementato)
    HWPANEL_GET_AUTOSTOP_TIME = 18  # NON ELIMINARE # Comando per ottenere il tempo di autostop (non implementato)
    HWPANEL_SET_AUTOSTOP_FRAMES = 19  # NON ELIMINARE # Comando per impostare i frame di autostop (non implementato)
    HWPANEL_GET_FILM_TIME = 20  # NON ELIMINARE # Comando per ottenere il tempo del film (non implementato)
    HWPANEL_GET_FPS = 21  # NON ELIMINARE # Comando per ottenere i frame per secondo (non implementato)
    HWPANEL_SET_FILM_S8 = 22  # NON ELIMINARE # Comando per impostare il film come Super 8 (non implementato)
    HWPANEL_SET_FILM_R8 = 23  # NON ELIMINARE # Comando per impostare il film come Regular 8 (non implementato)
    HWPANEL_SET_EXPOSURE = 24  # NON ELIMINARE # Comando per impostare l'esposizione (non implementato)
    HWPANEL_SET_WB_RED = 25  # NON ELIMINARE # Comando per impostare il bilanciamento del rosso (non implementato)
    HWPANEL_SET_WB_BLUE = 26  # NON ELIMINARE # Comando per impostare il bilanciamento del blu (non implementato)

    CMD_GET_CNT_STATUS = 2  # NON ELIMINARE # Comando alternativo per lo stato (non usato nel codice corrente)

    # NON ELIMINARE # Metodo speciale per implementare il pattern Singleton
    def __new__(cls, *args, **kwargs):
        if cls._instance is None:  # NON ELIMINARE # Controlla se l'istanza esiste già
            cls._instance = super().__new__(cls)  # NON ELIMINARE # Crea una nuova istanza se non esiste
        return cls._instance  # NON ELIMINARE # Restituisce l'istanza esistente o nuova

    # NON ELIMINARE # Metodo di inizializzazione della classe HwPanel
    def __init__(self, master_win, callback):
        print("\n\nDEBUG: Entrato in HwPanel.__init__")  # NON ELIMINARE # Stampa un messaggio di debug all'avvio
        if not hasattr(self, 'initialized'):  # NON ELIMINARE # Controlla se l'oggetto è già stato inizializzato
            self.main_win = master_win  # NON ELIMINARE # Salva la finestra principale della GUI
            self.AltScan8Callback = callback  # NON ELIMINARE # Salva la funzione di callback per comunicare con Alt_01.py
            print(f"DEBUG: hwpanel inizializzato: win={master_win}, callback={callback}")  # NON ELIMINARE # Stampa un messaggio di debug
            
            self.I2C_ADDRESS = 0x08  # NON ELIMINARE # Indirizzo I2C del Pico
            self.bus = smbus2.SMBus(1)  # NON ELIMINARE # Inizializza il bus I2C sul canale 1
            self.INT_PIN = 23  # NON ELIMINARE # Pin GPIO per l'interrupt dal Pico
            self.GUI_ACTIVE_PIN = 24  # NON ELIMINARE # Pin GPIO per segnalare che la GUI è attiva
            self.panel_connected = False  # NON ELIMINARE # Flag per indicare se il pannello è connesso
            
            GPIO.setwarnings(False)  # NON ELIMINARE # Disabilita gli avvisi GPIO
            GPIO.setmode(GPIO.BCM)  # NON ELIMINARE # Imposta la modalità BCM per i pin GPIO
            GPIO.setup(self.INT_PIN, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)  # NON ELIMINARE # Configura il pin di interrupt come input con pull-down
            GPIO.setup(self.GUI_ACTIVE_PIN, GPIO.OUT)  # NON ELIMINARE # Configura il pin della GUI come output
            GPIO.output(self.GUI_ACTIVE_PIN, GPIO.LOW)  # NON ELIMINARE # Imposta il pin della GUI a basso inizialmente
            print("DEBUG: GPIO configurati")  # NON ELIMINARE # Stampa un messaggio di debug
            
            if self.check_connection():  # NON ELIMINARE # Controlla la connessione I2C con il Pico
                self.panel_connected = True  # NON ELIMINARE # Imposta il flag di connessione a True
                GPIO.output(self.GUI_ACTIVE_PIN, GPIO.HIGH)  # NON ELIMINARE # Segnala che la GUI è attiva
                self.AltScan8Callback(self.HWPANEL_REGISTER, True)  # NON ELIMINARE # Notifica la registrazione del pannello
                print("DEBUG: Connessione iniziale Pico confermata")  # NON ELIMINARE # Stampa un messaggio di debug
            else:  # NON ELIMINARE # Se la connessione fallisce
                print("DEBUG: Nessuna connessione Pico all’avvio")  # NON ELIMINARE # Stampa un messaggio di debug
            
            GPIO.add_event_detect(self.INT_PIN, GPIO.RISING, callback=self._handle_interrupt, bouncetime=50)  # NON ELIMINARE # Configura l'interrupt sul pin GPIO
            print("DEBUG: Interrupt configurati")  # NON ELIMINARE # Stampa un messaggio di debug
            
            self.monitor_connection()  # NON ELIMINARE # Avvia il monitoraggio della connessione
            self.initialized = True  # NON ELIMINARE # Segna l'oggetto come inizializzato

    # NON ELIMINARE # Funzione per verificare la connessione I2C con il Pico
    def check_connection(self):
        try:
            self.bus.read_byte(self.I2C_ADDRESS)  # NON ELIMINARE # Tenta di leggere un byte dall'indirizzo I2C del Pico
            print("DEBUG: I2C connessione OK")  # NON ELIMINARE # Stampa un messaggio di debug se la connessione è ok
            return True  # NON ELIMINARE # Restituisce True se la connessione è riuscita
        except Exception as e:  # NON ELIMINARE # Cattura eventuali errori
            print(f"DEBUG: I2C connessione fallita: {e}")  # NON ELIMINARE # Stampa un messaggio di debug con l'errore
            return False  # NON ELIMINARE # Restituisce False se la connessione fallisce

    # NON ELIMINARE # Funzione per monitorare periodicamente la connessione con il Pico
    def monitor_connection(self):
        if not self.ExitingApp:  # NON ELIMINARE # Controlla se l'applicazione non sta terminando
            connected = self.check_connection()  # NON ELIMINARE # Verifica lo stato della connessione
            if connected and not self.panel_connected:  # NON ELIMINARE # Se il Pico si connette e non era connesso
                self.panel_connected = True  # NON ELIMINARE # Imposta il flag di connessione a True
                GPIO.output(self.GUI_ACTIVE_PIN, GPIO.HIGH)  # NON ELIMINARE # Segnala che la GUI è attiva
                self.AltScan8Callback(self.HWPANEL_REGISTER, True)  # NON ELIMINARE # Notifica la registrazione del pannello
                print("DEBUG: Pico riconnesso")  # NON ELIMINARE # Stampa un messaggio di debug
            elif not connected and self.panel_connected:  # NON ELIMINARE # Se il Pico si disconnette ed era connesso
                self._handle_disconnect()  # NON ELIMINARE # Gestisce la disconnessione
            self.main_win.after(500, self.monitor_connection)  # NON ELIMINARE # Richiama la funzione dopo 500 ms

    # NON ELIMINARE # Funzione vuota chiamata al completamento dell'inizializzazione
    def init_completed(self):
        print("DEBUG: init_completed chiamato")  # NON ELIMINARE # Stampa un messaggio di debug

    # NON ELIMINARE # Funzione per gestire la chiusura dell'applicazione
    def shutdown_started(self):
        global ExitingApp  # NON ELIMINARE # Usa la variabile globale ExitingApp
        self.ExitingApp = True  # NON ELIMINARE # Imposta il flag di uscita a True
        GPIO.output(self.GUI_ACTIVE_PIN, GPIO.LOW)  # NON ELIMINARE # Spegne il segnale della GUI
        GPIO.cleanup()  # NON ELIMINARE # Pulisce le configurazioni GPIO
        self.bus.close()  # NON ELIMINARE # Chiude il bus I2C
        print("DEBUG: HwPanel shutdown completato")  # NON ELIMINARE # Stampa un messaggio di debug

    # NON ELIMINARE # Funzione vuota per gestire i frame catturati (non implementata)
    def captured_frame(self):
        pass  # NON ELIMINARE # Placeholder per future implementazioni

    # NON ELIMINARE # Funzione vuota per avvisare di film nel filmgate (non implementata)
    def film_in_filmgate_warning(self):
        return False  # NON ELIMINARE # Restituisce False per indicare nessun avviso

    # NON ELIMINARE # Funzione vuota per avvisare di film indietro (non implementata)
    def film_back_warning(self):
        return False  # NON ELIMINARE # Restituisce False per indicare nessun avviso

    # NON ELIMINARE # Funzione per gestire gli interrupt dal Pico
    def _handle_interrupt(self, channel):
        print(f"DEBUG: Interrupt ricevuto su GPIO {channel}")  # NON ELIMINARE # Stampa un messaggio di debug con il canale GPIO
        command = self._read_panel_command()  # NON ELIMINARE # Legge il comando dal Pico
        print(f"DEBUG: Comando ricevuto dal Pico: {command}")  # NON ELIMINARE # Stampa un messaggio di debug con il comando
        if command == self.HWPANEL_REGISTER:  # NON ELIMINARE # Controlla se il comando è di registrazione
            self.panel_connected = True  # NON ELIMINARE # Imposta il flag di connessione a True
            GPIO.output(self.GUI_ACTIVE_PIN, GPIO.HIGH)  # NON ELIMINARE # Segnala che la GUI è attiva
            self.AltScan8Callback(self.HWPANEL_REGISTER, True)  # NON ELIMINARE # Notifica la registrazione del pannello
            print("DEBUG: Pannello registrato")  # NON ELIMINARE # Stampa un messaggio di debug
        elif command == self.HWPANEL_START_STOP:  # NON ELIMINARE # Controlla se il comando è Start/Stop
            if self.panel_connected:  # NON ELIMINARE # Verifica che il pannello sia connesso
                self._start_stop_scan()  # NON ELIMINARE # Esegue l'azione Start/Stop
                print("DEBUG: Start/Stop elaborato")  # NON ELIMINARE # Stampa un messaggio di debug

    # NON ELIMINARE # Funzione per leggere il comando dal Pico via I2C
    def _read_panel_command(self):
        try:
            command = self.bus.read_byte(self.I2C_ADDRESS)  # NON ELIMINARE # Legge un byte dal Pico
            print(f"DEBUG: Lettura I2C riuscita: {command}")  # NON ELIMINARE # Stampa un messaggio di debug
            return command  # NON ELIMINARE # Restituisce il comando letto
        except Exception as e:  # NON ELIMINARE # Cattura eventuali errori
            print(f"DEBUG: Errore I2C lettura: {e}")  # NON ELIMINARE # Stampa un messaggio di debug con l'errore
            self._handle_disconnect()  # NON ELIMINARE # Gestisce la disconnessione
            return None  # NON ELIMINARE # Restituisce None in caso di errore

    # NON ELIMINARE # Funzione per inviare lo stato al Pico via I2C
    def send_panel_state(self, state):
        try:
            self.bus.write_byte(self.I2C_ADDRESS, state)  # NON ELIMINARE # Scrive lo stato al Pico
            print(f"DEBUG: Stato inviato al Pico: {state}")  # NON ELIMINARE # Stampa un messaggio di debug
            self.panel_connected = True  # NON ELIMINARE # Conferma la connessione
        except Exception as e:  # NON ELIMINARE # Cattura eventuali errori
            print(f"DEBUG: Errore I2C invio stato: {e}")  # NON ELIMINARE # Stampa un messaggio di debug con l'errore
            self._handle_disconnect()  # NON ELIMINARE # Gestisce la disconnessione

    # NON ELIMINARE # Funzione per gestire la disconnessione del Pico
    def _handle_disconnect(self):
        if self.panel_connected:  # NON ELIMINARE # Controlla se il pannello era connesso
            self.panel_connected = False  # NON ELIMINARE # Imposta il flag di connessione a False
            GPIO.output(self.GUI_ACTIVE_PIN, GPIO.LOW)  # NON ELIMINARE # Spegne il segnale della GUI
            self.AltScan8Callback(self.HWPANEL_REGISTER, False)  # NON ELIMINARE # Notifica la disconnessione del pannello
            print("DEBUG: Pannello scollegato rilevato")  # NON ELIMINARE # Stampa un messaggio di debug

    # NON ELIMINARE # Funzione per avviare o fermare la scansione
    def _start_stop_scan(self):
        global ScanOngoing  # NON ELIMINARE # Usa la variabile globale ScanOngoing
        self.AltScan8Callback(self.HWPANEL_START_STOP, None)  # NON ELIMINARE # Chiama il callback con il comando Start/Stop
        self.send_panel_state(1 if ScanOngoing else 0)  # NON ELIMINARE # Invia lo stato aggiornato al Pico

# ---- FINE di hw_panel.py ---- NON ELIMINARE
# NON ELIMINARE # Fine del file hw_panel.py

