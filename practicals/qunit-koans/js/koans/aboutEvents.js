

module('About Backbone.Events', {
    
    setup: function() {
        this.obj = {};
        _.extend(this.obj, Backbone.Events);
        this.obj.unbind(); // remove all custom events before each spec is run.
    }

});
    
    test('Can extend javascript objects to support custom events.', function() {

        expect(3);
        var basicObject = {};
        
        // How would you give basicObject these functions?
        // Hint: http://documentcloud.github.com/backbone/#Events
        
        _.extend(basicObject, Backbone.Events);

        equal(typeof basicObject.bind, 'function');
        equal(typeof basicObject.unbind, 'function');
        equal(typeof basicObject.trigger, 'function');
    });
    
    test('Allows us to bind and trigger custom named events on an object.', function() {
        
        expect(1);

        var callback = function(){
            ok('custom event callback called');
            start();
        }
        
        
        this.obj.bind('basic event', callback);

        stop();
        this.obj.trigger("basic event");
        // How would you cause the callback for this custom event to be called?
        
    });
    
    test('Also passes along any arguments to the callback when an event is triggered.', function() {

        expect(1);

        var passedArgs = [];
        
        this.obj.bind('some event', function() {
            for (var i = 0; i < arguments.length; i++) {
                passedArgs.push(arguments[i]);
            }
        });
        
        this.obj.trigger('some event', 'arg1', 'arg2');
        
        deepEqual(passedArgs, ['arg1', 'arg2']);
    });
    
    test('Can also bind the passed context to the event callback.', function() {

        expect(2);

        var foo = { color: 'blue' };
        
        var changeColor = function() {
            this.color = 'red';
            ok('color changed');
            start();
        }
        
        // How would you get 'this.color' to refer to 'foo' in the changeColor function?
        
        this.obj.bind('an event', changeColor, foo);
        
        stop();
        this.obj.trigger('an event');
        
        equal(foo.color, 'red');
    });
    
    test("Uses 'all' as a special event name to capture all events bound to the object.", function() {

        expect(2);

        var calledCount = 0;
        var callback = function(){
            ok('custom event callback called');
            calledCount +=1;
            start();
        }
        
        
        this.obj.bind('all', callback);
        
        stop();
        this.obj.trigger("custom event");

        equal(calledCount, 1);

    });
    
    test('Also can remove custom events from objects.', function() {

        expect(4);

        var spyCount1 = 0, 
            spyCount2 = 0, 
            spyCount3 = 0;

        var spy1 = function(){
            ok( true, 'spy 1');
            spyCount1 +=1;
            start();
        }
        var spy2 = function(){
            ok( true, 'spy 2');
            spyCount2+=1;
            start();
        }
        
        var spy3 = function(){
            ok( true, 'spy 1');
            spyCount3+=1;
            start();
        }

        this.obj.bind('foo', spy1);
        this.obj.bind('foo', spy2);
        this.obj.bind('foo', spy3);
        this.obj.bind('bar', spy1);
        
        // How do you unbind just a single callback for the event?

        this.obj.unbind('foo', spy1);

        stop();
        this.obj.trigger('foo');
        
        //equal(spyCount1, 1);
        
        // How do you unbind all callbacks tied to the event with a single method?
        
        this.obj.unbind('foo');

        //stop();
        this.obj.trigger('foo');
        
        equal(spyCount2, 1);
        equal(spyCount3, 1);
        
        // How do you unbind all callbacks and events tied to the object with a single method?
        
        this.obj.unbind('bar');

        //stop();
        this.obj.trigger('bar');
        
    
    });
    


