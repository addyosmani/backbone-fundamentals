define([
    "backbone",
    "modules/view/abstract/BasicView",
    "lodash",
    "modules/TodosCollection"
],

    function (Backbone, BasicView, _, TodosCollection) {
        return BasicView.extend({
            id : "example_page_2",
            initialize : function(){
                TodosCollection.on('all', this.render, this);
                TodosCollection.fetch();
            },
            getSpecificTemplateValues : function () {
                return {
                    'completed' : TodosCollection.completed().length
                };
            },
            events : function () {
                return _.extend({
                    'click #clear-completed' : 'clearCompleted'
                }, this.constructor.__super__.events);
            },
            clearCompleted : function () {
                _.each(TodosCollection.completed(), function (todo) {
                    todo.destroy({silent : true});
                });

                //return true;
            }
        });
    });
