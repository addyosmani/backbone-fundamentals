# Usage Examples

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

#### Loading rules from an external file
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

#### Specifiying output formatters
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