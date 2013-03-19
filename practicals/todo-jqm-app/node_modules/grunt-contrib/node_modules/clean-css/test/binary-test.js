var vows = require('vows'),
  assert = require('assert'),
  exec = require('child_process').exec,
  fs = require('fs');

var binaryContext = function(options, context) {
  context.topic = function() {
    // We add __DIRECT__=1 to switch binary into 'non-piped' mode
    exec("__DIRECT__=1 ./bin/cleancss " + options, this.callback);
  };
  return context;
};

var pipedContext = function(css, options, context) {
  context.topic = function() {
    exec("echo \"" + css + "\" | ./bin/cleancss " + options, this.callback);
  };
  return context;
};

exports.commandsSuite = vows.describe('binary commands').addBatch({
  'no options': binaryContext('', {
    'should output help': function(stdout) {
      assert.equal(/usage:/.test(stdout), true);
    }
  }),
  'help': binaryContext('-h', {
    'should output help': function(error, stdout) {
      assert.equal(/usage:/.test(stdout), true);
    }
  }),
  'version': binaryContext('-v', {
    'should output help': function(error, stdout) {
      var version = JSON.parse(fs.readFileSync('./package.json')).version;
      assert.equal(stdout, version + "\n");
    }
  }),
  'stdin': pipedContext("a{color: #f00}", '', {
    'should output data': function(error, stdout) {
      assert.equal(stdout, "a{color:red}");
    }
  }),
  'no empty by default': pipedContext('a{}', '', {
    'should preserve content': function(error, stdout) {
      assert.equal(stdout, "a{}");
    }
  }),
  'empty': pipedContext('a{}', '-e', {
    'should preserve content': function(error, stdout) {
      assert.equal(stdout, "");
    }
  }),
  'from source': binaryContext('./test/data/reset.css', {
    'should minimize': function(error, stdout) {
      var minimized = fs.readFileSync('./test/data/reset-min.css', 'utf-8').replace(/\n/g, '');
      assert.equal(stdout, minimized);
    }
  }),
  'to file': binaryContext('-o reset-min.css ./test/data/reset.css', {
    'should give no output': function(error, stdout) {
      assert.equal(stdout, '');
    },
    'should minimize': function(stdout) {
      var minimized = fs.readFileSync('./test/data/reset-min.css', 'utf-8').replace(/\n/g, '');
      var target = fs.readFileSync('./reset-min.css', 'utf-8').replace(/\n/g, '');
      assert.equal(minimized, target);
    },
    teardown: function() {
      exec('rm reset-min.css');
    }
  })
});