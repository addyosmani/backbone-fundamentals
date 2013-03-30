var grunt = require('grunt');

exports.jade = {
  compile: function(test) {
    'use strict';

    test.expect(4);

    var actual = grunt.file.read('tmp/amd/jade.js');
    var expected = grunt.file.read('test/expected/amd/jade.js');
    test.equal(expected, actual, 'should compile jade templates to js');

    actual = grunt.file.read('tmp/amd/jade2.js');
    expected = grunt.file.read('test/expected/amd/jade2.js');
    test.equal(expected, actual, 'should compile jade templates to js (multiple files support)');

    actual = grunt.file.read('tmp/amd/jadeInclude.js');
    expected = grunt.file.read('test/expected/amd/jadeInclude.js');
    test.equal(expected, actual, 'should compile jade templates to js with an include');

    actual = grunt.file.read('tmp/amd/jadeTemplate.js');
    expected = grunt.file.read('test/expected/amd/jadeTemplate.js');
    test.equal(expected, actual, 'should compile jade templates to js with grunt template support');

    test.done();
  }
};