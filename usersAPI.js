const DATABASE = 'Users';
const TIME_INTERVAL = 10;

const mongoose = require('mongoose');

// data schema for collection 'Utenti' documents
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
const insertUser = (obj) => {
	const user = new UserModel(obj);
	user.save(() => {
        console.log("User added: " + user);
    });
}

// function that search a user by chat id in the collection and returns it
const searchUser = async (chatID) => {
    return await UserModel.findOne({ chat_id: chatID }).exec();
}

// function that return a list of chatID of the users in the collection
// *to try what happend if there is only one user*
const allUser = (callback) => {
    UserModel.find({}, (err, userList) => {
        if(err){
            return callback(err);
        } else if (userList){
            return callback(null,userList);
        } else {
            return callback();
        }
    });
}

// function that delete a user from the collection
const removeUser = (chatID) => {
    UserModel.deleteOne({chat_id: chatID }, () => {
        console.log('user with chat_id: ' + chatID + ' deleted');
    });
} 

const updateUser = async (chatID, updatedValues) => {
    await UserModel.updateOne({ chat_id: chatID }, updatedValues);
    console.log('user with chat_id: ' + chatID + ' updated with ');
    console.log(updatedValues);
}



module.exports = {TIME_INTERVAL, insertUser, searchUser, allUser, removeUser, updateUser}