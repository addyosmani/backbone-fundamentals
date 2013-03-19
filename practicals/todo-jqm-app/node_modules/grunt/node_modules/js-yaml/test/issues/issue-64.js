'use strict';
/*global it */


var assert = require('assert');

var yaml = require('../../lib/js-yaml');


it('Wrong error message when yaml file contains tabs', function () {
  assert.doesNotThrow(
    function () { require('./data/issue-64.yml'); },
    yaml.YAMLException);
});
