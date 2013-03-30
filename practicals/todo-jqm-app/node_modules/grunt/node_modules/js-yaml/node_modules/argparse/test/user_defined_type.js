/*global describe, it*/

'use strict';

var assert = require('assert');

var ArgumentParser = require('../lib/argparse').ArgumentParser;

describe('user defined type', function () {
  var parser;
  var args;

  it("should handle builtin types", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['--eggs'], {type: 'int'});
    parser.addArgument(['spam'], {type: 'float'});

    args = parser.parseArgs(['--eggs=42', '42']);
    assert.deepEqual(args, {eggs: 42, spam: 42.0});
    args = parser.parseArgs(['1024.675']);
    assert.deepEqual(args, {eggs: null, spam: 1024.675});
    assert.throws(
      function () { parser.parseArgs(['--eggs', 'a']); },
        /Invalid int value: a/i
    );
  });

  it("should handle user-defined type", function () {
    function myType(arg) {
      return arg;
    }
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-x'], {type: myType});
    parser.addArgument(['spam'], {type: myType});

    args = parser.parseArgs(['a', '-x', 'b']);
    assert.deepEqual(args, {x: myType('b'), spam: myType('a')});
    args = parser.parseArgs(['-xf', 'g']);
    assert.deepEqual(args, {x: myType('f'), spam: myType('g')});
  });

  it("should give consistent type errors", function () {
    function dateType(arg) {
      var x = new Date(arg);
      if (x.toString().match('Invalid')) {
        throw new TypeError("" + arg + " is not a valid date.");
      }
      return x;
    }
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-i'], {type: 'int', defaultValue: 0});
    parser.addArgument(['-f'], {type: 'float', defaultValue: 0});
    parser.addArgument(['-d'], {type: dateType, defaultValue: new Date(0)});
    assert.throws(
    function () { parser.parseArgs(['-f', 'abc']); },
      /Invalid float value: abc/i
    );
    assert.throws(
    function () { parser.parseArgs(['-i', 'abc']); },
      /Invalid int value: abc/i
    );
    args = parser.parseArgs([]);
    assert.deepEqual(args, {i: 0, f: 0, d: new Date(0)});
    args = parser.parseArgs(['-d', '1/1/2012']);
    assert.deepEqual(args, {i: 0, f: 0, d: new Date('1/1/2012')});
    assert.throws(
      function () {parser.parseArgs(['-d', '13/1/2000']); },
        /Invalid dateType value: (.*)/i
      /*
      it used to insert the function code rather than its name
      Invalid <dateType.toString()> value: 13/1/2000
      */
    );
    assert.throws(
    function () { parser.parseArgs(['-d', 'abc']); },
      /Invalid dateType value: (.*)/i
    );
  });

  it("test a user-defined type by registering it", function () {
    function dateType(arg) {
      var x = new Date(arg);
      if (x.toString().match('Invalid')) {
        throw new TypeError("" + arg + " is not a valid date.");
      }
      return x;
    }
    parser = new ArgumentParser({debug: true});
    parser.register('type', 'dateType', dateType);
    parser.addArgument(['-d'], {type: 'dateType'});
    args = parser.parseArgs(['-d', '1/1/2012']);
    assert.deepEqual(args, {d: new Date('1/1/2012')});
    assert.throws(
    function () { parser.parseArgs(['-d', '13/1/2000']); },
      /Invalid dateType value: (.*)/
    );
  });

  it("test an anonymous user-defined type", function () {
    var dateType = function (arg) {
      var x = new Date(arg);
      if (x.toString().match('Invalid')) {
        throw new TypeError("" + arg + " is not a valid date.");
      }
      return x;
    };
    //dateType.displayName = 'dateType';
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-d'], {type: dateType});
    args = parser.parseArgs(['-d', '1/1/2012']);
    assert.deepEqual(args, {d: new Date('1/1/2012')});
    assert.throws(
    function () { parser.parseArgs(['-d', 'abc']); },
      /Invalid <function> value: abc\nabc is not a valid date/im
    );
  });
});
// could test for: Error: "dateType" is not callable
// by using an unregistered string or other nonfunction
