/*global describe, it, beforeEach*/
// ActionContainer _addContainerActions() had errors passing groups from
// parent to child parser.  This tests for those.
'use strict';

var assert = require('assert');

var ArgumentParser = require('../lib/argparse').ArgumentParser;

describe('child group', function () {
  var args;
  var parentParser, group, xgroup, childParser;

  beforeEach(function () {
    // parent has name group and exclusive group that should be passed to child
    parentParser = new ArgumentParser({prog: 'PROG', debug: true, addHelp: false});
    parentParser.addArgument(['--foo'], {help: 'parent foo'});
    // parentParser.addArgument(['pbar'], {help: 'parent positional'});
    group = parentParser.addArgumentGroup({title: 'parent group'});
    group.addArgument(['--gfoo'], {help: 'group foo help'});
    group.addArgument(['gbar'], {help: 'group bar help'});
    xgroup = parentParser.addMutuallyExclusiveGroup({required: true});
    xgroup.addArgument(['--xfoo'], {action: 'storeTrue', help: 'xfoo or xbar, set true'});
    xgroup.addArgument(['--xbar'], {action: 'storeFalse', help: 'xfoo or xbar, set false'});
    childParser = new ArgumentParser({parents: [parentParser], description: 'child parser', debug: true});
    childParser.addArgument(['--cbar'], {help: 'child bar opt arg'});
  });

  it('compare help parent and child', function () {
    // format helps and compare selected passages
    var phelp = parentParser.formatHelp();
    var chelp = childParser.formatHelp();
    assert(phelp.match(/parent group:/));
    assert(chelp.match(/parent group:/));
  });

  it('child should  throw error if an xclusive group member is missing', function () {
    assert.throws(
      function () {
        args = childParser.parseArgs(['gbararg']);
      },
      /one of the arguments (.*) is required/
    );
  });

  it('child accepts an xgroup item and positional arg from parent', function () {
    args = childParser.parseArgs(['--xbar', 'gbararg']);
    assert.equal(args.gbar, 'gbararg');
    assert.equal(args.xbar, false);
  });

  it('child throws error if both xclusive options are given', function () {
    assert.throws(
      function () {
        args = childParser.parseArgs(['--xfoo', '--xbar']);
      },
      /Not allowed with argument/
    );
  });
});
