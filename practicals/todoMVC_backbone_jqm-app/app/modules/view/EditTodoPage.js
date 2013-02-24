define([
    "backbone", "modules/view/abstract/BasicView"],
    function (Backbone, BasicView) {
        return BasicView.extend({
            id : "editTodoView",
            getHeaderTitle : function () {
                return "Edit Todo";
            },
            getSpecificTemplateValues : function () {
                return this.model.toJSON();
            },
            events : function () {
                return _.extend({
                    'click #saveDescription' : 'saveDescription'
                }, this.constructor.__super__.events);
            },
            saveDescription : function (clickEvent) {
//                this.model.save({
//                        title : $("#todoDescription", this.el).val()
//                    },
//                    {
//                        success : function (model, response) {
//                            window.BackboneRouter.navigate("#", {trigger : true});
//                        }
//                    }
//                );
//
//                clickEvent.preventDefault();
//                return false;
            }
        });
    });
