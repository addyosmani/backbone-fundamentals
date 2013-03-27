module.exports = function (grunt) {

    //grunt.loadNpmTasks('grunt-contrib');
    grunt.loadNpmTasks('grunt-contrib-handlebars');
    grunt.loadNpmTasks('grunt-contrib-connect');
    grunt.loadNpmTasks('grunt-contrib-clean');

    grunt.loadNpmTasks('grunt-contrib-requirejs');
    grunt.loadNpmTasks('grunt-contrib-jst');
    grunt.loadNpmTasks('grunt-contrib-concat');

    grunt.loadNpmTasks('grunt-contrib-csslint');
    grunt.loadNpmTasks('grunt-contrib-uglify');
    grunt.loadNpmTasks('grunt-contrib-copy');
    grunt.loadNpmTasks('grunt-release');

    grunt.registerTask("default", ["clean", "handlebars", "copy", "requirejs", "concat"]);

    //grunt.registerTask("release", ["default", "min", "mincss"]);

    grunt.initConfig({
            clean : ["dist/"],
            // The lint task will run the build configuration and the application
            // JavaScript through JSHint and report any errors.  You can change the
            // options for this task, by reading this:
            // https://github.com/cowboy/grunt/blob/master/docs/task_lint.md
            lint : {
                files : [
                    "build/config.js", "app/**/*.js"
                ]
            },
            copy: {
                main: {
                    files: [
                        {cwd: 'dist/debug/', src: ['**'], dest: 'dist/release/', expand: true},
                        {cwd: 'assets/', src: ['**'], dest: 'dist/release/', expand: true},
                        {src: ['index.html'], dest: 'dist/release/'}
                    ]
                }
            },
            // The jshint option for scripturl is set to lax, because the anchor
            // override inside main.js needs to test for them so as to not accidentally
            // route.
            jshint : {
                options : {
                    scripturl : true
                }
            },
            handlebars : {
                compile : {
                    options : {
                        namespace : "JST",
                        processName : function (filePath) {
                            return getSimpleFileName(filePath);
                        },
                        processPartialName : function (filePath) { // input:  templates/_header.handlebar
                            return getSimpleFileName(filePath);
                        },
                        partialRegex : /\.template_partial$/
                    },
                    files : {
                        "dist/debug/handlebars_packaged.js" : 'app/templates/**/*.template*'
                    }
                }
            },

            watch : {
                files : ['app/templates/**/*.template*'], // 'app/**/*.less',
                tasks : 'handlebars'
            },

            // The concatenate task is used here to merge the almond require/define
            // shim and the templates into the application code.  It's named
            // dist/debug/require.js, because we want to only load one script file in
            // index.html.
            concat : {
                "dist/debug/require.js" : [
                    "assets/js/libs/almond.js",
                    "dist/debug/templates.js",
                    "dist/debug/require.js"
                ],
                "dist/debug/handlebars_packaged.js" : [
                    "app/handlebars_helpers.js",
                    "app/alltemplates.js"
                ]
            },

            mincss : {
                "dist/release/index.css" : [
                    "assets/css/h5bp.css"
                ]
            },

            min : {
                "dist/release/require.js" : [
                    "dist/debug/require.js"
                ]
            },
            connect: {
                server: {
                    options: {
                        port: 9001,
                        base: '.'
                    },
                    keepalive : true
                }
            },
            requirejs : {
                compile : {
                    options : {
                        baseUrl : "app/",
                        mainConfigFile : "app/config.js",
                        out : "dist/debug/require.js",
                        name : "config",
                        // Do not wrap everything in an IIFE
                        wrap : false
                    }
                }
            },
            qunit : {
                all : ["test/qunit/*.html"]
            }

        }
    );

    var getSimpleFileName = function (fullFilePath) {
        var fileName = fullFilePath.substring(fullFilePath.lastIndexOf('/') + 1);
        return fileName.substring(0, fileName.indexOf('.'));
    };

}
;
