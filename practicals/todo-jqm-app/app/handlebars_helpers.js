define(['handlebars'], function (Handlebars) {

    return function () {
        Handlebars.registerHelper('whatis', function (param) {
            console.log(param);
        });

        // https://github.com/wycats/handlebars.js/issues/182
        Handlebars.registerHelper('$', function ( child, options ) {
            if ( typeof child !== 'object' ) {
                return '';
            }
            child['$_'] = this;
            return options.fn( child );
        });

        Handlebars.registerHelper('ifequal', function (val1, val2, fn, elseFn) {
            if (val1 === val2) {
                return fn();
            }
            else if (elseFn) {
                return elseFn();
            }
        });
    }();
});


