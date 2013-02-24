## Compile Stylus files to CSS
> Contributed By: [Eric Woroshow](/errcw) (@errcw)

### Overview

Inside your `grunt.js` file add a section named `stylus`. This section specifies the files to compile and the options passed to [stylus](http://learnboost.github.com/stylus/).

#### Parameters

##### files ```object```

This defines what files this task will process and should contain key:value pairs.

The key (destination) should be an unique filepath (supports [grunt.template](https://github.com/cowboy/grunt/blob/master/docs/api_template.md)) and the value (source) should be a filepath or an array of filepaths (supports [minimatch](https://github.com/isaacs/minimatch)).

Note: When the value contains an array of multiple filepaths, the contents are concatenated in the order passed.

##### options ```object```

This controls how this task (and its helpers) operate and should contain key:value pairs, see options below.

#### Options

##### compress ```boolean```

This specifies if we should compress the compiled css.

##### paths ```string|array```

This specifies directories to scan for @import directives when parsing.

#### Config Example

``` javascript
stylus: {
  compile: {
    options: {
      compress: true,
      paths: ['path/to/import', 'another/to/import']
    },
    files: {
      'path/to/result.css': 'path/to/source.styl',
      'path/to/another.css': ['path/to/sources/*.styl', 'path/to/more/*.style'],
    }
  }
},
```
