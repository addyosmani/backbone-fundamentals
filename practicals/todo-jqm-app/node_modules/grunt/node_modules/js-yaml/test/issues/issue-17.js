'use strict';
/*global it */


var assert = require('assert');

require('../../lib/js-yaml');


it('Non-specific "!" tags should resolve to !!str', function () {
  var data = require('./data/issue-17.yml');

  assert.equal(typeof data, 'string');
});
