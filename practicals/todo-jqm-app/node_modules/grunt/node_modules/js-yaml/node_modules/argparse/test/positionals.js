/*global describe, it*/

'use strict';

var assert = require('assert');

var ArgumentParser = require('../lib/argparse').ArgumentParser;

describe('positionals', function () {
  var parser;
  var args;

  it("test the 'append' action", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'spam' ], { action: 'append' });
    parser.addArgument([ 'spam' ], { action: 'append', nargs: 2 });

    args = parser.parseArgs([ 'a', 'b', 'c' ]);
    assert.deepEqual(args, { spam: [ 'a', [ 'b', 'c' ] ] });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--foo' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a', 'b' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a', 'b', 'c', 'd' ]);
    });
  });

  it('test a set of integer choices', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'spam' ], {
      type: 'int',
      choices: [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 ]
    });

    args = parser.parseArgs([ '4' ]);
    assert.deepEqual(args, { spam: 4 });
    args = parser.parseArgs([ '15' ]);
    assert.deepEqual(args, { spam: 15 });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--foo' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'h' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '42' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'ef' ]);
    });
  });

  it('test a set of single-character choices', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'spam' ], { choices: [ 'a', 'c', 'b', 'e', 'd', 'g', 'f' ] });

    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { spam: 'a' });
    args = parser.parseArgs([ 'g' ]);
    assert.deepEqual(args, { spam: 'g' });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--foo' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'h' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '42' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'ef' ]);
    });
  });

});
