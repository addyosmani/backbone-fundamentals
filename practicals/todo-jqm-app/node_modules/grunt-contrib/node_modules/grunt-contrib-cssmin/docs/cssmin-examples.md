# Usage Examples

```js
cssmin: {
  compress: {
    files: {
      'path/to/output.css': ['path/to/input_one.css', 'path/to/input_two.css']
    }
  },
  with_banner: {
    options: {
      banner: '/* My minified css file */'
    },
    files: {
      'path/to/output.css': ['path/to/**/*.css']
    }
  }
}
```
