var filesize = require("../lib/filesize.js");

exports["filesize"] = {
	setUp: function (done) {
		this.num   = 1024;
		this.str   = "1024";
		this.invld = "abc";
		this.Kb    = 500;
		this.neg   = -1024;
		this.byte  = 1;
		this.zero  = 0;
		done();
	},
	valid: function (test) {
		test.expect(16);
		test.equal(filesize(this.Kb),         "3.91Kb",  "Should match");
		test.equal(filesize(this.Kb,true),    "3.9k",    "Should match");
		test.equal(filesize(this.num),        "1.00KB",  "Should match");
		test.equal(filesize(this.str),        "1.00KB",  "Should match");
		test.equal(filesize(this.num, 1),     "1.0KB",   "Should match");
		test.equal(filesize(this.str, 1),     "1.0KB",   "Should match");
		test.equal(filesize(this.num, true),  "1K",      "Should match");
		test.equal(filesize(this.str, true),  "1K",      "Should match");
		test.equal(filesize(this.neg),        "-1.00KB", "Should match");
		test.equal(filesize(this.neg, true),  "-1K",     "Should match");
		test.equal(filesize(this.byte),       "1.00B",   "Should match");
		test.equal(filesize(this.byte, 1),    "1.0B",    "Should match");
		test.equal(filesize(this.byte, true), "1B",      "Should match");
		test.equal(filesize(this.zero),       "0.00B",   "Should match");
		test.equal(filesize(this.zero, 1),    "0.0B",    "Should match");
		test.equal(filesize(this.zero, true), "0B",      "Should match");
		this.byte  = 1;
		test.done();
	},
	invalid: function (test) {
		test.expect(3);
		test.throws(function () { filesize(this.invld) },       Error, "Should match");
		test.throws(function () { filesize(this.invld, 1) },    Error, "Should match");
		test.throws(function () { filesize(this.invld, true) }, Error, "Should match");
		test.done();
	}
};
