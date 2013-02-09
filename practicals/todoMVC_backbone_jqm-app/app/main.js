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
    "modules/view/ExamplePage2",
    "modules/view/EditTodoPopup"
    //,"modules/view/EditTodoDialog"
],
//    jqValidationUnused
    function ($, Backbone, bbsuperUnused, _, Handlebars, initializeSettings, handlebarscompUnused, jqmUnused, TodosView, TodosCollection, Common, ExamplePage2,  EditTodoPopup) { // ExampleDialog
        initializeSettings.init();

        var Router = Backbone.Router.extend({
            routes : {
                "" : "index",
                "todo" : "index",
                "todo/:filter" : "filter",
                "editTodo/:id" : "editTodo",
                'pages/second' : 'secondPage',
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

                new EditTodoPopup({
                    model : TodosCollection.get(id)
                });
            },
            openDialog : function (todoCID) {
             //   new ExampleDialog({model : TodosCollection.getByCid(todoCID)});
            },
            secondPage : function () {
                new ExamplePage2();
            }
        });

        $(function () {
            new Router();
            Backbone.history.start({ pushState : false });
        });
    });
