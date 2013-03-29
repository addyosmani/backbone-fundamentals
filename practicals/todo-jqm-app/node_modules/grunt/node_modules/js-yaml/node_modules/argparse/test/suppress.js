/*global describe, it*/

'use strict';

var assert = require('assert');

var ArgumentParser = require('../lib/argparse').ArgumentParser;
var $$ = require('../lib/const');

describe('suppress defaults', function () {
  var parser;
  var args;
  it('test actions with suppressed defaults', function () {
    parser = new ArgumentParser({debug: true});

    parser.addArgument([ 'foo' ], {
      default: $$.SUPPRESS,
      nargs: '?',
      defaultValue: $$.SUPPRESS
    });
    parser.addArgument([ 'bar' ], {
      default: $$.SUPPRESS,
      nargs: '*',
      defaultValue: $$.SUPPRESS
    });
    parser.addArgument([ '--baz' ], {
      default: $$.SUPPRESS,
      action: 'storeTrue',
      defaultValue: $$.SUPPRESS
    });

    args = parser.parseArgs([]);
    assert.deepEqual(args, {});
    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { foo: 'a' });
    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: 'a', bar: [ 'b' ] });
    args = parser.parseArgs([ '--baz' ]);
    assert.deepEqual(args, { baz: true });
    args = parser.parseArgs([ 'a', '--baz' ]);
    assert.deepEqual(args, { foo: 'a', baz: true });
    args = parser.parseArgs([ '--baz', 'a', 'b' ]);
    assert.deepEqual(args, { bar: [ 'b' ], foo: 'a', baz: true });

    assert.throws(function () {
      args = parser.parseArgs([ '-x' ]);
    });
  });

  it('test actions with a parser-level default of SUPPRESS', function () {
    parser = new ArgumentParser({
      argument_default: $$.SUPPRESS,
      argumentDefault: $$.SUPPRESS,
      debug: true
    });
    parser.addArgument([ 'foo' ], { nargs: '?' });
    parser.addArgument([ 'bar' ], { nargs: '*' });
    parser.addArgument([ '--baz' ], { action: 'storeTrue' });

    args = parser.parseArgs([]);
    assert.deepEqual(args, {});
    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { foo: 'a' });
    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: 'a', bar: [ 'b' ] });
    args = parser.parseArgs([ '--baz' ]);
    assert.deepEqual(args, { baz: true });
    args = parser.parseArgs([ 'a', '--baz' ]);
    assert.deepEqual(args, { foo: 'a', baz: true });
    args = parser.parseArgs([ '--baz', 'a', 'b' ]);
    assert.deepEqual(args, { bar: [ 'b' ], foo: 'a', baz: true });

    assert.throws(function () {
      args = parser.parseArgs([ '-x' ]);
    });
  });

});
