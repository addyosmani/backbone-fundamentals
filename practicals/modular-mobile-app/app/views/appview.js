define( ['jquery', 'backbone', 'utils', 'views/resultList', 'views/photoList'],
        function( $, Backbone, utils, ResultList, PhotoList ) {
            // Using ECMAScript 5 strict mode during development. By default r.js will ignore that.
            "use strict";


            var AppView = Backbone.View.extend( {
                el: $( "#appview" ),
                
                events: {
                    "submit #queryForm" : "keyLoadResults",
                    "change #sortBy": "keyLoadResults",
                    "keydown #searchbox" : "handleKey"
                },

                setView: function( option ) {
                    if ( option == 'search' ) {
                        this.result_view = new ResultList;
                    }
                    else {
                        this.photo_view = new PhotoList;
                    }
                },

                handleKey : function( event ) {
                },

                keyLoadResults: function( event ) {
                    var query = $( '#searchbox' ).val();

                    if ( query ) {

                        var sort = $( '#sortBy' ).val(),
                        endpoint = mobileSearch.utils.queryConstructor( query, sort, 1 );
                        location.hash = endpoint;

                    }
                    else {
                        mobileSearch.utils.loadPrompt( 'Please enter a search query to continue' );
                    }
                    return false;
                }
            } );

            return AppView;
        } );





