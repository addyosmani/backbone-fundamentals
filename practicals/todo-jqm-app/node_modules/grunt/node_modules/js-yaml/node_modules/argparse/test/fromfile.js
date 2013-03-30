/*global describe, it, beforeEach, before, after*/

'use strict';

var assert = require('assert');

var ArgumentParser = require('../lib/argparse').ArgumentParser;

var fs = require('fs');
var os = require('os');
var path = require('path');
var assert = require('assert');
var _ = require('underscore');
_.str = require('underscore.string');

var oldcwd = process.cwd();

function setup_tempdir() {
  // setup a temporary directory as cwd
  var tdir = path.join(os.tmpDir(), 'argparse_temp');
  try {
    fs.mkdirSync(tdir);
  } catch (error) {
    if (!error.message.match(/EEXIST/)) {
      throw error;
    }
  }
  oldcwd = process.cwd();
  process.chdir(tdir);
  // console.log('Now in ' + process.cwd());
  return oldcwd;
}

function teardown_tempdir(oldcwd) {
  // remove the temp dir
  var tdir = process.cwd();
  process.chdir(oldcwd);
  if (_.str.startsWith(tdir, os.tmpDir())) {
    var dirls = fs.readdirSync(tdir);
    //console.log(tdir, dirls)
    dirls.forEach(function (f) {
        fs.unlinkSync(path.join(tdir, f));
      });
    fs.rmdirSync(tdir);
    //console.log('Removed ' + tdir);
  }
}

function setup_files() {
  var file_texts = [['hello', 'hello world!\n'],
              ['recursive', '-a\n', 'A\n', '@hello'],
              ['invalid', '@no-such-path\n']];

  oldcwd = setup_tempdir();
  // write the test files
  file_texts.forEach(function (tpl) {
    var filename = tpl[0];
    var data = tpl.slice(1).join('');
    fs.writeFileSync(filename, data);
  });
}

describe('from file', function () {
  var parser;
  var args;
  before(function () {
    setup_files();
  });
  beforeEach(function () {
    parser = new ArgumentParser({debug: true, fromfilePrefixChars: '@'});
    parser.addArgument(['-a']);
    parser.addArgument(['x']);
    parser.addArgument(['y'], {nargs: '+'});
  });
  after(function () {
    teardown_tempdir(oldcwd);
  });
  it("test reading arguments from a file", function () {
    args = parser.parseArgs(['X', 'Y']);
    assert.deepEqual(args, {a: null, x: 'X', y: ['Y']});
    args = parser.parseArgs(['X', '-a', 'A', 'Y', 'Z']);
    assert.deepEqual(args, { a: 'A',  x: 'X',  y: ['Y', 'Z']});
    args = parser.parseArgs(['@hello', 'X']);
    assert.deepEqual(args, {a: null, x: 'hello world!', y: ['X']});
    args = parser.parseArgs(['X', '@hello']);
    assert.deepEqual(args, {a: null, x: 'X', y: ['hello world!']});
  });
  it("test recursive reading arguments from files", function () {
    args = parser.parseArgs(['-a', 'B', '@recursive', 'Y', 'Z']);
    assert.deepEqual(args, {a: 'A', x: 'hello world!', y: ['Y', 'Z']});
    args = parser.parseArgs(['X', '@recursive', 'Z', '-a', 'B']);
    assert.deepEqual(args, {a: 'B', x: 'X', y: ['hello world!', 'Z']});
  });
  it('fest reading arguments from an invalid file', function () {
    assert.throws(
      function () {
        args = parser.parseArgs(['@invalid']);
      },
      /ENOENT, no such file or directory/
    );
  });
  it('test reading arguments from an missing file', function () {
    assert.throws(
      function () {
        args = parser.parseArgs(['@missing']);
      },
      /ENOENT, no such file or directory/
    );
  });
  it('test custom convertArgLineToArgs function', function () {
    parser.convertArgLineToArgs = function (argLine) {
        // split line into 'words'
        args = argLine.split(' ');
        args = args.map(function (arg) {return arg.trim(); });
        args = args.filter(function (arg) {return arg.length > 0; });
        return args;
      };
    args = parser.parseArgs(['X', '@hello']);
    assert.deepEqual(args, {a: null, x: 'X', y: ['hello', 'world!']});
  });
});

