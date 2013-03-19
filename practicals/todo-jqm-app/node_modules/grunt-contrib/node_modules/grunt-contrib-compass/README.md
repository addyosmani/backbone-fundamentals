# grunt-contrib-compass [![Build Status](https://secure.travis-ci.org/gruntjs/grunt-contrib-compass.png?branch=master)](http://travis-ci.org/gruntjs/grunt-contrib-compass)

> Compile Compass to CSS



## Getting Started
This plugin requires Grunt `~0.4.0`

If you haven't used [Grunt](http://gruntjs.com/) before, be sure to check out the [Getting Started](http://gruntjs.com/getting-started) guide, as it explains how to create a [Gruntfile](http://gruntjs.com/sample-gruntfile) as well as install and use Grunt plugins. Once you're familiar with that process, you may install this plugin with this command:

```shell
npm install grunt-contrib-compass --save-dev
```

Once the plugin has been installed, it may be enabled inside your Gruntfile with this line of JavaScript:

```js
grunt.loadNpmTasks('grunt-contrib-compass');
```




## Compass task
_Run this task with the `grunt compass` command._

This task requires you to have [Ruby](http://www.ruby-lang.org/en/downloads/) and [Compass](http://compass-style.org/install/). If you're on OS X or Linux you probably already have Ruby installed, try `ruby -v` in your terminal. When you've confirmed you have Ruby installed, run `gem update --system && gem install compass` to install Compass.

Compass operates on a folder level, because of this you don't specify any src/dest, but instead define the `sassDir` and `cssDir` options.

_This plugin is only compatible with Grunt `>= 0.4.x`._

### Options

Compass doesn't expose all of its [options][config] through the CLI, which this task makes use of. If you need an option not mentioned below you can either specify a path to a config.rb file in the `config` option or embed it directly into the `raw` option. Options defined in your Gruntfile will override those specified in your config.rb or `raw` property. `config` and `raw` are mutually exclusive.

#### config ```string```

Specify the location of the Compass [configuration file][config] explicitly. Defaults to the same path as your Gruntfile.

#### raw ```string```

String form of the Compass [configuration file][config].

#### basePath ```string```

The the path Compass will run from. Defaults to the same path as your Gruntfile.

#### app ```string```

Tell compass what kind of application it is integrating with. Can be `stand_alone` (default) or `rails`.

#### sassDir ```string```

The source directory where you keep your Sass stylesheets.

#### cssDir ```string```

The target directory where you keep your CSS stylesheets.

#### specify ```string|array```

Lets you specify which files you want to compile. Useful if you don't want to compile the whole folder. Globbing supported. Ignores filenames starting with underscore. Files must be in the directory you specified in `sassDir`.

#### imagesDir ```string```

The directory where you keep your images.

#### javascriptsDir ```string```

The directory where you keep your JavaScript files.

#### fontsDir ```string```

The directory where you keep your fonts.

#### environment ```string```

Use sensible defaults for your current environment. Can be: `development` (default) or `production`

#### outputStyle ```string```

CSS output mode. Can be: `nested`, `expanded`, `compact`, `compressed`.

#### relativeAssets ```boolean```

Make Compass asset helpers generate relative urls to assets.

#### noLineComments ```boolean```

Disable line comments.

#### require ```string|array```

Require the given Ruby library before running commands. This is used to access Compass plugins without having a project configuration file.

#### load ```string|array```

Load the framework or extensions found in the specified directory.

#### loadAll ```string|array```

Load all the frameworks or extensions found in the specified directory.

#### importPath ```string|array```

Makes files under the specified folder findable by Sass's @import directive.

#### debugInfo ```boolean```

Causes the line number and file where a selector is defined to be emitted into the compiled CSS in a format that can be understood by the browser. Automatically disabled when using `outputStyle: 'compressed'`.

#### quiet ```boolean```

Quiet mode.

#### trace ```boolean```

Show a full stacktrace on error.

#### force ```boolean```

Allows Compass to overwrite existing files.

#### dryRun ```boolean```

Dry Run. Tells you what it plans to do.

#### boring ```boolean```

Turn off colorized output.

#### bundleExec ```boolean```

Run `compass compile` with [bundle exec](http://gembundler.com/man/bundle-exec.1.html): `bundle exec compass compile`.


[config]: http://compass-style.org/help/tutorials/configuration-reference/

### Usage Examples

#### Example config

```javascript
grunt.initConfig({
  compass: {                  // Task
    dist: {                   // Target
      options: {              // Target options
        sassDir: 'sass',
        cssDir: 'css',
        environment: 'production'
      }
    },
    dev: {                    // Another target
      options: {
        sassDir: 'sass',
        cssDir: 'css'
      }
    }
  }
});

grunt.loadNpmTasks('grunt-contrib-compass');

grunt.registerTask('default', ['jshint', 'compass']);
```


#### Example usage


##### Use external config file

```javascript
grunt.initConfig({
  compass: {
    dist: {
      options: {
        config: 'config/config.rb'
      }
    }
  }
});
```

##### Override setting in external config file

```javascript
grunt.initConfig({
  compass: {
    dist: {
      options: {
        config: 'config/config.rb',  // css_dir = 'dev/css'
        cssDir: 'dist/css'
      }
    }
  }
});
```

##### Use `raw` option

```javascript
grunt.initConfig({
  compass: {
    dist: {
      options: {
        sassDir: 'sass',
        cssDir: 'css',
        raw: 'preferred_syntax = :sass\n' // Use `raw` since it's not directly available
      }
    }
  }
});
```


## Release History

 * 2013-02-27   v0.1.3   Fixes bundleExec.
 * 2013-02-17   v0.1.2   Ensure Gruntfile.js is included on npm.
 * 2013-02-15   v0.1.1   First official release for Grunt 0.4.0.
 * 2013-02-05   v0.1.1rc8   Added new options: basePath specify, debugInfo.
 * 2013-01-25   v0.1.1rc7   Updating grunt/gruntplugin dependencies to rc7. Changing in-development grunt/gruntplugin dependency versions from tilde version ranges to specific versions.
 * 2013-01-10   v0.1.1rc5   Updating to work with grunt v0.4.0rc5.
 * 2012-10-22   v0.1.0   Initial release

---

Task submitted by [Sindre Sorhus](http://github.com/sindresorhus)

*This file was generated on Wed Feb 27 2013 23:08:31.*
