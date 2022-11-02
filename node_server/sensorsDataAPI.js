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


// function that adds data to the database
const insertData = async (dataObj) => {
	await DataModel.create(dataObj, () => {
        console.log("Data added:");
        console.log(dataObj);
    });
}

// function that delete all documents from the collection
const removeAllData = () => {
    DataModel.deleteMany({}, () => {
        // after removed all data, insert a new document with parameter to 0 so when client asks for data, server won't crush
        // better not to do this because if the user asks for all data in the db, it is difficult to manage the undefined entry
        /* insertData({
            soil_humidity: undefined,
            brightness: undefined,
        }); */
        console.log("all data removed");
    });
} 

// function that returns the last data obj saved in the db
const getLastData = async () => {
    const singleListObj = await DataModel.find().sort({_id:-1}).limit(1);
    return singleListObj[0];
}

// function that returns a list of the last data obj saved in the db
const getLastNData = async (N) => {
    const singleListObj = await DataModel.find().sort({_id:-1}).limit(N);
    return singleListObj;
}

// return parse data
const parseData = (dataObj) => {
    return {
        soil_humidity: dataObj.soil_humidity,
        brightness: dataObj.brightness
    };
}


module.exports = {insertData, removeAllData, getLastData, getLastNData, parseData}