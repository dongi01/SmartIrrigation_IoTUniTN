const DATABASE = 'Users';
const TIME_INTERVAL = 10;

const mongoose = require('mongoose');

// data schema for collection 'Users' documents
const UserSchema = new mongoose.Schema({
	first_name: String,
	last_name: String,
	username: String,
	chat_id: String,
    realtime: {type: Boolean, default: false},
    interval: {type: Number, default: 0},
	date: {type: Date, default: new Date()}
},{ collection: DATABASE });

// association between schema and collection
const UserModel = mongoose.model(DATABASE, UserSchema);

// function that adds a user to the database
const insertUser = async (userObj) => {
    await UserModel.create(userObj, () => {
        console.log("User added: ");
        console.log(userObj);
    });
}

// function that search a user by chat id in the collection and returns it
const searchUser = async (chatID) => {
    return await UserModel.findOne({ chat_id: chatID }).exec();
}

// function that returns the list of all users in the db
const allUser = async () => {
    return await UserModel.find();
}

// function that delete a user from the collection
const removeUser = (chatID) => {
    UserModel.deleteOne({chat_id: chatID }, () => {
        console.log('user with chat_id: ' + chatID + ' deleted');
    });
} 

// function that update a user with 'updatedValues'
const updateUser = async (chatID, updatedValues) => {
    await UserModel.updateOne({ chat_id: chatID }, updatedValues);
    console.log('user with chat_id: ' + chatID + ' updated with ');
    console.log(updatedValues);
}


module.exports = {TIME_INTERVAL, insertUser, searchUser, allUser, removeUser, updateUser}