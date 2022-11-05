// lists of all commands
const commandList = () => {
    return 'Commands:\n' + 
    '/get_last_humidity\u{1F4A7}: for the last humidity value recived\n' + 
    '/get_last_brightness\u{2600}: for the last enviromental brightness value recived\n' + 
    '/get_last_data: for last data recived\n' + 
    '/get_realtime_data: to enter in realtime mode and see real time data\n' + 
    '/stop_realtime to exit realtime mode\n' + 
    '/start_pump to manually star the pump\n' +
    '/stop_pump to manually stop the pump';
}

pumpStartedMsg = () => {
	return 'The pump has been started!';
}

pumpStoppedMsg = () => {
	return 'The pump has been stopped!';
}

// check if the lengh of an array is smaller than a number
const checkLength = (arrayLen, N) => {
    return arrayLen < N ?  false :  true;
}

module.exports = {commandList, pumpStartedMsg, pumpStoppedMsg, checkLength};