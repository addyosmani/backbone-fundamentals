define(['jquery'], function ($) {

    $(document).on("mobileinit", function () {

        // disable hash-routing
        $.mobile.hashListeningEnabled = false;
// disable anchor-control
        $.mobile.linkBindingEnabled = false;
// can cause calling object creation twice and back button issues are solved
        $.mobile.ajaxEnabled = false;
// Otherwise on first page load, it needs to be a page present
     //   $.mobile.autoInitializePage = false;
// we want to handle caching and cleaning the DOM ourselves
        $.mobile.page.prototype.options.domCache = false;

// consider due to performance & compatibility issues
// not always supported by browsers
        $.mobile.pushStateEnabled = false;
        $.mobile.defaultPageTransition = "none";
        $.mobile.defaultDialogTransition = "none";
        $.mobile.page.prototype.options.degradeInputs.date = true;
// Solving phonegap issues with back button
        $.mobile.phonegapNavigationEnabled = true;


    });

});