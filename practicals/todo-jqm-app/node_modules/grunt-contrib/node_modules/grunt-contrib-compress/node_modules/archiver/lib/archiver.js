/**
 * node-archiver
 *
 * Copyright (c) 2012-2013 Chris Talkington, contributors.
 * Licensed under the MIT license.
 * https://github.com/ctalkington/node-archiver/blob/master/LICENSE-MIT
 */

var tarArchiver = require('./archiver/tar');
var zipArchiver = require('./archiver/zip');

var archiver = module.exports = function(type, options) {
  if (type === 'zip') {
    return new zipArchiver(options);
  } else if (type === 'tar') {
    return new tarArchiver(options);
  } else {
    throw new Error('Unknown archive type');
  }
};

archiver.create = archiver;

archiver.createTar = function(options) {
  return new tarArchiver(options);
};

archiver.createZip = function(options) {
  return new zipArchiver(options);
};
