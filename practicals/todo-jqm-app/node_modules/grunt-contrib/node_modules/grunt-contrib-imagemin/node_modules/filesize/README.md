[![build status](https://secure.travis-ci.org/avoidwork/filesize.js.png)](http://travis-ci.org/avoidwork/filesize.js)
# filesize.js

filesize.js provides a simple way to get a human readable file size string from a number (float or integer) or string.  An optional second parameter is the decimal place to round to (default is 2), or _true_ which triggers Unix style output.

## Examples

``` js
filesize(500);                    // "3.91Kb"
filesize(500, true);              // "3.9k"
filesize(1500);                   // "1.46KB"
filesize("1500000000");           // "1.40GB"
filesize("1500000000", 0);        // "1GB"
filesize(1212312421412412);       // "1.08PB"
filesize(1212312421412412, true); // "1.1P" - shorthand output, similar to *nix "ls -lh"
```

## How can I load filesize.js?

filesize.js supports AMD loaders (require.js, curl.js, etc.), node.js & npm (npm install filesize), or using a script tag.

## Information

#### License

filesize.js is licensed under BSD-3 http://www.opensource.org/licenses/BSD-3-Clause

#### Copyright

Copyright (c) 2012, Jason Mulligan <jason.mulligan@avoidwork.com>