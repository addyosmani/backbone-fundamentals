define([
    "backbone", "modules/view/abstract/BasicView"],
    function (Backbone, BasicView) {
        var BasicPopup = BasicView.extend({
            role : 'popup',
            getTemplateID : function () {
                return "basic_popup";
            },
            enhanceJQMComponentsAPI : function () {
                $(this.el).popup("open");
            }
        });
        return BasicPopup;
    });