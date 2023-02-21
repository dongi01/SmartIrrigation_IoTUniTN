# Smart Irrigation System IoT UniTN

<div align="center">
<img src="readmeImages/appLogo.png" width="200">
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#about-the-project">About The Project</a></li>
    <li><a href="#project-layout">Project Layout</a></li>
    <li>
      <a href="#basic">Basic</a>
      <ul>
        <li><a href="#basic-requirements">Requirements</a></li>
        <ul>
            <li><a href="#Hardware">Hardware</a></li>
            <li><a href="#Software">Software</a></li>
        </ul>
      </ul>
      <ul>
        <li><a href="#Project-wiring">Project wiring</a></li>
      </ul>
      <ul>
        <li><a href="#Get-started">Get started</a></li>
      </ul>
    </li>
    <li>
      <a href="#iot-integration">IoT Integration</a>
      <ul>
        <li><a href="#iot-requirements">Requirements</a></li>
        <ul>
            <li><a href="#hardware">Hardware</a></li>
            <li><a href="#software">Software</a></li>
        </ul>
        <li><a href="#project-wiring">Project Wiring</a></li>
        <li><a href="#get-started">Get Started</a></li>
            <ol>
                <li><a href="#set-nodejs-server-ip-adress-in-esp32-code">Set NodeJS server IP adress in ESP32 code</a></li>
                <li><a href="#create-env-file">Create .env file</a></li>
                <li><a href="#load-program-image-on-esp32">Load program image on ESP32</a></li>
                <li><a href="#start-the-all-project">Start the all project</a></li>
                <li><a href="#telegram-bot-interface">Telegram bot interface</a></li>
            </ol>
      </ul>
    </li>
  </ol>
</details>


## About the project
Welcome to the Smart Irrigation System!

This system is designed to help you water your plants efficiently and effectively. It uses a combination of sensors, microcontrollers, and a water pump to automatically water your plants, or even control it remotely.

The system includes the following components:
  - <strong>MSP432</strong>: This device reads the data from the sensors, determines whether the plants need to be watered and is connected to internet by an ESP32.
  - <strong> ESP32</strong>: This microcontroller is uesd to connect the system to internet in order to use a Telegram bot that lets overview the framework.
  - <strong>Soil moisture sensor</strong>: This sensor measures the moisture level in the soil and sends data to the msp microcontroller.
  - <strong>Temperature and light sensor </strong>: This sensor measures the temperature and light of the surrounding environment and sends data to the msp microcontroller.
  - <strong>Water pump </strong>: This pump is connected to a water source and is used to water the plants when necessary.

The system works by continuously monitoring the soil moisture level and environmental conditions. If the moisture level is too low and the temperature and humidity are within acceptable ranges, the microcontroller will activate the water pump to water the plants.

Otherwise the user can control the system remotely with our Telegram bot. It is integrated with the microcontroller and can receive data from the sensors. It can also send commands to the microcontroller to control the water pump. The user can access the Telegram bot from their smartphone or computer.

Overall, the Telegram bot is a useful feature that adds flexibility and convenience to the Smart Automatic Irrigation System.

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

# Basic

The basic idea of the project is based on a offline irrigation system, which can be controlled directly by the MSP432. Using the lcd screen, it has different features to be execute by choosing with its own integrated controller and buttons.

## Basic Requirements

### Hardware
You will need an MSP432p401r of the Texas Instrument company with its own expansion: the BOOSTXL-EDUMKII. Moreover you will have to use a capacitive soil moisture sensor for the moisture detection, a 5v relay, a 5v pump and some jumper wires.

### Software
In order to flash the program into the Texas Instrument components, you first need to install [Code Composer Studio](https://www.ti.com/tool/download/CCSTUDIO/12.2.0). 

The next step is to create a new CCS project, choosing the right target system (MSP432P401R), and paste all the files in ccsProject/mainProjectFolder and ccsProject/sensorsLibraries folders. You need also to exlude all the .c files, exept for the main.c and the one from the sensorsLibraries, from the build with a right click on them.

Finally to run the project you will need to add to the linker option and to the compiler option the driverlib library downloading it at this [link](https://drive.google.com/file/d/1krZfBNAFiE6yAChQfHZVE-b0wfuDitgV/view?usp=sharing). 

#### Linker option

<p align="center">
<img src="readmeImages/libraryImage.png" width="500">
</p>

#### Compiler option

<p align="center">
<img src="readmeImages/armCompilerOption.png" width="500">
</p>

## Project wiring

<p align="center">
<img src="readmeImages/electricSchemaBasic.png" width="400"> 
</p>

## Get started
If all the software requirements are satisfied, you need to connect all the sensors and the microcontroller as you can see in the electric schema above. Now, you are able to flash the program on the MSP, using the command flash on CCS, and start using the whole offline system. 

<p align="center">
<img src="readmeImages/menuImage.jpg"  width="200" hspace=20>
<img src="readmeImages/showSensorsImage.jpg"  width="200" hspace=20>
</p>

As you can see in the image above, once the program start running you will see the app logo appearing for a few seconds and you will soon be able use the app menu to try the different functionalities.

In particular, in the second image it is shown a sample of the sensors data page that is constantly refreshing with the brand new data collected by the sensors.



# IoT integration

This extension allows you to connect the basic system to the internet. In this way you can visualize all your data from your Telegram account with different mode, like in real time or on demand. You will also be able to start and stop manually the water pump.

This bot can respond to different Telegram accounts and the people that start it will be included in a group. The group created will always be update with a message when the pump changes its state, for whatever reason (by manual command, by telegram command or automatically).

## IoT Requirements

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

Now that `NodeJS` and `npm` are installed, opne the `node_server` folder in your terminal and type this command

```
npm install
```
This command will install for you all the NodeJS packages needed.

#### **Arduino IDE**
Simply download the version for you on their [official web site](https://www.arduino.cc/en/software) and follow the installation procedure.

Once you have installed it you have to download some libraries to be able to operate on the ESP32 and manage http communication between NodeJS server and ESP32.

First of all open Arduino IDE and go to `Tools > Board > Boards Manager`, search for ESP32 by Espressif Systems and press install.

<p align="center">
<img src="readmeImages/boardManagerW.png"  width="500">
</p>

Then we want to install the libraries required for the http comunication.

Go to `Tools > Manage Libraries` and search for:

1. HttpClient by Adrian McEwen
2. WebServer_ESP32_SC_ENC by Khoi Hoang
3. WiFiManager by tablatronix


<p align="center">
<img src="readmeImages/librariesManagerW.png"  width="500">
</p>

Nice, now your Arduino enviroment is ready.

#### **Telegram app, account**

You can download `Telegram` app directly from your store and create a new account if you haven't already.

#### **Telegram bot**

For this extension you have to create your own bot for obvious reason but don't worry, it will be easy!

To create your own bot open [BotFather]( https://t.me/BotFather) and press start button below.


<p align="center">
<img src="readmeImages/botFatherStart.png"  height="450">
</p>


Then you will see some of the operations that the BotFather can do for you so digit or press 
```
/newbot
```
Now you only have to follow its instruction. Remember to save safely the token that it will give you, you will need it later!
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

You can create your local cluster with `MongoDB` and `MongoDBCompass` following this [guide](https://zellwk.com/blog/local-mongodb/) (how to create the database and how to connect it, don't look at how to connect from server NodeJS). This method is good if you have a home server but it is not the best if you haven't because your machine should run 24/7.

Otherwhise you can create your MongoDB account and create your cluster online. Follow this [guide](https://www.mongodb.com/basics/clusters/mongodb-cluster-setup) for that.

Once you have your cluster you have to create one database, witch will be composed of two collections:
1. Users
2. Sensors_data

It is very important to name these collection exactly in this way.

At the end you should have something like this in your Database.

<p align="center">
<img src="readmeImages/mongoDBCompass.png"  width="500">
</p>

Now the configuration of the software is done, later we will have to modify some code!

## Project wiring

<p align="center">
<img src="readmeImages/electricSchemaIOT.png" width="400">
</p>

## Get started

Now we have to modify some code to make it work for you.

### Set NodeJS server IP adress in ESP32 code

We need to tell the ESP32 which address to send the data to so, in the `ESP32code` folder open `httpClient.cpp` in a text editor and change the 6th line. Insert the local IP address of the pc where the server will be running.

```c++
// node server address
String serverNode = "http://<yourServerIP>:3000";
```
Now ESP32 code is ready!

### Create .env file

Now you have to create a file named `.env` in the `node_server` folder. <br>
Once you have created it, open it and in the first 2 line write:
```javascript
TELEGRAM_TOKEN = '<yourTelegramBotToken>'
DB_URL = '<yourMongoDBconnectioinLink>/<yourDatabaseName>'
```

In the first you need to copy and paste the token given to you by the BotFather and in the second you have to copy and paste the connection link of your database, followed by the name of your cluster.<br>
If your database is locally hosted the string will look something like 
```
mongodb://localhost:27017
```
otherwise it will look something like
```
mongodb+srv://<username>:<password>@<clusterName>.[randomCharacter].mongodb.net
```

### Load program image on ESP32

It is time to load the ESP32 code on the board.

In the `ESP32code` folder open `ESP32code.ino` with Arduino IDE, connect the ESP32 to the computer with a USB cable and then press the little arrow icon on the top left of the screen to load the code in the board. It will take some time.

<p align="center">
<img src="readmeImages/loadESP32code.png"  width="500">
</p>

*NOTE: when a sequence of dots appears in the bottom of the Arduino IDE, make sure to press the boot button on the ESP32 until the dots stop.*

You should end with something like that

<p align="center">
<img src="readmeImages/loadESP32Done.png"  width="500">
</p>

### Start the all project

First of all power the ESP32 via USB or input pin.

In this first phase we want to tell to ESP32 what network it should connect to. <br>
The board will act like an access point, so go to your wifi settings (via pc or smartphone) and connect to `AutoConnectAP`: a web page like below should open automatically, if not open your browser and search for WiFiManager in the top bar

<p align="center">
<img src="readmeImages/WiFiManagerSearch.png"  width="500">
</p>

<p align="center">
<img src="readmeImages/wifiManager.png"  width="500">
</p>

After connecting to local WiFi with ESP32, you can power the MSP432 as well and replicate the project wiring.

Lastly we have to start our NodeJS server. Just open the `node_server` folder in a terminal and digit
```
node app.js
```
In this terminal you will see lots of log information that can be usefull, in some cases.

At this point the project should be perfectly running!

### Telegram bot interface 

If you want to be able to use your Telegram bot just open your app and search for the bot that you have created, start it and enjoy!
You can ask for the last soil moisture, enviromental brightness and temperature registered in the database, you can see data in real time activating realtimie mode (it will send you data every 10 seconds), you can ask for the last N data saved on the database and finally you can manually start and stop the pump!

Here an example of the telegram interface.

<p align="center">
<img src="readmeImages/telegramExample.png"  width="500">
</p>