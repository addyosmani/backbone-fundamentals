/*
 * grunt-contrib-jade
 * http://gruntjs.com/
 *
 * Copyright (c) 2012 Eric Woroshow, contributors
 * Licensed under the MIT license.
 */

'use strict';

module.exports = function(grunt) {

  // Project configuration.
  grunt.initConfig({
    jshint: {
      all: [
        'Gruntfile.js',
        'tasks/*.js',
        '<%= nodeunit.tests %>'
      ],
      options: {
        jshintrc: '.jshintrc'
      }
    },

    // Before generating any new files, remove any previously-created files.
    clean: {
      test: ['tmp']
    },

    // Configuration to be run (and then tested).
    jade: {
      compile: {
        files: {
          'tmp/jade.html': ['test/fixtures/jade.jade'],
          'tmp/jade2.html': ['test/fixtures/jade2.jade'],
          'tmp/jadeInclude.html': ['test/fixtures/jadeInclude.jade'],
          'tmp/jadeTemplate.html': ['test/fixtures/jadeTemplate.jade']
        },
        options: {
          data: {
            test: true,
            year: '<%= grunt.template.today("yyyy") %>'
          }
        }
      },

      compile_amd: {
        files: {
          'tmp/amd/jade.js': ['test/fixtures/jade.jade'],
          'tmp/amd/jade2.js': ['test/fixtures/jade2.jade'],
          'tmp/amd/jadeInclude.js': ['test/fixtures/jadeInclude.jade'],
          'tmp/amd/jadeTemplate.js': ['test/fixtures/jadeTemplate.jade']
        },
        options: {
          client: true,
          amd: true,
          namespace: false,
          compileDebug: false,
          data: {
            test: true,
            year: '<%= grunt.template.today("yyyy") %>'
          }
        }
      },

      compile_jst: {
        files: {
          'tmp/jst/jade.js': ['test/fixtures/jade.jade'],
          'tmp/jst/jade2.js': ['test/fixtures/jade2.jade'],
          'tmp/jst/jadeInclude.js': ['test/fixtures/jadeInclude.jade'],
          'tmp/jst/jadeTemplate.js': ['test/fixtures/jadeTemplate.jade']
        },
        options: {
          client: true,
          compileDebug: false,
          processName: function(str) { return str.match(/^test\/fixtures\/(.*)\.jade$/)[1]; },
          data: {
            test: true,
            year: '<%= grunt.template.today("yyyy") %>'
          }
        }
      }
    },

    // Unit tests.
    nodeunit: {
      tests: ['test/*_test.js']
    }
  });

  // Actually load this plugin's task(s).
  grunt.loadTasks('tasks');

  // These plugins provide necessary tasks.
  grunt.loadNpmTasks('grunt-contrib-clean');
  grunt.loadNpmTasks('grunt-contrib-jshint');
  grunt.loadNpmTasks('grunt-contrib-nodeunit');
  grunt.loadNpmTasks('grunt-contrib-internal');

  // Whenever the "test" task is run, first clean the "tmp" dir, then run this
  // plugin's task(s), then test the result.
  grunt.registerTask('test', ['clean', 'jade', 'nodeunit']);

  // By default, lint and run all tests.
  grunt.registerTask('default', ['jshint', 'test', 'build-contrib']);

};
