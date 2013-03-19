module.exports = function (grunt) {
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
                            return grunt.helper("getSimpleFileName", filePath);
                        },
                        processPartialName : function (filePath) { // input:  templates/_header.handlebar
                            return grunt.helper("getSimpleFileName", filePath);
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

            server : {
                files : {
                    "favicon.ico" : "favicon.ico"
                },

                debug : {
                    files : {
                        "favicon.ico" : "favicon.ico"
                    },

                    folders : {
                        "app" : "dist/debug",
                        "assets/js/libs" : "dist/debug"
                    }
                },

                release : {
                    // These two options make it easier for deploying, by using whatever
                    // PORT is available in the environment and defaulting to any IP.
                    host : "0.0.0.0",
                    port : process.env.PORT || 8000,

                    files : {
                        "favicon.ico" : "favicon.ico"
                    },

                    folders : {
                        "app" : "dist/release",
                        "assets/js/libs" : "dist/release",
                        "assets/css" : "dist/release"
                    }
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

    grunt.registerHelper('getSimpleFileName', function (fullFilePath) {
        var fileName = fullFilePath.substring(fullFilePath.lastIndexOf('/') + 1);
        return fileName.substring(0, fileName.indexOf('.'));
    });

    grunt.registerTask("default", "clean lint jst requirejs concat");

    grunt.registerTask("debug", "default");

    grunt.registerTask("release", "default min mincss");

    grunt.loadNpmTasks('grunt-contrib');

}
;
