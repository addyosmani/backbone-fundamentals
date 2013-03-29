'use strict';
/*jshint latedef:false */


var AssertionError = require('assert').AssertionError;
var ownUtils = require('./utilities');
var Report = require('./report');


function ensureEqual(context, actual, expected) {
  ensureEqualValues(
    context,
    typeof actual,
    typeof expected,
    'types');

  if (ownUtils.isObject(actual) &&
      ownUtils.isObject(expected)) {

    ensureEqualValues(
      context,
      Object.getPrototypeOf(actual),
      Object.getPrototypeOf(expected),
      'prototypes');

    if (ownUtils.isInstanceOf(Date, actual, expected)) {
      ensureEqualDates(context, actual, expected);

    } else if (ownUtils.isInstanceOf(Buffer, actual, expected)) {
      ensureEqualObjects(context, actual, expected, {
        exclude: [ 'parent', 'offset' ]
      });

    } else {
      ensureEqualObjects(context, actual, expected);
    }

  } else {
    ensureEqualValues(context, actual, expected, 'values');
  }
}


function ensureEqualValues(context, actual, expected, reason) {
  if (!(Number.isNaN(actual) &&
        Number.isNaN(expected))) {
    if (actual !== expected) {
      throw new AssertionError({
        actual:   context.actual,
        expected: context.expected,
        operator: 'is paranoidly equal to',
        message:  new Report(context, actual, expected, reason)
      });
    }
  }
}


function ensureEqualDates(context, actual, expected) {
  ensureEqualValues(
    context,
    actual.getTime(),
    expected.getTime(),
    'date_timestamps');

  ensureEqualObjects(
    context,
    actual,
    expected);
}


function ensureEqualObjects(context, actual, expected, options) {
  var actualKeys, expectedKeys, includeKeys, excludeKeys,
      index, length, currentKey;

  if (!ownUtils.isNothing(options)) {
    includeKeys = options.include;
    excludeKeys = options.exclude;
  }

  actualKeys   = ownUtils.collectKeys(actual, includeKeys, excludeKeys);
  expectedKeys = ownUtils.collectKeys(expected, includeKeys, excludeKeys);

  ensureEqualValues(
    context,
    actualKeys.length,
    expectedKeys.length,
    'object_property_amounts');

  actualKeys.sort();
  expectedKeys.sort();

  length = actualKeys.length;

  for (index = 0; index < length; index += 1) {
    ensureEqualValues(
      context,
      actualKeys[index],
      expectedKeys[index],
      'object_property_names');
  }

  for (index = 0; index < length; index += 1) {
    currentKey = actualKeys[index];
    ensureEqual(
      context.descent(currentKey),
      actual[currentKey],
      expected[currentKey]);
  }
}


module.exports.ensureEqual        = ensureEqual;
module.exports.ensureEqualValues  = ensureEqualValues;
module.exports.ensureEqualDates   = ensureEqualDates;
module.exports.ensureEqualObjects = ensureEqualObjects;
