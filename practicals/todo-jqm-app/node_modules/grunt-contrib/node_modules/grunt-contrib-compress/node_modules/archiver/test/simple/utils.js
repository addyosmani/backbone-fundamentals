var util = require('../../lib/util');

var date1 = new Date('Jan 03 2013 14:26:38 GMT');
var octal1 = 12071312436;
var dos1 = 1109619539;

var date2 = new Date('Jan 03 2013 12:24:36 GMT-0400');
var octal2 = 12071330304;
var dos2 = 1109623570;

var offset = date1.getTimezoneOffset();

function adjustByOffset(d) {
  if (offset >= 1) {
    d.setMinutes(d.getMinutes() - offset);
  } else {
    d.setMinutes(d.getMinutes() + Math.abs(offset));
  }

  return d;
}

module.exports = {
  convertDateTimeDos: function(test) {
    test.expect(2);

    var actual = adjustByOffset(util.convertDateTimeDos(dos1)).toUTCString();
    var expected = date1.toUTCString();
    test.equal(actual, expected);

    actual = adjustByOffset(util.convertDateTimeDos(dos2)).toUTCString();
    expected = date2.toUTCString();
    test.equal(actual, expected);

    test.done();
  },

  convertDateTimeOctal: function(test) {
    test.expect(2);

    var actual = util.convertDateTimeOctal(octal1).toUTCString();
    var expected = date1.toUTCString();
    test.equal(actual, expected);

    actual = util.convertDateTimeOctal(octal2).toUTCString();
    expected = date2.toUTCString();
    test.equal(actual, expected);

    test.done();
  },

  dosDateTime: function(test) {
    test.expect(2);

    var actual = util.dosDateTime(date1, true);
    var expected = dos1;
    test.equal(actual, expected);

    actual = util.dosDateTime(date2, true);
    expected = dos2;
    test.equal(actual, expected);

    test.done();
  },

  defaults: function(test) {
    test.expect(2);

    var actual = util.defaults({value1: true}, {
      value2: true
    });

    var expected = {
      value1: true,
      value2: true
    };

    test.deepEqual(actual, expected, 'should default when missing key');

    actual = util.defaults({value3: null}, {
      value3: true,
      value4: true
    });

    expected = {
      value3: true,
      value4: true
    };

    test.deepEqual(actual, expected, 'should default key containing null value');

    test.done();
  },

  octalDateTime: function(test) {
    test.expect(2);

    var actual = util.octalDateTime(date1);
    var expected = octal1;
    test.equal(actual, expected);

    actual = util.octalDateTime(date2);
    expected = octal2;
    test.equal(actual, expected);

    test.done();
  },

  sanitizeFilePath: function(test) {
    test.expect(1);

    var actual = util.sanitizeFilePath('\\this/path//file.txt');
    var expected = 'this/path/file.txt';

    test.equal(actual, expected);

    test.done();
  }
};