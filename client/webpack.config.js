const path = require('path');

module.exports = {
  entry: './src/index.bs.js',
  output: {
    path: path.join(__dirname, "../server"),
    filename: 'index.js',
  },
};
