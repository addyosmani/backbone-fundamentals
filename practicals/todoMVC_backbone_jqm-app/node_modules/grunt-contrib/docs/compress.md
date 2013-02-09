## Compress files and folders

### Overview

Inside your `grunt.js` file, add a section named `compress`. This section specifies the files to compress and the options passed to either [zipstream](https://github.com/wellawaretech/node-zipstream) (for zip) or [tar](https://github.com/isaacs/node-tar) (for tar/tgz) or [zlib](http://nodejs.org/api/zlib.html#zlib_options) (for gzip).

#### Parameters

##### files ```object```

This defines what files this task will compress and should contain key:value pairs.

The key (destination) should be an unique filepath (supports [grunt.template](https://github.com/cowboy/grunt/blob/master/docs/api_template.md)) and the value (source) should be a filepath or an array of filepaths (supports [minimatch](https://github.com/isaacs/minimatch)).

##### options ```object```

This controls how this task operates and should contain key:value pairs, see options below.

#### Options

##### mode ```string```

This is used to define which mode to use, currently supports gzip, tar, tgz (tar gzip) and zip.

##### basePath ```string```

This option adjusts internal filenames to be relative to provided path, within the resulting archive file.

##### flatten ```boolean```

This option performs a flat copy that dumps all the files into the root of the destination file, overwriting files if they exist.

##### level ```integer``` (zip only)

This option sets the level of archive compression (defaults to 1).

> Currently, gzip compression related options are not supported due to deficiencies in node's zlib library.

#### Config Example

``` javascript
compress: {
  zip: {
    options: {
      mode: "zip",
      basePath: "path/to",
      level: 1
    },
    files: {
      "path/to/result.zip": "path/to/source/*", // includes files in dir
      "path/to/another.gz": "path/to/source/**", // includes files in dir and subdirs
      "path/to/final.zip": ["path/to/sources/*.js", "path/to/more/*.js"], // include JS files in two diff dirs
      "path/to/project-<%= pkg.version %>.zip": "path/to/source/**" // variables in destination
    }
  }

  tar: {
    options: {
      mode: "tar",
      basePath: "path/to"
    },
    files: {
      "path/to/result.tar": "path/to/file.ext"
    }
  },

  tgz: {
    options: {
      mode: "tgz",
      basePath: "path/to"
    },
    files: {
      "path/to/result.tgz": "path/to/file.ext"
    }
  },

  gzip: {
    options: {
      mode: "gzip"
    },
    files: {
      "path/to/result.gz": "path/to/file.ext"
    }
  }
}
```