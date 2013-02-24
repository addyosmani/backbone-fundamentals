var vows = require('vows'),
  path = require('path'),
  fs = require('fs'),
  assert = require('assert'),
  cleanCSS = require('../index');

var batchContexts = function() {
  var context = {};
  fs.readdirSync(path.join(__dirname, 'data')).forEach(function(filename) {
    if (/min.css$/.exec(filename)) return;
    var testName = filename.split('.')[0];

    context[testName] = {
      topic: function() {
        return {
          plain: fs.readFileSync(path.join(__dirname, 'data', testName + '.css')).toString('utf-8'),
          minimized: fs.readFileSync(path.join(__dirname, 'data', testName + '-min.css')).toString('utf-8').replace(/\n/g, '')
        };
      }
    }
    context[testName]['minimizing ' + testName + '.css'] = function(data) {
      assert.equal(cleanCSS.process(data.plain, { removeEmpty: true }), data.minimized)
    };
  });

  return context;
};

vows.describe('clean-batch')
  .addBatch(batchContexts())
  .export(module);
