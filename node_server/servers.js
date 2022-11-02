const {Telegraf} = require('telegraf');
const express = require('express');

// create a bot with the telegram token
const bot = new Telegraf('5620276745:AAEGHDBhupZ_7pl46xoT1pzI3cnTqqP_i3Y');

// create an app with express
const app = express();
app.use(express.json());

module.exports = {bot, app};
