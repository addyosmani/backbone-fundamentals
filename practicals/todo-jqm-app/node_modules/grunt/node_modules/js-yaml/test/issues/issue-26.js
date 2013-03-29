'use strict';
/*global it */


var assert = require('assert');

require('../../lib/js-yaml');


it('should convert new line into white space', function () {
  var data = require('./data/issue-26.yml');

  assert.equal(data.test, 'a b c\n');
});
