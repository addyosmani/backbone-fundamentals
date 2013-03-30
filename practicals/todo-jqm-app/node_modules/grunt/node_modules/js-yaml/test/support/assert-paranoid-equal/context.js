'use strict';


function Context(actual, expected, path) {
  this.actual   = actual;
  this.expected = expected;
  this.path     = path || [];
}


Context.prototype.descent = function descent(key) {
  return new Context(this.actual, this.expected, this.path.concat(key));
};


module.exports = Context;
