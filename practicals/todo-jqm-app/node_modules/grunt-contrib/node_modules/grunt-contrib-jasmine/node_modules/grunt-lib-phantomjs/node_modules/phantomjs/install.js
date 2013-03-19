// Copyright 2012 The Obvious Corporation.

/*
 * This simply fetches the right version of phantom for the current platform.
 */

'use strict'

var cp = require('child_process')
var fs = require('fs')
var http = require('http')
var path = require('path')
var url = require('url')
var rimraf = require('rimraf').sync
var unzip = require('unzip')

var helper = require('./lib/phantomjs')

fs.existsSync = fs.existsSync || path.existsSync

var libPath = path.join(__dirname, 'lib', 'phantom')
var tmpPath = path.join(__dirname, 'tmp')
var downloadUrl = 'http://phantomjs.googlecode.com/files/phantomjs-1.8.2-'
var fileName

if (process.platform === 'linux' && process.arch === 'x64') {
  downloadUrl += 'linux-x86_64.tar.bz2'
} else if (process.platform === 'linux') {
  downloadUrl += 'linux-i686.tar.bz2'
} else if (process.platform === 'darwin') {
  downloadUrl += 'macosx.zip'
} else if (process.platform === 'win32') {
  downloadUrl += 'windows.zip'
} else {
  console.log('Unexpected platform or architecture:', process.platform, process.arch)
  process.exit(1)
}

var fileName = downloadUrl.split('/').pop()
var downloadedFile = path.join(tmpPath, fileName)

function mkdir(name) {
  var dir = path.dirname(name)
  if (!fs.existsSync(dir)) {
    fs.mkdirSync(dir)
  }
}

function getOptions() {
  if (process.env.http_proxy) {
    var options = url.parse(process.env.http_proxy)
    options.path = downloadUrl
    options.headers = { Host: url.parse(downloadUrl).host }
    // turn basic authorization into proxy-authorization
    if (options.auth) { 
      options.headers['Proxy-Authorization'] = 'Basic ' +
              new Buffer(options.auth).toString('base64')
      delete options.auth
    }

    return options
  } else {
    return url.parse(downloadUrl)
  }
}

function finishIt(err, stdout, stderr) {
  // on Windows, A/V software can lock the directory, causing this
  // to fail with an EPERM. Try again on failure, for up to 10 seconds.
  // TODO Fix this by not unpacking into a temp directory, instead just
  // renaming things on the way out of the tarball.
  function moveIntoPlace (folder, unpackTarget, cb) {
    var start = Date.now()
    fs.rename(folder, unpackTarget, function retryCallback (er) {
      if (er && process.platform === 'win32' && er.code === 'EPERM') {
        if (Date.now() - start < 10000) {
          return fs.rename(folder, unpackTarget, retryCallback)
        } else {
          console.log('File renaming is taking too long, probably due to anti-virus software locking up the files.')
          console.log('Try re-running the installer and temporarily turning off the anti-virus software.')
          process.exit(1)
          return
        }
      }

      cb(er)
    })
  }

  function afterRename(err) {
    // For issolating extraction problems, https://github.com/Obvious/phantomjs/issues/15
    if (err) {
      console.log('Temporary files not renamed, maybe zip extraction failed.')
      process.exit(1)
      return
    }

    // Check that the binary is user-executable and fix it if it isn't (problems with unzip library)
    if (process.platform != 'win32') {
      var stat = fs.statSync(helper.path)
      // 64 == 0100 (no octal literal in strict mode)
      if (!(stat.mode & 64)) {
        console.log('Fixing file permissions')
        fs.chmodSync(helper.path, '755')
      }
    }

    console.log('Done. Phantomjs binary available at', helper.path)
  }

  if (err) {
    console.log('Error extracting archive', err)
    process.exit(1)
  } else {
    // Look for the extracted directory, so we can rename it.
    var files = fs.readdirSync(tmpPath)
    for (var i = 0; i < files.length; i++) {
      var file = path.join(tmpPath, files[i])
      if (fs.statSync(file).isDirectory()) {
        console.log('Renaming extracted folder', files[i], ' -> phantom')
        moveIntoPlace(file, libPath, afterRename)
      }
    }
  }
}

function extractIt() {
  var options = {
    cwd: tmpPath
  }

  if (fileName.substr(-4) === '.zip') {
    console.log('Extracting zip contents')

    var unzipStream = unzip.Extract({ path: path.dirname(downloadedFile) })
    unzipStream.on('error', finishIt)
    unzipStream.on('close', finishIt)

    var readStream = fs.createReadStream(downloadedFile)
    readStream.pipe(unzipStream)
    readStream.on('error', finishIt)
    readStream.on('close', function () { console.log('Read stream closed')})

  } else {
    console.log('Extracting tar contents (via spawned process)')
    cp.execFile('tar', ['jxf', downloadedFile], options, finishIt)
  }
}

function fetchIt() {
  var notifiedCount = 0
  var count = 0

  rimraf(tmpPath)
  rimraf(libPath)

  mkdir(downloadedFile)

  var outFile = fs.openSync(downloadedFile, 'w')

  function onData(data) {
    fs.writeSync(outFile, data, 0, data.length, null)
    count += data.length
    if ((count - notifiedCount) > 800000) {
      console.log('Received ' + Math.floor(count / 1024) + 'K...')
      notifiedCount = count
    }
  }

  function onEnd() {
    console.log('Received ' + Math.floor(count / 1024) + 'K total.')
    fs.closeSync(outFile)
    extractIt()
  }

  function onResponse(response) {
    var status = response.statusCode
    console.log('Receiving...')

    if (status === 200) {
      response.addListener('data', onData)
      response.addListener('end', onEnd)
    } else {
      console.log('Error with http request', response.headers)
      client.abort()
      process.exit(1)
    }
  }

  var client = http.get(getOptions(), onResponse)

  console.log('Requesting ' + downloadedFile)
}

fetchIt()
