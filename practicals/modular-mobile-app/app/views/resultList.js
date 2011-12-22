define( ['jquery', 'backbone', 'underscore', 'models/ResultCollection', 'text!templates/listview.html'],
        function( $, Backbone, _, ResultCollection, listTemplate ) {
            // Using ECMAScript 5 strict mode during development. By default r.js will ignore that.
            "use strict";

            var ResultList = Backbone.View.extend( {
                el: $( "#listviewholder" ),

                initialize: function() {
                    this.collection = new ResultCollection;
                    _.bindAll(this, "renderList");
                    this.collection.bind( "reset", this.renderList );
                },

                renderList: function() {

                    var compiled_template = _.template( listTemplate ),
                        collection = this.collection,
                        $el = $(this.el);

                    mobileSearch.utils.loadPrompt( "Loading results..." );
                    mobileSearch.utils.toggleNavigation( true );
                    $el.html( compiled_template( { results: collection.models } ) );

                    setTimeout( function() {
                       $el.listview('refresh');
                    }, 0 );

                }
            } );

            return ResultList;
        });
