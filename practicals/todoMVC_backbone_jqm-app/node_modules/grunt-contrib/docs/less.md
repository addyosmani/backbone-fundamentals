## Compile LESS files to CSS
> Contributed By: [Tyler Kellen](/tkellen) (@tkellen)

### Overview

Inside your `grunt.js` file add a section named `less`. This section specifies the files to compile and the options passed to [LESS](http://lesscss.org/#-server-side-usage).

#### Parameters

##### files ```object```

This defines what files this task will process and should contain key:value pairs.

The key (destination) should be an unique filepath (supports [grunt.template](https://github.com/cowboy/grunt/blob/master/docs/api_template.md)) and the value (source) should be a filepath or an array of filepaths (supports [minimatch](https://github.com/isaacs/minimatch)).

Note: When the value contains an array of multiple filepaths, the contents are concatenated in the order passed.

##### options ```object```

This controls how this task (and its helpers) operate and should contain key:value pairs, see options below.

#### Options

##### paths ```string|array```

This specifies directories to scan for @import directives when parsing.

#### Config Example

``` javascript
less: {
  compile: {
    options: {
      paths: ["assets/css"]
    },
    files: {
      "path/to/result.css": "path/to/source.less"
    }
  }
}
```