## Compile handlebars templates to JST file
> Contributed By: [Tim Branyen](/tbranyen) (@tbranyen)

### Overview

Inside your `grunt.js` file, add a section named `handlebars`. This section specifies the files to compile and the options used with [handlebars](http://handlebarsjs.com/).

##### files ```object```

This defines what files this task will process and should contain key:value pairs.

The key (destination) should be an unique filepath (supports [grunt.template](https://github.com/cowboy/grunt/blob/master/docs/api_template.md)) and the value (source) should be a filepath or an array of filepaths (supports [minimatch](https://github.com/isaacs/minimatch)).

Note: Values are precompiled to the namespaced JST array in the order passed.

##### options ```object```

This controls how this task operates and should contain key:value pairs, see options below.

#### Options

##### namespace ```string```

The namespace in which the resulting JST templates are assigned to.

##### processName ```function```

This option accepts a function which takes one argument (the template filepath) and returns a string which will be used as the key for the precompiled template object.  The example below stores all templates on the default JST namespace in capital letters.

``` javascript
options: {
  processName: function(filename) {
    return filename.toUpperCase();
  }
}
```

##### processPartialName ```function```

This option accepts a function which takes one argument (the partial filepath) and returns a string which will be used as the key for the precompiled partial object when it is registered in Handlebars.partials. The example below stores all partials using only the actual filename instead of the full path.

``` javascript
options: {
	processPartialName: function(filePath) { // input:  templates/_header.handlebar
    var pieces = filePath.split("/");
    return pieces[pieces.length - 1];      // output: _header.handlebar
  }
}
````

Note: If processPartialName is not provided as an option the default assumes that partials will be stored by stripping trailing underscore characters and filename extensions. For example, the path *templates/_header.handlebar* will become *header* and can be referenced in other templates as *{{> header}}*.

##### partialRegex ```regex```

This option accepts a regex that defines the prefix character that is used to identify Handlebars partial files. (The default is _).

``` javascript
options: {
	partialRegex: /^par_/ // assumes partial files would be prefixed with "par_" ie: "par_header.handlebar"
}
```

#### Config Example

``` javascript
handlebars: {
  compile: {
    options: {
      namespace: "JST"
    },
    files: {
      "path/to/result.js": "path/to/source.handlebar",
      "path/to/another.js": ["path/to/sources/*.handlebar", "path/to/more/*.handlebar"]
    }
  }
}
```