# grunt-contrib-requirejs [![Build Status](https://secure.travis-ci.org/gruntjs/grunt-contrib-requirejs.png?branch=master)](http://travis-ci.org/gruntjs/grunt-contrib-requirejs)

> Optimize RequireJS projects using r.js.



## Getting Started
This plugin requires Grunt `~0.4.0`

If you haven't used [Grunt](http://gruntjs.com/) before, be sure to check out the [Getting Started](http://gruntjs.com/getting-started) guide, as it explains how to create a [Gruntfile](http://gruntjs.com/sample-gruntfile) as well as install and use Grunt plugins. Once you're familiar with that process, you may install this plugin with this command:

```shell
npm install grunt-contrib-requirejs --save-dev
```

One the plugin has been installed, it may be enabled inside your Gruntfile with this line of JavaScript:

```js
grunt.loadNpmTasks('grunt-contrib-requirejs');
```

*This plugin was designed to work with Grunt 0.4.x. If you're still using grunt v0.3.x it's strongly recommended that [you upgrade](http://gruntjs.com/upgrading-from-0.3-to-0.4), but in case you can't please use [v0.3.3](https://github.com/gruntjs/grunt-contrib-requirejs/tree/grunt-0.3-stable).*



## Requirejs task
_Run this task with the `grunt requirejs` command._

Task targets and options may be specified according to the grunt [Configuring tasks](http://gruntjs.com/configuring-tasks) guide.
### Options

For a full list of possible options, [see the r.js example build file](https://github.com/jrburke/r.js/blob/master/build/example.build.js).
### Usage Examples

```js
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


## Release History

 * 2013-02-14   v0.4.0   First official release for Grunt 0.4.0.
 * 2013-01-22   v0.4.0rc7   Updating to work with grunt v0.4.0rc7.
 * 2013-01-08   v0.4.0rc5   Updating to work with grunt v0.4.0rc5.
 * 2012-10-11   v0.3.3   Rename grunt-contrib-lib dep to grunt-lib-contrib.
 * 2012-10-08   v0.3.1   Bump to RequireJS 2.1.x. Run optimizer async.
 * 2012-09-22   v0.3.0   Options no longer accepted from global config key.
 * 2012-09-09   v0.2.0   Refactored from grunt-contrib into individual repo.

---

Task submitted by [Tyler Kellen](http://goingslowly.com/)

*This file was generated on Mon Feb 18 2013 08:59:34.*
