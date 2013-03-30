var fs = require('fs');

var mkdir = require('mkdirp');
var rimraf = require('rimraf');

var archiver = require('../../lib/archiver');
var common = require('../common');

var HashStream = common.HashStream;
var WriteHashStream = common.WriteHashStream;
var binaryBuffer = common.binaryBuffer;

var date1 = new Date('Jan 03 2013 14:26:38 GMT');
var date2 = new Date('Feb 10 2013 10:24:42 GMT');

mkdir.sync('tmp');

exports.input = {
  buffer: function(test) {
    test.expect(1);

    var archive = archiver('tar');
    var testStream = new WriteHashStream('tmp/buffer.tar');

    archive.pipe(testStream);

    testStream.on('close', function() {
      test.equals(testStream.digest, 'e87af3cdd4b01bb72ebab46baa97ee1eb814a1d3', 'data hex values should match.');
      test.done();
    });

    archive
      .append(binaryBuffer(20000), {name: 'buffer.txt', date: date1})
      .finalize();
  },

  stream: function(test) {
    test.expect(1);

    var archive = archiver('tar');
    var testStream = new WriteHashStream('tmp/stream.tar');

    archive.pipe(testStream);

    testStream.on('close', function() {
      test.equals(testStream.digest, 'da02a931d670f725c0de20ef30b112b53d149a3d', 'data hex values should match.');
      test.done();
    });

    archive
      .append(fs.createReadStream('test/fixtures/test.txt'), {name: 'stream.txt', date: date1})
      .finalize();
  },

  string: function(test) {
    test.expect(1);

    var archive = archiver('tar');
    var testStream = new WriteHashStream('tmp/string.tar');

    archive.pipe(testStream);

    testStream.on('close', function() {
      test.equals(testStream.digest, '333f843838ba5ee7727b3cc8afa017cab3d70d72', 'data hex values should match.');
      test.done();
    });

    archive
      .append('string', {name: 'string.txt', date: date1})
      .finalize();
  },

  multiple: function(test) {
    test.expect(1);

    var archive = archiver('tar');
    var testStream = new WriteHashStream('tmp/multiple.tar');

    archive.pipe(testStream);

    testStream.on('close', function() {
      test.equals(testStream.digest, '0f2cfcb20ebc1958c2a9e78ad9d969fb7cae19df', 'data hex values should match.');
      test.done();
    });

    archive
      .append('string', {name: 'string.txt', date: date1})
      .append(binaryBuffer(20000), {name: 'buffer.txt', date: date2})
      .append(fs.createReadStream('test/fixtures/test.txt'), {name: 'stream.txt', date: date1})
      .finalize();
  }
};