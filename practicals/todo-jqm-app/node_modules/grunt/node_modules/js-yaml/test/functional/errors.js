'use strict';


var path   = require('path');
var assert = require('assert');
var jsyaml = require('../../lib/js-yaml');
var helper = require('../support/helper');

var TEST_SCHEMA   = require('../support/schema');
var YAMLException = require('../../lib/js-yaml/exception');


helper.generateTests({
  description: 'Errors on loading a series of documents.',
  directory: path.join(__dirname, 'errors'),
  files: ['.loader-error'],
  test: function (errorFile) {
    assert.throws(
      function () {
        jsyaml.loadAll(
          errorFile.content,
          function () {},
          { filename: errorFile.path,
            schema: TEST_SCHEMA,
            strict: true });
      },
      YAMLException,
      'In file "' + errorFile.path + '"');
  }
});
