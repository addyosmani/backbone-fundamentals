/**
 * node-archiver
 *
 * Copyright (c) 2012-2013 Chris Talkington, contributors.
 * Licensed under the MIT license.
 * https://github.com/ctalkington/node-archiver/blob/master/LICENSE-MIT
 */

var inherits = require('util').inherits;

var Archiver = require('./core');
var headers = require('../headers/tar');
var util = require('../util');

var tarArchiver = module.exports = function(options) {
  Archiver.call(this);

  options = this.options = util.defaults(options, {
    recordSize: 512,
    recordsPerBlock: 20
  });

  this.recordSize = options.recordSize;
  this.blockSize = options.recordsPerBlock * options.recordSize;
};

inherits(tarArchiver, Archiver);

tarArchiver.prototype._processFile = function(source, data, callback) {
  var self = this;
  self.archiver.processing = true;

  var file = util.defaults(data, {
    name: null,
    comment: '',
    date: null,
    gid: 0,
    mode: null,
    mtime: null,
    uid: 0
  });

  if (typeof file.name !== 'string' || file.name.length === 0) {
    callback(new Error('File name is empty or not a valid string value'));
    return;
  }

  file.name = util.sanitizeFilePath(file.name);
  file.type = '0';
  file.size = 0;

  file.date = util.dateify(file.date);

  if (typeof file.mtime !== 'number') {
    file.mtime = util.octalDateTime(file.date);
  }

  file.gid = typeof file.gid === 'number' ? file.gid : 0;
  file.mode = typeof file.mode === 'number' ? file.mode : parseInt('777', 8) & 0xfff;
  file.uid = typeof file.uid === 'number' ? file.uid : 0;

  file.mode = util.padNumber(file.mode, 7);
  file.uid = util.padNumber(file.uid, 7);
  file.gid = util.padNumber(file.gid, 7);
  file.mtime = util.padNumber(file.mtime, 11);

  file.offset = self.archiver.pointer;

  self.archiver.file = file;

  var sourceBuffer;
  var extraBytes;

  function onend() {
    file.size = util.padNumber(sourceBuffer.length, 11);
    extraBytes = self.recordSize - (sourceBuffer.length % self.recordSize || self.recordSize);

    self._push(headers.file.toBuffer(file));
    self._push(sourceBuffer);
    self._push(util.cleanBuffer(extraBytes));

    self.archiver.files.push(file);
    self.archiver.processing = false;

    callback();
    self._processQueue();
  }

  if (Buffer.isBuffer(source)) {
    sourceBuffer = source;
    onend();
  } else if (util.isStream(source)) {
    util.collectStream(source, function(err, buffer) {
      if (err) {
        callback(err);
        return;
      }

      sourceBuffer = buffer;
      onend();
    });
  } else {
    callback(new Error('A valid Stream or Buffer instance is needed as input source'));
  }
};

tarArchiver.prototype._finalize = function() {
  var endBytes = this.blockSize - (this.archiver.pointer % this.blockSize);

  this.archiver.finalize = false;
  this.archiver.finalized = true;

  this._push(util.cleanBuffer(endBytes));
};