'use strict';


var jsyaml  = require('../../lib/js-yaml');
var classes = require('./classes');


module.exports = new jsyaml.Schema({
  include: [
    jsyaml.DEFAULT_SCHEMA
  ],
  explicit: [
    new jsyaml.Type('!tag3', {
      loader: {
        kind: 'object',
        resolver: classes.Tag3.fromYAMLNode
      },
      dumper: {
        kind: 'object',
        instanceOf: classes.Tag3,
        representer: classes.Tag3.toYAMLNode
      }
    }),
    new jsyaml.Type('!tag2', {
      loader: {
        kind: 'string',
        resolver: classes.Tag2.fromYAMLNode
      },
      dumper: {
        kind: 'object',
        instanceOf: classes.Tag2,
        representer: classes.Tag2.toYAMLNode
      }
    }),
    new jsyaml.Type('!tag1', {
      loader: {
        kind: 'object',
        resolver: classes.Tag1.fromYAMLNode
      },
      dumper: {
        kind: 'object',
        instanceOf: classes.Tag1,
      }
    }),
    new jsyaml.Type('!foo', {
      loader: {
        kind: 'object',
        resolver: classes.Foo.fromYAMLNode
      },
      dumper: {
        kind: 'object',
        instanceOf: classes.Foo,
        representer: classes.Foo.toYAMLNode
      }
    })
  ]
});
