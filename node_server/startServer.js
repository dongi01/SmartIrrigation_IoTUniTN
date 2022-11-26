// modules
const mongoose = require('mongoose');
const servers = require('./servers');


// function that starts the express app and telegram bot
startServer = async () => {
    await servers.bot.launch()
	console.log("telegram bot started");

    servers.app.listen(3000, () => {
        console.log("listening on port 3000\n");
    });
}

// connects the DB and starts the server after the connection is done
const connectDBandStartServer = (URL) => {
    console.log('db connecting...');
	try {
		mongoose.connect(URL, () => {
			console.log('db connected!');
			startServer();  // start the server after the database connection is done
		});
	} catch (err) {
		console.log('Failed to connect to MongoDB', err);
	}
};

module.exports = {connectDBandStartServer};
