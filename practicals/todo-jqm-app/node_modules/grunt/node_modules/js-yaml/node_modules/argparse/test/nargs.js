/*global describe, it*/

'use strict';

var assert = require('assert');

var ArgumentParser = require('../lib/argparse').ArgumentParser;

var $$ = require('../lib/const');

describe('nargs', function () {
  var parser;
  var args;
  it('test specifying the 1 arg for an Optional', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ '-x' ], { nargs: 1 });

    args = parser.parseArgs([]);
    assert.deepEqual(args, { x: null });
    args = parser.parseArgs([ '-x', 'a' ]);
    assert.deepEqual(args, { x: [ 'a' ] });

    assert.throws(function () {
      args = parser.parseArgs([ 'a' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-x' ]);
    });
  });

  it('test specifying the 3 args for an Optional', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ '-x' ], { nargs: 3 });

    args = parser.parseArgs([]);
    assert.deepEqual(args, { x: null });
    args = parser.parseArgs([ '-x', 'a', 'b', 'c' ]);
    assert.deepEqual(args, { x: [ 'a', 'b', 'c' ] });

    assert.throws(function () {
      args = parser.parseArgs([ 'a' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-x' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-x', 'a' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-x', 'a', 'b' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a', '-x' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a', '-x', 'b' ]);
    });
  });

  it('tests not specifying the number of args for an Optional', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ '-x' ], {});

    args = parser.parseArgs([]);
    assert.deepEqual(args, { x: null });
    args = parser.parseArgs([ '-x', 'a' ]);
    assert.deepEqual(args, { x: 'a' });

    assert.throws(function () {
      args = parser.parseArgs([ 'a' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-x' ]);
    });
  });

  it('test specifying an args for an Optional, that accepts one or more', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ '-x' ], { nargs: '+' });
    parser.addArgument([ '-y' ], { default: 'spam', nargs: '+', defaultValue: 'spam' });

    args = parser.parseArgs([]);
    assert.deepEqual(args, { y: 'spam', x: null });
    args = parser.parseArgs([ '-x', 'a' ]);
    assert.deepEqual(args, { y: 'spam', x: [ 'a' ] });
    args = parser.parseArgs([ '-x', 'a', 'b' ]);
    assert.deepEqual(args, { y: 'spam', x: [ 'a', 'b' ] });
    args = parser.parseArgs([ '-y', 'a' ]);
    assert.deepEqual(args, { y: [ 'a' ], x: null });
    args = parser.parseArgs([ '-y', 'a', 'b' ]);
    assert.deepEqual(args, { y: [ 'a', 'b' ], x: null });

    assert.throws(function () {
      args = parser.parseArgs([ 'a' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-x' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-y' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a', '-x' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a', '-y', 'b' ]);
    });
  });

  it('test specifying an Optional arg for an Optional', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ '-w' ], { nargs: '?' });
    parser.addArgument([ '-x' ], { const: 42, nargs: '?', constant: 42 });
    parser.addArgument([ '-y' ], { default: 'spam', nargs: '?', defaultValue: 'spam' });
    parser.addArgument([ '-z' ], {
      default: '84',
      nargs: '?',
      type: 'int',
      const: '42',
      defaultValue: '84',
      constant: '42'
    });

    args = parser.parseArgs([]);
    assert.deepEqual(args, { y: 'spam', x: null, z: 84, w: null });
    args = parser.parseArgs([ '-w' ]);
    assert.deepEqual(args, { y: 'spam', x: null, z: 84, w: null });
    args = parser.parseArgs([ '-w', '2' ]);
    assert.deepEqual(args, { y: 'spam', x: null, z: 84, w: '2' });
    args = parser.parseArgs([ '-x' ]);
    assert.deepEqual(args, { y: 'spam', x: 42, z: 84, w: null });
    args = parser.parseArgs([ '-x', '2' ]);
    assert.deepEqual(args, { y: 'spam', x: '2', z: 84, w: null });
    args = parser.parseArgs([ '-y' ]);
    assert.deepEqual(args, { y: null, x: null, z: 84, w: null });
    args = parser.parseArgs([ '-y', '2' ]);
    assert.deepEqual(args, { y: '2', x: null, z: 84, w: null });
    args = parser.parseArgs([ '-z' ]);
    assert.deepEqual(args, { y: 'spam', x: null, z: 42, w: null });
    args = parser.parseArgs([ '-z', '2' ]);
    assert.deepEqual(args, { y: 'spam', x: null, z: 2, w: null });

    assert.throws(function () {
      args = parser.parseArgs([ '2' ]);
    });
  });

  it('test specifying an args for an Optional that accepts zero or more', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ '-x' ], { nargs: '*' });
    parser.addArgument([ '-y' ], { default: 'spam', nargs: '*', defaultValue: 'spam' });

    args = parser.parseArgs([]);
    assert.deepEqual(args, { y: 'spam', x: null });
    args = parser.parseArgs([ '-x' ]);
    assert.deepEqual(args, { y: 'spam', x: [] });
    args = parser.parseArgs([ '-x', 'a' ]);
    assert.deepEqual(args, { y: 'spam', x: [ 'a' ] });
    args = parser.parseArgs([ '-x', 'a', 'b' ]);
    assert.deepEqual(args, { y: 'spam', x: [ 'a', 'b' ] });
    args = parser.parseArgs([ '-y' ]);
    assert.deepEqual(args, { y: [], x: null });
    args = parser.parseArgs([ '-y', 'a' ]);
    assert.deepEqual(args, { y: [ 'a' ], x: null });
    args = parser.parseArgs([ '-y', 'a', 'b' ]);
    assert.deepEqual(args, { y: [ 'a', 'b' ], x: null });

    assert.throws(function () {
      args = parser.parseArgs([ 'a' ]);
    });
  });

  it('test a Positional that specifies an nargs of 1', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { nargs: 1 });

    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { foo: [ 'a' ] });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-x' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a', 'b' ]);
    });
  });

  it('test a Positional that specifies an nargs of 2', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { nargs: 2 });

    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: [ 'a', 'b' ] });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-x' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a', 'b', 'c' ]);
    });
  });

  it('test a Positional with 2 nargs followed by one with none', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { nargs: 2 });
    parser.addArgument([ 'bar' ], {});

    args = parser.parseArgs([ 'a', 'b', 'c' ]);
    assert.deepEqual(args, { foo: [ 'a', 'b' ], bar: 'c' });

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

  it('test a Positional with 2 nargs followed by one with one or more', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { nargs: 2 });
    parser.addArgument([ 'bar' ], { nargs: '+' });

    args = parser.parseArgs([ 'a', 'b', 'c' ]);
    assert.deepEqual(args, { foo: [ 'a', 'b' ], bar: [ 'c' ] });

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
  });

  it('test a Positional with 2 nargs followed by one optional', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { nargs: 2 });
    parser.addArgument([ 'bar' ], { nargs: '?' });

    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: [ 'a', 'b' ], bar: null });
    args = parser.parseArgs([ 'a', 'b', 'c' ]);
    assert.deepEqual(args, { foo: [ 'a', 'b' ], bar: 'c' });

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
      args = parser.parseArgs([ 'a', 'b', 'c', 'd' ]);
    });
  });

  it('test a Positional with 2 nargs followed by one with unlimited', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { nargs: 2 });
    parser.addArgument([ 'bar' ], { nargs: '*' });

    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: [ 'a', 'b' ], bar: [] });
    args = parser.parseArgs([ 'a', 'b', 'c' ]);
    assert.deepEqual(args, { foo: [ 'a', 'b' ], bar: [ 'c' ] });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--foo' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a' ]);
    });
  });

  it('test a Positional that doesn\'t specify nargs', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], {});

    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { foo: 'a' });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-x' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a', 'b' ]);
    });
  });

  it('test a Positional with no nargs followed by one with 1', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], {});
    parser.addArgument([ 'bar' ], { nargs: 1 });

    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: 'a', bar: [ 'b' ] });

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
      args = parser.parseArgs([ 'a', 'b', 'c' ]);
    });
  });

  it('test two Positionals that don\'t specify nargs', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], {});
    parser.addArgument([ 'bar' ], {});

    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: 'a', bar: 'b' });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-x' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a', 'b', 'c' ]);
    });
  });

  it('test a Positional with no nargs followed by one with one or more', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], {});
    parser.addArgument([ 'bar' ], { nargs: '+' });

    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: 'a', bar: [ 'b' ] });
    args = parser.parseArgs([ 'a', 'b', 'c' ]);
    assert.deepEqual(args, { foo: 'a', bar: [ 'b', 'c' ] });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--foo' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a' ]);
    });
  });

  it('test three Positionals: no nargs, one or more nargs and 1 nargs', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], {});
    parser.addArgument([ 'bar' ], { nargs: '+' });
    parser.addArgument([ 'baz' ], { nargs: 1 });

    args = parser.parseArgs([ 'a', 'b', 'c' ]);
    assert.deepEqual(args, { bar: [ 'b' ], foo: 'a', baz: [ 'c' ] });
    args = parser.parseArgs([ 'a', 'b', 'c', 'd' ]);
    assert.deepEqual(args, { bar: [ 'b', 'c' ], foo: 'a', baz: [ 'd' ] });

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
      args = parser.parseArgs([ 'b' ]);
    });
  });

  it('test a Positional with no nargs followed by one with an Optional', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], {});
    parser.addArgument([ 'bar' ], { nargs: '?' });

    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { foo: 'a', bar: null });
    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: 'a', bar: 'b' });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--foo' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a', 'b', 'c' ]);
    });
  });

  it('test three Positionals: no nargs, optional narg and 1 nargs', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], {});
    parser.addArgument([ 'bar' ], { default: 0.625, nargs: '?', defaultValue: 0.625 });
    parser.addArgument([ 'baz' ], { nargs: 1 });

    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { bar: 0.625, foo: 'a', baz: [ 'b' ] });
    args = parser.parseArgs([ 'a', 'b', 'c' ]);
    assert.deepEqual(args, { bar: 'b', foo: 'a', baz: [ 'c' ] });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--foo' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a' ]);
    });
  });

  it('test a Positional with no nargs followed by one with unlimited', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], {});
    parser.addArgument([ 'bar' ], { nargs: '*' });

    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { foo: 'a', bar: [] });
    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: 'a', bar: [ 'b' ] });
    args = parser.parseArgs([ 'a', 'b', 'c' ]);
    assert.deepEqual(args, { foo: 'a', bar: [ 'b', 'c' ] });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--foo' ]);
    });
  });

  it('test three Positionals: no nargs, unlimited nargs and 1 nargs', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], {});
    parser.addArgument([ 'bar' ], { nargs: '*' });
    parser.addArgument([ 'baz' ], { nargs: 1 });

    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { bar: [], foo: 'a', baz: [ 'b' ] });
    args = parser.parseArgs([ 'a', 'b', 'c' ]);
    assert.deepEqual(args, { bar: [ 'b' ], foo: 'a', baz: [ 'c' ] });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--foo' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a' ]);
    });
  });

  it('test a Positional that specifies one or more nargs', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { nargs: '+' });

    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { foo: [ 'a' ] });
    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: [ 'a', 'b' ] });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-x' ]);
    });
  });

  it('test a Positional with one or more nargs followed by one with 1', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { nargs: '+' });
    parser.addArgument([ 'bar' ], { nargs: 1 });

    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: [ 'a' ], bar: [ 'b' ] });
    args = parser.parseArgs([ 'a', 'b', 'c' ]);
    assert.deepEqual(args, { foo: [ 'a', 'b' ], bar: [ 'c' ] });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--foo' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a' ]);
    });
  });

  it('test a Positional with one or more nargs followed by one with none', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { nargs: '+' });
    parser.addArgument([ 'bar' ], {});

    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: [ 'a' ], bar: 'b' });
    args = parser.parseArgs([ 'a', 'b', 'c' ]);
    assert.deepEqual(args, { foo: [ 'a', 'b' ], bar: 'c' });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--foo' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a' ]);
    });
  });

  it('tests an Optional Positional', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { nargs: '?' });

    args = parser.parseArgs([]);
    assert.deepEqual(args, { foo: null });
    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { foo: 'a' });

    assert.throws(function () {
      args = parser.parseArgs([ '-x' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a', 'b' ]);
    });
  });

  it('test a Positional with an Optional nargs followed by one with 1', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { nargs: '?' });
    parser.addArgument([ 'bar' ], { nargs: 1 });

    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { foo: null, bar: [ 'a' ] });
    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: 'a', bar: [ 'b' ] });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--foo' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a', 'b', 'c' ]);
    });
  });

  it('test an Optional Positional with a default value (that needs to be converted to the appropriate type.)', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { default: '42', type: 'int', nargs: '?', defaultValue: '42' });

    args = parser.parseArgs([]);
    assert.deepEqual(args, { foo: 42 });
    args = parser.parseArgs([ '1' ]);
    assert.deepEqual(args, { foo: 1 });

    assert.throws(function () {
      args = parser.parseArgs([ '-x' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a', 'b' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '1', '2' ]);
    });
  });

  it('tests an Optional Positional with a default value', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { default: 42, nargs: '?', defaultValue: 42 });

    args = parser.parseArgs([]);
    assert.deepEqual(args, { foo: 42 });
    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { foo: 'a' });

    assert.throws(function () {
      args = parser.parseArgs([ '-x' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a', 'b' ]);
    });
  });

  it('test a Positional with an Optional nargs followed by one with none', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { default: 42, nargs: '?', defaultValue: 42 });
    parser.addArgument([ 'bar' ], {});

    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { foo: 42, bar: 'a' });
    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: 'a', bar: 'b' });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--foo' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a', 'b', 'c' ]);
    });
  });

  it('test an Optional narg followed by one or more nargs', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { nargs: '?' });
    parser.addArgument([ 'bar' ], { nargs: '+' });

    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { foo: null, bar: [ 'a' ] });
    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: 'a', bar: [ 'b' ] });
    args = parser.parseArgs([ 'a', 'b', 'c' ]);
    assert.deepEqual(args, { foo: 'a', bar: [ 'b', 'c' ] });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--foo' ]);
    });
  });

  it('test two optional nargs', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { nargs: '?' });
    parser.addArgument([ 'bar' ], { default: 42, nargs: '?', defaultValue: 42 });

    args = parser.parseArgs([]);
    assert.deepEqual(args, { foo: null, bar: 42 });
    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { foo: 'a', bar: 42 });
    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: 'a', bar: 'b' });

    assert.throws(function () {
      args = parser.parseArgs([ '--foo' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ 'a', 'b', 'c' ]);
    });
  });

  it('test an Optional narg followed by unlimited nargs', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { nargs: '?' });
    parser.addArgument([ 'bar' ], { nargs: '*' });

    args = parser.parseArgs([]);
    assert.deepEqual(args, { foo: null, bar: [] });
    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { foo: 'a', bar: [] });
    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: 'a', bar: [ 'b' ] });
    args = parser.parseArgs([ 'a', 'b', 'c' ]);
    assert.deepEqual(args, { foo: 'a', bar: [ 'b', 'c' ] });

    assert.throws(function () {
      args = parser.parseArgs([ '--foo' ]);
    });
  });

  it('test a Positional that specifies unlimited nargs', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { nargs: '*' });

    args = parser.parseArgs([]);
    assert.deepEqual(args, { foo: [] });
    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { foo: [ 'a' ] });
    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: [ 'a', 'b' ] });

    assert.throws(function () {
      args = parser.parseArgs([ '-x' ]);
    });
  });

  it('test a Positional with unlimited nargs followed by one with 1', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { nargs: '*' });
    parser.addArgument([ 'bar' ], { nargs: 1 });

    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { foo: [], bar: [ 'a' ] });
    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: [ 'a' ], bar: [ 'b' ] });
    args = parser.parseArgs([ 'a', 'b', 'c' ]);
    assert.deepEqual(args, { foo: [ 'a', 'b' ], bar: [ 'c' ] });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--foo' ]);
    });
  });

  it('test a Positional that specifies unlimited nargs and a default', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { default: 'bar', nargs: '*', defaultValue: 'bar' });

    args = parser.parseArgs([]);
    assert.deepEqual(args, { foo: 'bar' });
    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { foo: [ 'a' ] });
    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: [ 'a', 'b' ] });

    assert.throws(function () {
      args = parser.parseArgs([ '-x' ]);
    });
  });

  it('test a Positional with unlimited nargs followed by one with none', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'foo' ], { nargs: '*' });
    parser.addArgument([ 'bar' ], {});

    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { foo: [], bar: 'a' });
    args = parser.parseArgs([ 'a', 'b' ]);
    assert.deepEqual(args, { foo: [ 'a' ], bar: 'b' });
    args = parser.parseArgs([ 'a', 'b', 'c' ]);
    assert.deepEqual(args, { foo: [ 'a', 'b' ], bar: 'c' });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '--foo' ]);
    });
  });


  it('test specifying a positional with nargs=REMAINDER', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'x' ], {});
    parser.addArgument([ 'y' ], { nargs: $$.REMAINDER });
    parser.addArgument([ '-z' ], {});

    args = parser.parseArgs([ 'X' ]);
    assert.deepEqual(args, { y: [], x: 'X', z: null });
    args = parser.parseArgs([ '-z', 'Z', 'X' ]);
    assert.deepEqual(args, { y: [], x: 'X', z: 'Z' });
    args = parser.parseArgs([ 'X', 'A', 'B', '-z', 'Z' ]);
    assert.deepEqual(args, { y: [ 'A', 'B', '-z', 'Z' ], x: 'X', z: null });
    args = parser.parseArgs([ 'X', 'Y', '--foo' ]);
    assert.deepEqual(args, { y: [ 'Y', '--foo' ], x: 'X', z: null });

    assert.throws(function () {
      args = parser.parseArgs([]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-z' ]);
    });
    assert.throws(function () {
      args = parser.parseArgs([ '-z', 'Z' ]);
    });
  });

  it('test specifying an args for an Optional that accepts zero or more', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ '-x' ], { nargs: '*' });
    parser.addArgument([ 'y' ], { nargs: '*' });

    args = parser.parseArgs([]);
    assert.deepEqual(args, { y: [], x: null });
    args = parser.parseArgs([ '-x' ]);
    assert.deepEqual(args, { y: [], x: [] });
    args = parser.parseArgs([ '-x', 'a' ]);
    assert.deepEqual(args, { y: [], x: [ 'a' ] });
    args = parser.parseArgs([ '-x', 'a', '--', 'b' ]);
    assert.deepEqual(args, { y: [ 'b' ], x: [ 'a' ] });
    args = parser.parseArgs([ 'a' ]);
    assert.deepEqual(args, { y: [ 'a' ], x: null });
    args = parser.parseArgs([ 'a', '-x' ]);
    assert.deepEqual(args, { y: [ 'a' ], x: [] });
    args = parser.parseArgs([ 'a', '-x', 'b' ]);
    assert.deepEqual(args, { y: [ 'a' ], x: [ 'b' ] });

  });

  
  it("should accept defaultValue for nargs:'*'", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-f', '--foo']);
    parser.addArgument(['bar'], { nargs: '*', defaultValue: 42});

    args = parser.parseArgs([]);
    assert.equal(args.bar, 42);
  });
});
