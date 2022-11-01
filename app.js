const connectDBandStart = require("./startServer");
const dataAPI = require('./sensorsDataAPI.js');
const servers = require('./servers');
const usersAPI = require('./usersAPI');

var flag = false;

// http api
servers.app.post('/addData', (req, res) => {
    let data = req.body;
    console.log('data recived from post api:');
    console.log(data);
    dataAPI.insertData(data);
    res.send("post OK");
})

// bot commands
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

    context.reply('Commands:\n/get_last_humidity\u{1F4A7}: for the last humidity value recived\n/get_last_brightness\u{2600}: for the last enviromental brightness value recived\n/get_last_data: for last data recived\n/get_realtime_data: to enter in realtime mode and see real time data\n/stop_realtime to exit realtime mode');
})

servers.bot.command('get_last_humidity', async (context) => {
    let data = await dataAPI.getLastData();
    context.reply('soil humidity \u{1F4A7}: ' + dataAPI.parseData(data).soil_humidity);
    console.log('humidity sent to ' + context.message.from.first_name + ' ' + context.message.from.last_name);
})

servers.bot.command('get_last_brightness', async (context) => {
    let data = await dataAPI.getLastData();
    context.reply('enviromental brightness\u{2600}: ' + dataAPI.parseData(data).brightness);
    console.log('brightness sent to ' + context.message.from.first_name + ' ' + context.message.from.last_name);
})

servers.bot.command('get_last_data', async (context) => {
    let data = await dataAPI.getLastData();
    context.reply('soil humidity\u{1F4A7}: ' + dataAPI.parseData(data).soil_humidity + '\nenviromental brightness\u{2600}: ' + dataAPI.parseData(data).brightness );
    console.log('data sent to ' + context.message.from.first_name + ' ' + context.message.from.last_name);
})

// salvare nel campo 'realtime' dell'user true se avvia 'get_real_time' e salvare nel campo 'interval' l'interval di quell'user
servers.bot.command('get_realtime_data', async (context) => {

    let user = await usersAPI.searchUser(context.message.from.id);
    if (user.realtime === false) {
        console.log('realtime mode activated by ' + context.message.from.first_name + ' ' + context.message.from.last_name);
        context.reply('realt time mdoe: started\u{1F7E2}');
        context.reply('you will see data every ' + usersAPI.TIME_INTERVAL + ' seconds');

        user_interval = setInterval( async () => {
            let data = await dataAPI.getLastData();
            context.reply('soil humidity\u{1F4A7}: ' + dataAPI.parseData(data).soil_humidity + '\nenviromental brightness\u{2600}: ' + dataAPI.parseData(data).brightness );
        }, 1000*usersAPI.TIME_INTERVAL);

        await usersAPI.updateUser(context.message.from.id, {realtime: true , interval: Number(user_interval) });
    } else {
        context.reply('you already started realime mode');
    }

})

// controllare ogni user e se uno ha 'realtime' a true allora portlo a falses e leggere il campo interval per poi disattivarlo
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

/* servers.bot.command('/start_pump', async (context) => {

    if (flag === true) {
        context.reply('already started');
    } else {
        flag = true;
        context.reply('pump started')
    }
    console.log('flag set by ' + context.message.from.first_name + ' ' + context.message.from.last_name);
}) */

servers.bot.command('commands', async (context) => {
    context.reply('Commands:\n/get_last_humidity\u{1F4A7}: for the last humidity value recived\n/get_last_brightness\u{2600}: for the last enviromental brightness value recived\n/get_last_data: for last data recived\n/get_realtime_data: to enter in realtime mode and see real time data\n/stop_realtime to exit realtime mode');
    console.log('commands sent to ' + context.message.from.first_name + ' ' + context.message.from.last_name);
})



connectDBandStart.connectDBandStartServer("mongodb+srv://lorenzo:lorenzo@telegrambot.dz7srkj.mongodb.net/Automatic_irrigation_IoT");