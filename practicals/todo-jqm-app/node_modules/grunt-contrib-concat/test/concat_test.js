'use strict';

var grunt = require('grunt');
var comment = require('../tasks/lib/comment').init(grunt);

exports.concat = {
  default_options: function(test) {
    test.expect(1);

    var actual = grunt.file.read('tmp/default_options');
    var expected = grunt.file.read('test/expected/default_options');
    test.equal(actual, expected, 'should describe what the default behavior is.');

    test.done();
  },
  custom_options: function(test) {
    test.expect(1);

    var actual = grunt.file.read('tmp/custom_options');
    var expected = grunt.file.read('test/expected/custom_options');
    test.equal(actual, expected, 'should utilize custom banner, footer and separator.');

    test.done();
  },
  handling_invalid_files: function(test) {
    test.expect(1);

    var actual = grunt.file.read('tmp/handling_invalid_files');
    var expected = grunt.file.read('test/expected/handling_invalid_files');
    test.equal(actual, expected, 'will have warned, but should not fail.');

    test.done();
  },
  strip_banner: function(test) {
    test.expect(7);

    var src = grunt.file.read('test/fixtures/banner.js');
    test.equal(comment.stripBanner(src), '// Comment\n\n/* Comment */\n', 'It should strip the top banner.');
    test.equal(comment.stripBanner(src, {block: true}), '// Comment\n\n/* Comment */\n', 'It should strip the top banner.');

    src = grunt.file.read('test/fixtures/banner2.js');
    test.equal(comment.stripBanner(src), '\n/*! SAMPLE\n * BANNER */\n\n// Comment\n\n/* Comment */\n', 'It should not strip the top banner.');
    test.equal(comment.stripBanner(src, {block: true}), '// Comment\n\n/* Comment */\n', 'It should strip the top banner.');

    src = grunt.file.read('test/fixtures/banner3.js');
    test.equal(comment.stripBanner(src), '\n// This is\n// A sample\n// Banner\n\n// But this is not\n\n/* And neither\n * is this\n */\n', 'It should not strip the top banner.');
    test.equal(comment.stripBanner(src, {block: true}), '\n// This is\n// A sample\n// Banner\n\n// But this is not\n\n/* And neither\n * is this\n */\n', 'It should not strip the top banner.');
    test.equal(comment.stripBanner(src, {line: true}), '// But this is not\n\n/* And neither\n * is this\n */\n', 'It should strip the top banner.');
    test.done();
  }
};