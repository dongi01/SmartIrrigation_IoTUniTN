const {Telegraf} = require('telegraf');
const express = require('express');

//.env file
require('dotenv').config();

// create a bot with the telegram token
const bot = new Telegraf(process.env.TELEGRAM_TOKEN);

// create an app with express
const app = express();
app.use(express.json());

module.exports = {bot, app};
