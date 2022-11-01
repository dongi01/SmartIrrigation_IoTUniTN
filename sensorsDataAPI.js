const DATABASE = 'Sensors_data'

const mongoose = require('mongoose');

// data schema for collection 'Sensors_data' documents
const DataSchema = new mongoose.Schema({
	soil_humidity: Number,
	brightness: Number,
	date: {type: Date, default: new Date()}
},{ collection: DATABASE });

// association between schema and collection
const DataModel = mongoose.model(DATABASE, DataSchema);

// ------------------------- da qua in giù rivedere tutto -------------------------

// function that adds data to the database
const insertData = async (dataObj) => {
	await UserModel.create(dataObj, () => {
        console.log("Data added: " + dataObj);
    });
}

// function that delete all documents from the collection
const removeAllData = async () => {
    await DataModel.deleteMany({}, () => {
        console.log("all data removed");
    });
} 

// function that search a user by chat id in the database
const getLastData = async () => {
    const singleListObj = await DataModel.find().sort({_id:-1}).limit(1);
    return singleListObj[0];
}

// return parse data
const parseData = (dataObj) => {
    return {
        soil_humidity: dataObj.soil_humidity,
        brightness: dataObj.brightness
    };
}


module.exports = {insertData, removeAllData, getLastData, parseData}