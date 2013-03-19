/*
 * grunt-contrib-connect
 * http://gruntjs.com/
 *
 * Copyright (c) 2012 "Cowboy" Ben Alman, contributors
 * Licensed under the MIT license.
 */

'use strict';

module.exports = function(grunt) {

  // Nodejs libs.
  var path = require('path');

  // External libs.
  var connect = require('connect');

  grunt.registerMultiTask('connect', 'Start a connect web server.', function() {

    // Merge task-specific options with these defaults.
    var options = this.options({
      port: 8000,
      hostname: 'localhost',
      base: '.',
      keepalive: false,
      middleware: function(connect, options) {
        return [
          // Serve static files.
          connect.static(options.base),
          // Make empty directories browsable.
          connect.directory(options.base),
        ];
      }
    });

    // Connect requires the base path to be absolute.
    options.base = path.resolve(options.base);

    var middleware = options.middleware ? options.middleware.call(this, connect, options) : [];

    // If --debug was specified, enable logging.
    if (grunt.option('debug')) {
      connect.logger.format('grunt', ('[D] server :method :url :status ' +
        ':res[content-length] - :response-time ms').magenta);
      middleware.unshift(connect.logger('grunt'));
    }

    // Start server.
    grunt.log.writeln('Starting connect web server on ' + options.hostname + ':' + options.port + '.');

    connect.apply(null, middleware)
      .listen(options.port, options.hostname)
      .on('error', function(err) {
        if (err.code === 'EADDRINUSE') {
          grunt.fatal('Port ' + options.port + ' is already in use by another process.');
        } else {
          grunt.fatal(err);
        }
      });

    // So many people expect this task to keep alive that I'm adding an option
    // for it. Running the task explicitly as grunt:keepalive will override any
    // value stored in the config. Have fun, people.
    if (this.flags.keepalive || options.keepalive) {
      // This is now an async task. Since we don't store a handle to the "done"
      // function, this task will never, ever, ever terminate. Have fun!
      this.async();
      grunt.log.write('Waiting forever...\n');
    }
  });

};
