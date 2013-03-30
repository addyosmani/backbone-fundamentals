'use strict';


var util   = require('util');
var common = require('./common');


function Tag1(parameters) {
  this.x = parameters.x;
  this.y = parameters.y || 0;
  this.z = parameters.z || 0;
}

Tag1.fromYAMLNode = common.makeClassConstructor(Tag1, {
  required: ['x'],
  optional: ['y', 'z']
});


function Tag2() {
  Tag1.apply(this, arguments);
}

util.inherits(Tag2, Tag1);

Tag2.fromYAMLNode = function fromYAMLNode(object /*, explicit*/) {
  return new Tag2({
    x: ('number' === typeof object) ? object : parseInt(object, 10)
  });
};

Tag2.toYAMLNode = function toYAMLNode(object /*, style*/) {
  return String(object.x);
};


function Tag3() {
  Tag2.apply(this, arguments);
}

util.inherits(Tag3, Tag2);

Tag3.fromYAMLNode = common.makeClassConstructor(Tag3, {
  map: { '=': 'x' },
  required: ['x'],
  optional: ['y', 'z']
});

Tag3.toYAMLNode = function toYAMLNode(object /*, style*/) {
  return {
    '=' : object.x,
    'y' : object.y,
    'z' : object.z,
  };
};


function Foo(parameters) {
  this.myParameter        = parameters.myParameter;
  this.myAnotherParameter = parameters.myAnotherParameter;
}

Foo.fromYAMLNode = common.makeClassConstructor(Foo, {
  map: {
    'my-parameter':         'myParameter',
    'my-another-parameter': 'myAnotherParameter'
  },
  optional: ['myParameter', 'myAnotherParameter']
});

Foo.toYAMLNode = function toYAMLNode(object /*, style*/) {
  return {
    'my-parameter'         : object.myParameter,
    'my-another-parameter' : object.myAnotherParameter,
  };
};


module.exports.Tag1 = Tag1;
module.exports.Tag2 = Tag2;
module.exports.Tag3 = Tag3;
module.exports.Foo  = Foo;
