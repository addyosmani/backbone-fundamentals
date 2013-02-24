require([
    "jquery",
    "backbone",
    "backbone_super",
    "lodash",
    "handlebars",
    "initialize.config",
    "handlebars.compiled",
    "jquerymobile",
    "modules/view/TodosView",
    "modules/TodosCollection",
    "common",
    "modules/view/EditTodoPage"
],
//    jqValidationUnused
    function ($, Backbone, bbsuperUnused, _, Handlebars, initializeSettings, handlebarscompUnused, jqmUnused, TodosView, TodosCollection, Common, EditTodoPage) { // ExampleDialog
        initializeSettings.init();

        var Router = Backbone.Router.extend({
            routes : {
                "" : "index",
                "todo" : "index",
                "todo/:filter" : "filter",
                "editTodo/:id" : "editTodo",
                '*path' : 'index'
            },
            index : function () {
                new TodosView();
            },
            filter : function (filter) {
                Common.TodoFilter = filter.trim() === 'all' ? '' : filter.trim() || '';
                TodosCollection.trigger('filter');
            },
            editTodo : function (id){
                new EditTodoPage({
                    model : TodosCollection.getByCid(id)
                });
            }
        });

        $(function () {
            window.BackboneRouter = new Router();
            Backbone.history.start({ pushState : false });
        });
    });
