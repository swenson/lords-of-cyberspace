var ProvidePlugin = require('webpack').ProvidePlugin;
var path = require('path');

module.exports = {
    entry: [
        './js/index.js'
    ],
    module: {
        loaders: [{
            test: /\.js$/,
            exclude: /(node_modules)/,
            loader: 'babel-loader'
        }, {
            test: /\.html$/,
            loader: 'file?name=[name].[ext]'
        }, {
            test: /\.(jpe?g|png|gif)$/,
            exclude: /(node_modules)/,
            loader: 'url-loader?limit=10000'
        }]
    },
    resolve: {
        extensions: ['.js', '.css']
    }
};