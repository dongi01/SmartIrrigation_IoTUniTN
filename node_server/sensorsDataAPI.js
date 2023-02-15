const DATABASE = 'Sensors_data'

const mongoose = require('mongoose');

// data schema for collection 'Sensors_data' documents
const DataSchema = new mongoose.Schema({
	soil_moisture: Number,
	brightness: Number,
    temperature: Number,
	date: {type: Date, default: new Date()}
},{ collection: DATABASE });

// association between schema and collection
const DataModel = mongoose.model(DATABASE, DataSchema);


// functio that count documents in collection 
const countData = async () => {
    return await DataModel.countDocuments();
}


// function that adds data to the database
const insertData = async (dataObj) => {
	await DataModel.create(dataObj, () => {
        console.log("Data added:");
        console.log(dataObj);
        console.log("\n");
    });
}

// function that delete all documents from the collection
const removeAllData = () => {
    DataModel.deleteMany({}, () => {
        // after removed all data, insert a new document with parameter to 0 so when client asks for data, server won't crush
        // better not to do this because if the user asks for all data in the db, it is difficult to manage the undefined entry
        /* insertData({
            soil_moisture: undefined,
            brightness: undefined,
        }); */
        console.log("all data removed");
    });
}

// function that delete the oldest N documents in the database
const removeOldestNData = async (N) => {
    const firtNObj = await DataModel.find().sort({_id: 1}).limit(N+1);
    DataModel.deleteMany({_id:{"$lt": firtNObj[N]._id }}, () => {
        console.log("oldest " + N + " objects have been removed");
    });
}

// function that returns the last data obj saved in the db
const getLastData = async () => {
    const singleListObj = await DataModel.find().sort({_id:-1}).limit(1);
    return singleListObj[0];
}

// function that returns a list of the last data obj saved in the db
const getLastNData = async (N) => {
    const firtNObj = await DataModel.find().sort({_id:-1}).limit(N);
    return firtNObj;
}

// return parse data
const parseData = (dataObj) => {
    return {
        soil_moisture: dataObj.soil_moisture,
        brightness: dataObj.brightness,
        temperature: dataObj.temperature
    };
}


module.exports = {countData, insertData, removeAllData, removeOldestNData, getLastData, getLastNData, parseData}