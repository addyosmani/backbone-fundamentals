'use strict';
/*global it */


var assert = require('assert');

require('../../lib/js-yaml');


it('refactor compact variant of MarkedYAMLError.toString', function () {
  assert.throws(
    function () { require('./data/issue-33.yml'); },
    "require('issue-33.yml') should throw, but it does not");
});
