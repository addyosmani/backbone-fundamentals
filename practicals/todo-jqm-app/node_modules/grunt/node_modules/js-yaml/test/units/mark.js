'use strict';
/*global it */


var fs     = require('fs');
var path   = require('path');
var assert = require('assert');
var Mark   = require('../../lib/js-yaml/mark');


it('Mark', function () {
  var filepath = path.join(__dirname, 'samples/mark.data'),
      filedata = fs.readFileSync(filepath, 'utf8');

  filedata.split('---\n').slice(1).forEach(function (input) {
    var index = 0, line = 0, column = 0,
        mark, snippet, data, pointer, temp;

    assert(0 <= input.indexOf('*'));

    while (input[index] !== '*') {
      if (input[index] === '\n') {
        line += 1;
        column = 0;
      } else {
        column += 1;
      }
      index += 1;
    }

    mark = new Mark(filepath, input, index, line, column);
    snippet = mark.getSnippet(2, 79);

    assert(typeof snippet, 'string');

    temp = snippet.split('\n');
    assert.strictEqual(temp.length, 2);

    data = temp[0];
    pointer = temp[1];

    assert(data.length < 82);
    assert.strictEqual(data[pointer.length - 1], '*');
  });
});
