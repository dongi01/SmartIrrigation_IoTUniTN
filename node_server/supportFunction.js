// lists of all commands
const commandList = () => {
    return 'Commands:\n' + 
    '/get_last_moisture \u{1F4A7} : for the last soil moisture value recived\n' + 
    '/get_last_brightness \u{2600} : for the last enviromental brightness value recived\n' + 
    '/get_last_temperature \u{1F321} : for the last temperature value recived\n' + 
    '/get_last_data \u{1F4CB} : for last data recived\n' + 
    '/get_last_N_data N \u{1F4D1} : for the last N data (if possible)' +
    '/get_realtime_data \u{1F7E2} : to enter in realtime mode and see real time data\n' + 
    '/stop_realtime \u{1F534} : to exit realtime mode\n' + 
    '/start_pump \u{1F6B0} : to manually star the pump\n' +
    '/stop_pump \u{1F6B1} : to manually stop the pump';
}

const pumpMsg = (event) => {
    if (event == "start") {
        return 'The pump has been started! \u{1F6B0}';
    } else if (event == "stop"){
        return 'The pump has been stopped! \u{1F6B1}';
    } else {
        return "error: event not recognized";
    }
}

const pumpMsgLog = (event, user) => {
    if (event == "start") {
        return "alert start pump sent to " + user;
    } else if (event == "stop") {
        return "alert stop pump sent to " + user;
    } else {
        return "error: event not recognized";
    }
}

// check if the lengh of an array is smaller than a number
const checkLength = (arrayLen, N) => {
    return arrayLen < N ?  false :  true;
}

// print data strings
const printMoisture = (soil_moisture) => {
    return 'soil moisture\u{1F4A7}: ' + soil_moisture;
}

const printBrightness = (brightness) => {
    return 'enviromental brightness\u{2600}: ' + brightness;
}

const printTemperature = (temperature) => {
    return 'temperature\u{1F321}: ' + temperature;
}

const printData = (soil_moisture, brightness, temperature) => {
    return printMoisture(soil_moisture) + '\n' + printBrightness(brightness) + '\n' + printTemperature(temperature);
}

module.exports = {commandList, pumpMsg, pumpMsgLog, checkLength, printMoisture, printBrightness, printTemperature, printData};