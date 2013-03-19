'use strict';

var classes = require('../../support/classes');

module.exports = [
  new classes.Tag1({ x: 1 }),
  new classes.Tag1({ x: 1, y: 2, z: 3 }),
  new classes.Tag2({ x: 10 }),
  new classes.Tag3({ x: 1 }),
  new classes.Tag3({ x: 1, y: 2, z: 3 }),
  new classes.Tag3({ x: 1, y: 2, z: 3 }),
  new classes.Foo({ myParameter: 'foo', myAnotherParameter: [1, 2, 3] })
];
