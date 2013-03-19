## Minify CSS files
> Contributed By: [Tim Branyen](/tbranyen) (@tbranyen)

### Configuration

Inside your `grunt.js` file, add a section named `mincss`. This section specifies the files to compress with [clean-css](https://github.com/GoalSmashers/clean-css).

#### Parameters

##### files ```object```

This defines what files this task will process and should contain key:value pairs.

The key (destination) should be an unique filepath (supports [grunt.template](https://github.com/cowboy/grunt/blob/master/docs/api_template.md)) and the value (source) should be a filepath or an array of filepaths (supports [minimatch](https://github.com/isaacs/minimatch)).

Note: When the value contains an array of multiple filepaths, the contents are concatenated in the order passed.

#### Config Example

``` javascript
mincss: {
  compress: {
    files: {
      "path/to/output.css": ["path/to/input_one.css", "path/to/input_two.css"]
    } 
  }
}

```