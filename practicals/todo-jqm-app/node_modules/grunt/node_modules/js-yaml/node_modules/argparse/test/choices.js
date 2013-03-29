/*global describe, it*/


'use strict';

var assert = require('assert');

var ArgumentParser = require('../lib/argparse').ArgumentParser;

describe('choices', function () {
  var parser;
  var args;

  it("should store correct choice(choices defined as string)", function () {
    parser = new ArgumentParser({ debug: true });
    parser.addArgument(['--foo'], {choices: 'abc'});

    args = parser.parseArgs('--foo a'.split(' '));
    assert.equal(args.foo, 'a');
  });

  it("should drop down with 'Invalid choice' error for incorrect choices(choices defined as string)", function () {
    parser = new ArgumentParser({ debug: true });
    parser.addArgument(['--foo'], {choices: 'abc'});

    assert.throws(
      function () {
        args = parser.parseArgs('--foo e'.split(' '));
        console.dir(args);
      },
      /Invalid choice:/
    );
    assert.throws(
      function () {
        args = parser.parseArgs('--foo 0'.split(' '));
        console.dir(args);
      },
      /Invalid choice:/
    );
  });


  it("should store correct choice(choices defined as array)", function () {
    parser = new ArgumentParser({ debug: true });
    parser.addArgument(['--foo'], {choices: ['a', 'abc', 'd']});

    args = parser.parseArgs('--foo abc'.split(' '));
    assert.equal(args.foo, 'abc');
  });

  it("should drop down with 'Invalid choice' error for incorrect choices(choices defined as array)", function () {
    parser = new ArgumentParser({ debug: true });
    parser.addArgument(['--foo'], {choices: ['a', 'abc', 'd']});

    assert.throws(
      function () {
        args = parser.parseArgs('--foo e'.split(' '));
        console.dir(args);
      },
      /Invalid choice:/
    );
    assert.throws(
      function () {
        args = parser.parseArgs('--foo 0'.split(' '));
        console.dir(args);
      },
      /Invalid choice:/
    );
  });
});
