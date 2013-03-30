this["JST"] = this["JST"] || {};

this["JST"]["jade2"] = function anonymous(locals, attrs, escape, rethrow, merge) {
attrs = attrs || jade.attrs; escape = escape || jade.escape; rethrow = rethrow || jade.rethrow; merge = merge || jade.merge;
var buf = [];
with (locals || {}) {
var interp;
buf.push('<div id="test" class="test"><span id="data">data</span>');
if ( test)
{
buf.push('<div>testing 2</div>');
}
buf.push('</div>');
}
return buf.join("");
};