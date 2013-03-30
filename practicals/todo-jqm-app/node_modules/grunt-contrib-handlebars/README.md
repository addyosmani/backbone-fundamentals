# grunt-contrib-handlebars [![Build Status](https://secure.travis-ci.org/gruntjs/grunt-contrib-handlebars.png?branch=master)](http://travis-ci.org/gruntjs/grunt-contrib-handlebars)

> Precompile Handlebars templates to JST file.



## Getting Started
This plugin requires Grunt `~0.4.0`

If you haven't used [Grunt](http://gruntjs.com/) before, be sure to check out the [Getting Started](http://gruntjs.com/getting-started) guide, as it explains how to create a [Gruntfile](http://gruntjs.com/sample-gruntfile) as well as install and use Grunt plugins. Once you're familiar with that process, you may install this plugin with this command:

```shell
npm install grunt-contrib-handlebars --save-dev
```

Once the plugin has been installed, it may be enabled inside your Gruntfile with this line of JavaScript:

```js
grunt.loadNpmTasks('grunt-contrib-handlebars');
```

*This plugin was designed to work with Grunt 0.4.x. If you're still using grunt v0.3.x it's strongly recommended that [you upgrade](http://gruntjs.com/upgrading-from-0.3-to-0.4), but in case you can't please use [v0.3.3](https://github.com/gruntjs/grunt-contrib-handlebars/tree/grunt-0.3-stable).*



## Handlebars task
_Run this task with the `grunt handlebars` command._

Task targets, files and options may be specified according to the grunt [Configuring tasks](http://gruntjs.com/configuring-tasks) guide.
### Options

#### separator
Type: `String`
Default: linefeed + linefeed

Concatenated files will be joined on this string.

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

#### partialsUseNamespace
Type: `Boolean`
Default: 'false'

When set to `true`, partials will be registered in the `namespace` in addition to templates.

#### wrapped
Type: `Boolean`
Default: `true`

Determine if preprocessed template functions will be wrapped in Handlebars.template function.

#### node
Type: `Boolean`
Default: `false`

Enable the compiled file to be required on node.js by preppending and appending proper declarations. You can use the file safely on the front-end.

For this option to work you need to define the `namespace` option.

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

#### processContent
Type: `function`

This option accepts a function which takes one argument (the template file content) and returns a string which will be used as the source for the precompiled template object.  The example below removes leading and trailing spaces to shorten templates.

```js
options: {
  processContent: function(content) {
    content = content.replace(/^[\x20\t]+/mg, '').replace(/[\x20\t]+$/mg, '');
    content = content.replace(/^[\r\n]+/, '').replace(/[\r\n]*$/, '\n');
    return content;
  }
}
```

#### processAST
Type: `function`

This option accepts a function which takes one argument (the parsed Abstract Syntax Tree) and returns a modified version which will be used as the source for the precompiled template object.  The example below removes any partial and replaces it with the text `foo`.

```js
options: {
  processAST: function(ast) {
    ast.statements.forEach(function(statement, i) {
      if (statement.type === 'partial') {
        ast.statements[i] = { type: 'content', string: 'foo' };
      }
    });
    return ast;
  }
}
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

#### processPartialName
Type: ```function```

This option accepts a function which takes one argument (the partial filepath) and returns a string which will be used as the key for the precompiled partial object when it is registered in Handlebars.partials. The example below stores all partials using only the actual filename instead of the full path.

```js
options: {
  processPartialName: function(filePath) { // input:  templates/_header.hbs
    var pieces = filePath.split("/");
    return pieces[pieces.length - 1]; // output: _header.hbs
  }
}
````

Note: If processPartialName is not provided as an option the default assumes that partials will be stored by stripping trailing underscore characters and filename extensions. For example, the path *templates/_header.hbs* will become *header* and can be referenced in other templates as *{{> header}}*.

#### partialRegex
Type: `Regexp`
Default: /^_/

This option accepts a regex that defines the prefix character that is used to identify Handlebars partial files.

``` javascript
// assumes partial files would be prefixed with "par_" ie: "par_header.hbs"
options: {
  partialRegex: /^par_/
}
```

#### partialsPathRegex
Type: `Regexp`
Default: '/./'

This option accepts a regex that defines the path to a directory of Handlebars partials files. The example below shows how to mark every file in a specific directory as a partial.

``` javascript
options: {
  partialRegex: /.*/,
  partialsPathRegex: /\/partials\//
}
```

### Usage Examples

```js
handlebars: {
  compile: {
    options: {
      namespace: "JST"
    },
    files: {
      "path/to/result.js": "path/to/source.hbs",
      "path/to/another.js": ["path/to/sources/*.hbs", "path/to/more/*.hbs"]
    }
  }
}
```


## Release History

 * 2013-03-13   v0.5.8   Update handlebars dep to ~1.0.10
 * 2013-03-10   v0.5.7   Fix regression with 'wrapped' option.
 * 2013-03-06   v0.5.6   [object Object]
 * 2013-02-26   v0.5.5   Add new options partialsUseNamespace, partialRegex, partialsPathRegex
 * 2013-02-14   v0.5.4   First official release for Grunt 0.4.0.
 * 2013-02-07   v0.5.4rc7   When `namespace` is false and `amd` is true, return handlebars templates directly from AMD wrapper.
 * 2013-01-31   v0.5.3rc7   Add `node` option to produce dual node.js / front-end compiled file.
 * 2013-01-28   v0.5.2rc7   Define handlebars as a dependency for AMD option.
 * 2013-01-27   v0.5.1rc7   Add AMD compilation option. Add processContent option. Do not generate templates into a namespaces when namespace option is false.
 * 2013-01-22   v0.5.0rc7   Updating grunt/gruntplugin dependencies to rc7. Changing in-development grunt/gruntplugin dependency versions from tilde version ranges to specific versions. Default wrapped option to true.
 * 2013-01-08   v0.4.0rc5   Updating to work with grunt v0.4.0rc5. Switching to this.files api.
 * 2012-11-20   v0.3.3   Reset for each target
 * 2012-10-11   v0.3.2   Rename grunt-contrib-lib dep to grunt-lib-contrib.
 * 2012-10-02   v0.3.1   Bugfix default processPartialName.
 * 2012-09-22   v0.3.0   Options no longer accepted from global config key.
 * 2012-09-15   v0.2.3   Support for nested namespaces.
 * 2012-09-11   v0.2.2   Escape single quotes in filenames.
 * 2012-09-09   v0.2.0   Refactored from grunt-contrib into individual repo.

---

Task submitted by [Tim Branyen](http://tbranyen.com)

*This file was generated on Thu Mar 14 2013 09:50:33.*
