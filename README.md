# Smart Irrigation IoT UniTN

## Basic

### Project Layout
```
SmartIrrigation_IoTUniTN
├── ccsProject  (temporaneo)
│   ├── dataCompression             # credo tutto da togliere
│   ├── file to add to workspace    # cambiare nome cartella
│   └── mainProjectFolder 
├── ESP32code
│   ├── ESP32code.ino               # ESP32 main
│   ├── httpClient                  # personal functions
│   ├── httpServer                  # |
|   └── readUARTdata                # └
├── node_server
│   ├── app.js                      # main nodejs app
│   ├── sensorsDataAPI.js           # functions to manage data in DB
│   ├── servers.js                  # declaring nodejs http server and telegram bot
│   ├── startServer.js              # start telegram bot, http server and connect to database
│   ├── supportFunction.js          # personal function
│   └── usersAPI.js                 # functions to manage users data in DB
└── README.md
```

## IoT integration