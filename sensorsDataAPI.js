let database = 'Sensors_data'

const mongoose = require('mongoose');

// data schema for collection 'Sensors_data' documents
const DataSchema = new mongoose.Schema({
	soil_humidity: Number,
	brightness: Number,
	date: {type: Date, default: new Date()}
},{ collection: database });

// association between schema and collection
const DataModel = mongoose.model(database, DataSchema);

// ------------------------- da qua in giù rivedere tutto -------------------------
// da sistemare tutte le funcioni del db per renderle sincrone e aspettare che le operazioni vengano concluse
// https://stackoverflow.com/questions/37104199/how-to-await-for-a-callback-to-return

// function that adds data to the database
const insertData = async (obj) => {
	const data = new DataModel(obj);
	data.save(() => {
        console.log("Data added to DB: " + data);
    });
}

// function that delete all documents from the collection
const removeAllData = async () => {
    DataModel.deleteMany({}, () => {
        console.log("all data removed");
    });
} 

// function that search a user by chat id in the database
const getLastData = async () => {
    const singleListObj = await DataModel.find().sort({_id:-1}).limit(1);
    return singleListObj[0];
}

const parseData = (objData) => {
    return {
        soil_humidity: objData.soil_humidity,
        brightness: objData.brightness
    };
}
module.exports = {insertData, removeAllData, getLastData, parseData}