# Options

## pretty
Type: `Boolean`
Default: false

Output indented HTML.

## data
Type: `Object`

Sets the data passed to `jade` during template compilation. Any data can be passed to the template (including `grunt` templates).

## comileDebug
Type: `Boolean`

Set `compileDebug: fase` to remove `jade` debug instructions in javascript templates.

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