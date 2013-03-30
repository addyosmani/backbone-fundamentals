/**
 * node-archiver
 *
 * Copyright (c) 2012-2013 Chris Talkington, contributors.
 * Licensed under the MIT license.
 * https://github.com/ctalkington/node-archiver/blob/master/LICENSE-MIT
 */

var inherits = require('util').inherits;

var Archiver = require('./core');
var headers = require('../headers/zip');
var util = require('../util');
var DeflateRawChecksum = require('../util/DeflateRawChecksum');

var zipArchiver = module.exports = function(options) {
  Archiver.call(this);

  options = this.options = util.defaults(options, {
    comment: '',
    forceUTC: false,
    zlib: {
      level: 1
    }
  });
};

inherits(zipArchiver, Archiver);

zipArchiver.prototype._buildCentralDirectory = function() {
  var files = this.archiver.files;
  var comment = this.options.comment;

  var cdoffset = this.archiver.pointer;

  var ptr = 0;
  var cdsize = 0;

  var centralDirectoryBuffers = [];
  var centralDirectoryBuffer;

  for (var i = 0; i < files.length; i++) {
    var file = files[i];

    centralDirectoryBuffer = headers.centralHeader.toBuffer(file);
    centralDirectoryBuffers.push(centralDirectoryBuffer);
    ptr += centralDirectoryBuffer.length;
  }

  cdsize = ptr;

  var centralDirectoryFooterData = {
    directoryRecordsDisk: files.length,
    directoryRecords: files.length,
    directorySize: cdsize,
    directoryOffset: cdoffset,
    comment: comment
  };

  var centralDirectoryFooterBuffer = headers.centralFooter.toBuffer(centralDirectoryFooterData);

  centralDirectoryBuffers.push(centralDirectoryFooterBuffer);
  ptr += centralDirectoryFooterBuffer.length;

  return Buffer.concat(centralDirectoryBuffers, ptr);
};

zipArchiver.prototype._processFile = function(source, data, callback) {
  var self = this;
  self.archiver.processing = true;

  var file = util.defaults(data, {
    name: null,
    comment: '',
    date: null,
    mode: null,
    store: false,
    lastModifiedDate: null
  });

  if (typeof file.name !== 'string' || file.name.length === 0) {
    callback(new Error('File name is empty or not a valid string value'));
    return;
  }

  file.name = util.sanitizeFilePath(file.name);
  file.date = util.dateify(file.date);

  if (typeof file.lastModifiedDate !== 'number') {
    file.lastModifiedDate = util.dosDateTime(file.date, self.options.forceUTC);
  }

  file.versionMadeBy = 20;
  file.versionNeededToExtract = 20;
  file.flags = (1<<3) | (1<<11);
  file.compressionMethod = file.store ? 0 : 8;
  file.uncompressedSize = 0;
  file.compressedSize = 0;

  self.archiver.file = file;

  file.offset = self.archiver.pointer;
  self._push(headers.file.toBuffer(file));

  var deflate;

  if (file.store === false) {
    deflate = new DeflateRawChecksum(self.options.zlib);

    deflate.on('error', callback);

    deflate.on('data', function(chunk) {
      self._push(chunk);
    });

    deflate.on('end', function() {
      file.crc32 = deflate.digest;
      file.uncompressedSize = deflate.rawSize;
      file.compressedSize = deflate.compressedSize;
      onend();
    });
  }

  function onend() {
    self._push(headers.descriptor.toBuffer(file));

    self.archiver.files.push(file);
    self.archiver.processing = false;

    callback();
    self._processQueue();
  }

  if (Buffer.isBuffer(source)) {
    if (file.store) {
      file.uncompressedSize += source.length;
      file.crc32 = util.crc32.createCRC32().update(source).digest();
      file.compressedSize = file.uncompressedSize;
      self._push(source);
      onend();
    } else {
      deflate.write(source);
      deflate.end();
    }
  } else if (util.isStream(source)) {
    if (file.store) {
      var checksum = util.crc32.createCRC32();

      source.on('error', callback);

      source.on('data', function(chunk) {
        checksum.update(chunk);
        file.uncompressedSize += chunk.length;
        self._push(chunk);
      });

      source.on('end', function () {
        file.compressedSize = file.uncompressedSize;
        file.crc32 = checksum.digest();
        onend();
      });
    } else {
      source.pipe(deflate);
    }

    if (typeof source.resume === 'function') {
      source.resume();
    }
  } else {
    callback(new Error('A valid Stream or Buffer instance is needed as input source'));
  }
};

zipArchiver.prototype._finalize = function() {
  this.archiver.finalize = false;
  this.archiver.finalized = true;

  this._push(this._buildCentralDirectory());
};