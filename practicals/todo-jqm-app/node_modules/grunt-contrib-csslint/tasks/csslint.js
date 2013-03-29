/*
 * grunt-contrib-cssmin
 * http://gruntjs.com/
 *
 * Copyright (c) 2012 Tim Branyen, contributors
 * Licensed under the MIT license.
 */

'use strict';

module.exports = function(grunt) {
  grunt.registerMultiTask( "csslint", "Lint CSS files with csslint", function() {
    var csslint = require( "csslint" ).CSSLint;
    var ruleset = {};
    var verbose = grunt.verbose;
    var externalOptions = {};
    var combinedResult = {};
    var options = this.options();

    // Read JSHint options from a specified jshintrc file.
    if (options.csslintrc) {
      externalOptions = grunt.file.readJSON( options.csslintrc );
      // delete csslintrc option to not confuse csslint if a future release
      // implements a rule or options on its own
      delete options.csslintrc;
    }

    // merge external options with options specified in gruntfile
    options = grunt.util._.extend( options, externalOptions );

    csslint.getRules().forEach(function( rule ) {
      ruleset[ rule.id ] = 1;
    });

    for ( var rule in options ) {
      if ( !options[ rule ] ) {
        delete ruleset[rule];
      } else {
        ruleset[ rule ] = options[ rule ];
      }
    }
    var hadErrors = 0;
    this.filesSrc.forEach(function( filepath ) {
      var file = grunt.file.read( filepath ),
        message = "Linting " + filepath + "...",
        result;

      // skip empty files
      if (file.length) {
        result = csslint.verify( file, ruleset );
        verbose.write( message );
        if (result.messages.length) {
          verbose.or.write( message );
          grunt.log.error();
        } else {
          verbose.ok();
        }

        // store combined result for later use with formatters
        combinedResult[filepath] = result;

        result.messages.forEach(function( message ) {
          grunt.log.writeln( "[".red + (typeof message.line !== "undefined" ? ( "L" + message.line ).yellow + ":".red + ( "C" + message.col ).yellow : "GENERAL".yellow) + "]".red );
          grunt.log[ message.type === "error" ? "error" : "writeln" ]( message.message + " " + message.rule.desc + " (" + message.rule.id + ")" );
        });
        if ( result.messages.length ) {
          hadErrors += 1;
        }
      } else {
        grunt.log.writeln( "Skipping empty file " + filepath);
      }

    });

    // formatted output
    if (options.formatters && grunt.util._.isArray( options.formatters )) {
      options.formatters.forEach(function ( formatterDefinition ) {
        if (formatterDefinition.id && formatterDefinition.dest) {
          var formatter = csslint.getFormatter( formatterDefinition.id );
          if (formatter) {
            var output = formatter.startFormat();
            grunt.util._.each( combinedResult, function ( result, filename ) {
              output += formatter.formatResults( result, filename, {});
            });
            output += formatter.endFormat();
            grunt.file.write( formatterDefinition.dest, output );
          }
        }
      });
    }

    if ( hadErrors ) {
      return false;
    }
    grunt.log.ok( this.filesSrc.length + " files lint free." );
  });
};
