## Example config

```javascript
grunt.initConfig({
  imagemin: {                          // Task
    dist: {                            // Target
      options: {                       // Target options
        optimizationLevel: 3
      },
      files: {                         // Dictionary of files
        'dist/img.png': 'src/img.png', // 'destination': 'source'
        'dist/img.jpg': 'src/img.jpg'
      }
    },
    dev: {                             // Another target
      options: {                       // Target options
        optimizationLevel: 0
      },
      files: {
        'dev/img.png': 'src/img.png',
        'dev/img.jpg': 'src/img.jpg'
      }
    }
  }
});

grunt.registerTask('default', ['imagemin']);
```
