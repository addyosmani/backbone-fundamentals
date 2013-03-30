/*global describe, it*/

'use strict';

var assert = require('assert');

var ArgumentParser = require('../lib/argparse').ArgumentParser;

describe('Argument conflict handling', function () {
  var parser;
  var help;

  it("test_bad_type", function () {
    assert.throws(function () {
      parser = new ArgumentParser({conflictHandler: 'foo'});
    },
    /invalid conflict resolution value: foo/i
    );
  });
  it("test_conflict_error", function () {
    parser = new ArgumentParser();
    parser.addArgument(['-x']);
    assert.throws(function () {
      parser.addArgument(['-x']);
    },
    /Conflicting option string/i
    );
    parser.addArgument(['--spam']);
    assert.throws(function () {
      parser.addArgument(['--spam']);
    },
    /Conflicting option string/i
    );
  });
  it("test_resolve_error", function () {
    parser = new ArgumentParser({prog: 'PROG', conflictHandler: 'resolve'});
    parser.addArgument(['-x'], {help: 'OLD X'});
    parser.addArgument(['-x'], {help: 'NEW X'});
    help = parser.formatHelp();
    /* expect
    usage: PROG [-h] [-x X]

    optional arguments:
      -h, --help  show this help message and exit
      -x X        NEW X
    */
    assert(help.match(/usage: PROG \[-h\] \[-x X\]/im));
    assert(help.match(/Show this help message and exit/im));
    assert(help.match(/-x X\s*NEW X/im));
    parser.addArgument(['--spam'], {metavar: 'OLD_SPAM'});
    parser.addArgument(['--spam'], {metavar: 'NEW_SPAM'});
    help = parser.formatHelp();
    /* expect
    usage: PROG [-h] [-x X] [--spam NEW_SPAM]

    optional arguments:
      -h, --help       show this help message and exit
      -x X             NEW X
      --spam NEW_SPAM
    */
    assert(help.match(/--spam NEW_SPAM/im));
  });
  it("TypeError with multiple conflicts", function () {
    parser = new ArgumentParser({debug: true});
    parser.addArgument(['-f', '--foo']);
    parser.addArgument(['-b', '--bar']);
    assert.throws(
      function () { parser.addArgument(['--foo', '--bar', '--foobar']); },
      /argument "--foo\/--bar\/--foobar": Conflicting option string\(s\): --foo, --bar/
    );
  });
  it("resolving multiple conflicts", function () {
    parser = new ArgumentParser({debug: true, conflictHandler: 'resolve'});
    parser.addArgument(['-f', '--foo']);
    parser.addArgument(['--bar']);
    help = parser.formatHelp();
    /*
    usage: _mocha [-h] [-f FOO] [--bar BAR]
    Optional arguments:
      -h, --help         Show this help message and exit.
      -f FOO, --foo FOO
      --bar BAR
    */
    assert(help.match(/-f FOO, --foo FOO/im));
    parser.addArgument(['--foo', '--bar', '--foobar']);
    help = parser.formatHelp();
    assert(help.match(/-f FOO$/im));
    assert(help.match(/--foo FOO, --bar FOO, --foobar FOO/im));
    /*
    usage: _mocha [-h] [-f FOO] [--foo FOO]
    Optional arguments:
      -h, --help            Show this help message and exit.
      -f FOO
      --foo FOO, --bar FOO, --foobar FOO
    */
  });
});
