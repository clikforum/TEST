# ---- INIZIO di hw_panel_callback ---- NON ELIMINARE
# NON ELIMINARE # Inizio della funzione di callback per gestire i comandi dal pannello hardware
# ---Row 4429  --- HwPanel callback function
# NON ELIMINARE # Commento originale che indica la riga approssimativa nel file
# ---Row 4429  --- HwPanel callback function
# NON ELIMINARE # Commento originale ripetuto
def hw_panel_callback(command, param1=None):  # NON ELIMINARE # Definizione della funzione che elabora i comandi del pannello
    global hwpanel_registered, ScanOngoing  # NON ELIMINARE # Dichiarazione delle variabili globali usate
    from hw_panel import HwPanel  # NON ELIMINARE # Importa la classe HwPanel dal modulo hw_panel
    if 'hwpanel_registered' not in globals():  # NON ELIMINARE # Controlla se la variabile globale hwpanel_registered è definita
        hwpanel_registered = False  # NON ELIMINARE # Inizializza hwpanel_registered a False se non esiste
    print(f"DEBUG: Callback chiamato con comando: {command}, param1: {param1}, hwpanel_registered: {hwpanel_registered}")  # NON ELIMINARE # Stampa un messaggio di debug con i dettagli del comando
    if command == HWPANEL_REGISTER:  # NON ELIMINARE # Controlla se il comando è di registrazione del pannello
        hwpanel_registered = param1  # NON ELIMINARE # Imposta lo stato di registrazione in base al parametro ricevuto
        if param1:  # NON ELIMINARE # Se il pannello è registrato (param1 è True)
            logo_label.config(bg="sky blue")  # NON ELIMINARE # Cambia il colore dello sfondo del logo a blu cielo
            print("DEBUG: Pannello connesso, label impostata su sky blue")  # NON ELIMINARE # Stampa un messaggio di debug
            HwPanel._instance.send_panel_state(1 if ScanOngoing else 0)  # NON ELIMINARE # Invia lo stato attuale della scansione al pannello
        else:  # NON ELIMINARE # Se il pannello non è registrato (param1 è False)
            logo_label.config(bg="AntiqueWhite1")  # NON ELIMINARE # Cambia il colore dello sfondo del logo a bianco antico
            print("DEBUG: Pannello scollegato, label impostata su AntiqueWhite1")  # NON ELIMINARE # Stampa un messaggio di debug
    if hwpanel_registered:  # NON ELIMINARE # Controlla se il pannello è registrato prima di elaborare altri comandi
        if command == HWPANEL_START_STOP:  # NON ELIMINARE # Controlla se il comando è Start/Stop
            start_scan()  # NON ELIMINARE # Chiama la funzione per avviare o fermare la scansione
            HwPanel._instance.send_panel_state(1 if ScanOngoing else 0)  # NON ELIMINARE # Invia lo stato aggiornato della scansione al pannello
            print(f"DEBUG: Start/Stop eseguito, stato inviato: {1 if ScanOngoing else 0}")  # NON ELIMINARE # Stampa un messaggio di debug
        elif command == HWPANEL_FORWARD:  # NON ELIMINARE # Controlla se il comando è Forward
            cmd_advance_movie()  # NON ELIMINARE # Chiama la funzione per avanzare il film
        elif command == HWPANEL_BACKWARD:  # NON ELIMINARE # Controlla se il comando è Backward
            cmd_retreat_movie()  # NON ELIMINARE # Chiama la funzione per retrocedere il film
        elif command == HWPANEL_FF:  # NON ELIMINARE # Controlla se il comando è Fast Forward
            cmd_fast_forward_movie()  # NON ELIMINARE # Chiama la funzione per avanzare velocemente il film
        elif command == HWPANEL_RW:  # NON ELIMINARE # Controlla se il comando è Rewind
            cmd_rewind_movie()  # NON ELIMINARE # Chiama la funzione per riavvolgere il film
# ---- FINE di hw_panel_callback ---- NON ELIMINARE
# NON ELIMINARE # Fine della funzione di callback