var path = require('path');
var commonConfig = require('./webpack.common.config');

var output = {
    path: path.resolve(__dirname, 'public'),
    publicPath: 'https://lords-of-cyberspace.glitch.me/',
    filename: 'client.js'
};

module.exports = Object.assign(commonConfig, {
    output: output,
    devtool: 'source-map'
});