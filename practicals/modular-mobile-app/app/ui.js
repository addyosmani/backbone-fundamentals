define( ['jquery', 'utils' ],
        function( $, utils ) {
            // Using ECMAScript 5 strict mode during development. By default r.js will ignore that.
            "use strict";

            var ui = {};
            ui.nextOption = $( '#nextSet' );
            ui.prevOption = $( '#prevSet' );

            ui.nextOption.on( 'click', function( e ) {
                e.preventDefault();
                utils.historySwitch( 'next' );
            } );

            ui.prevOption.on( 'click', function( e ) {
                e.preventDefault();
                utils.historySwitch( 'prev' );
            } );

            return ui;
        } );