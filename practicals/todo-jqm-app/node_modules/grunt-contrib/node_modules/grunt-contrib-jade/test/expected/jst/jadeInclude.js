this["JST"] = this["JST"] || {};

this["JST"]["jadeInclude"] = function anonymous(locals, attrs, escape, rethrow, merge) {
attrs = attrs || jade.attrs; escape = escape || jade.escape; rethrow = rethrow || jade.rethrow; merge = merge || jade.merge;
var buf = [];
with (locals || {}) {
var interp;
buf.push('<html><head><title>TEST</title></head><body></body></html>');
 var a = 'hello jade test'
buf.push('<p>');
var __val__ = a
buf.push(escape(null == __val__ ? "" : __val__));
buf.push('</p>');
}
return buf.join("");
};