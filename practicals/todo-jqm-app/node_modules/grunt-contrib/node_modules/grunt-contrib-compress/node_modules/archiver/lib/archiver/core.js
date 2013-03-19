/**
 * node-archiver
 *
 * Copyright (c) 2012-2013 Chris Talkington, contributors.
 * Licensed under the MIT license.
 * https://github.com/ctalkington/node-archiver/blob/master/LICENSE-MIT
 */

require('../compat/buffer');

var inherits = require('util').inherits;
var Readable = require('stream').Readable || require('readable-stream');

var util = require('../util');

var Archiver = module.exports = function(options) {
  Readable.call(this, options);

  this.archiver = {
    processing: false,
    finalize: false,
    finalized: false,
    eof: false,
    pointer: 0,
    file: {},
    files: [],
    queue: []
  };
};

inherits(Archiver, Readable);

Archiver.prototype._read = function(n, callback) {
  this._processQueue();
};

Archiver.prototype._push = function(data) {
  this.push(data);

  if (data) {
    this.archiver.pointer += data.length;
  }
};

Archiver.prototype._emitErrorCallback = function(err, data) {
  if (err) {
    this.emit('error', err);
  }
};

Archiver.prototype._processFile = function(source, data, callback) {
  callback(new Error('method not implemented'));
};

Archiver.prototype._processQueue = function() {
  if (this.archiver.processing) {
    return;
  }

  if (this.archiver.queue.length > 0) {
    var next = this.archiver.queue.shift();

    this._processFile(next.source, next.data, next.callback);
  } else if (this.archiver.finalized && this.archiver.eof === false) {
    this.archiver.eof = true;
    this._push(null);
  } else if (this.archiver.finalize && this.archiver.queue.length === 0) {
    this._finalize();
  }
};

Archiver.prototype._finalize = function() {
  this.emit('error', new Error('method not implemented'));
};

Archiver.prototype.append = function(source, data, callback) {
  if (typeof source === 'string') {
    source = new Buffer(source, 'utf-8');
  } else if (source && source.pause) {
    source.pause();
  }

  if (typeof callback !== 'function') {
    callback = this._emitErrorCallback.bind(this);
  }

  if (this.archiver.processing || this.archiver.queue.length) {
    this.archiver.queue.push({
      data: data,
      source: source,
      callback: callback
    });
  } else {
    this._processFile(source, data, callback);
  }

  return this;
};

Archiver.prototype.addFile = Archiver.prototype.append;

Archiver.prototype.finalize = function(callback) {
  if (typeof callback === 'function') {
    this.on('end', function() {
      callback(null, this.archiver.pointer);
    }.bind(this));
  }

  this.archiver.finalize = true;

  this._processQueue();

  return this;
};