var fs = require('fs');
var zlib = require('zlib');

var archiver = require('archiver');

var gzipper = zlib.createGzip();
var output = fs.createWriteStream(__dirname + '/example-output.tar.gz');
var archive = archiver('tar');

archive.on('error', function(err) {
  throw err;
});

archive.pipe(gzipper).pipe(output);

var file1 = __dirname + '/fixtures/file1.txt';
var file2 = __dirname + '/fixtures/file2.txt';

archive
  .append(fs.createReadStream(file1), { name: 'file1.txt' })
  .append(fs.createReadStream(file2), { name: 'file2.txt' });

archive.finalize(function(err, written) {
  if (err) {
    throw err;
  }

  console.log(written + ' total bytes written');
});