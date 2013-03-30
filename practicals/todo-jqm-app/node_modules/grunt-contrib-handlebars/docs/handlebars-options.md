# Options

## separator
Type: `String`
Default: linefeed + linefeed

Concatenated files will be joined on this string.

## namespace
Type: `String` `false`
Default: 'JST'

The namespace in which the precompiled templates will be assigned.  *Use dot notation (e.g. App.Templates) for nested namespaces or false for no namespace wrapping.*  When false with `amd` option set `true`, templates will be returned directly from the AMD wrapper.

Example:
```js
options: {
  namespace: 'MyApp.Templates'
}
```

## partialsUseNamespace
Type: `Boolean`
Default: 'false'

When set to `true`, partials will be registered in the `namespace` in addition to templates.

## wrapped
Type: `Boolean`
Default: `true`

Determine if preprocessed template functions will be wrapped in Handlebars.template function.

## node
Type: `Boolean`
Default: `false`

Enable the compiled file to be required on node.js by preppending and appending proper declarations. You can use the file safely on the front-end.

For this option to work you need to define the `namespace` option.

## amd
Type: `Boolean`
default: `false`

Wraps the output file with an AMD define function and returns the compiled template namespace unless namespace has been explicitly set to false in which case the template function will be returned directly.

```js
define(function() {
    //...//
    returns this['[template namespace]'];
});
```

## processContent
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

## processAST
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

## processName
Type: `function`

This option accepts a function which takes one argument (the template filepath) and returns a string which will be used as the key for the precompiled template object.  The example below stores all templates on the default JST namespace in capital letters.

```js
options: {
  processName: function(filename) {
    return filename.toUpperCase();
  }
}
```

## processPartialName
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

## partialRegex
Type: `Regexp`
Default: /^_/

This option accepts a regex that defines the prefix character that is used to identify Handlebars partial files.

``` javascript
// assumes partial files would be prefixed with "par_" ie: "par_header.hbs"
options: {
  partialRegex: /^par_/
}
```

## partialsPathRegex
Type: `Regexp`
Default: '/./'

This option accepts a regex that defines the path to a directory of Handlebars partials files. The example below shows how to mark every file in a specific directory as a partial.

``` javascript
options: {
  partialRegex: /.*/,
  partialsPathRegex: /\/partials\//
}
```
