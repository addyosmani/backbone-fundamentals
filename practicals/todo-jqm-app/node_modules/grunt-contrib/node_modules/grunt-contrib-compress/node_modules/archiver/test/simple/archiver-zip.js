var fs = require('fs');

var mkdir = require('mkdirp');

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

    var archive = archiver('zip', {
      forceUTC: true
    });

    var testStream = new WriteHashStream('tmp/buffer.zip');

    archive.pipe(testStream);

    testStream.on('close', function() {
      test.equals(testStream.digest, '9c14aaaab831cad774d0dfaf665ae6da8e33577c', 'data hex values should match.');
      test.done();
    });

    archive
      .append(binaryBuffer(20000), {name: 'buffer.txt', date: date1})
      .finalize();
  },

  stream: function(test) {
    test.expect(1);

    var archive = archiver('zip', {
      forceUTC: true
    });

    var testStream = new WriteHashStream('tmp/stream.zip');

    archive.pipe(testStream);

    testStream.on('close', function() {
      test.equals(testStream.digest, 'd7e3970142a06d4a87fbd6458284eeaf8f5de907', 'data hex values should match.');
      test.done();
    });

    archive
      .append(fs.createReadStream('test/fixtures/test.txt'), {name: 'stream.txt', date: date1})
      .finalize();
  },

  string: function(test) {
    test.expect(1);

    var archive = archiver('zip', {
      forceUTC: true
    });

    var testStream = new WriteHashStream('tmp/string.zip');

    archive.pipe(testStream);

    testStream.on('close', function() {
      test.equals(testStream.digest, '3de2c37ba3745618257f6816fe979ee565e24aa0', 'data hex values should match.');
      test.done();
    });

    archive
      .append('string', {name: 'string.txt', date: date1})
      .finalize();
  },

  multiple: function(test) {
    test.expect(1);

    var archive = archiver('zip', {
      forceUTC: true
    });
    var testStream = new WriteHashStream('tmp/multiple.zip');

    archive.pipe(testStream);

    testStream.on('close', function() {
      test.equals(testStream.digest, 'dac10ec60ee700ea07a90bca3e6d1a8db2670a9b', 'data hex values should match.');
      test.done();
    });

    archive
      .append('string', {name: 'string.txt', date: date1})
      .append(binaryBuffer(20000), {name: 'buffer.txt', date: date2})
      .append(fs.createReadStream('test/fixtures/test.txt'), {name: 'stream.txt', date: date2})
      .append(fs.createReadStream('test/fixtures/test.txt'), {name: 'stream-store.txt', date: date1, store: true})
      .finalize();
  }
};

exports.store = {
  buffer: function(test) {
    test.expect(1);

    var archive = archiver('zip', {
      forceUTC: true
    });

    var testStream = new WriteHashStream('tmp/store-buffer.zip');

    archive.pipe(testStream);

    testStream.on('close', function() {
      test.equals(testStream.digest, '09305770a3272cbcd7c151ee267cb1b0075dd29e', 'data hex values should match.');
      test.done();
    });

    archive
      .append(binaryBuffer(20000), {name: 'buffer.txt', date: date1, store: true})
      .finalize();
  },

  stream: function(test) {
    test.expect(1);

    var archive = archiver('zip', {
      forceUTC: true
    });

    var testStream = new WriteHashStream('tmp/store-stream.zip');

    archive.pipe(testStream);

    testStream.on('close', function() {
      test.equals(testStream.digest, '999f407f3796b551d91608349a06521b8f80f229', 'data hex values should match.');
      test.done();
    });

    archive
      .append(fs.createReadStream('test/fixtures/test.txt'), {name: 'stream.txt', date: date1, store: true})
      .finalize();
  }
};

exports.feature = {
  comments: function(test) {
    test.expect(1);

    var archive = archiver.createZip({
      comment: 'this is a zip comment',
      forceUTC: true
    });

    var testStream = new WriteHashStream('tmp/comments.zip');

    archive.pipe(testStream);

    testStream.on('close', function() {
      test.equals(testStream.digest, 'ea7911cbe2508682c2a17d30b366ac33527ba84f', 'data hex values should match.');
      test.done();
    });

    archive
      .append(binaryBuffer(20000), {name: 'buffer.txt', date: date1, comment: 'this is a file comment'})
      .finalize();
  }
};