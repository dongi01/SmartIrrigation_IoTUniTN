const connectDBandStart = require("./startServer");
const dataAPI = require('./sensorsDataAPI.js');
const servers = require('./servers');
const usersAPI = require('./usersAPI');

//.env file
require('dotenv').config();

// lists of all commands
function commandList(){
    return 'Commands:\n' + 
    '/get_last_humidity\u{1F4A7}: for the last humidity value recived\n' + 
    '/get_last_brightness\u{2600}: for the last enviromental brightness value recived\n' + 
    '/get_last_data: for last data recived\n' + 
    '/get_realtime_data: to enter in realtime mode and see real time data\n' + 
    '/stop_realtime to exit realtime mode\n' + 
    '/start_pump to manually star the pump\n' +
    '/stop_pump to manually stop the pump'
}

// ---------- bot commands ----------
servers.bot.start(async (context) => {
    console.log('Service started by ' + context.message.from.first_name + ' ' + context.message.from.last_name);
    
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
    // ++ to do ++ create a function in a new telegram support function file that print commands
    context.reply(commandList());
})

// print last value of humidity in the db
servers.bot.command('get_last_humidity', async (context) => {

    let user = await usersAPI.searchUser(context.message.from.id);

    if(user.realtime === false) {
        let data = await dataAPI.getLastData();
        context.reply('soil humidity \u{1F4A7}: ' + dataAPI.parseData(data).soil_humidity);
        console.log('humidity sent to ' + context.message.from.first_name + ' ' + context.message.from.last_name);
    }else{
        context.reply('You are in realtime mode, please exit from it to use this command');
    }
})

// print last value of brightness in the db
servers.bot.command('get_last_brightness', async (context) => {

    let user = await usersAPI.searchUser(context.message.from.id);

    if(user.realtime === false){
        let data = await dataAPI.getLastData();
        context.reply('enviromental brightness\u{2600}: ' + dataAPI.parseData(data).brightness);
        console.log('brightness sent to ' + context.message.from.first_name + ' ' + context.message.from.last_name);
    }else{
        context.reply('You are in realtime mode, please exit from it to use this command');
    }
})

// print last value of humidity and brightness in the db
servers.bot.command('get_last_data', async (context) => {

    let user = await usersAPI.searchUser(context.message.from.id);

    if(user.realtime === false){
        let data = await dataAPI.getLastData();
        context.reply('soil humidity\u{1F4A7}: ' + dataAPI.parseData(data).soil_humidity + '\nenviromental brightness\u{2600}: ' + dataAPI.parseData(data).brightness );
        console.log('data sent to ' + context.message.from.first_name + ' ' + context.message.from.last_name);
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
            context.reply('soil humidity\u{1F4A7}: ' + dataAPI.parseData(data).soil_humidity + '\nenviromental brightness\u{2600}: ' + dataAPI.parseData(data).brightness )
            .catch(error => {
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
        context.reply('pump started');
        console.log('pump started by ' + context.message.from.first_name + ' ' + context.message.from.last_name);
    }
})

// set 'pump_started' to false to flag that a user what to stop the pump
servers.bot.command('/stop_pump', async (context) => {

    if (pump_started) {
        pump_started = false;
        context.reply('pump stopped');
        console.log('pump stopped by ' + context.message.from.first_name + ' ' + context.message.from.last_name);
    } else {
        context.reply('pump is not running');
    }
})

// shows all commands available
// ++ to do ++ create a function in a new telegram support function file that print commands 
servers.bot.command('commands', async (context) => {
    context.reply(commandList());
    console.log('commands sent to ' + context.message.from.first_name + ' ' + context.message.from.last_name);
})

// delete all sensors data from db
// mabye this function can be omitted in the command list
servers.bot.command('delete_sensors_data', async (context) => {
    dataAPI.removeAllData();
    context.reply('All sensors data deleted');
    console.log('sensors data deleted by ' + context.message.from.first_name + ' ' + context.message.from.last_name);
})

// ---------- http api ----------

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
    if (postDBCounter === 10) {
        dataAPI.insertData(data);
        postDBCounter = 0;
    }
    if (postCounter === 200) {
        // delete first 100
        // ++ to do ++ create the function in sensorsDataAPI
        postCounter = 100;
    }
    
    res.send("post OK");
})

// listens for get request and send whether or not the pump_started flag is true 
servers.app.get('/getPumpState', (req, res) => {
    console.log('get request recived');
    if (pump_started) {
        // to see what to send...
        res.send({test: true});
    } else {
        // to see what to send...
        res.send({test: false});
    }
    console.log('get request send');
})

// starts server
connectDBandStart.connectDBandStartServer(process.env.DB_URL);