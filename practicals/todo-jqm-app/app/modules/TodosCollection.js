define([
    'lodash',
    'backbone',
    'localstorage',
    'modules/TodoModel'
], function (_, Backbone, Store, TodoModel) {

    var TodosCollection = Backbone.Collection.extend({
        model : TodoModel,
        localStorage : new Store('todos-backbone'),
        events : {

        },
        completed : function () {
            return this.filter(function (todo) {
                return todo.get('completed');
            });
        },
        remaining : function () {
            return this.without.apply(this, this.completed());
        },
        // We keep the Todos in sequential order, despite being saved by unordered
        // GUID in the database. This generates the next order number for new items.
        nextOrder : function () {
            if (!this.length) {
                return 1;
            }
            return this.last().get('order') + 1;
        },

        // Todos are sorted by their original insertion order.
        comparator : function (todo) {
            return todo.get('order');
        }
    });

    var instance = new TodosCollection();
    return instance;
});