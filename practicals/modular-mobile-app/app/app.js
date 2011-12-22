/*!
 * Flickly - Backbone jQuery Mobile demo
 * http://addyosmani.com
 * Copyright (c) 2011 Addy Osmani
 * Dual licensed under the MIT and GPL licenses.
 *
 */
define( ['backbone', 'views/appview', 'routers/workspace', 'utils', 'ui'],
        function( Backbone, AppView, Workspace, utils, ui ) {
            // Using ECMAScript 5 strict mode during development. By default r.js will ignore that.
            "use strict";
           
           $(function(){
            
            window.mobileSearch = window.mobileSearch || {
                views: {
                    appview: new AppView
                },
                routers:{
                    workspace:new Workspace()
                },
                utils: utils,
                ui: ui,
                defaults:{
                    resultsPerPage: 16,
                    safeSearch: 2,
                    maxDate:'',
                    minDate:'01/01/1970'
                }
            }


            window.mobileSearch.utils.toggleNavigation( false );
            Backbone.history.start();
        });

        } );
