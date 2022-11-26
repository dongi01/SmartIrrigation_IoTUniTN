const connectDBandStart = require("./startServer");
const dataAPI = require('./sensorsDataAPI.js');
const servers = require('./servers');
const usersAPI = require('./usersAPI');
const supportFunction = require('./supportFunction');

//.env file
require('dotenv').config();

// -------------------- bot commands --------------------

servers.bot.start(async (context) => {
    console.log('Service started by ' + context.message.from.first_name + ' ' + context.message.from.last_name + '\n');
    
    // if a new user starts the bot, his data will be saved in the db
    let user = await usersAPI.searchUser(context.message.from.id);
    if (user === null) {
        usersAPI.insertUser({
            first_name: context.message.from.first_name,
            last_name: context.message.from.last_name,
            username: context.message.from.username,
            chat_id: context.message.from.id
        });
    }
    context.reply(supportFunction.commandList());
})

// print last value of moisture in the db
servers.bot.command('get_last_moisture', async (context) => {

    let user = await usersAPI.searchUser(context.message.from.id);

    if(user.realtime === false) {
        let data = await dataAPI.getLastData();
        context.reply(supportFunction.printMoisture(dataAPI.parseData(data).soil_moisture));
        console.log('moisture sent to ' + context.message.from.first_name + ' ' + context.message.from.last_name + '\n');
    }else{
        context.reply('You are in realtime mode, please exit from it to use this command');
    }
})

// print last value of brightness in the db
servers.bot.command('get_last_brightness', async (context) => {

    let user = await usersAPI.searchUser(context.message.from.id);

    if(user.realtime === false){
        let data = await dataAPI.getLastData();
        context.reply(supportFunction.printBrightness(dataAPI.parseData(data).brightness));
        console.log('brightness sent to ' + context.message.from.first_name + ' ' + context.message.from.last_name + '\n');
    }else{
        context.reply('You are in realtime mode, please exit from it to use this command');
    }
})

// print last value of temperature in the db
servers.bot.command('get_last_temperature', async (context) => {

    let user = await usersAPI.searchUser(context.message.from.id);

    if(user.realtime === false){
        let data = await dataAPI.getLastData();
        context.reply(supportFunction.printTemperature(dataAPI.parseData(data).temperature));
        console.log('temperature sent to ' + context.message.from.first_name + ' ' + context.message.from.last_name + '\n');
    }else{
        context.reply('You are in realtime mode, please exit from it to use this command');
    }
})

// print last value of moisture and brightness in the db
servers.bot.command('get_last_data', async (context) => {

    let user = await usersAPI.searchUser(context.message.from.id);

    if(user.realtime === false){
        let data = await dataAPI.getLastData();
        context.reply(supportFunction.printData(
            dataAPI.parseData(data).soil_moisture,
            dataAPI.parseData(data).brightness,
            dataAPI.parseData(data).temperature)
        );
        console.log('data sent to ' + context.message.from.first_name + ' ' + context.message.from.last_name + '\n');
    }else{
        context.reply('You are in realtime mode, please exit from it to use this command');
    }
})

// it starts realtime mode of a single user
servers.bot.command('get_realtime_data', async (context) => {

    let user = await usersAPI.searchUser(context.message.from.id);
    if (user.realtime === false) {
        console.log('realtime mode activated by ' + context.message.from.first_name + ' ' + context.message.from.last_name);
        context.reply('realt time mdoe: started\u{1F7E2}');
        context.reply('you will see data every ' + usersAPI.TIME_INTERVAL + ' seconds');

        user_interval = setInterval( async () => {
            let data = await dataAPI.getLastData();
            context.reply(supportFunction.printData(
                dataAPI.parseData(data).soil_moisture,
                dataAPI.parseData(data).brightness,
                dataAPI.parseData(data).temperature)
            ).catch(error => {
                // if the user stops the bot during realtime mode, delete user from db and clear interval
				if (error.response && error.response.error_code === 403) {
					usersAPI.removeUser(context.message.from.id);
                    clearInterval(user_interval);
				} else {
					console.log(error);
				}
			});;
        }, 1000*usersAPI.TIME_INTERVAL);

        await usersAPI.updateUser(context.message.from.id, {realtime: true , interval: Number(user_interval) });
    } else {
        context.reply('you already started realime mode');
    }
    console.log();
})

// it stops realtime mode of a single user
servers.bot.command('stop_realtime', async (context) => {
    let user = await usersAPI.searchUser(context.message.from.id);

    if (user.realtime === true) {
        clearInterval(user.interval);

        console.log('realtime mode stopped by ' + context.message.from.first_name + ' ' + context.message.from.last_name);
        context.reply('real time mode: stopped\u{1F534}');
    
        await usersAPI.updateUser(context.message.from.id, {realtime: false});
    } else {
        context.reply('you haven\'t started realtime mode yet');
    }
    console.log();
})

// print last value of moisture and brightness in the db
servers.bot.command('get_last_N_data', async (context) => {

    msg = context.message.text;
    if(msg != "/get_last_N_data") {
        msgArray = msg.split(' ');
        if (msgArray.length === 2) {
            N = Number(msgArray[1]);
            dataArray = await dataAPI.getLastNData(N);
            dataArrayDim = dataArray.length;
            if (N === NaN) {
                context.reply('You have to insert a number after the command');
            } else {
                console.log(dataArrayDim + ' data sent to ' + context.message.from.first_name + ' ' + context.message.from.last_name + '\n');

                if (!supportFunction.checkLength(dataArrayDim, N)) {
                    context.reply('There are only ' + dataArrayDim + ' documents');
                }

                dataArray.forEach(dataObj => {
                    if (dataAPI.parseData(dataObj).soil_moisture != undefined) {
                        context.reply(supportFunction.printData(
                            dataAPI.parseData(dataObj).soil_moisture,
                            dataAPI.parseData(dataObj).brightness,
                            dataAPI.parseData(dataObj).temperature)
                        );
                    }
                });
            }
        } else {
            context.reply('Use only one number paramether after the command');
        }
    } else {
        context.reply('Use \'command NumArgument\'');
    }
})

// set global variable to control whether or not the pump is running
// this varible will be usefull for httpGET from the ESP01 module
var pump_started = false;
// set 'pump_started' to true to flag that a user what to start the pump
servers.bot.command('/start_pump', async (context) => {

    if (pump_started) {
        context.reply('pump already started');
    } else {
        pump_started = true;
        await alertStatePump("start");
        console.log('pump started by ' + context.message.from.first_name + ' ' + context.message.from.last_name + '\n');
    }
})

// set 'pump_started' to false to flag that a user what to stop the pump
servers.bot.command('/stop_pump', async (context) => {

    if (pump_started) {
        pump_started = false;
        await alertStatePump("stop");
        console.log('pump stopped by ' + context.message.from.first_name + ' ' + context.message.from.last_name + '\n');
    } else {
        context.reply('pump is not running');
    }
})

// shows all commands available
servers.bot.command('commands', async (context) => {
    context.reply(supportFunction.commandList());
    console.log('commands sent to ' + context.message.from.first_name + ' ' + context.message.from.last_name + '\n');
})

// delete all sensors data from db
// mabye this function can be omitted in the command list
servers.bot.command('delete_sensors_data', async (context) => {
    dataAPI.removeAllData();
    context.reply('All sensors data deleted');
    console.log('sensors data deleted by ' + context.message.from.first_name + ' ' + context.message.from.last_name + '\n');
})

// function that send alert mesages to all users that start the bot. If they close it, they will be removed from the db
const alertStatePump = async (event) => {
    let userslist = await usersAPI.allUser()
    userslist.forEach( user => {
        servers.bot.telegram.sendMessage(user.chat_id, supportFunction.pumpMsg(event))
        // catches the error and make sure to delete the user only if the user has blocked the bot
        .catch(error => {
            if (error.response && error.response.error_code === 403) {
                usersAPI.removeUser(user.chat_id);
            } else {
                console.log(error);
            }
        });
        console.log(supportFunction.pumpMsgLog(event, user.first_name + ' ' + user.last_name));
    });
}

// -------------------- http api --------------------

// post request counter
var postCounter = 0;
var postDBCounter = 0;
// listens for new sensor data and inserts them in the db
servers.app.post('/addSensorsData', (req, res) => {
    postCounter++;
    postDBCounter++;
    let data = req.body;
    console.log('data recived from post api:');
    console.log(data);
    // insert data in db every 10 post recived
    if (postDBCounter === 1) {
        dataAPI.insertData(data);
        postDBCounter = 0;
    }
    if (postCounter === 200) {
        // delete first 100
        // ++ to do ++ create the function in sensorsDataAPI
        postCounter = 100;
    }
    
    res.send("post OK");
    console.log("post response send\n");
})

// mabye we can listen for a single get req and manage the start/stop cases with parameters
// listens for messages that indicate the start of the pump
servers.app.get('/alertStartPump', async (req, res) => {
    console.log('get request recived');
    // function that sent to all user in the db a msg whitch say that the pump is running
    await alertStatePump("start");
    res.send("OK");
    console.log('get response send\n');
})

// listens for messages that indicate the start of the pump
servers.app.get('/alertStopPump', async (req, res) => {
    console.log('get request recived');
    // function that sent to all user in the db a msg whitch say that the pump is running
    await alertStatePump("stop");
    res.send("OK");
    console.log('get response send\n');
})

// starts server
connectDBandStart.connectDBandStartServer(process.env.DB_URL);