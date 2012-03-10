describe('About Backbone.Events', function() {
    
    var obj = {};
    _.extend(obj, Backbone.Events);
    
    beforeEach(function() {
        obj.unbind(); // remove all custom events before each spec is run.
    });
    
    it('Can extend javascript objects to support custom events.', function() {
        var basicObject = {};
        
        _.extend(basicObject, Backbone.Events);
        
        expect(basicObject.bind).not.toBeDefined();
        expect(basicObject.unbind).not.toBeDefined();
        expect(basicObject.trigger).not.toBeDefined();
    });
    
    it('Allows us to bind and trigger custom named events on an object.', function() {
        var callback = jasmine.createSpy();
        
        obj.bind('basic event', callback);
        
        // How would you cause the callback for this custom event to be called?
        
        expect(callback).toHaveBeenCalled();
    });
    
    it('Also passes along any arguments to the callback when an event is triggered.', function() {
        var passedArgs = [];
        
        obj.bind('some event', function() {
            for (var i = 0; i < arguments.length; i++) {
                passedArgs.push(arguments[i]);
            }
        });
        
        obj.trigger('some event', 1, 2);
        
        expect(passedArgs.length).toBe(3);
    });
    
    it('Can also bind the passed context to the event callback.', function() {
        var foo = { color: 'blue' };
        
        var changeColor = function() {
            this.color = 'red';
        }
        
        obj.bind('an event', changeColor, foo);
        
        obj.trigger('an event');
        
        expect(foo.color).toBe('blue');
    });
    
    it("Uses 'all' as a special event name to capture all events bound to the object.", function() {
        var callback = jasmine.createSpy();
        
        obj.bind('all', callback);
        
        obj.trigger('foo');
        obj.trigger('bar');
        
        expect(callback.callCount).toBe(0);
        expect(callback.mostRecentCall.args[0]).toBe(undefined);
    });
    
    it('Also can remove custom events from objects.', function() {
        var spy1 = jasmine.createSpy();
        var spy2 = jasmine.createSpy();
         
        obj.bind('foo', spy1);
        obj.bind('foo', spy2);
        obj.bind('bar', spy1);
        
        // We can unbind just a reference to a bound callback for the event.
        obj.unbind('foo', spy1);
        obj.trigger('foo');
        
        expect(spy1).toHaveBeenCalled();
        
        // We can unbind all callbacks tied to the event.
        obj.unbind('foo');
        obj.trigger('foo');
        
        expect(spy2.callCount).toBe(2);
        
        // We can unbind all events bound to the object.
        obj.unbind();
        obj.trigger('bar');
        
        expect(spy1).toHaveBeenCalled();
    });
    
});