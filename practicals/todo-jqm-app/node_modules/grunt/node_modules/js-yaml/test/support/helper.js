'use strict';
/*global describe, it */


var path = require('path');
var fs = require('fs');
var common = require('./common');


function collectDataFiles(directory) {
  var files = fs.readdirSync(directory),
      collection = {};

  files.forEach(function (filename) {
    var extname = path.extname(filename),
        basename = path.basename(filename, extname);

    if (!collection.hasOwnProperty(basename)) {
      collection[basename] = [];
    }

    collection[basename].push(extname);
  });

  return collection;
}


function generateTests(settings) {
  var description = settings.description,
      directory   = settings.directory,
      files       = common.toArray(settings.files),
      skip        = common.toArray(settings.skip),
      testHandler = settings.test;

  describe(description, function () {
    var availableFiles = collectDataFiles(directory);

    common.each(availableFiles, function (extnames, basename) {

      function shouldSkipFile() {
        return extnames.some(function (ext) {
          return 0 <= skip.indexOf(ext);
        });
      }

      function shouldTakeFile() {
        return files.every(function (ext) {
          return !shouldSkipFile() && (0 <= extnames.indexOf(ext));
        });
      }

      function takeFile(ext) {
        return new common.DataFile(path.join(directory, (basename + ext)));
      }

      if (shouldTakeFile()) {
        it(basename, function () {
          testHandler.apply(this, files.map(takeFile));
        });
      }
    });
  });
}


module.exports.collectDataFiles = collectDataFiles;
module.exports.generateTests    = generateTests;
