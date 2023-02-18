# Smart Irrigation IoT UniTN

## Basic
<img src="readmeImages/image.png"  width="50%" height="50%">

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
├── readmeImages
│   └── images                      # images for readme
└── README.md
```

## IoT integration

This extension allows you to connect the basic system to the internet. In this way you can visualize all your data from your Telegram account with different mode, like in realtime or ondemand. You will also be able to start and stop manually the water pump.

This bot can respond to different Telegram account and the people that starts it will be included in a gruop. The group created will be always update with a message when the pump changes its state, for whatever reason (by manual command, by telegram command or automatically).

## Requirements

### Hardware

You will only need an ESP32 (ESP-WROOM-32) and some other jumpers wire.

### Software

1. NodeJS 
2. npm
3. Arduino IDE (version 1.8.19 is recommended)
4. Telegram app and at least one account
5. Telegram bot
6. MongoDB, MongoDBCompass

#### **NodeJS, npm**
How can I install all of this? Just follow next instructions.
To install `NodeJS` and `npm` you can easily follow the instruction on this [guide]( https://docs.npmjs.com/downloading-and-installing-node-js-and-npm).

Now that `NodeJS` and `npm` are installed, opned the node_server folder in your terminal and type this command

```
npm install
```
This command will install for you all the NodeJS packages needed.

#### **Arduino IDE**
Simply download the version for you on their [official web site](https://www.arduino.cc/en/software) and follow the installation procedure.

Once you installed it you have to download some libraries to be able to operate on the ESP32 and manage http comounication between NodeJS server and ESP32.

First of all oper Arduino IDE and go to `tool > Board > Boards Manager`, search for ESP32 by Espressif Systems and press install.


<img src="readmeImages/boardManager.png"  width="500">


<img src="readmeImages/boardManagerW.png"  width="500">


Then we want to install the libraries required for the http comunication.

Go to `tool > Manage Libraries` and search for:

1. HttpClient by Adrian McEwen
2. WebServer_ESP32_SC_ENC by Khoi Hoang
3. WiFiManager by tablatronix

<img src="readmeImages/librariesManager.png"  width="500">

<img src="readmeImages/librariesManagerW.png"  width="500">


Nice, now your Arduino enviroment is ready.

#### **Telegram app, account**

You can download Telegram app directly from your store and create a new account if you haven't already.

#### **Telegram bot**

For this extension you have to create your own bot for obvious reason but don't warry, it will be easy!

To create your own bot open [BotFather]( https://t.me/BotFather) and prest start button below.


<img src="readmeImages/botFatherStart.png"  height="450">


readmeImages/librariesManagerW.png

Then you will see some of the operation that the BotFather can do for you so digit or press 
```
/newbot
```
Now you only have to follow its incruction. Remember to save saftily the token thet it will give you, you will need it later!
The last thing you should do in the BotFather is to set commands. Simply digit 
```
/setcommands
```
and then copy and paste this list of commands
```
commands - for all commands
get_last_moisture - for the last soil moisture value recived
get_last_brightness - for the last enviromental brightness value recived
get_last_temperature - for the last temperature value recived
get_last_data - for last data recived
get_last_n_data - for the last n data (if possible). NOTE: write your number after command
get_realtime_data - to enter in realtime mode and see real time data
stop_realtime - to exit realtime mode
start_pump - to manually star the pump
stop_pump - to manually stop the pump
```

Now the configuration of the bot is done too!

#### **MongoDB, MongoDBCompass**

Now you have two solution:
1. running the database on your local machine
2. running the database on the cloud

You can careate your local cluster with MongoDB and MongoDBCompass following this [guide](https://zellwk.com/blog/local-mongodb/) (how to create the database and how to connect it, don't look at how to connect from server NodeJS). This method is good if you have a home server but it is not the best if you haven't because your machine should run 24/7.

Otherwhise you can create your MongoDB account and create your cluster online. Follow this [guide](https://www.mongodb.com/basics/clusters/mongodb-cluster-setup) for that.

Onece you have your cluster you have to create one database, witch will be composed of two collection:
1. Users
2. Sensors_data

It is very important to name these collection exactly in this way.

At the end you should have something like this in your Database.

<img src="readmeImages/mongoDBCompass.png"  width="500">


Now the configuration of the software is done, later we will have to modify some code!