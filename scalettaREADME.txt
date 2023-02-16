- Titolo e spiegazione generale di cosa fa il progettino
- Requinremets
    - Hardware
        - MSP432
        - ESP32
        - Boosterpack MSP432
        - sensore umidità (capacitive soil moisture sensor v2)
        - relay 5v
        - pompa 5v
        - jumpers wire
    - Software
        - CCS
            - scaricare librerie
            - includere librerie
            - includere i nostri file di libreria
        - Arduino
            - scaricare librerie
- Schema elettrico


da sistemare:
- file system del progetto -> magari da mettere prima dei requisiti software così da avere più chiaro dove sono le cartelle sulle quali scaricare i file
- come settare il wifi di ESP32 (wifimanager)
- sequenza di accensione dei dispositivi (prima MSP432 poi ESP32) -> se no esp potrebbe leggere valori a cazzo dalla uart e fare casino sul DB e variabile pompa
- come usarlo in generale
- telegram
- settare li indirizzi ip nel codice (solo quello del server node, quello dell'esp è già settato, deve solo essere nella stessa rete locale)
