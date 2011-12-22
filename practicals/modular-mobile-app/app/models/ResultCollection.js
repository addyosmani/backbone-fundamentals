define( ['jquery', 'backbone', 'models/ResultEntry'],
        function( $, Backbone, ResultEntry ) {
            // Using ECMAScript 5 strict mode during development. By default r.js will ignore that.
            "use strict";

            var ResultCollection = Backbone.Collection.extend( {
                model: ResultEntry,
                parse: function( response ) {
                    return response;
                }
            } );

            return ResultCollection;
        } );
            
