/*global describe, it*/


'use strict';

var assert = require('assert');

var ArgumentParser = require('../lib/argparse').ArgumentParser;

describe('base', function () {
  var parser;
  var args;

  it("should parse argument in short form", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-f', '--foo']);

    args = parser.parseArgs('-f 1'.split(' '));
    assert.equal(args.foo, 1);
    args = parser.parseArgs('-f=1'.split(' '));
    assert.equal(args.foo, 1);
    args = parser.parseArgs('-f1'.split(' '));
    assert.equal(args.foo, 1);
  });

  it("should parse argument in long form", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-f', '--foo']);

    args = parser.parseArgs('--foo 1'.split(' '));
    assert.equal(args.foo, 1);
    args = parser.parseArgs('--foo=1'.split(' '));
    assert.equal(args.foo, 1);
  });

  it("should parse multiple arguments", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-f', '--foo']);
    parser.addArgument(['--bar']);

    args = parser.parseArgs('--foo 5 --bar 6'.split(' '));
    assert.equal(args.foo, 5);
    assert.equal(args.bar, 6);
  });

  it("should check argument type", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-f', '--foo']);
    parser.addArgument(['--bar' ], { type: 'int' });

    assert.throws(function () {
      parser.parseArgs('--bar bar'.split(' '));
    });
    assert.doesNotThrow(function () {
      parser.parseArgs('--bar 1'.split(' '));
    });
  });

  it("should not drop down with empty args (without positional arguments)", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-f', '--foo']);

    assert.doesNotThrow(function () { parser.parseArgs([]); });
  });

  it("should drop down with empty args (positional arguments)", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-f', '--foo']);
    parser.addArgument([ 'baz']);

    assert.throws(
      function ()  {parser.parseArgs([]); },
      /too few arguments/
    );
  });

  it("should support pseudo-argument", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-f', '--foo']);
    parser.addArgument([ 'bar' ], { nargs: '+' });

    args = parser.parseArgs([ '-f', 'foo', '--', '-f', 'bar' ]);
    assert.equal(args.foo, 'foo');
    assert.equal(args.bar.length, 2);
  });

  it("should support #setDefaults", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-f', '--foo']);
    parser.setDefaults({bar: 1});

    args = parser.parseArgs([]);
    assert.equal(args.bar, 1);
  });

  it("should throw TypeError with conflicting options", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-f', '--foo']);

    assert.throws(
      function () { parser.addArgument(['-f']); },
      /Conflicting option string/
    );
    assert.throws(
      function () { parser.addArgument(['--foo']); },
      /Conflicting option string/
    );
    assert.throws(
      function () { parser.addArgument(['-f', '--flame']); },
      /Conflicting option string/
    );
    assert.throws(
      function () { parser.addArgument(['-m', '--foo']); },
      /Conflicting option string/
    );
  });

  it("should parse negative arguments", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-f', '--foo']);
    parser.addArgument([ 'bar' ], { type: 'int', });

    args = parser.parseArgs(['-1']);
    assert.equal(args.bar, -1);
  });

  it("No negative number options; neg number is positional argument", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-x'], {dest: 'x'});
    parser.addArgument(['foo'], {nargs: '?'});

    // no negative number options, so -1 is a positional argument
    args = parser.parseArgs(['-x', '-1']);
    // Namespace(foo=None, x='-1')
    assert.equal(args.x, '-1');
    // no negative number options, so -1 and -5 are positional arguments
    args = parser.parseArgs(['-x', '-1', '-5']);
    // Namespace(foo='-5', x='-1') order not determined
    assert.equal(args.x, '-1');
    assert.equal(args.foo, '-5');
  });

  it("negative number options present, so any neg number is an option", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-1'], {dest: 'one'});
    parser.addArgument(['foo'], {nargs: '?'});

    // negative number options present, so -1 is an option
    args = parser.parseArgs(['-1', 'X']);
    // Namespace(foo=None, one='X')
    assert.equal(args.one, 'X');
    // negative number options present, so -2 is an option
    assert.throws(
      function () {
        parser.parseArgs(['-2']);
      },
      /Unrecognized arguments: -2/
    );
    // negative number options present, so both -1s are options
    assert.throws(
      function () {
        parser.parseArgs(['-1', '-1']);
      },
      /argument "-1": Expected one argument/
    );
    args = parser.parseArgs(['--', '-f']);
    // Namespace(foo='-f', one=None)
    assert.equal(args.foo, '-f');
  });


  it("should infer option destination from long and short options", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-f', '--foo']);        // from long option
    parser.addArgument(['-g']);                 // from short option
    parser.addArgument(['-x'], { dest: 'xxx' });// from dest keyword

    args = parser.parseArgs(['-f', '1']);
    assert.deepEqual(args, { foo: '1', g: null, xxx: null});
    args = parser.parseArgs(['-g', '2']);
    assert.deepEqual(args, { foo: null, g: '2', xxx: null});
    args = parser.parseArgs(['-f', 1, '-g', 2, '-x', 3]);
    assert.deepEqual(args, { foo: 1, g: 2, xxx: 3});
  });

  it("should accept 0 defaultValue", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-f', '--foo']);
    parser.addArgument(['bar'], { nargs: '?', defaultValue: 0});

    args = parser.parseArgs([]);
    assert.equal(args.bar, 0);
    // could also test for '', and false
  });


  it("getDefault() should get defaults", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-f', '--foo']);
    parser.addArgument(['-g', '--goo'], {defaultValue: 42});

    assert.equal(parser.getDefault('goo'), 42);
    assert.equal(parser.getDefault('help'), require('../lib/const').SUPPRESS);
  });

  it("should handle mixed positional and optional args", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-f', '--foo']);
    parser.addArgument(['x']);
    parser.addArgument(['y']);

    args = parser.parseArgs(['X', 'Y']);
    assert.deepEqual(args, {"foo": null, "x": "X", "y": "Y"});
    args = parser.parseArgs(['-f', 'A', 'X', 'Y']);
    assert.deepEqual(args, {"foo": "A", "x": "X", "y": "Y"});
    args = parser.parseArgs(['X', '-f', 'A', 'Y']);
    assert.deepEqual(args, {"foo": "A", "x": "X", "y": "Y"});
    // was giving: Error: _mocha: error: Unrecognized arguments: X.
  });

  it('test empty and space containing arguments', function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument([ 'x' ], { nargs: '?' });
    parser.addArgument([ '-y', '--yyy' ], { dest: 'y' });

    args = parser.parseArgs([ '' ]);
    assert.deepEqual(args, { y: null, x: '' });
    args = parser.parseArgs([ 'a badger' ]);
    assert.deepEqual(args, { y: null, x: 'a badger' });
    args = parser.parseArgs([ '-a badger' ]);
    assert.deepEqual(args, { y: null, x: '-a badger' });
    args = parser.parseArgs([ '-y', '' ]);
    assert.deepEqual(args, { y: '', x: null });
    args = parser.parseArgs([ '-y', 'a badger' ]);
    assert.deepEqual(args, { y: 'a badger', x: null });
    args = parser.parseArgs([ '-y', '-a badger' ]);
    assert.deepEqual(args, { y: '-a badger', x: null });
    args = parser.parseArgs([ '--yyy=a badger' ]);
    assert.deepEqual(args, { y: 'a badger', x: null });
    args = parser.parseArgs([ '--yyy=-a badger' ]);
    assert.deepEqual(args, { y: '-a badger', x: null });

    assert.throws(function () {
      args = parser.parseArgs([ '-y' ]);
    });
  });
});
