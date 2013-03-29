exports.init = function (grunt) {
  'use strict';

  var exports = {};

  // build the array of arguments to build the compass command
  exports.buildArgsArray = function (options) {
    var helpers = require('grunt-lib-contrib').init(grunt);
    var args = ['compile'];
    var raw = options.raw;
    var basePath = options.basePath;
    var path = require('path');

    grunt.verbose.writeflags(options, 'Options');

    if (raw && options.config) {
      grunt.fail.fatal('The options `raw` and `config` are mutually exclusive');
    }

    if (process.platform === 'win32') {
      args.unshift('compass.bat');
    } else {
      args.unshift('compass');
    }

    if (options.bundleExec) {
      args.unshift('bundle', 'exec');
    }

    if (options.basePath) {
      args.push(options.basePath);
    }

    if (options.specify) {
      var files = grunt.file.expand({
        filter: function (filePath) {
          return path.basename(filePath)[0] !== '_';
        }
      }, options.specify);

      if (files.length > 0) {
        [].push.apply(args, files);
      } else {
        return grunt.log.writeln('`specify` option used, but no files were found.');
      }
    }

    // don't want these as CLI flags
    delete options.raw;
    delete options.bundleExec;
    delete options.basePath;
    delete options.specify;

    // add converted options
    [].push.apply(args, helpers.optsToArgs(options));

    // Compass doesn't have a long flag for this option:
    // https://github.com/chriseppstein/compass/issues/1055
    if (options.importPath) {
      args = args.map(function (el) {
        return el.replace('--import-path', '-I');
      });
    }

    return args;
  };

  return exports;
};
