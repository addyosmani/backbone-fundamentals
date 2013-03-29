'use strict';


Number.isNaN = Number.isNaN || function (subject) {
  // There is no Number.isNaN in Node 0.6.x
  return ('number' === typeof subject) && isNaN(subject);
};
