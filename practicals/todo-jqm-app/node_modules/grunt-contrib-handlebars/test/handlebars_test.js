var grunt = require('grunt');

exports.handlebars = {
  compile: function(test) {
    'use strict';
    test.expect(1);

    var actual = grunt.file.read('tmp/handlebars.js');
    var expected = grunt.file.read('test/expected/handlebars.js');
    test.equal(actual, expected, 'should compile partials into Handlebars.partials and handlebars template into JST');

    test.done();
  },
  compileNode: function(test) {
    'use strict';
    test.expect(1);

    var actual = grunt.file.read('tmp/handlebars-node.js');
    var expected = grunt.file.read('test/expected/handlebars-node.js');
    test.equal(actual, expected, 'should compile as per compile test and also have node directives prepended and appended');

    test.done();
  },
  nowrap: function(test) {
    'use strict';
    test.expect(1);

    var actual = grunt.file.read('tmp/handlebarsnowrap.js');
    var expected = grunt.file.read('test/expected/handlebarsnowrap.js');
    test.equal(actual, expected, 'should compile partials into Handlebars.partials and handlebars template into JST');

    test.done();
  },
  uglyfile: function(test) {
    'use strict';
    test.expect(1);

    var actual = grunt.file.read('tmp/uglyfile.js');
    var expected = grunt.file.read('test/expected/uglyfile.js');
    test.equal(actual, expected, 'should escape single quotes in filenames');

    test.done();
  },
  ns_nested: function(test) {
    'use strict';
    test.expect(1);

    var actual = grunt.file.read('tmp/ns_nested.js');
    var expected = grunt.file.read('test/expected/ns_nested.js');
    test.equal(actual, expected, 'should define parts of nested namespaces');

    test.done();
  },
  ns_nested_this: function(test) {
    'use strict';
    test.expect(1);

    var actual = grunt.file.read('tmp/ns_nested_this.js');
    var expected = grunt.file.read('test/expected/ns_nested.js'); // same as previous test
    test.equal(actual, expected, 'should define parts of nested namespaces, ignoring this.');

    test.done();
  },
  no_namespace:function(test) {
    'use strict';
    test.expect(1);

    var actual = grunt.file.read('tmp/no_namespace.js');
    var expected = grunt.file.read('test/expected/no_namespace.js');
    test.equal(actual, expected, 'should skip the creation of a namespace array arround the generated template file');

    test.done();
  },
  processcontent: function(test) {
    'use strict';
    test.expect(1);

    var actual = grunt.file.read('tmp/processcontent.js');
    var expected = grunt.file.read('test/expected/processcontent.js');
    test.equal(actual, expected, 'should remove leading and trailing spaces');

    test.done();
  },
  process_ast: function(test) {
    'use strict';
    test.expect(1);

    var actual = grunt.file.read('tmp/process_ast.js');
    var expected = grunt.file.read('test/expected/process_ast.js');
    test.equal(actual, expected, 'should allow the AST to be modified before compilation');

    test.done();
  },
  amd_compile: function(test) {
    'use strict';
    test.expect(1);

    var actual = grunt.file.read('tmp/amd_compile.js');
    var expected = grunt.file.read('test/expected/amd_compile.js'); // same as previous test
    test.equal(actual, expected, 'should wrap everything with an AMD define block.');

    test.done();
  },
  amd_compile_direct: function(test) {
    'use strict';
    test.expect(1);

    var actual = grunt.file.read('tmp/amd_compile_direct.js');
    var expected = grunt.file.read('test/expected/amd_compile_direct.js'); // same as previous test
    test.equal(actual, expected, 'should wrap everything with an AMD define block and directly return the template.');

    test.done();
  },
  custom_separator: function(test) {
    'use strict';
    test.expect(1);

    var actual = grunt.file.read('tmp/custom_separator.js');
    var expected = grunt.file.read('test/expected/custom_separator.js');
    test.equal(actual, expected, 'should use custom file separators as specified.');

    test.done();
  },
  processname: function(test) {
    'use strict';
    test.expect(1);

    var actual = grunt.file.read('tmp/processname.js');
    var expected = grunt.file.read('test/expected/processname.js');
    test.equal(actual, expected, 'should convert template name to upper case.');

    test.done();
  },
  process_partial_name: function(test) {
    'use strict';
    test.expect(1);

    var actual = grunt.file.read('tmp/process_partial_name.js');
    var expected = grunt.file.read('test/expected/process_partial_name.js');
    test.equal(actual, expected, 'should support custom handling of partial naming conventions.');

    test.done();
  },
  partial_regex: function(test) {
    'use strict';
    test.expect(1);

    var actual = grunt.file.read('tmp/partial_regex.js');
    var expected = grunt.file.read('test/expected/partial_regex.js');
    test.equal(actual, expected, 'should support custom file name identifiers for partials.');

    test.done();
  },
  partials_use_namespace: function(test) {
    'use strict';
    test.expect(1);

    var actual = grunt.file.read('tmp/partials_use_namespace.js');
    var expected = grunt.file.read('test/expected/partials_use_namespace.js');
    test.equal(actual, expected, 'should allow partials to be added to template namespace.');

    test.done();
  },
  partials_path: function(test) {
    'use strict';
    test.expect(1);

    var actual = grunt.file.read('tmp/partials_path_regex.js');
    var expected = grunt.file.read('test/expected/partials_path_regex.js');
    test.equal(actual, expected, 'should support custom path to partials.');

    test.done();
  }
};
