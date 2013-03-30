# grunt-contrib-csslint [![Build Status](https://secure.travis-ci.org/gruntjs/grunt-contrib-csslint.png?branch=master)](http://travis-ci.org/gruntjs/grunt-contrib-csslint)

> Lint CSS files.



## Getting Started
This plugin requires Grunt `~0.4.0`

If you haven't used [Grunt](http://gruntjs.com/) before, be sure to check out the [Getting Started](http://gruntjs.com/getting-started) guide, as it explains how to create a [Gruntfile](http://gruntjs.com/sample-gruntfile) as well as install and use Grunt plugins. Once you're familiar with that process, you may install this plugin with this command:

```shell
npm install grunt-contrib-csslint --save-dev
```

Once the plugin has been installed, it may be enabled inside your Gruntfile with this line of JavaScript:

```js
grunt.loadNpmTasks('grunt-contrib-csslint');
```




## Csslint task
_Run this task with the `grunt csslint` command._

Task targets, files and options may be specified according to the grunt [Configuring tasks](http://gruntjs.com/configuring-tasks) guide.

Files are linted with [csslint](https://github.com/stubbornella/csslint).
### Options

Any specified option will be passed through directly to [csslint](http://csslint.net/), thus you can specify any option that csslint supports. The csslint API is a bit awkward: For each rule, a value of `false` ignores the rule, a value of `2` will set it to become an error. Otherwise all rules are considered warnings.

For the current csslint version, these rules are available:

* important
* adjoining-classes
* known-properties
* box-sizing
* box-model
* overqualified-elements
* display-property-grouping
* bulletproof-font-face
* compatible-vendor-prefixes
* regex-selectors
* errors
* duplicate-background-images
* duplicate-properties
* empty-rules
* selector-max-approaching
* gradients
* fallback-colors
* font-sizes
* font-faces
* floats
* star-property-hack
* outline-none
* import
* ids
* underscore-property-hack
* rules-count
* qualified-headings
* selector-max
* shorthand
* text-indent
* unique-headings
* universal-selector
* unqualified-attributes
* vendor-prefix
* zero-units

For an explanation of those rules, [check the csslint wiki](https://github.com/stubbornella/csslint/wiki/Rules).

*Side note: To update this list, run this:*

```bash
node -e "require('csslint').CSSLint.getRules().forEach(function(x) { console.log(x.id) })"
```
A few additional options are supported:

###### csslintrc
Type: `String`
Default value: `null`

If this filename is specified, options and globals defined therein will be used. Task and target options override the options within the `csslintrc` file. The `csslint` file must be valid JSON and looks something like this:

```json
{
  "qualified-headings": true,
  "unique-headings": true,
  "known-properties": false
}
```
###### formatters
Type: `array`
Default value: `null`

If the formatters options is specified, the grunt csslint task is able to use the
all formatters provided by csslint.

For the current csslint version, the following formatters are supported:

* text
* compact
* lint-xml
* csslint-xml
* checkstyle-xml
* junit-xml

For an explanation of those formatters, [check the csslint wiki](https://github.com/stubbornella/csslint/wiki/Command-line-interface), section Options/--format.
### Usage Examples

```js
csslint: {
  strict: {
    options: {
      import: 2
    },
    src: ['path/to/**/*.css']
  },
  lax: {
    options: {
      import: false
    },
    src: ['path/to/**/*.css']
  }
}
```

###### Loading rules from an external file
```js
csslint: {
  options: {
    csslintrc: '.csslintrc'
  },
  strict: {
    options: {
      import: 2
    },
    src: ['path/to/**/*.css']
  },
  lax: {
    options: {
      import: false
    },
    src: ['path/to/**/*.css']
  }
}
```

###### Specifiying output formatters
```js
csslint: {
  options: {
    csslintrc: '.csslintrc',
    formatters: [
      {id: 'junit-xml', dest: 'report/csslint_junit.xml'},
      {id: 'csslint-xml', dest: 'report/csslint.xml'}
    ]
  },
  strict: {
    options: {
      import: 2
    },
    src: ['path/to/**/*.css']
  }
}
```

## Release History

 * 2013-03-13   v0.1.1   Support formatters Support .csslintrc
 * 2013-03-06   v0.1.0   Initial release.

---

Task submitted by [Jörn Zaefferer](http://bassistance.de)

*This file was generated on Thu Mar 14 2013 08:58:26.*
