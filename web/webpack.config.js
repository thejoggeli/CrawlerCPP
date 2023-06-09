const HtmlWebpackPlugin = require('html-webpack-plugin');
const CopyWebpackPlugin = require('copy-webpack-plugin');
const path = require('path');

module.exports = {
    module: {
        rules: [
            {
                test: /\.js$/,
                exclude: /node_modules/,
                use: {
                    loader: 'babel-loader',
                },
            },
            {
                test: /\.(png|jpg|gif)$/,
                use: [
                    {
                        loader: 'url-loader',
                        options: {
                            limit: 8192
                        }
                    }]
            },
            {
                test: /\.css$/i,
                use: ["style-loader", "css-loader"],
            },
        ],
    },
    plugins: [
        new HtmlWebpackPlugin({
            template: './public/index.html',
            filename: './index.html',
        }),
        new CopyWebpackPlugin({
            patterns: [
                { from: "public/img", to: "img/" },
                { from: "public/css", to: "css/" },
                { from: "public/font", to: "font/" },
            ],
        }),
    ],
    resolve: {
        alias: {
            "logic": path.resolve(__dirname, 'src/logic/'),
            "msl": path.resolve(__dirname, 'src/msl/'),
        },
    },
};