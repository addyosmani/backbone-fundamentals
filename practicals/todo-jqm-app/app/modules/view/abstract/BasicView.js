define([
    "lodash",
    "backbone",
    "handlebars",
    "handlebars_helpers"
],

    function (_, Backbone, Handlebars) {
        var BasicView = Backbone.View.extend({
            initialize : function () {
                _.bindAll();
                this.render();
            },
            events : {
                "click #backButton" : "goBackInHistory"
            },
            role : "page",
            attributes : function () {
                return {
                    "data-role" : this.role
                };
            },
            getHeaderTitle : function () {
                return this.getSpecificTemplateValues().headerTitle;
            },
            getTemplateID : function () {
                return "basic_page_simple";
            },
            goBackInHistory : function(clickEvent){
                history.go(-1);
                return false;
            },
            getTemplateResult : function (templateDefinitionID, templateValues) {
                return window.JST[templateDefinitionID](templateValues);
            },
            getBasicPageTemplateResult : function () {
                var templateValues = {templatePartialPageID : this.id, headerTitle : this.getHeaderTitle()};
                $.extend(templateValues, this.getSpecificTemplateValues());
                return this.getTemplateResult(this.getTemplateID(), templateValues);
            },
            getRequestedPageTemplateResult : function () {
                this.getBasicPageTemplateResult();
            },
            render : function () {
                this.cleanupPossiblePageDuplicationInDOM();
                $(this.el).html(this.getBasicPageTemplateResult());
                this.addPageToDOMAndRenderJQM();
                this.enhanceJQMComponentsAPI();
            },
            enhanceJQMComponentsAPI : function () {
                $.mobile.changePage("#" + this.id, {
                    transition : 'none',
                    changeHash : false,
                    role : this.role
                });
            },
            addPageToDOMAndRenderJQM : function () {
                $("body").append($(this.el));
                $("#" + this.id).page();
            },
            cleanupPossiblePageDuplicationInDOM : function () {

                var $previousEl = $("#" + this.id);
                var isAlreadyInDom = $previousEl.length >= 0;
                if (isAlreadyInDom) {
                    // used because from-to page switch
                    // needs events at that point of time
                    // have in mind, that detach will not remove handlers
                    $previousEl.detach();
                }
            }
        });

        return BasicView;
    });
