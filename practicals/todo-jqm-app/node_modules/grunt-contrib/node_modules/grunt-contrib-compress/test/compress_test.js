'use strict';

var grunt = require('grunt');
var path = require('path');
var compress = require('../tasks/lib/compress')(grunt);

exports.compress = {
  zip: function(test) {
    test.expect(1);

    var actual = compress.getSize('tmp/compress_test_files.zip', false);
    var expected = compress.getSize('test/expected/compress_test_files.zip', false);
    test.equal(actual, expected, 'should compress files into zip');

    test.done();
  },
  tar: function(test) {
    test.expect(1);

    var actual = compress.getSize('tmp/compress_test_files.tar', false);
    var expected = compress.getSize('test/expected/compress_test_files.tar', false);
    test.equal(actual, expected, 'should compress files into tar');

    test.done();
  },
  tgz: function(test) {
    test.expect(1);

    var actual = compress.getSize('tmp/compress_test_files.tgz', false) >= 300;
    test.equal(actual, true, 'should add files into tar. compressing the tar with gzip.');

    test.done();
  },
  gzip: function(test) {
    test.expect(3);
    [
      'test.js.gz',
      path.join('folder_one', 'one.css.gz'),
      path.join('folder_two', 'two.js.gz'),
    ].forEach(function(file) {
      var actual = compress.getSize(path.join('tmp', 'gzip', file), false);
      var expected = compress.getSize(path.join('test', 'expected', 'gzip', file), false);
      test.equal(actual, expected, 'should have been the same file size as the expected file.');
    });
    test.done();
  }
};