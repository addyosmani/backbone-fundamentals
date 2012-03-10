

module('About Backbone.Collection');

    test('Can add Model instances as objects and arrays.', function() {

        expect(3);

        var todos = new TodoList();
        
        equal(todos.length, 0);
        
        todos.add({ text: 'Clean the kitchen' });

        console.log(todos);
        
        equal(todos.length, 1);
        
        todos.add([
            { text: 'Do the laundry', done: true }, 
            { text: 'Go to the gym'}
        ]);
        
        equal(todos.length, 3);
    });
    
    test('Can have a url property to define the basic url structure for all contained models.', function() {
        expect(1);
        var todos = new TodoList();
        equal(todos.url, '/todos/');
    });
    
    test('Fires custom named events when the models change.', function() {

        expect(2);
        var todos = new TodoList();
        
        var addModelCallback = function(){
            ok( true, 'add model callback called');  
            start(); 
        }

        stop();
        todos.bind('add', addModelCallback);
        
        // How would you get the 'add' event to trigger?
        todos.add({text:"New todo"});
        
        var removeModelCallback = function(){
            ok( true, 'remove model callback called');
            start();
        }

        stop();
        todos.bind('remove', removeModelCallback);
        
        // How would you get the 'remove' callback to trigger?
        todos.remove(todos.last());
        
    });



