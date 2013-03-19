## Optimize RequireJS projects using r.js
> Contributed By: [Tim Branyen](/tbranyen) (@tbranyen)

### Overview

Inside your `grunt.js` file add a section named `requirejs`. This section specifies the options passed to [RequireJS Optimizer](http://requirejs.org/docs/optimization.html).

#### Parameters

##### options ```object```

This controls how this task (and its helpers) operate and should contain key:value pairs, see options below.

#### Options

For a full list of possible options, [see the r.js example build file](https://github.com/jrburke/r.js/blob/master/build/example.build.js).

#### Config Example

``` javascript
requirejs: {
  compile: {
    options: {
      baseUrl: "path/to/base",
      mainConfigFile: "path/to/config.js",
      out: "path/to/optimized.js"
    }
  }
}
```