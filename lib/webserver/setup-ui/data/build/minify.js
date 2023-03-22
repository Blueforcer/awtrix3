// Finalize Nodejs Script
// 1 - Append JS in HTML Document
// 2 - Gzip HTML
// 3 - Covert to Raw Bytes
// 4 - ( Save to File: webpage.h ) in dist Folder

const fs = require('fs');
const minify = require('@node-minify/core');
const terser = require('@node-minify/terser');
const cssnano = require('@node-minify/cssnano');
// const csso = require('@node-minify/csso');
// const uglifyjs = require('@node-minify/uglify-js');
// const gcc = require('@node-minify/google-closure-compiler');
// const htmlMinifier = require('@node-minify/html-minifier');

const converter = require('./stringConverter');

minify({
  compressor: terser,
  input: '../app.js',
  output: './min/app.js',
  sync: true,
  callback: function (err, min) { if (err) console.log(err); }
});

minify({
  compressor: cssnano,
  input: '../style.css',
  output: './min/style.css',
  sync: true,
  callback: function (err, min) { if (err) console.log(err); }
});


let html = fs.readFileSync('../setup.htm').toString();
let css = fs.readFileSync('./min/style.css');
let appjs = fs.readFileSync('./min/app.js');
console.log(css.length);
console.log(appjs.length);

html = html.replace('<link href=style.css rel=stylesheet type=text/css />', `<style>${css}</style>`);
html = html.replace('<script src=app.js></script>', `<script>${appjs}</script>`);

// minify({
//   compressor: htmlMinifier,
//   content: html
// }).then(function (min) {
//   console.log('html min');
// });

fs.writeFile('./min/all.htm', html, function (err) {
  if (err) return console.log(err);
});

const { createGzip } = require('node:zlib');
const { pipeline } = require('node:stream');
const { createReadStream, createWriteStream } = require('node:fs');

const gzip = createGzip();
const source = createReadStream('./min/all.htm');
var destination = createWriteStream('./min/all.htm.gz');

pipeline(source, gzip, destination, (err) => {
  if (err) {
    console.error('An error occurred:', err);
    process.exitCode = 1;
  }

  var c_array = converter.toString(fs.readFileSync('./min/all.htm.gz'), 16);
  fs.writeFileSync('setup_htm.h', c_array, 'utf8');
})