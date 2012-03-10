

module('About Backbone.View', {
    

    setup: function() {
        
        $('body').append('<ul id="todoList"></ul>');
        this.todoView = new TodoView({ model: new Todo() });
    },
    
    teardown: function() {
        this.todoView.remove();
        $('#todoList').remove();
    }

});
    
    test('Should be tied to a DOM element when created, based off the property provided.', function() {
        equal(this.todoView.el.tagName.toLowerCase(), 'li');
    });
    
    test('Is backed by a model instance, which provides the data.', function() {
        notEqual(this.todoView.model, undefined);
        equal(this.todoView.model.get('done'), false);
    });
    
    test('Can render, after which the DOM representation of the view will be visible.', function() {
       this.todoView.render();
       
        // Hint: render() just builds the DOM representation of the view, but doesn't insert it into the DOM.
        //       How would you append it to the ul#todoList? 
        //       How do you access the view's DOM representation?
        //
        // Hint: http://documentcloud.github.com/backbone/#View-el

        $('ul#todoList').append(this.todoView.el);
        equal($('#todoList').find('li').length, 1);
    });
    
    test('Can use an events hash to wire up view methods to DOM elements.', function() {
        var viewElt;
        
        //spyOn(todoView.model, 'toggle');
        
        
        $('#todoList').append(this.todoView.render().el);
        
        
        setTimeout(function() {
            viewElt = $('#todoList li input.check').filter(':first');
            equal(viewElt.length > 0, true);
        }, 1000);
        
        //runs(function() {
            // Hint: How would you trigger the view, via a DOM Event, to toggle the 'done' status?
            //       (See todos.js line 70, where the events hash is defined.)
            //
            // Hint: http://api.jquery.com/click
            //todoView.toggleDone();
            //expect(todoView.model.toggle).toHaveBeenCalled();
        //});
    });

