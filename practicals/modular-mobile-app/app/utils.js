define( ['jquery' ],
        function( $ ) {
            // Using ECMAScript 5 strict mode during development. By default r.js will ignore that.
            //"use strict";
            var utils = {};


            // summary:
            //            Manage passing search queries to the necessary handlers and the UI
            //            changes that are required based on query-type.
            // searchType: String
            //            The type of search to conduct. Supports 'search' for results or
            //            'photo' for individual photo entries
            // ctx: String
            //            The context (view) for which the requests are being made
            // query: String
            //            The query-string to lookup. For search this is a keyword or set of
            //            keywords in string form, for photos this refers to the photo ID
            // sort: String
            //            How the results returned should be sorted. All of the Flickr API sort
            //            modes are supported here
            // page: Integer
            //            The pagination index currently being queried. e.g 2 refers to page 2. 

            utils.dfdQuery = function( searchType, ctx, query, sort, page ) {

                if(!(query==undefined || query == "")){

                    var entries = null;
                    page = (page == undefined) ? 1 : page;

                    utils.loadPrompt( 'Querying Flickr API...' );

                    $.when( utils.fetchResults( searchType, query, sort, page ) )
                            .then( $.proxy( function( response ) {

                                ctx.setView( searchType );

                                // The application can handle routes that come in
                                // through a bookmarked URL differently if needed
                                // simply check against workspace.bookmarkMode
                                // e.g if(!mobileSearch.routers.workspace.bookmarkMode) etc.

                                if ( searchType == 'search' || searchType == undefined ) {

                                    entries = response.photos.photo;

                                    mobileSearch.routers.workspace.q = query;
                                    mobileSearch.routers.workspace.p = page;
                                    mobileSearch.routers.workspace.s = sort;

                                    $( '.search-meta p' ).html( 'Page: ' + response.photos.page + ' / ' + response.photos.pages );
                                    
                                    ctx.result_view.collection.reset( entries );

                                    // switch to search results view
                                    utils.changePage( "#search", "slide", false, false );

                                    // update title
                                    utils.switchTitle( query + ' (Page ' + page + ' of ' + response.photos.total + ')' );

                                }
                                else {

                                    entries = response.photo;
                                    ctx.photo_view.collection.reset( entries );

                                    // switch to the individual photo viewer
                                    utils.changePage( "#photo", "slide", false, false );
                                     
                                }

                    }, ctx ) );
                }else{
                    utils.loadPrompt( 'Please enter a valid search query.' );
                }
            };


            // summary:
            //            A convenience method for accessing $mobile.changePage(), included
            //            in case any other actions are required in the same step.
            // changeTo: String
            //            Absolute or relative URL. In this app references to '#index', '#search' etc.
            // effect: String
            //            One of the supported jQuery mobile transition effects
            // direction: Boolean
            //            Decides the direction the transition will run when showing the page
            // updateHash: Boolean
            //            Decides if the hash in the location bar should be updated

            utils.changePage = function( viewID, effect, direction, updateHash ) {
                $.mobile.changePage( viewID, { transition: effect, reverse:direction, changeHash: updateHash} );
            };


            // summary:
            //            Query for search results or individual photos from the Flickr API
            // searchType: String
            //            The type of search to conduct. Supports 'search' for results or
            //            'photo' for individual photo entries
            // query: String
            //            The query-string to lookup. For search this is a keyword or set of
            //            keywords in string form, for photos this refers to the photo ID
            // sort: String
            //            How the results returned should be sorted. All of the Flickr API sort
            //            modes are supported here
            // page: Integer
            //            The pagination index currently being queried. e.g 2 refers to page 2. 
            // returns:
            //            A promise for the ajax call to be completed

            utils.fetchResults = function( searchType, query, sort, page ) {

                var serviceUrl = "http://api.flickr.com/services/rest/?format=json&jsoncallback=?",
                        apiKey = "8662e376985445d92a07c79ff7d12ff8",
                        geoTagged = null,
                        quantity = 0,
                        safeSearch = '',
                        minDate = "",
                        maxDate = "";


                if ( searchType == 'search' || searchType == undefined ) {

                    quantity = $( '#slider' ).val() || mobileSearch.defaults.resultsPerPage;

                    maxDate = utils.dateFormatter( $( '#date-max' ).val() ) || "";
                    minDate = utils.dateFormatter( $( '#date-min' ).val() ) || "";

                    minDate = "",maxDate = "";

                    ($( '#geo-choice-z1' ).prop( 'checked' ) || mobileSearch.defaults.geoTagged) ? geoTagged = 0 : geoTagged = 1;
                    page = (page == undefined) ? 0 : page;
                    sort = (sort == undefined) ? ($( '#sortBy' ).val()) : sort;
                    serviceUrl += "&method=flickr.photos.search" + "&per_page=" + quantity + "&page=" + page + "&is_geo=" + geoTagged + "&safe_search=" + safeSearch + "&sort=" + sort + "&min_taken_date=" + minDate + "&max_taken_date=" + maxDate + "&text=" + encodeURIComponent(query) + "&api_key=" + apiKey;

                } else if ( searchType == 'photo' ) {
                    serviceUrl += "&method=flickr.photos.getInfo&photo_id=" + encodeURIComponent(query) + "&api_key=" + apiKey;
                }


                return $.ajax( serviceUrl, { dataType: "json" } );
            };


            // summary:
            //            Format dates so that they're compatible with input passed through
            //            the datepicker component
            // date: String
            //            The date string to be formatted
            // returns:
            //            A formatted date
            utils.dateFormatter = function ( dateStr ) {
                return (dateStr == undefined)? '' : $.datepicker.formatDate( '@', new Date( dateStr ) );
            };


            // summary:
            //            Manage the URL construction and navigation for pagination
            //            (e.g next/prev)
            //
            // state: String
            //            The direction in which to navigate (either 'next' or 'prev')

            utils.historySwitch = function( state ) {
                var sortQuery,
                    hashQuery = "", pageQuery = 0, increment = 0;

                (mobileSearch.routers.workspace.q == undefined) ? hashQuery = '' : hashQuery = mobileSearch.routers.workspace.q;
                (mobileSearch.routers.workspace.p == undefined) ? pageQuery = 1 : pageQuery = mobileSearch.routers.workspace.p;
                (mobileSearch.routers.workspace.s == undefined) ? sortQuery = 'relevance' : sortQuery = mobileSearch.routers.workspace.s;

                pageQuery = parseInt( pageQuery );
                (state == 'next') ? pageQuery += 1 : pageQuery -= 1;

                (pageQuery < 1) ? utils.changePage( "/", "slide" ) : location.hash = utils.queryConstructor( hashQuery, sortQuery, pageQuery );

            };


            // summary:
            //            Display a custom notification using the loader extracted from jQuery mobile.
            //            The only reason this is here is for further customization.
            //
            // message: String
            //            The message to display in the notification dialog

            utils.loadPrompt = function( message ) {
                message = (message == undefined) ? "" : message;

                $( "<div class='ui-loader ui-overlay-shadow ui-body-e ui-corner-all'><h1>" + message + "</h1></div>" )
                .css( { "display": "block", "opacity": 0.96, "top": $( window ).scrollTop() + 100 } )
                .appendTo( $.mobile.pageContainer )
                .delay( 800 )
                .fadeOut( 400, function() {
                    $( this ).remove();
                } );
                
            };


            // summary:
            //            Adjust the title of the current view
            //
            // title: String
            //            The title to update the view with
            utils.switchTitle = function( title ) {
                $( '.ui-title' ).text( title || "" );
            };


            // summary:
            //            Construct a search query for processing
            //
            // query: String
            //            The query-string to lookup. For search this is a keyword or set of
            //            keywords in string form, for photos this refers to the photo ID
            // sortType: String
            //            How the results returned should be sorted. All of the Flickr API sort
            //            modes are supported here
            // page: Integer
            //            The pagination index currently being queried. e.g 2 refers to page 2

            utils.queryConstructor = function( query, sortType, page ) {
                return 'search/' + query + '/s' + sortType + '/p' + page;
            };


            // summary:
            //            Toggle whether the navigation is displayed or hidden
            //
            // toggleState: Boolean
            //            A boolean that decides whether the navigation should be toggled on or off.

            utils.toggleNavigation = function( toggleState ) {
                mobileSearch.ui.nextOption.toggle( toggleState );
                mobileSearch.ui.prevOption.toggle( toggleState );
            };

            return utils;
        } );




