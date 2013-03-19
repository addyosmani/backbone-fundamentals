var path = require('path');

if (process.platform === 'darwin') {
	exports.path = path.join(__dirname, '../vendor/osx', 'jpegtran');
} else if (process.platform === 'linux') {
	if (process.arch === 'x64') {
		exports.path = path.join(__dirname, '../vendor/linux/x64', 'jpegtran');
	} else {
		exports.path = path.join(__dirname, '../vendor/linux/x86', 'jpegtran');
	}
} else if (process.platform === 'win32') {
	if (process.arch === 'x64') {
		exports.path = path.join(__dirname, '../vendor/win64', 'jpegtran.exe');
	} else {
		exports.path = path.join(__dirname, '../vendor/win32', 'jpegtran.exe');
	}
} else {
	console.log('Unsupported platform:', process.platform, process.arch);
}
