# grunt-contrib-jade [![Build Status](https://secure.travis-ci.org/gruntjs/grunt-contrib-jade.png?branch=master)](http://travis-ci.org/gruntjs/grunt-contrib-jade)

> Compile Jade files to HTML.



## Getting Started
This plugin requires Grunt `~0.4.0`

If you haven't used [Grunt](http://gruntjs.com/) before, be sure to check out the [Getting Started](http://gruntjs.com/getting-started) guide, as it explains how to create a [Gruntfile](http://gruntjs.com/sample-gruntfile) as well as install and use Grunt plugins. Once you're familiar with that process, you may install this plugin with this command:

```shell
npm install grunt-contrib-jade --save-dev
```

Once the plugin has been installed, it may be enabled inside your Gruntfile with this line of JavaScript:

```js
grunt.loadNpmTasks('grunt-contrib-jade');
```

*This plugin was designed to work with Grunt 0.4.x. If you're still using grunt v0.3.x it's strongly recommended that [you upgrade](http://gruntjs.com/upgrading-from-0.3-to-0.4), but in case you can't please use [v0.3.1](https://github.com/gruntjs/grunt-contrib-jade/tree/grunt-0.3-stable).*



## Jade task
_Run this task with the `grunt jade` command._

Task targets, files and options may be specified according to the grunt [Configuring tasks](http://gruntjs.com/configuring-tasks) guide.
### Options

#### pretty
Type: `Boolean`
Default: false

Output indented HTML.

#### data
Type: `Object`

Sets the data passed to `jade` during template compilation. Any data can be passed to the template (including `grunt` templates).

#### comileDebug
Type: `Boolean`

Set `compileDebug: fase` to remove `jade` debug instructions in javascript templates.

#### namespace
Type: `String` `false`
Default: 'JST'

The namespace in which the precompiled templates will be assigned.  *Use dot notation (e.g. App.Templates) for nested namespaces or false for no namespace wrapping.*  When false with `amd` option set `true`, templates will be returned directly from the AMD wrapper.

Example:
```js
options: {
  namespace: 'MyApp.Templates'
}
```

#### amd
Type: `Boolean`
default: `false`

Wraps the output file with an AMD define function and returns the compiled template namespace unless namespace has been explicitly set to false in which case the template function will be returned directly.

```js
define(function() {
    //...//
    returns this['[template namespace]'];
});
```

#### processName
Type: `function`

This option accepts a function which takes one argument (the template filepath) and returns a string which will be used as the key for the precompiled template object.  The example below stores all templates on the default JST namespace in capital letters.

```js
options: {
  processName: function(filename) {
    return filename.toUpperCase();
  }
}
```
### Usage Examples

```js
jade: {
  compile: {
    options: {
      data: {
        debug: false
      }
    },
    files: {
      "path/to/dest.html": ["path/to/templates/*.jade", "another/path/tmpl.jade"]
    }
  }
}
```

If you want to generate a debug file and a release file from the same template:

```js
jade: {
  debug: {
    options: {
      data: {
        debug: true
      }
    },
    files: {
      "debug.html": "test.jade"
    }
  },
  release: {
    options: {
      data: {
        debug: false
      }
    },
    files: {
      "release.html": "test.jade"
    }
  }
}
```

If you want to use `grunt` template in `options.data`:

```js
jade: {
  debug: {
    options: {
      data: {
        debug: true,
        timestamp: "<%= new Date().getTime() %>"
      }
    },
    files: {
      "debug.html": "test.jade"
    }
  }
}
```

or you can use `grunt` helpers (grunt object was exposed at template context):

```js
jade: {
  debug: {
    options: {
      data: {
        debug: true,
        timestamp: "<%= grunt.template.today() %>"
      }
    },
    files: {
      "debug.html": "test.jade"
    }
  }
}
```


## Release History

 * 2013-03-06   v0.5.0   Allow compilation to JS functions Support JST and AMD formats
 * 2013-02-14   v0.4.0   First official release for Grunt 0.4.0.
 * 2013-01-23   v0.4.0rc7   Updating grunt/gruntplugin dependencies to rc7. Changing in-development grunt/gruntplugin dependency versions from tilde version ranges to specific versions.
 * 2013-01-08   v0.4.0rc5   Updating to work with grunt v0.4.0rc5. Switching to this.files api.
 * 2012-10-11   v0.3.1   Rename grunt-contrib-lib dep to grunt-lib-contrib.
 * 2012-09-23   v0.3.0   Options no longer accepted from global config key.
 * 2012-09-09   v0.2.0   Refactored from grunt-contrib into individual repo.

---

Task submitted by [Eric Woroshow](http://ericw.ca/)

*This file was generated on Thu Mar 07 2013 08:08:26.*
