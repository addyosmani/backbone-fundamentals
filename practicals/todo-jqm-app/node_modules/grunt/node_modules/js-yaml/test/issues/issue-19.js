'use strict';
/*global it */


var assert = require('assert');

require('../../lib/js-yaml');


it('Timestamp parsing is one month off', function () {
  var data = require('./data/issue-19.yml');

  // JS month starts with 0 (0 => Jan, 1 => Feb, ...)
  assert.equal(data.xmas.getTime(), Date.UTC(2011, 11, 24));
});
