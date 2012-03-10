

module('About Backbone Applications', {

    setup: function() {
        Backbone.localStorageDB = new Store('testTodos');
        $('#qunit-fixture').append('<div id="app"></div>');   
        this.App = new TodoApp({ appendTo: $('#app') });
    },
    
    teardown: function() {
        this.App.todos.reset();
        $('#app').remove();
    }

});
    
    test('Should bootstrap the application by initializing the Collection.', function() {
        expect(2);
        notEqual(this.App.todos, undefined);
        equal(this.App.todos.length, 0);
    });
    
    /*
    test('Should bind Collection events to View creation.', function() {
        $('#new-todo').val('Foo');
        $('#new-todo').trigger(new $.Event('keypress', { keyCode: 13 }));
        equal(this.App.todos.length, 1);
    });

*/