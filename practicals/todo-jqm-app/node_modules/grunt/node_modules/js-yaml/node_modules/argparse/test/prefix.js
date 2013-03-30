/*global describe, it*/

'use strict';

var assert = require('assert');

var ArgumentParser = require('../lib/argparse').ArgumentParser;

describe('prefix', function () {
  var parser;
  var args;
  it('test an Optional with option strings with custom prefixes', function () {
    parser = new ArgumentParser({
      addHelp: false,
      prefixChars: '+:/',
      debug: true
    });
    parser.addArgument([ '+f' ], { action: 'storeTrue' });
    parser.addArgument([ '::bar' ], {});
    parser.addArgument([ '/baz' ], { action: 'storeConst', const: 42, constant: 42 });

    args = parser.parseArgs([]);
    assert.deepEqual(args, { bar: null, baz: null, f: false });
    args = parser.parseArgs([ '+f' ]);
    assert.deepEqual(args, { bar: null, baz: null, f: true });
    args = parser.parseArgs([ '::ba', 'B' ]);
    assert.deepEqual(args, { bar: 'B', baz: null, f: false });
    args = parser.parseArgs([ '+f', '::bar', 'B' ]);
    assert.deepEqual(args, { bar: 'B', baz: null, f: true });
    args = parser.parseArgs([ '+f', '/b' ]);
    assert.deepEqual(args, { bar: null, baz: 42, f: true });
    args = parser.parseArgs([ '/ba', '+f' ]);
    assert.deepEqual(args, { bar: null, baz: 42, f: true });

    assert.throws(function () {
      args = parser.parseArgs([ '--bar' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-fbar' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-b', 'B' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'B' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-f' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--bar', 'B' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-baz' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-h' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--help' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '+h' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '::help' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '/help' ]);
    });
  });

  // http://bugs.python.org/issue9444
  it('when "-" not in prefix_chars, default operators created for help should use the prefix_chars in use rather than - or --', function () {
    parser = new ArgumentParser({
      addHelp: true,
      prefixChars: '+:/',
      debug: true
    });
    parser.addArgument([ '+f' ], { action: 'storeTrue' });
    parser.addArgument([ '::bar' ], {});
    parser.addArgument([ '/baz' ], { action: 'storeConst', const: 42, constant: 42 });

    args = parser.parseArgs([]);
    assert.deepEqual(args, { bar: null, baz: null, f: false });
    args = parser.parseArgs([ '+f' ]);
    assert.deepEqual(args, { bar: null, baz: null, f: true });
    args = parser.parseArgs([ '::ba', 'B' ]);
    assert.deepEqual(args, { bar: 'B', baz: null, f: false });
    args = parser.parseArgs([ '+f', '::bar', 'B' ]);
    assert.deepEqual(args, { bar: 'B', baz: null, f: true });
    args = parser.parseArgs([ '+f', '/b' ]);
    assert.deepEqual(args, { bar: null, baz: 42, f: true });
    args = parser.parseArgs([ '/ba', '+f' ]);
    assert.deepEqual(args, { bar: null, baz: 42, f: true });

    assert.throws(function () {
      args = parser.parseArgs([ '--bar' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-fbar' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-b', 'B' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'B' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-f' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--bar', 'B' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-baz' ]);
    });
  });

  it('verify that Optionals must be called with their defined prefixes', function () {
    parser = new ArgumentParser({
      addHelp: false,
      prefixChars: '+-',
      debug: true
    });
    parser.addArgument([ '-x' ], { action: 'storeTrue' });
    parser.addArgument([ '+y' ], { action: 'storeTrue' });
    parser.addArgument([ '+z' ], { action: 'storeTrue' });

    args = parser.parseArgs([]);
    assert.deepEqual(args, { y: false, x: false, z: false });
    args = parser.parseArgs([ '-x' ]);
    assert.deepEqual(args, { y: false, x: true, z: false });
    args = parser.parseArgs([ '+y', '-x' ]);
    assert.deepEqual(args, { y: true, x: true, z: false });
    args = parser.parseArgs([ '+yz', '-x' ]);
    assert.deepEqual(args, { y: true, x: true, z: true });

    assert.throws(function () {
      args = parser.parseArgs([ '-w' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-xyz' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '+x' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-y' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '+xyz' ]);
    });
  });

  it('test when one double-dash option string is a prefix of another', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ '--badger' ], { action: 'storeTrue' });
    parser.addArgument([ '--ba' ], {});

    args = parser.parseArgs([]);
    assert.deepEqual(args, { ba: null, badger: false });
    args = parser.parseArgs([ '--ba', 'X' ]);
    assert.deepEqual(args, { ba: 'X', badger: false });
    args = parser.parseArgs([ '--ba=X' ]);
    assert.deepEqual(args, { ba: 'X', badger: false });
    args = parser.parseArgs([ '--bad' ]);
    assert.deepEqual(args, { ba: null, badger: true });
    args = parser.parseArgs([ '--badg' ]);
    assert.deepEqual(args, { ba: null, badger: true });
    args = parser.parseArgs([ '--badge' ]);
    assert.deepEqual(args, { ba: null, badger: true });
    args = parser.parseArgs([ '--badger' ]);
    assert.deepEqual(args, { ba: null, badger: true });

    assert.throws(function () {
      args = parser.parseArgs([ '--bar' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--b' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--ba' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--b=2' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--badge', '5' ]);
    });
  });

  it('test arguments from prefix chars only', function () {
    parser = new ArgumentParser({
      prefixChars: '-+',
      debug: true
    });
    parser.addArgument([ '-' ], { dest: 'x', const: 'badger', nargs: '?', constant: 'badger' });
    parser.addArgument([ '+' ], { default: 42, dest: 'y', type: 'int', defaultValue: 42 });
    parser.addArgument([ '-+-' ], { action: 'storeTrue', dest: 'z' });

    args = parser.parseArgs([]);
    assert.deepEqual(args, { y: 42, x: null, z: false });
    args = parser.parseArgs([ '-' ]);
    assert.deepEqual(args, { y: 42, x: 'badger', z: false });
    args = parser.parseArgs([ '-', 'X' ]);
    assert.deepEqual(args, { y: 42, x: 'X', z: false });
    args = parser.parseArgs([ '+', '-3' ]);
    assert.deepEqual(args, { y: -3, x: null, z: false });
    args = parser.parseArgs([ '-+-' ]);
    assert.deepEqual(args, { y: 42, x: null, z: true });
    args = parser.parseArgs([ '-', '===' ]);
    assert.deepEqual(args, { y: 42, x: '===', z: false });

    assert.throws(function () {
      args = parser.parseArgs([ '-y' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '+', '-' ]);
    });
  });
 
});
