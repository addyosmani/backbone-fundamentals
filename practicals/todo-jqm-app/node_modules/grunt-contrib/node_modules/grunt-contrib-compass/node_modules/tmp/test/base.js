var
  assert = require('assert'),
  path   = require('path'),
  exec   = require('child_process').exec;

function _spawnTest(passError, testFile, params, cb) {
  var
    filename,
    command = [ 'node', path.join(__dirname, testFile) ].concat(params).join(' ');

  exec(command, function _execDone(err, stdout, stderr) {
    if (passError) {
      if (err) {
        return cb(err);
      } else if (stderr.length > 0) {
        return cb(stderr.toString());
      }
    }

    return cb(null, stdout.toString());
  });
}

function _testStat(stat, mode) {
  assert.equal(stat.uid, process.getuid(), 'should have the same UID');
  assert.equal(stat.gid, process.getgid(), 'should have the same GUID');
  assert.equal(stat.mode, mode);
}

function _testPrefix(prefix) {
  return function _testPrefixGenerated(err, name, fd) {
    assert.equal(path.basename(name).slice(0, prefix.length), prefix, 'should have the provided prefix');
  };
}

function _testPostfix(postfix) {
  return function _testPostfixGenerated(err, name, fd) {
    assert.equal(name.slice(name.length - postfix.length, name.length), postfix, 'should have the provided postfix');
  };
}

function _testKeep(type, keep, cb) {
  _spawnTest(true, 'keep.js', [ type, keep ], cb);
}

function _testGraceful(type, graceful, cb) {
  _spawnTest(false, 'graceful.js', [ type, graceful ], cb);
}

function _assertName(err, name) {
  assert.isString(name);
}


module.exports.testStat = _testStat;
module.exports.testPrefix = _testPrefix;
module.exports.testPostfix = _testPostfix;
module.exports.testKeep = _testKeep;
module.exports.testGraceful = _testGraceful;
module.exports.assertName = _assertName;
