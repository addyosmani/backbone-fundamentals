/**
 * node-archiver
 *
 * Copyright (c) 2012-2013 Chris Talkington, contributors.
 * Licensed under the MIT license.
 * https://github.com/ctalkington/node-archiver/blob/master/LICENSE-MIT
 */

var util = require('../util');

var headers = module.exports = {};

headers.file = {
  fields: [
    {'field': 'name', 'length': 100, 'type': 'string'},
    {'field': 'mode', 'length': 8, 'type': 'number'},
    {'field': 'uid', 'length': 8, 'type': 'number'},
    {'field': 'gid', 'length': 8, 'type': 'number'},
    {'field': 'size','length': 12, 'type': 'number'},
    {'field': 'mtime', 'length': 12, 'type': 'number'},
    {'field': 'checksum', 'length': 8, 'type': 'string', 'default': util.repeat(' ', 8)},
    {'field': 'type', 'length': 1, 'type': 'number'},
    {'field': 'linkName', 'length': 100, 'type': 'string'},
    {'field': 'ustar', 'length': 8, 'type': 'string', 'default': 'ustar '},
    {'field': 'owner', 'length': 32, 'type': 'string'},
    {'field': 'group', 'length': 32, 'type': 'string'},
    {'field': 'majorNumber', 'length': 8, 'type': 'number'},
    {'field': 'minorNumber', 'length': 8, 'type': 'number'},
    {'field': 'filenamePrefix', 'length': 155, 'type': 'string'},
    {'field': 'padding', 'length': 12}
  ],

  toBuffer: function(data) {
    var self = this;

    var buffer = util.cleanBuffer(512);
    var offset = 0;

    var val;

    self.fields.forEach(function(value) {
      val = data[value.field] || value.default || '';

      buffer.write(val, offset);
      offset += value.length;
    });

    var checksum = self.createChecksum(buffer);

    for (var i = 0, length = 6; i < length; i += 1) {
      buffer[i + 148] = checksum.charCodeAt(i);
    }

    buffer[154] = 0;
    buffer[155] = 0x20;

    return buffer;
  },

  toObject: function(buffer) {
    var self = this;

    var data = {};
    var offset = 0;

    self.fields.forEach(function(value) {
      data[value.field] = buffer.toString('utf8', offset, offset + (value.length - 1)).replace(/\u0000.*/, '');
      offset += value.length;
    });

    delete data.padding;

    return data;
  },

  createChecksum: function(buffer) {
    var checksum = 0;
    for (var i = 0, length = buffer.length; i < length; i += 1) {
      checksum += buffer[i];
    }

    checksum = checksum.toString(8);
    while (checksum.length < 6) {
      checksum = '0' + checksum;
    }

    return checksum;
  }
};