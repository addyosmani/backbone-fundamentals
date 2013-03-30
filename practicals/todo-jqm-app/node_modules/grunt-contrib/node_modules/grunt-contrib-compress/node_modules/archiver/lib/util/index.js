/**
 * node-archiver
 *
 * Copyright (c) 2012-2013 Chris Talkington, contributors.
 * Licensed under the MIT license.
 * https://github.com/ctalkington/node-archiver/blob/master/LICENSE-MIT
 */

var fs = require('fs');
var path = require('path');
var stream = require('stream');

var util = module.exports = {};

util.crc32 = require('./crc32');

util.cleanBuffer = function(length) {
  var buffer = new Buffer(length);

  buffer.fill(0);

  return buffer;
};

util.collectStream = function(stream, callback) {
  var collection = [];
  var size = 0;

  stream.on('error', callback);

  stream.on('data', function(chunk) {
    collection.push(chunk);
    size += chunk.length;
  });

  stream.on('end', function() {
    var buffer = new Buffer(size, 'utf8');
    var offset = 0;

    collection.forEach(function(data) {
      data.copy(buffer, offset);
      offset += data.length;
    });

    callback(null, buffer);
  });

  if (typeof stream.resume === 'function') {
    stream.resume();
  }
};

util.convertDateTimeDos = function(input) {
 return new Date(
  ((input >> 25) & 0x7f) + 1980,
  ((input >> 21) & 0x0f) - 1,
  (input >> 16) & 0x1f,
  (input >> 11) & 0x1f,
  (input >> 5) & 0x3f,
  (input & 0x1f) << 1);
};

util.convertDateTimeOctal = function(input) {
  input = parseInt(input, 8) * 1000;

  return new Date(input);
};

util.dateify = function(dateish) {
  var date;

  if (dateish instanceof Date) {
    date = dateish;
  } else if (typeof dateish === 'string') {
    date = new Date(dateish);
  } else {
    date = new Date();
  }

  return date;
};

util.defaults = function(obj) {
  obj = obj || {};
  var args = Array.prototype.slice.call(arguments, 1);

  args.forEach(function(source) {
    if (source) {
      for (var prop in source) {
        if (obj[prop] == null) {
          obj[prop] = source[prop];
        }
      }
    }
  });

  return obj;
};

util.dosDateTime = function(d, utc) {
  d = (d instanceof Date) ? d : new Date();
  utc = utc || false;

  var year = (utc === true) ? d.getUTCFullYear() : d.getFullYear();

  if (year < 1980) {
    return (1<<21) | (1<<16);
  }

  var val = {
    year: year,
    month: (utc === true) ? d.getUTCMonth() : d.getMonth(),
    date: (utc === true) ? d.getUTCDate() : d.getDate(),
    hours: (utc === true) ? d.getUTCHours() : d.getHours(),
    minutes: (utc === true) ? d.getUTCMinutes() : d.getMinutes(),
    seconds: (utc === true) ? d.getUTCSeconds() : d.getSeconds()
  };

  return ((val.year-1980) << 25) | ((val.month+1) << 21) | (val.date << 16) |
    (val.hours << 11) | (val.minutes << 5) | (val.seconds / 2);
};

util.isStream = function(source) {
  return (source instanceof stream.Stream);
};

util.octalDateTime = function(d) {
  d = (d instanceof Date) ? d : new Date();

  return Math.round(d / 1000).toString(8);
};

util.padNumber = function(num, bytes, base) {
  num = num.toString(base || 8);
  return util.repeat('0', bytes - num.length) + num;
};

util.repeat = function(pattern, count) {
  if (count < 1) {
    return '';
  }

  var result = '';

  while (count > 0) {
    if (count & 1) {
      result += pattern;
    }

    count >>= 1, pattern += pattern;
  }

  return result;
};

util.sanitizeFilePath = function(filepath) {
  filepath = path.normalize(filepath);
  filepath = util.unixifyPath(filepath);

  while (filepath.substring(0, 1) === '/') {
    filepath = filepath.substring(1);
  }

  return filepath;
};

util.unixifyPath = function(filepath) {
  return filepath.replace(/\\/g, '/');
};