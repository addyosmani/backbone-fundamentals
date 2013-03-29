'use strict';


function isNothing(subject) {
  return (undefined === subject) || (null === subject);
}


function isObject(subject) {
  return ('object' === typeof subject) && (null !== subject);
}


function isInstanceOf(constructor /*, subjects... */) {
  var index,
      length = arguments.length;

  if (length < 2) {
    return false;
  }

  for (index = 1; index < length; index += 1) {
    if (!(arguments[index] instanceof constructor)) {
      return false;
    }
  }

  return true;
}


function collectKeys(subject, include, exclude) {
  var result = Object.getOwnPropertyNames(subject);

  if (!isNothing(include)) {
    include.forEach(function (key) {
      if (0 > result.indexOf(key)) {
        result.push(key);
      }
    });
  }

  if (!isNothing(exclude)) {
    result = result.filter(function (key) {
      return 0 > exclude.indexOf(key);
    });
  }

  return result;
}


module.exports.isNothing     = isNothing;
module.exports.isObject      = isObject;
module.exports.isInstanceOf  = isInstanceOf;
module.exports.collectKeys   = collectKeys;
