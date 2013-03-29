var path = require('path');

if (process.platform === 'darwin') {
    exports.path = path.join(__dirname, '../vendor/osx', 'optipng');
} else if (process.platform === 'linux') {
    if (process.arch === 'x64') {
        exports.path = path.join(__dirname, '../vendor/linux/x64', 'optipng');
    } else {
        exports.path = path.join(__dirname, '../vendor/linux/x86', 'optipng');
    }
} else if (process.platform === 'win32') {
	exports.path = path.join(__dirname, '../vendor/win32', 'optipng.exe');
} else {
	console.log('Unsupported platform:', process.platform, process.arch);
}
