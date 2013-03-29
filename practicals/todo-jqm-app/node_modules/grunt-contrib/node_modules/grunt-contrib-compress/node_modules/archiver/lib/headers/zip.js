/**
 * node-archiver
 *
 * Copyright (c) 2012-2013 Chris Talkington, contributors.
 * Licensed under the MIT license.
 * https://github.com/ctalkington/node-archiver/blob/master/LICENSE-MIT
 */

var headers = module.exports = {};

headers.file = {
  fields: [
    {'field': 'signature', 'length': 4, 'type': 'UInt32LE', 'default': 0x04034b50},
    {'field': 'versionNeededToExtract', 'length': 2, 'type': 'UInt16LE'},
    {'field': 'flags', 'length': 2, 'type': 'UInt16LE'},
    {'field': 'compressionMethod', 'length': 2, 'type': 'UInt16LE'},
    {'field': 'lastModifiedDate', 'length': 4, 'type': 'UInt32LE'},
    {'field': 'crc32', 'length': 4, 'type': 'Int32LE', 'default': 0},
    {'field': 'compressedSize', 'length': 4, 'type': 'UInt32LE'},
    {'field': 'uncompressedSize', 'length': 4, 'type': 'UInt32LE'},
    {'field': 'filenameLength', 'length': 2, 'type': 'UInt16LE'},
    {'field': 'extraFieldLength', 'length': 2, 'type': 'UInt16LE'},
    {'field': 'name', 'length': -1, 'type': 'string'},
    {'field': 'extraField', 'length': -1, 'type': 'string'}
  ],

  toBuffer: function(data) {
    var self = this;

    var buffer = new Buffer(1024);
    var offset = 0;
    var val;
    var fallback;

    self.fields.forEach(function(value) {
      fallback = (value.type === 'string') ? '' : 0;
      val = data[value.field] || value.default || fallback;

      if (value.field === 'name') {
        value.length = buffer.write(val, offset);
        buffer.writeUInt16LE(value.length, 26);
      } else if (value.field === 'extraField') {
        value.length = (val.length > 0) ? buffer.write(val, offset) : 0;
        buffer.writeUInt16LE(value.length, 28);
      } else if (value.type === 'UInt32LE') {
        buffer.writeUInt32LE(val, offset);
      } else if (value.type === 'Int32LE') {
        buffer.writeInt32LE(val, offset);
      } else if (value.type === 'UInt16LE') {
        buffer.writeUInt16LE(val, offset);
      } else {
        buffer.write(val, offset);
      }

      offset += value.length;
    });

    return buffer.slice(0, offset);
  }
};

headers.descriptor = {
  fields: [
    {'field': 'signature', 'length': 4, 'type': 'UInt32LE', 'default': 0x08074b50},
    {'field': 'crc32', 'length': 4, 'type': 'Int32LE'},
    {'field': 'compressedSize', 'length': 4, 'type': 'UInt32LE'},
    {'field': 'uncompressedSize', 'length': 4, 'type': 'UInt32LE'}
  ],

  toBuffer: function(data) {
    var self = this;

    var buffer = new Buffer(16);
    var offset = 0;
    var val;

    self.fields.forEach(function(value) {
      val = data[value.field] || value.default || 0;

      if (value.type === 'UInt32LE') {
        buffer.writeUInt32LE(val, offset);
      } else if (value.type === 'Int32LE') {
        buffer.writeInt32LE(val, offset);
      }

      offset += value.length;
    });

    return buffer;
  }
};

headers.centralHeader = {
  fields: [
    {'field': 'signature', 'length': 4, 'type': 'UInt32LE', 'default': 0x02014b50},
    {'field': 'versionMadeBy', 'length': 2, 'type': 'UInt16LE'},
    {'field': 'versionNeededToExtract', 'length': 2, 'type': 'UInt16LE'},
    {'field': 'flags', 'length': 2, 'type': 'UInt16LE'},
    {'field': 'compressionMethod', 'length': 2, 'type': 'UInt16LE'},
    {'field': 'lastModifiedDate', 'length': 4, 'type': 'UInt32LE'},
    {'field': 'crc32', 'length': 4, 'type': 'Int32LE'},
    {'field': 'compressedSize', 'length': 4, 'type': 'UInt32LE'},
    {'field': 'uncompressedSize', 'length': 4, 'type': 'UInt32LE'},
    {'field': 'filenameLength', 'length': 2, 'type': 'UInt16LE'},
    {'field': 'extraFieldLength', 'length': 2, 'type': 'UInt16LE'},
    {'field': 'commentLength', 'length': 2, 'type': 'UInt16LE'},
    {'field': 'diskNumberStart', 'length': 2, 'type': 'UInt16LE'},
    {'field': 'internalFileAttributes', 'length': 2, 'type': 'UInt16LE'},
    {'field': 'externalFileAttributes', 'length': 4, 'type': 'UInt32LE'},
    {'field': 'offset', 'length': 4, 'type': 'UInt32LE'},
    {'field': 'name', 'length': -1, 'type': 'string'},
    {'field': 'extraField', 'length': -1, 'type': 'string'},
    {'field': 'comment', 'length': -1, 'type': 'string'}
  ],

  toBuffer: function(data) {
    var self = this;

    var buffer = new Buffer(1024);
    var offset = 0;
    var val;
    var fallback;

    var nameLength;

    self.fields.forEach(function(value) {
      fallback = (value.type === 'string') ? '' : 0;
      val = data[value.field] || value.default || fallback;

      if (value.field === 'name') {
        value.length = buffer.write(val, offset);
        buffer.writeUInt16LE(value.length, 28);
      } else if (value.field === 'extraField') {
        value.length =  (val.length > 0) ? buffer.write(val, offset) : 0;
        buffer.writeUInt16LE(value.length, 30);
      } else if (value.field === 'comment') {
        value.length = (val.length > 0) ? buffer.write(val, offset) : 0;
        buffer.writeUInt16LE(value.length, 32);
      } else if (value.type === 'UInt32LE') {
        buffer.writeUInt32LE(val, offset);
      } else if (value.type === 'Int32LE') {
        buffer.writeInt32LE(val, offset);
      } else if (value.type === 'UInt16LE') {
        buffer.writeUInt16LE(val, offset);
      } else {
        buffer.write(val, offset);
      }

      offset += value.length;
    });

    return buffer.slice(0, offset);
  }
};

headers.centralFooter = {
  fields: [
    {'field': 'signature', 'length': 4, 'type': 'UInt32LE', 'default': 0x06054b50},
    {'field': 'diskNumber', 'length': 2, 'type': 'UInt16LE'},
    {'field': 'diskNumberStart', 'length': 2, 'type': 'UInt16LE'},
    {'field': 'directoryRecordsDisk', 'length': 2, 'type': 'UInt16LE'},
    {'field': 'directoryRecords', 'length': 2, 'type': 'UInt16LE'},
    {'field': 'directorySize', 'length': 4, 'type': 'UInt32LE'},
    {'field': 'directoryOffset', 'length': 4, 'type': 'UInt32LE'},,
    {'field': 'commentLength', 'length': 2, 'type': 'UInt16LE'},
    {'field': 'comment', 'length': -1, 'type': 'string'}
  ],

  toBuffer: function(data) {
    var self = this;

    var buffer = new Buffer(512);
    var offset = 0;
    var val;
    var fallback;

    self.fields.forEach(function(value) {
      fallback = (value.type === 'string') ? '' : 0;
      val = data[value.field] || value.default || fallback;

      if (value.field === 'comment') {
        value.length = (val.length > 0) ? buffer.write(val, offset) : 0;
        buffer.writeUInt16LE(value.length, 20);
      } else if (value.type === 'UInt32LE') {
        buffer.writeUInt32LE(val, offset);
      } else if (value.type === 'Int32LE') {
        buffer.writeInt32LE(val, offset);
      } else if (value.type === 'UInt16LE') {
        buffer.writeUInt16LE(val, offset);
      } else {
        buffer.write(val, offset);
      }

      offset += value.length;
    });

    return buffer.slice(0, offset);
  }
};