define( ['jquery', 'backbone', 'underscore', 'models/PhotoCollection', 'text!templates/photoview.html'],
        function( $, Backbone, _ , PhotoCollection, photoTemplate) {
            // Using ECMAScript 5 strict mode during development. By default r.js will ignore that.
            "use strict";

            var PhotoList = Backbone.View.extend( {
                el: $( "#photoviewholder" ),

                initialize: function() {
                    this.collection = new PhotoCollection;
                    _.bindAll(this, "renderList");
                    this.collection.bind( "reset", this.renderList );
                },

                renderList: function( collection ) {

                    var compiled_template = _.template(photoTemplate),
                        collection = this.collection,
                        $el = $(this.el);

                    mobileSearch.utils.loadPrompt( "Loading photo..." );
                    $( '#photo .ui-title' ).html( 'Photo view' );
                    $el.html( compiled_template( { results: collection.models } ) );

                    setTimeout( function() {
                        $el.listview('refresh');
                    }, 0 );

                }
            } );

            return PhotoList;
        } );
