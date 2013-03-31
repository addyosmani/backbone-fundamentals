module.exports = function (grunt) {

    grunt.loadNpmTasks('grunt-contrib');
//    grunt.loadNpmTasks('grunt-release');
    grunt.loadNpmTasks('grunt-contrib-handlebars');

    grunt.loadNpmTasks('grunt-contrib-connect');
    grunt.loadNpmTasks('grunt-contrib-clean');

    grunt.loadNpmTasks('grunt-contrib-requirejs');
    grunt.loadNpmTasks('grunt-contrib-concat');

    grunt.loadNpmTasks('grunt-contrib-csslint');
    grunt.loadNpmTasks('grunt-contrib-uglify');
    grunt.loadNpmTasks('grunt-contrib-copy');

    grunt.registerTask("default", ["server"]);
    grunt.registerTask("server", ["handlebars", "connect"]);
    grunt.registerTask("liveload", ["watch"]);
    grunt.registerTask("release", ["clean", "handlebars", "requirejs", "concat", "copy"]);
    /* if release-task supports grunt 0.4.0, use instead
    / grunt.registerTask("default", "clean lint jst requirejs concat");
    / grunt.registerTask("release", ["default", "min", "mincss"]);
    */

    grunt.initConfig({
            clean : ["dist/"],
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
                        base: '.',
                        keepalive : true
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
            // The concatenate task is used here to merge the almond require/define
            // shim and the templates into the application code.  It's named
            // dist/debug/require.js, because we want to only load one script file in
            // index.html.
            ,concat : {
                "dist/debug/require.js" : [
                    "assets/js/libs/almond.js",
                    "dist/debug/templates.js",
                    "dist/debug/require.js"
                ],
                "dist/debug/handlebars_packaged.js" : [
                    "app/handlebars_helpers.js",
                    "app/alltemplates.js"
                ]
            }

            /*
             With the upgrade to grunt 0.4.0, the grunt-release task is not supported, yet.
             With 0.4.0, the release-task was replaced by the copy- and connect-task,
             which slows down the development process of server reloading.

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
             */
        }


    );

    var getSimpleFileName = function (fullFilePath) {
        var fileName = fullFilePath.substring(fullFilePath.lastIndexOf('/') + 1);
        return fileName.substring(0, fileName.indexOf('.'));
    };

}
;
