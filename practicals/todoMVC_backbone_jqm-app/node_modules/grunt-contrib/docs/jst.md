## Compile underscore templates to JST file
> Contributed By: [Tim Branyen](/tbranyen) (@tbranyen)

### Overview

This task compiles Underscore compatible templates into functions that can be concatenated and minified with existing source files.

Inside your `grunt.js` file, add a section named `jst`. This section specifies the files to compile and the options passed to [underscore.template](http://underscorejs.org/#template).

#### Parameters

##### files ```object```

This defines what files this task will process and should contain key:value pairs.

The key (destination) should be an unique filepath (supports [grunt.template](https://github.com/cowboy/grunt/blob/master/docs/api_template.md)) and the value (source) should be a filepath or an array of filepaths (supports [minimatch](https://github.com/isaacs/minimatch)).

Note: Values are precompiled to the namespaced JST array in the order passed.

##### options ```object```

This controls how this task (and its helpers) operate and should contain key:value pairs, see options below.

#### Options

##### namespace ```string```

The namespace in which the resulting JST templates are assigned to.

##### templateSettings ```object```

The settings passed to underscore when compiling templates.

#### Config Examples

``` javascript
jst: {
  compile: {
    options: {
      templateSettings: {
        interpolate : /\{\{(.+?)\}\}/g
      }
    },
    files: {
      "path/to/compiled/templates.js": ["path/to/source/**/*.html"]
    }
  }
}
```