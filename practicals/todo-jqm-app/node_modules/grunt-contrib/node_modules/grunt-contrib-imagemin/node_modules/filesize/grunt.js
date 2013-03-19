module.exports = function(grunt) {
	grunt.initConfig({
		pkg : "<json:package.json>",
		meta : {
			banner : "/**\n" + 
			         " * <%= pkg.name %>\n" +
			         " *\n" +
			         " * @author <%= pkg.author.name %> <<%= pkg.author.email %>>\n" +
			         " * @copyright <%= grunt.template.today('yyyy') %> <%= pkg.author.name %>\n" +
			         " * @license <%= pkg.licenses[0].type %> <<%= pkg.licenses[0].url %>>\n" +
			         " * @link <%= pkg.homepage %>\n" +
			         " * @module <%= pkg.name %>\n" +
			         " * @version <%= pkg.version %>\n" +
			         " */"
		},
		concat: {
			dist: {
				src : [
					"<banner>",
					"src/filesize.js"
				],
				dest : "lib/filesize.js"
			}
		},
		test : {
			files : ["test/**/*.js"]
		},
		lint : {
			files : ["grunt.js"]
		},
		min : {
			"lib/filesize.min.js" : ["<banner>", "lib/filesize.js"]
		},
		watch : {
			files : "<config:lint.files>",
			tasks : "default"
		},
		jshint : {
			options : {
				curly   : true,
				eqeqeq  : true,
				immed   : true,
				latedef : true,
				newcap  : true,
				noarg   : true,
				sub     : true,
				undef   : true,
				boss    : true,
				eqnull  : true,
				node    : true
			},
			globals: {
				exports : true
			}
		}
	});

	grunt.registerTask("default", "concat min test");
};