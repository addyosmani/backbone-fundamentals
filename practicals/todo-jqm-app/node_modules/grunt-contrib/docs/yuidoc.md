## Compile YUIDoc Documentation
> Contributed By: [George Pantazis](/gcpantazis) (@gcpantazis)

### Overview

Inside your `grunt.js` file add a section named `yuidoc`. [Visit the YUIDoc project home](http://yui.github.com/yuidoc/) for more information on YUIDocs and commenting syntax.

#### Parameters

##### options ```object```

This controls how this task operates and should contain key:value pairs, see options below.

#### Options

Options correlate to [YUIDoc config objects](http://yui.github.com/yuidoc/args/index.html).

##### paths (required) ```string|array```

What directory should be scanned to generate doc data.

##### outdir (required) ```string```

Where to save the documentation.

#### Config Example

``` javascript
yuidoc: {
  compile: {
    "name": "Project Name",
    "description": "Project Description",
    "version": "1.0.2",
    "url": "http://projecturl.com/",
    options: {
      paths: "path/to/source/code/",
      outdir: "where/to/save/docs/"
    }
  }
}
```