define([
    "backbone",
    "jquery",
    "lodash",
    "modules/view/abstract/BasicView",
    "modules/TodosCollection",
    "modules/view/SingleTodoView",
    "common"
],
    function (Backbone, $, _, BasicView, TodosCollection, SingleTodoView, Common) {
        var TodosView = BasicView.extend({
            id : "todosPage", // used for template
            initialize : function () {
                TodosCollection.unbind();
                TodosCollection.on('add', this.addOne, this);
                TodosCollection.on('reset', this.addAllTodos, this);
                TodosCollection.on('change:completed', this.filterOne, this);
                TodosCollection.on("filter", this.filterAll, this);
                TodosCollection.on('all', this.render, this);
                TodosCollection.fetch();
            },
            events : {
                'keypress #new-todo' : 'createOnEnter',
                'click #clear-completed' : 'clearCompleted',
                'change #toggle-all' : 'toggleAllComplete'
            },
            getSpecificTemplateValues : function () {
                return {
                    'completed' : TodosCollection.completed().length,
                    'remaining' : TodosCollection.remaining().length,
                    'isAnyOrOneRemaining' : TodosCollection.remaining().length > 1,
                    'isAnyCompleted' : TodosCollection.completed().length > 0
                };
            },
            render : function () {
                this.input = this.$('#new-todo');
                this.allCheckbox = this.$('#toggle-all')[0];
                this.$footer = this.$('#footer');
                var currentFooterHTML = Handlebars.partials['todosStatisticsAndBulkFooter'](this.getSpecificTemplateValues());
                this.$('#footer').html(currentFooterHTML).trigger("create");

                this.$main = this.$('#main');

                var completed = TodosCollection.completed().length;
                var remaining = TodosCollection.remaining().length;

                if (TodosCollection.length) {
                    this.$main.show();
                    this.$footer.show();

                    this.$('#filters li a')
                        .removeClass('selected')
                        .filter('[href="#/' + ( Common.TodoFilter || '' ) + '"]')
                        .addClass('selected');
                } else {
                    this.$main.hide();
                    this.$footer.hide();
                }
                this.allCheckbox.checked = !remaining;
            },
            addOne : function (todo) {
                var view = new SingleTodoView({ model : todo });
                $('#todo-list').append(view.render().el).trigger("create");
            },
            addAllTodos : function () {
                this._super("render", {});
                this.$('#todo-list').html('');
                TodosCollection.each(this.addOne, this);
            },
            filterOne : function (todo) {
                todo.trigger("visible");
            },
            filterAll : function () {
                TodosCollection.each(this.filterOne, this);
            },
            newAttributes : function () {
                return {
                    title : this.input.val().trim(),
                    order : TodosCollection.nextOrder(),
                    completed : false
                };
            },
            createOnEnter : function (e) {
                if (e.which !== Common.ENTER_KEY || !this.input.val().trim()) {
                    return;
                }
                TodosCollection.create(this.newAttributes());
                this.input.val('');
            },
            toggleAllComplete : function () {
                var wasNoRemainingBefore = TodosCollection.remaining().length > 0;

              TodosCollection.each(function (todo) {
                    todo.toggle();
                    todo.save({
                        'completed' : wasNoRemainingBefore
                    });
                });
                this.delegateEvents(this.events);
            },
            clearCompleted : function () {
                _.each(TodosCollection.completed(), function (todo) {
                    todo.destroy();
                });
            }
        });

        return TodosView;
    })
;
