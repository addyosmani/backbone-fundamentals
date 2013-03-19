
'use strict';

exports.init = function(grunt, phantomjs) {
  // node api
  var fs = require('fs'),
      path = require('path');

  // npm
  var rimraf = require('rimraf');

  var baseDir = '.',
      tempDir = '.grunt/grunt-contrib-jasmine';

  var exports = {};

  exports.writeTempFile = function(dest, contents) {
    var file = path.join(tempDir,dest);
    grunt.file.write(file, contents);
  };

  exports.copyTempFile = function(src, dest) {
    var file = path.join(tempDir,dest);
    grunt.file.copy(src, file);
  };

  exports.cleanTemp = function() {
    rimraf.sync(tempDir);
  };

  exports.buildSpecrunner = function (src, options){

    exports.copyTempFile(__dirname + '/../jasmine/reporters/PhantomReporter.js', 'reporter.js');
    exports.copyTempFile(__dirname + '/../../vendor/jasmine-' + options.version + '/jasmine.css', 'jasmine.css');
    exports.copyTempFile(__dirname + '/../../vendor/jasmine-' + options.version + '/jasmine.js', 'jasmine.js');
    exports.copyTempFile(__dirname + '/../../vendor/jasmine-' + options.version + '/jasmine-html.js', 'jasmine-html.js');
    exports.copyTempFile(__dirname + '/../jasmine/jasmine-helper.js', 'jasmine-helper.js');

    var reporters = [
      tempDir + '/reporter.js'
    ];

    var jasmineCss = [
      tempDir + '/jasmine.css'
    ];

    jasmineCss = jasmineCss.concat(exports.getRelativeFileList(options.styles));

    var jasmineCore = [
      tempDir + '/jasmine.js',
      tempDir + '/jasmine-html.js'
    ];

    var jasmineHelper = tempDir + '/jasmine-helper.js';

    var context = {
      temp : tempDir,
      css  : jasmineCss,
      scripts : {
        jasmine   : exports.getRelativeFileList(jasmineCore),
        helpers   : exports.getRelativeFileList(options.helpers),
        specs     : exports.getRelativeFileList(options.specs),
        src       : exports.getRelativeFileList(src),
        vendor    : exports.getRelativeFileList(options.vendor),
        reporters : exports.getRelativeFileList(reporters),
        start     : exports.getRelativeFileList(jasmineHelper)
      },
      options : options.templateOptions || {}
    };

    var source = '',
      specrunner = path.join(baseDir,options.outfile);

    if (options.template.process) {
      var task = {
        writeTempFile : exports.writeTempFile,
        copyTempFile : exports.copyTempFile,
        phantomjs : phantomjs
      };
      source = options.template.process(grunt, task, context);
      grunt.file.write(specrunner, source);
    } else {
      grunt.file.copy(options.template, specrunner, {
        process : function(src) {
          source = grunt.util._.template(src, context);
          return source;
        }
      });
    }

    return source;
  };

  exports.getRelativeFileList = function (/* args... */) {

    var list = Array.prototype.slice.call(arguments);
    var base = path.resolve(baseDir);
    var files = [];
    list.forEach(function(listItem){
      if (listItem) files = files.concat(grunt.file.expand({nonull: true},listItem));
    });
    files = grunt.util._(files).map(function(file){
      return (/^https?:/).test(file) ? file : path.resolve(file).replace(base,'.').replace(/\\/g,'/');
    });
    return files;
  };

  return exports;
};

