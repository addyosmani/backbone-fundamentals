define(['jquery', 'handlebars'], function ($, Handlebars) {
    var settings = {};
    settings.init = function () {
        Handlebars.templates = window.JST;
//        Handlebars.partials = Handlebars.templates;

        // All navigation that is relative should be passed through the navigate
        // method, to be processed by the router. If the link has a `data-bypass`
        // attribute, bypass the delegation completely.
        $(document).on("click", "a:not([data-bypass])", function (evt) {
            // Get the anchor href and protcol
            var href = $(this).attr("href");
            var protocol = this.protocol + "//";

            // Ensure the protocol is not part of URL, meaning it's relative.
            if (href && href.slice(0, protocol.length) !== protocol &&
                href.indexOf("javascript:") !== 0) {
                // Stop the default event to ensure the link will not cause a page
                // refresh.
                evt.preventDefault();

                // `Backbone.history.navigate` is sufficient for all Routers and will
                // trigger the correct events. The Router's internal `navigate` method
                // calls this anyways.
                Backbone.history.navigate(href, true);
            }
        });
    };
    return settings;
});
