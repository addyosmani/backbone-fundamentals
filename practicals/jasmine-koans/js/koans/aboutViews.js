describe('About Backbone.View', function() {
    var todoView;
    
    beforeEach(function() {
        $('body').append('<ul id="todoList"></ul>');
        todoView = new TodoView({ model: new Todo({ text: 'Stop monkeys from throwing their own crap!' }) });
    });
    
    afterEach(function() {
        todoView.remove();
        $('#todoList').remove();
    });
    
    it('Should be tied to a DOM element when created, based off the property provided.', function() {
        expect(todoView.el.tagName.toLowerCase()).toBe('what html element tag name represents this view?');
    });
    
    it('Is backed by a model instance, which provides the data.', function() {
        expect(todoView.model).toBeDefined();
        expect(todoView.model.get('done')).toBe("What's the value for Todo.get('done') here?");
    });
    
    it('Can render, after which the DOM representation of the view will be visible.', function() {
        todoView.render();
        
        // Hint: render() just builds the DOM representation of the view, but doesn't insert it into the DOM.
        //       How would you append it to the ul#todoList? 
        //       How do you access the view's DOM representation?
        //
        // Hint: http://documentcloud.github.com/backbone/#View-el
        
        expect($('#todoList').find('li').length).toBe(1);
    });
    
    it('Can use an events hash to wire up view methods to DOM elements.', function() {



         var todoEl = todoView.render().el;
         var todoList = $('#todoList');

         todoList.append(todoEl);
         var viewEl = todoList.find('li input.check');//.filter(':first');

         console.log(viewEl);

          

          expect(viewEl.length).toBeGreaterThan(0);
            

        runs(function() {
            // Hint: How would you trigger the view, via a DOM Event, to toggle the 'done' status.
            //       (See todos.js line 70, where the events hash is defined.)
            //
            // Hint: http://api.jquery.com/click
            viewEl.click();
            expect(todoView.model.get('done')).toBe(true);
        });
    });
});