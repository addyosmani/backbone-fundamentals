# Unit Testing

One definition of unit testing is the process of taking the smallest piece of testable code in an application, isolating it from the remainder of your codebase, and determining if it behaves exactly as expected.

For an application to be considered 'well-tested', each function should ideally have its own separate unit tests where it's tested against the different conditions you expect it to handle. All tests must pass before functionality is considered 'complete'. This allows developers to both modify a unit of code and its dependencies with a level of confidence about whether these changes have caused any breakage.

A basic example of unit testing is where a developer asserts that passing specific values to a sum function results in the correct value being returned. For an example more relevant to this book, we may wish to assert that adding a new Todo item to a list correctly adds a Model of a specific type to a Todos Collection.

When building modern web-applications, it's typically considered best-practice to include automated unit testing as a part of your development process. In the following chapters we are going to look at three different solutions for unit testing your Backbone.js apps - Jasmine, QUnit and SinonJS.

# Jasmine

## Behavior-Driven Development

In this section, we'll be taking a look at how to unit test Backbone applications using a popular JavaScript testing framework called [Jasmine](http://pivotal.github.com/jasmine/) from Pivotal Labs.

Jasmine describes itself as a behavior-driven development (BDD) framework for testing JavaScript code. Before we jump into how the framework works, it's useful to understand exactly what [BDD](http://en.wikipedia.org/wiki/Behavior_Driven_Development) is.

BDD is a second-generation testing approach first described by [Dan North](http://dannorth.net/introducing-bdd/) (the authority on BDD) which attempts to test the behavior of software. It's considered second-generation as it came out of merging ideas from Domain driven design (DDD) and lean software development. BDD helps teams deliver high-quality software by answering many of the more confusing questions early on in the agile process. Such questions commonly include those concerning documentation and testing.

If you were to read a book on BDD, it's likely that it would be described as being 'outside-in and pull-based'. The reason for this is that it borrows the idea of 'pulling features' from Lean manufacturing which effectively ensures that the right software solutions are being written by a) focusing on the expected outputs of the system and b) ensuring these outputs are achieved.

BDD recognizes that there are usually multiple stakeholders in a project and not a single amorphous user of the system. These different groups will be affected by the software being written in differing ways and will have varying opinions of what quality in the system means to them. It's for this reason that it's important to understand who the software will be bringing value to and exactly what in it will be valuable to them.

Finally, BDD relies on automation. Once you've defined the quality expected, your team will want to check on the functionality of the solution being built regularly and compare it to the results they expect. In order to facilitate this efficiently, the process has to be automated. BDD relies heavily on the automation of specification-testing and Jasmine is a tool which can assist with this.

BDD helps both developers and non-technical stakeholders:


* Better understand and represent the models of the problems being solved
* Explain supported test cases in a language that non-developers can read
* Focus on minimizing translation of the technical code being written and the domain language spoken by the business

What this means is that developers should be able to show Jasmine unit tests to a project stakeholder and (at a high level, thanks to a common vocabulary being used) they'll ideally be able to understand what the code supports.

Developers often implement BDD in unison with another testing paradigm known as [TDD](http://en.wikipedia.org/wiki/Test-driven_development) (test-driven development). The main idea behind TDD is using the following development process:

1. Write unit tests which describe the functionality you would like your code to support
2. Watch these tests fail (as the code to support them hasn't yet been written)
3. Write code to make the tests pass
4. Rinse, repeat, and refactor

In this chapter we're going to use BDD (with TDD) to write unit tests for a Backbone application.

***Note:*** I've seen a lot of developers also opt for writing tests to validate behavior of their code after having written it. While this is fine, note that it can come with pitfalls such as only testing for behavior your code currently supports, rather than the behavior needed to fully solve the problem.


## Suites, Specs, & Spies

When using Jasmine, you'll be writing suites and specifications (specs). Suites basically describe scenarios while specs describe what can be done in these scenarios.

Each spec is a JavaScript function, described with a call to ```it()``` using a description string and a function. The description should describe the behaviour the particular unit of code should exhibit and, keeping in mind BDD, it should ideally be meaningful. Here's an example of a basic spec:

```javascript
it('should be incrementing in value', function(){
    var counter = 0;
    counter++;
});
```

On its own, a spec isn't particularly useful until expectations are set about the behavior of the code. Expectations in specs are defined using the ```expect()``` function and an [expectation matcher](https://github.com/pivotal/jasmine/wiki/Matchers) (e.g., ```toEqual()```, ```toBeTruthy()```, ```toContain()```). A revised example using an expectation matcher would look like:

```javascript
it('should be incrementing in value', function(){
    var counter = 0;
    counter++;
    expect(counter).toEqual(1);
});
```

The above code passes our behavioral expectation as ```counter``` equals 1. Notice how easy it was to read the expectation on the last line (you probably grokked it without any explanation).

Specs are grouped into suites which we describe using Jasmine's ```describe()``` function, again passing a string as a description and a function as we did for ```it()```. The name/description for your suite is typically that of the component or module you're testing.

Jasmine will use the description as the group name when it reports the results of the specs you've asked it to run. A simple suite containing our sample spec could look like:

```javascript
describe('Stats', function(){
    it('can increment a number', function(){
        ...
    });

    it('can subtract a number', function(){
        ...
    });
});
```

Suites also share a functional scope, so it's possible to declare variables and functions inside a describe block which are accessible within specs:

```javascript
describe('Stats', function(){
    var counter = 1;

    it('can increment a number', function(){
        // the counter was = 1
        counter = counter + 1;
        expect(counter).toEqual(2);
    });

    it('can subtract a number', function(){
        // the counter was = 2
        counter = counter - 1;
        expect(counter).toEqual(1);
    });
});
```

***Note:*** Suites are executed in the order in which they are described, which can be useful to know if you would prefer to see test results for specific parts of your application reported first.

Jasmine also supports **spies** - a way to mock, spy, and fake behavior in our unit tests. Spies replace the function they're spying on, allowing us to simulate behavior we would like to mock (i.e., test without using the actual implementation).

In the example below, we're spying on the ```setComplete``` method of a dummy Todo function to test that arguments can be passed to it as expected.

```javascript
var Todo = function(){
};

Todo.prototype.setComplete = function (arg){
    return arg;
}

describe('a simple spy', function(){
    it('should spy on an instance method of a Todo', function(){
        var myTodo = new Todo();
        spyOn(myTodo, 'setComplete');
        myTodo.setComplete('foo bar');

        expect(myTodo.setComplete).toHaveBeenCalledWith('foo bar');

        var myTodo2 = new Todo();
        spyOn(myTodo2, 'setComplete');

        expect(myTodo2.setComplete).not.toHaveBeenCalled();

    });
});
```

You are more likely to use spies for testing [asynchronous](http://en.wikipedia.org/wiki/Asynchronous_communication) behavior in your application such as AJAX requests. Jasmine supports:

* Writing tests which can mock AJAX requests using spies. This allows us to test both the code that initiates the AJAX request and the code executed upon its completion. It's also possible to mock/fake the server responses. The benefit of this type of testing is that it's faster as no real calls are being made to a server. The ability to simulate any response from the server is also of great benefit.
* Asynchronous tests which don't rely on spies

This example of the first kind of test shows how to fake an AJAX request and verify that the request was both calling the correct URL and executed a callback where one was provided.

```javascript
it('the callback should be executed on success', function () {

    // `.and.callFake()` calls a passed function when a spy
    // has been called
    spyOn($, 'ajax').and.callFake(function(options) {
        options.success();
    });

    // Create a new spy
    var callback = jasmine.createSpy();

    // Exexute the spy callback if the
    // request for Todo 15 is successful
    getTodo(15, callback);

    // Verify that the URL of the most recent call
    // matches our expected Todo item.
    expect($.ajax.calls.mostRecent().args[0]['url']).toEqual('/todos/15');

    // `expect(x).toHaveBeenCalled()` will pass if `x` is a
    // spy and was called.
    expect(callback).toHaveBeenCalled();
});

function getTodo(id, callback) {
    $.ajax({
        type: 'GET',
        url: '/todos/' + id,
        dataType: 'json',
        success: callback
    });
}
```

All of these are Spy-specific matchers and are documented on the Jasmine [wiki](https://github.com/pivotal/jasmine/wiki/Spies).

For the second type of test (asynchronous tests), we can take the above further by taking advantage of three other methods Jasmine supports:

* [waits(timeout)](https://github.com/pivotal/jasmine/wiki/Asynchronous-specs) - a native timeout before the next block is run
* [waitsFor(function, optional message, optional timeout)](https://github.com/pivotal/jasmine/wiki/Asynchronous-specs) - a way to pause specs until some other work has completed. Jasmine waits until the supplied function returns true here before it moves on to the next block.
* [runs(function)](https://github.com/pivotal/jasmine/wiki/Asynchronous-specs) - a block which runs as if it was directly called. They exist so that we can test asynchronous processes. 

```javascript
it('should make an actual AJAX request to a server', function () {

    // Create a new spy
    var callback = jasmine.createSpy();

    // Exexute the spy callback if the
    // request for Todo 16 is successful
    getTodo(16, callback);

    // Pause the spec until the callback count is
    // greater than 0
    waitsFor(function() {
        return callback.callCount > 0;
    });

    // Once the wait is complete, our runs() block
    // will check to ensure our spy callback has been
    // called
    runs(function() {
        expect(callback).toHaveBeenCalled();
    });
});

function getTodo(id, callback) {
    $.ajax({
        type: 'GET',
        url: 'todos.json',
        dataType: 'json',
        success: callback
    });
}
```

***Note:*** It's useful to remember that when making real requests to a web server in your unit tests, this has the potential to massively slow down the speed at which tests run (due to many factors including server latency). As this also introduces an external dependency that can (and should) be minimized in your unit testing, it is strongly recommended that you opt for spies to remove the dependency on a web server.

## beforeEach() and afterEach()

Jasmine also supports specifying code that can be run before each (```beforeEach()```) and after each (```afterEach()```) test. This is useful for enforcing consistent conditions (such as resetting variables that may be required by specs). In the following example, ```beforeEach()``` is used to create a new sample Todo model which specs can use for testing attributes.

```javascript
beforeEach(function(){
   this.todo = new Backbone.Model({
      text: 'Buy some more groceries',
      done: false
   });
});

it('should contain a text value if not the default value', function(){
   expect(this.todo.get('text')).toEqual('Buy some more groceries');
});
```

Each nested ```describe()``` in your tests can have their own ```beforeEach()``` and ```afterEach()``` methods which support including setup and teardown methods relevant to a particular suite. 




`beforeEach()` and `afterEach()` can be used together to write tests verifying that our Backbone routes are being correctly triggered when we navigate to the URL. We can start with the `index` action:

```javascript
describe('Todo routes', function(){

   beforeEach(function(){

        // Create a new router
        this.router = new App.TodoRouter();

        // Create a new spy
        this.routerSpy = jasmine.spy();

        // Begin monitoring hashchange events
        try{
            Backbone.history.start({
                silent:true,
                pushState: true
            });
        }catch(e){
           // ...
        }

        // Navigate to a URL
        this.router.navigate('/js/spec/SpecRunner.html');
   }); 

   afterEach(function(){

        // Navigate back to the URL
        this.router.navigate('/js/spec/SpecRunner.html');

        // Disable Backbone.history temporarily.
        // Note that this is not really useful in real apps but is
        // good for testing routers
        Backbone.history.stop();
   });

   it('should call the index route correctly', function(){
        this.router.bind('route:index', this.routerSpy, this);
        this.router.navigate('', {trigger: true});

        // If everything in our beforeEach() and afterEach()
        // calls have been correctly executed, the following
        // should now pass.
        expect(this.routerSpy).toHaveBeenCalledOnce();
        expect(this.routerSpy).toHaveBeenCalledWith();
   });

});
```

The actual TodoRouter for that would make the above test pass looks like:

```javascript
var App = App || {};
App.TodoRouter = Backbone.Router.extend({
    routes:{
        '': 'index'
    },
    index: function(){
        //...
    }
});
```

## Shared scope

Let's imagine we have a Suite where we wish to check for the existence of a new Todo item instance. This could be done by duplicating the spec as follows:

```javascript
describe("Todo tests", function(){
   
   // Spec
   it("Should be defined when we create it", function(){
        // A Todo item we are testing
        var todo = new Todo("Get the milk", "Tuesday");
        expect(todo).toBeDefined();
   }); 

   it("Should have the correct title", function(){
        // Where we introduce code duplication
        var todo = new Todo("Get the milk", "Tuesday");
        expect(todo.title).toBe("Get the milk");
   });

});
```

As you can see, we've introduced duplication that should ideally be refactored into something cleaner. We can do this using Jasmine's Suite (Shared) Functional Scope.

All of the specs within the same Suite share the same functional scope, meaning that variables declared within the Suite itself are available to all of the Specs in that suite. This gives us a way to work around our duplication problem by moving the creation of our Todo objects into the common functional scope:

```javascript
describe("Todo tests", function(){
    
    // The instance of Todo, the object we wish to test
    // is now in the shared functional scope
    var todo = new Todo("Get the milk", "Tuesday");

    // Spec
    it("should be correctly defined", function(){
        expect(todo).toBeDefined();
    });

    it("should have the correct title", function(){
        expect(todo.title).toBe("Get the milk");
    });

});
```

In the previous section you may have noticed that we initially declared ```this.todo``` within the scope of our ```beforeEach()``` call and were then able to continue using this reference in ```afterEach()```. 

This is again down to shared function scope, which allows such declarations to be common to all blocks (including ```runs()```). 

Variables declared outside of the shared scope (i.e within the local scope `var todo=...`) will however not be shared.

## Getting set up

Now that we've reviewed some fundamentals, let's go through downloading Jasmine and getting everything set up to write tests.

A standalone release of Jasmine can be [downloaded](https://github.com/pivotal/jasmine/releases/) from the official release page.

You'll need a file called SpecRunner.html in addition to the release. It can be downloaded from https://github.com/pivotal/jasmine/tree/master/lib/jasmine-core/example or as part of a download of the complete Jasmine [repo](https://github.com/pivotal/jasmine/zipball/master). Alternatively, you can ```git clone``` the main Jasmine repository from https://github.com/pivotal/jasmine.git.

Let's review [SpecRunner.html.jst](https://github.com/pivotal/jasmine/blob/master/lib/templates/SpecRunner.html.jst):

It first includes both Jasmine and the necessary CSS required for reporting:

    <link rel="stylesheet" type="text/css" href="lib/jasmine-<%= jasmineVersion %>/jasmine.css">
    <script src="lib/jasmine-<%= jasmineVersion %>/jasmine.js"></script>
    <script src="lib/jasmine-<%= jasmineVersion %>/jasmine-html.js"></script>
    <script src="lib/jasmine-<%= jasmineVersion %>/boot.js"></script>

Next come the sources being tested:

    <!-- include source files here... -->
    <script src="src/Player.js"></script>
    <script src="src/Song.js"></script>

Finally, some sample tests are included:

    <!-- include spec files here... -->
    <script src="spec/SpecHelper.js"></script>
    <script src="spec/PlayerSpec.js"></script>


***Note:*** Below this section of SpecRunner is code responsible for running the actual tests. Given that we won't be covering modifying this code, I'm going to skip reviewing it. I do however encourage you to take a look through [PlayerSpec.js](https://github.com/pivotal/jasmine/blob/master/lib/jasmine-core/example/spec/PlayerSpec.js) and [SpecHelper.js](https://github.com/pivotal/jasmine/blob/master/lib/jasmine-core/example/spec/SpecHelper.js). They're a useful basic example to go through how a minimal set of tests might work.

Also note that for the purposes of introduction, some of the examples in this section will be testing aspects of Backbone.js itself, just to give you a feel for how Jasmine works. You generally will not need to write testing ensuring a framework is working as expected.


## TDD With Backbone

When developing applications with Backbone, it can be necessary to test both individual modules of code as well as models, views, collections, and routers. Taking a TDD approach to testing, let's review some specs for testing these Backbone components using the popular Backbone [Todo](https://github.com/addyosmani/todomvc/tree/master/todo-example/backbone) application. 

## Models

The complexity of Backbone models can vary greatly depending on what your application is trying to achieve. In the following example, we're going to test default values, attributes, state changes, and validation rules.

First, we begin our suite for model testing using ```describe()```:

```javascript
describe('Tests for Todo', function() {
```

Models should ideally have default values for attributes. This helps ensure that when creating instances without a value set for any specific attribute, a default one (e.g., an empty string) is used instead. The idea here is to allow your application to interact with models without any unexpected behavior.

In the following spec, we create a new Todo without any attributes passed then check to find out what the value of the ```text``` attribute is. As no value has been set, we expect a default value of `''` to be returned.

```javascript
it('Can be created with default values for its attributes.', function() {
    var todo = new Todo();
    expect(todo.get('text')).toBe('');
});
```

If testing this spec before your models have been written, you'll incur a failing test, as expected. What's required for the spec to pass is a default value for the attribute ```text```. We can set this and some other useful defaults (which we'll be using shortly) in our Todo model as follows:

```javascript
window.Todo = Backbone.Model.extend({

    defaults: {
      text: '',
      done:  false,
      order: 0
    }
```

Next, it is common to include validation logic in your models to ensure that input passed from users or other modules in the application are valid.

A Todo app may wish to validate the text input supplied in case it contains rude words. Similarly if we're storing the ```done``` state of a Todo item using booleans, we need to validate that truthy/falsy values are passed and not just any arbitrary string.

In the following spec, we take advantage of the fact that validations which fail model.validate() trigger an "invalid" event. This allows us to test if validations are correctly failing when invalid input is supplied.

We create an errorCallback spy using Jasmine's built in ```createSpy()``` method which allows us to spy on the invalid event as follows:

```javascript
it('Can contain custom validation rules, and will trigger an invalid event on failed validation.', function() {

    var errorCallback = jasmine.createSpy('-invalid event callback-');

    var todo = new Todo();

    todo.on('invalid', errorCallback);

    // What would you need to set on the todo properties to
    // cause validation to fail?

    todo.set({done:'a non-boolean value'});

    var errorArgs = errorCallback.mostRecentCall.args;

    expect(errorArgs).toBeDefined();
    expect(errorArgs[0]).toBe(todo);
    expect(errorArgs[1]).toBe('Todo.done must be a boolean value.');
});

```

The code to make the above failing test support validation is relatively simple. In our model, we override the validate() method (as recommended in the Backbone docs), checking to make sure a model both has a 'done' property and that its value is a valid boolean before allowing it to pass.

```javascript
validate: function(attrs) {
    if (attrs.hasOwnProperty('done') && !_.isBoolean(attrs.done)) {
        return 'Todo.done must be a boolean value.';
    }
}
```

If you would like to review the final code for our Todo model, you can find it below:

```javascript

window.Todo = Backbone.Model.extend({

    defaults: {
      text: '',
      done:  false,
      order: 0
    },

    initialize: function() {
        this.set({text: this.get('text')}, {silent: true});
    },

    validate: function(attrs) {
        if (attrs.hasOwnProperty('done') && !_.isBoolean(attrs.done)) {
            return 'Todo.done must be a boolean value.';
        }
    },

    toggle: function() {
        this.save({done: !this.get('done')});
    }

});
```


## Collections

We now need to define specs to test a Backbone collection of Todo models (a TodoList). Collections are responsible for a number of list tasks including managing order and filtering.

A few specific specs that come to mind when working with collections are:

* Making sure we can add new Todo models as both objects and arrays
* Attribute testing to make sure attributes such as the base URL of the collection are values we expect
* Purposefully adding items with a status of ```done:true``` and checking against how many items the collection thinks have been completed vs. those that are remaining

In this section we're going to cover the first two of these with the third left as an extended exercise you can try on your own.

Testing that Todo models can be added to a collection as objects or arrays is relatively trivial. First, we initialize a new TodoList collection and check to make sure its length (i.e., the number of Todo models it contains) is 0. Next, we add new Todos, both as objects and arrays, checking the length property of the collection at each stage to ensure the overall count is what we expect:

```javascript
describe('Tests for TodoList', function() {

    it('Can add Model instances as objects and arrays.', function() {
        var todos = new TodoList();

        expect(todos.length).toBe(0);

        todos.add({ text: 'Clean the kitchen' });

        // how many todos have been added so far?
        expect(todos.length).toBe(1);

        todos.add([
            { text: 'Do the laundry', done: true },
            { text: 'Go to the gym'}
        ]);

        // how many are there in total now?
        expect(todos.length).toBe(3);
    });
...
```

Similar to model attributes, it's also quite straight-forward to test attributes in collections. Here we have a spec that ensures the collection url (i.e., the url reference to the collection's location on the server) is what we expect it to be:

```javascript
it('Can have a url property to define the basic url structure for all contained models.', function() {
        var todos = new TodoList();

        // what has been specified as the url base in our model?
        expect(todos.url).toBe('/todos/');
});

```

For the third spec (which you will write as an exercise), note that the implementation for our collection will have methods for filtering how many Todo items are done and how many are remaining - we'll call these ```done()``` and ```remaining()```. Consider writing a spec which creates a new collection and adds one new model that has a preset ```done``` state of ```true``` and two others that have the default ```done``` state of ```false```. Testing the length of what's returned using ```done()``` and ```remaining()``` will tell us whether the state management in our application is working or needs a little tweaking.

The final implementation for our TodoList collection can be found below:


```javascript
 window.TodoList = Backbone.Collection.extend({

        model: Todo,

        url: '/todos/',

        done: function() {
            return this.filter(function(todo) { return todo.get('done'); });
        },

        remaining: function() {
            return this.without.apply(this, this.done());
        },

        nextOrder: function() {
            if (!this.length) {
                return 1;
            }

            return this.last().get('order') + 1;
        },

        comparator: function(todo) {
            return todo.get('order');
        }

    });
```


## Views

Before we take a look at testing Backbone views, let's briefly review a jQuery plugin that can assist with writing Jasmine specs for them.

**The Jasmine jQuery Plugin**

As we know our Todo application will be using jQuery for DOM manipulation, there's a useful jQuery plugin called [jasmine-jquery](https://github.com/velesin/jasmine-jquery) we can use to help simplify BDD testing of the rendering performed by our views.

The plugin provides a number of additional Jasmine [matchers](https://github.com/pivotal/jasmine/wiki/Matchers) to help test jQuery-wrapped sets such as:

* ```toBe(jQuerySelector)``` e.g., ```expect($('<div id="some-id"></div>')).toBe('div#some-id')```
* ```toBeChecked()``` e.g., ```expect($('<input type="checkbox" checked="checked"/>')).toBeChecked()```
* ```toBeSelected()``` e.g., ```expect($('<option selected="selected"></option>')).toBeSelected()```

and [many others](https://github.com/velesin/jasmine-jquery). The complete list of matchers supported can be found on the project homepage. It's useful to know that similar to the standard Jasmine matchers, the custom matchers above can be inverted using the .not prefix (i.e ```expect(x).not.toBe(y)```):

```javascript
expect($('<div>I am an example</div>')).not.toHaveText(/other/)
```

jasmine-jquery also includes a fixtures module that can be used to load arbitrary HTML content we wish to use in our tests. Fixtures can be used as follows:

Include some HTML in an external fixtures file:

some.fixture.html:
```<div id="sample-fixture">some HTML content</div>```

Then inside our actual test we would load it as follows:

```javascript
loadFixtures('some.fixture.html')
$('some-fixture').myTestedPlugin();
expect($('#some-fixture')).to<the rest of your matcher would go here>
```

The jasmine-jquery plugin loads fixtures from a directory named spec/javascripts/fixtures by default. If you wish to configure this path you can do so by initially setting ```jasmine.getFixtures().fixturesPath = 'your custom path'```.

Finally, jasmine-jquery includes support for spying on jQuery events without the need for any extra plumbing work. This can be done using the ```spyOnEvent()``` and ```expect(eventName).toHaveBeenTriggeredOn(selector)``` functions. For example:

```javascript
spyOnEvent($('#el'), 'click');
$('#el').click();
expect('click').toHaveBeenTriggeredOn($('#el'));
```

### View testing

In this section we will review the three dimensions of specs writing for Backbone Views: initial setup, view rendering, and templating. The latter two of these are the most commonly tested, however we'll see shortly why writing specs for the initialization of your views can also be of benefit.

#### Initial setup

At their most basic, specs for Backbone views should validate that they are being correctly tied to specific DOM elements and are backed by valid data models. The reason to consider doing this is that these specs can identify issues which will trip up more complex tests later on. Also, they're fairly simple to write given the overall value offered.

To help ensure a consistent testing setup for our specs, we use ```beforeEach()``` to append both an empty ```<ul>``` (#todoList) to the DOM and initialize a new instance of a TodoView using an empty Todo model. ```afterEach()``` is used to remove the previous #todoList  ```<ul>``` as well as the previous instance of the view.

```javascript
describe('Tests for TodoView', function() {

    beforeEach(function() {
        $('body').append('<ul id="todoList"></ul>');
        this.todoView = new TodoView({ model: new Todo() });
    });


    afterEach(function() {
        this.todoView.remove();
        $('#todoList').remove();
    });

...
```

The first spec useful to write is a check that the TodoView we've created is using the correct ```tagName``` (element or className). The purpose of this test is to make sure it's been correctly tied to a DOM element when it was created.

Backbone views typically create empty DOM elements once initialized, however these elements are not attached to the visible DOM in order to allow them to be constructed without an impact on the performance of rendering.

```javascript
it('Should be tied to a DOM element when created, based off the property provided.', function() {
    //what html element tag name represents this view?
    expect(this.todoView.el.tagName.toLowerCase()).toBe('li');
});
```

Once again, if the TodoView has not already been written, we will experience failing specs. Thankfully, solving this is as simple as creating a new Backbone.View with a specific ```tagName```.

```javascript
var todoView = Backbone.View.extend({
    tagName:  'li'
});
```

If instead of testing against the ```tagName``` you would prefer to use a className instead, we can take advantage of jasmine-jquery's ```toHaveClass()``` matcher:

```
it('Should have a class of "todos"', function(){
   expect(this.todoView.$el).toHaveClass('todos');
});
```

The ```toHaveClass()``` matcher operates on jQuery objects and if the plugin hadn't been used, an exception would have been thrown. It is of course also possible to test for the className by accessing el.className if you don't use jasmine-jquery.

You may have noticed that in ```beforeEach()```, we passed our view an initial (albeit unfilled) Todo model. Views should be backed by a model instance which provides data. As this is quite important to our view's ability to function, we can write a spec to ensure a model is defined (using the ```toBeDefined()``` matcher) and then test attributes of the model to ensure defaults both exist and are the values we expect them to be.

```javascript
it('Is backed by a model instance, which provides the data.', function() {

    expect(this.todoView.model).toBeDefined();

    // what's the value for Todo.get('done') here?
    expect(this.todoView.model.get('done')).toBe(false); //or toBeFalsy()
});
```

#### View rendering


Next we're going to take a look at writing specs for view rendering. Specifically, we want to test that our TodoView elements are actually rendering as expected.

In smaller applications, those new to BDD might argue that visual confirmation of view rendering could replace unit testing of views. The reality is that when dealing with applications that might grow to a large number of views, it makes sense to automate this process as much as possible from the get-go. There are also aspects of rendering that require verification beyond what is visually presented on-screen (which we'll see very shortly).

We're going to begin testing views by writing two specs. The first spec will check that the view's ```render()``` method is correctly returning the view instance, which is necessary for chaining. Our second spec will check that the HTML produced is exactly what we expect based on the properties of the model instance that's been associated with our TodoView.

Unlike some of the previous specs we've covered, this section will make greater use of ```beforeEach()``` to both demonstrate how to use nested suites and also ensure a consistent set of conditions for our specs. In our first example we're simply going to create a sample model (based on Todo) and instantiate a TodoView with it.

```javascript
describe('TodoView', function() {

  beforeEach(function() {
    this.model = new Backbone.Model({
      text: 'My Todo',
      order: 1,
      done: false
    });
    this.view = new TodoView({model:this.model});
  });

  describe('Rendering', function() {

    it('returns the view object', function() {
      expect(this.view.render()).toEqual(this.view);
    });

    it('produces the correct HTML', function() {
      this.view.render();

      // let's use jasmine-jquery's toContain() to avoid
      // testing for the complete content of a todo's markup
      expect(this.view.el.innerHTML)
        .toContain('<label class="todo-content">My Todo</label>');
    });

  });

});
```


When these specs are run, only the second one ('produces the correct HTML') fails. Our first spec ('returns the view object'), which is testing that the TodoView instance is returned from ```render()```, passes since this is Backbone's default behavior and we haven't overwritten the ```render()``` method with our own version yet.

**Note:** For the purposes of maintaining readability, all template examples in this section will use a minimal version of the following Todo view template. As it's relatively trivial to expand this, please feel free to refer to this sample if needed:



	<div class="todo <%= done ? 'done' : '' %>">
	        <div class="display">
	          <input class="check" type="checkbox" <%= done ? 'checked="checked"' : '' %> />
	          <label class="todo-content"><%= text %></label>
	          <span class="todo-destroy"></span>
	        </div>
	        <div class="edit">
	          <input class="todo-input" type="text" value="<%= content %>" />
	        </div>
	</div>



The second spec fails with the following message:

```Expected '' to contain '<label class="todo-content">My Todo</label>'.```

The reason for this is the default behavior for render() doesn't create any markup. Let's write a replacement for render() which fixes this:

```javascript
render: function() {
  var template = '<label class="todo-content">+++PLACEHOLDER+++</label>';
  var output = template
    .replace('+++PLACEHOLDER+++', this.model.get('text'));
  this.$el.html(output);
  return this;
}
```

The above specifies an inline string template and replaces fields found in the template within the "+++PLACEHOLDER+++" blocks with their corresponding values from the associated model. As we're also returning the TodoView instance from the method, the first spec will still pass.

It would be impossible to discuss unit testing without mentioning fixtures. Fixtures typically contain test data (e.g., HTML) that is loaded in when needed (either locally or from an external file) for unit testing. So far we've been establishing jQuery expectations based on the view's el property. This works for a number of cases, however, there are instances where it may be necessary to render markup into the document. The most optimal way to handle this within specs is through using fixtures (another feature brought to us by the jasmine-jquery plugin).

Re-writing the last spec to use fixtures would look as follows:


```javascript
describe('TodoView', function() {

  beforeEach(function() {
    ...
    setFixtures('<ul class="todos"></ul>');
  });

  ...

  describe('Template', function() {

    beforeEach(function() {
      $('.todos').append(this.view.render().el);
    });

    it('has the correct text content', function() {
      expect($('.todos').find('.todo-content'))
        .toHaveText('My Todo');
    });

  });

});
```

What we're now doing in the above spec is appending the rendered todo item into the fixture. We then set expectations against the fixture, which may be something desirable when a view is setup against an element which already exists in the DOM. It would be necessary to provide both the fixture and test the ```el``` property correctly picking up the element expected when the view is instantiated.


#### Rendering with a templating system


When a user marks a Todo item as complete (done), we may wish to provide them with visual feedback (such as a striked line through the text) to differentiate the item from those that are remaining. This can be done by attaching a new class to the item. Let's begin by writing a test:


```javascript
describe('When a todo is done', function() {

  beforeEach(function() {
    this.model.set({done: true}, {silent: true});
    $('.todos').append(this.view.render().el);
  });

  it('has a done class', function() {
    expect($('.todos .todo-content:first-child'))
      .toHaveClass('done');
  });

});
```

This will fail with the following message:

```Expected '<label class="todo-content">My Todo</label>'
to have class 'done'.
```

which can be fixed in the existing render() method as follows:


```javascript
render: function() {
  var template = '<label class="todo-content">' +
    '<%= text %></label>';
  var output = template
    .replace('<%= text %>', this.model.get('text'));
  this.$el.html(output);
  if (this.model.get('done')) {
    this.$('.todo-content').addClass('done');
  }
  return this;
}
```


However, this can get unwieldy fairly quickly. As the level of complexity and logic in our templates increase, so do the challenges associated with testing them. We can ease this process by taking advantage of modern templating libraries, many of which have already been demonstrated to work well with testing solutions such as Jasmine.

JavaScript templating systems (such as [Handlebars](http://handlebarsjs.com/), [Mustache](http://mustache.github.com/), and Underscore's own [micro-templating](http://underscorejs.org/#template)) support conditional logic in template strings. What this effectively means is that we can add if/else/ternery expressions inline which can then be evaluated as needed, allowing us to build even more powerful templates.

In our case, we are going to use the micro-templating found in Underscore.js as no additional files are required to use it and we can easily modify our existing specs to use it without a great deal of effort.

Assuming our template is defined using a script tag of ID `myTemplate`:

```
<script type="text/template" id="myTemplate">
    <div class="todo <%= done ? 'done' : '' %>">
            <div class="display">
              <input class="check" type="checkbox" <%= done ? 'checked="checked"' : '' %> />
              <label class="todo-content"><%= text %></label>
              <span class="todo-destroy"></span>
            </div>
            <div class="edit">
              <input class="todo-input" type="text" value="<%= content %>" />
            </div>
    </div>
</script>
```

Our TodoView can be modified to use Underscore templating as follows:

```javascript
var TodoView = Backbone.View.extend({

  tagName: 'li',
  template: _.template($('#myTemplate').html()),

  initialize: function(options) {
    // ...
  },

  render: function() {
    this.$el.html(this.template(this.model.attributes));
    return this;
  },

  ...

});
```

So, what's going on here? We're first defining our template in a script tag with a custom script type (e.g., type="text/template"). As this isn't a script type any browser understands, it's simply ignored, however referencing the script by an id attribute allows the template to be kept separate to other parts of the page.

In our view, we're the using the Underscore `_.template()` method to compile our template into a function that we can easily pass model data to later on. In the line `this.model.toJSON()` we are simply returning a copy of the model's attributes for JSON stringification to the `template` method, creating a block of HTML that can now be appended to the DOM.

Note: Ideally all of your template logic should exist outside of your specs, either in individual template files or embedded using script tags within your SpecRunner. This is generally more maintainable.

If you are working with much smaller templates and are not doing this, there is however a useful trick that can be applied to automatically create or extend templates in the Jasmine shared functional scope for each test. 

By creating a new directory (say, 'templates') in the 'spec' folder and including a new script file with the following contents into SpecRunner.html, we can manually add custom attributes representing smaller templates we wish to use:

```javascript
beforeEach(function() {
  this.templates = _.extend(this.templates || {}, {
    todo: '<label class="todo-content">' +
            '<%= text %>' +
          '</label>'
  });
});
```

To finish this off, we simply update our existing spec to reference the template when instantiating the TodoView:

```javascript
describe('TodoView', function() {

  beforeEach(function() {
    ...
    this.view = new TodoView({
      model: this.model,
      template: this.templates.todo
    });
  });

  ...

});
```

The existing specs we've looked at would continue to pass using this approach, leaving us free to adjust the template with some additional conditional logic for Todos with a status of 'done':

```javascript
beforeEach(function() {
  this.templates = _.extend(this.templates || {}, {
    todo: '<label class="todo-content <%= done ? 'done' : '' %>"' +
            '<%= text %>' +
          '</label>'
  });
});
```

This will now also pass without any issues, however as mentioned, this last approach probably only makes sense if you're working with smaller, highly dynamic templates. 


## Conclusions

We have now covered how to write Jasmine tests for Backbone.js models, collections, and views. While testing routing can at times be desirable, some developers feel it can be more optimal to leave this to third-party tools such as Selenium, so do keep this in mind.

## Exercise

As an exercise, I recommend now trying the Jasmine Koans in `practicals\jasmine-koans` and trying to fix some of the purposefully failing tests it has to offer. This is an excellent way of not just learning how Jasmine specs and suites work, but working through the examples (without peeking back) will also put your Backbone skills to the test too.


## Further reading
* [Testing Backbone Apps With SinonJS](http://tinnedfruit.com/2011/04/26/testing-backbone-apps-with-jasmine-sinon-3.html) by James Newbry
* [Jasmine + Backbone Revisited](http://japhr.blogspot.com/2011/11/jasmine-backbonejs-revisited.html)
* [Backbone, PhantomJS and Jasmine](http://japhr.blogspot.com/2011/12/phantomjs-and-backbonejs-and-requirejs.html)
