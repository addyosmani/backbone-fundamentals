/*global describe, it, beforeEach*/

'use strict';

var assert = require('assert');

var ArgumentParser = require('../lib/argparse').ArgumentParser;

describe('constant actions', function () {
  var parser;
  var args;

  beforeEach(function () {
    parser = new ArgumentParser({debug: true});
  });

  it("storeConst should store constant as given", function () {
    parser.addArgument(['-a'], {action: 'storeConst', dest:   'answer',
          help:   'store constant', constant: 42});
    args = parser.parseArgs('-a'.split(' '));
    assert.equal(args.answer, '42');
  });

  it("storeConst should give error if constant not given (or misspelled)", function () {
    assert.throws(
      function () {
          parser.addArgument(
            ['-a'],
            {
              action: 'storeConst',
              dest:   'answer',
              help:   'store constant',
              const:  42
            }
          );
        },
        /constant option is required for storeAction/
      );
  });

  it("appendConst should append constant as given", function () {
    parser.addArgument([ '--str' ], {action: 'appendConst', dest:   'types',
      help:   'append constant "str" to types', constant: 'str'});
    parser.addArgument([ '--int' ], {action: 'appendConst', dest:   'types',
      help:   'append constant "int" to types', constant: 'int'});
    args = parser.parseArgs('--str --int'.split(' '));
    assert.deepEqual(args.types, [ 'str', 'int' ]);
  });

  it("appendConst should give error if constant not given (or misspelled)", function () {
    assert.throws(
      function () {
          parser.addArgument(['-a'], {action: 'appendConst', dest:   'answer',
              help:   'store constant', const: 42});
        },
      /constant option is required for appendAction/
    );
  });
});
