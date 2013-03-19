var grunt = require('grunt');

exports.jade = {
  compile: function(test) {
    'use strict';

    test.expect(4);

    var actual = grunt.file.read('tmp/jst/jade.js');
    var expected = grunt.file.read('test/expected/jst/jade.js');
    test.equal(expected, actual, 'should compile jade templates to JST template');

    actual = grunt.file.read('tmp/jst/jade2.js');
    expected = grunt.file.read('test/expected/jst/jade2.js');
    test.equal(expected, actual, 'should compile jade templates to JST template (multiple files support)');

    actual = grunt.file.read('tmp/jst/jadeInclude.js');
    expected = grunt.file.read('test/expected/jst/jadeInclude.js');
    test.equal(expected, actual, 'should compile jade templates to JST template with an include');

    actual = grunt.file.read('tmp/jst/jadeTemplate.js');
    expected = grunt.file.read('test/expected/jst/jadeTemplate.js');
    test.equal(expected, actual, 'should compile jade templates to JST template with grunt template support');

    test.done();
  }
};