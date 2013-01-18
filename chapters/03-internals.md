# The Internals

In this section, you'll learn the essentials of Backbone's models, views, collections and routers, as well as about using namespacing to organize your code. This isn't meant as a replacement for the official documentation, but it will help you understand many of the core concepts behind Backbone before you start building applications with it.

* Models
* Collections
* Routers
* Views
* Dependencies
* Namespacing


## Models

Backbone models contain interactive data for an application as well as the logic around this data. For example, we can use a model to represent the concept of a todo item including its attributes like title (todo content) and completed (current state of the todo).

Models can be created by extending `Backbone.Model` as follows:

```javascript
var Todo = Backbone.Model.extend({});

// We can then create our own concrete instance of a (Todo) model
// with no values at all:
var todo1 = new Todo();
console.log(todo1);

// or with some arbitrary data:
var todo2 = new Todo({ 
  title: 'Check attributes property of the both model instances in the console.', 
  completed: true
});
console.log(todo2);
```

#### Initialization

The `initialize()` method is called when a new instance of a model is created. Its use is optional, however you'll see why it's good practice to use it below.

```javascript
var Todo = Backbone.Model.extend({
  initialize: function(){
      console.log('This model has been initialized.');
  }
});

var myTodo = new Todo();
```

**Default values**

There are times when you want your model to have a set of default values (e.g. in a scenario where a complete set of data isn't provided by the user). This can be set using a property called `defaults` in your model.

```javascript
var Todo = Backbone.Model.extend({
  // Default todo attribute values
  defaults: {
    title: '',
    completed: false
  }
});

// Now we can create our concrete instance of the model 
// with default values as follows:
var todo1 = new Todo();
console.log(todo1);

// Or we could instantiate it with some of the attributes (e.g with custom title):
var todo2 = new Todo({ 
  title: 'Check attributes property of the logged models in the console.'
});
console.log(todo2);

// Or with all of the (default) attributes:
var todo3 = new Todo({
  title: 'This todo is done, so take no action on this one.',
  completed: true
});
console.log(todo3);
```

#### Getters & Setters

**Model.get()**

`Model.get()` provides easy access to a model's attributes. All attributes, regardless if default ones were passed through to the model on instantiation, are available for retrieval.

```javascript
var Todo = Backbone.Model.extend({
  // Default todo attribute values
  defaults: {
    title: '',
    completed: false
  }
});

var todo1 = new Todo();
console.log(todo1.get('title')); // empty string
console.log(todo1.get('completed')); // false

var todo2 = new Todo({
  title: "Retrieved with models get() method.",
  completed: true
});
console.log(todo2.get('title')); // Retrieved with models get() method.
console.log(todo2.get('completed')); // true
```

If you need to read or clone all of a model's data attributes use its `toJSON` method. Despite the name it doesn't return a JSON string but a copy of the attributes as an object. ("toJSON" is part of the JSON.stringify specification. Passing an object with a toJSON method makes it stringify the return value of that method instead of the object itself.)

```javascript
var Todo = Backbone.Model.extend({
  // Default todo attribute values
  defaults: {
    title: '',
    completed: false
  }
});

var todo1 = new Todo();
var todo1Attributes = todo1.toJSON();
// Following logs: {"title":"","completed":false} 
console.log(todo1Attributes);

var todo2 = new Todo({
  title: "Try these examples and check results in console.",
  completed: true
});
// logs: {"title":"Try examples and check results in console.","completed":true}
console.log(todo2.toJSON());
```

**Model.set()**

`Model.set()` allows us to pass attributes into an instance of our model. Attributes can either be set during initialization or at any time afterwards. Backbone uses Model.set() to know when to broadcast that a model's data has changed.

```javascript
var Todo = Backbone.Model.extend({
  // Default todo attribute values
  defaults: {
    title: '',
    completed: false
  }
});

// Setting the value of attributes via instantiation
var myTodo = new Todo({ 
  title: "Set through instantiation." 
});
console.log('Todo title: ' + myTodo.get('title'));
console.log('Completed: ' + myTodo.get('completed'));

// Set single attribute value at the time through Model.set():
myTodo.set("title", "Title attribute set through Model.set().");
console.log('Todo title: ' + myTodo.get('title'));
console.log('Completed: ' + myTodo.get('completed'));

// Set map of attributes through Model.set():
myTodo.set({ 
  title: "Both attributes set through Model.set().",
  completed: true
});
console.log('Todo title: ' + myTodo.get('title'));
console.log('Completed: ' + myTodo.get('completed'));
```

**Direct access**

If you really need to access the attributes in a model's instance directly, there is `Model.attributes`. But remember it is best practice to use Model.get(), Model.set() or direct instantiation as explained above.

#### Listening for changes to your model

Any and all of the attributes in a Backbone model can have listeners bound to them which detect when their values change. Listeners can be added to the `initialize()` function:

```javascript
var Todo = Backbone.Model.extend({
  // Default todo attribute values
  defaults: {
    title: '',
    completed: false
  },
  initialize: function(){
    console.log('This model has been initialized.');
    this.on('change', function(){
        console.log('- Values for this model have changed.');
    });
  }
});

var myTodo = new Todo();

myTodo.set('title', 'On each change of attribute values listener is triggered.');
console.log('Title has changed: ' + myTodo.get('title'));

myTodo.set('completed', true);
console.log('Completed has changed: ' + myTodo.get('completed'));

myTodo.set({
  title: 'Listener is triggered for each change, not for change of the each attribute.',
  'complete': true
});
```

In the following example, we log a message whenever a specific attribute (the title of our Todo model) is altered.

```javascript
var Todo = Backbone.Model.extend({
  // Default todo attribute values
  defaults: {
    title: '',
    completed: false
  },
  initialize: function(){
    console.log('This model has been initialized.');
    this.on('change:title', function(){
        console.log('Title value for this model have changed.');
    });
  },
  setTitle: function(newTitle){
    this.set({ title: newTitle });
  }
});

var myTodo = new Todo();

// Following changes trigger the listener:
myTodo.set('title', 'Check what\'s logged.');
myTodo.setTitle('Go fishing on Sunday.');

// But, this change type is not observed, so no listener is triggered:
myTodo.set('completed', true);
console.log('Todo set as completed: ' + myTodo.get('completed'));
```

#### Validation

Backbone supports model validation through `Model.validate()`, which allows checking the attribute values for a model prior to them being set.

Validation functions can be as simple or complex as necessary. If the attributes provided are valid, nothing should be returned from `.validate()`. If they are invalid, a custom error can be returned instead.

A basic example for validation can be seen below:

```javascript
var Todo = Backbone.Model.extend({
  validate: function(attribs){
    if(attribs.title === undefined){
        return "Remember to set a title for your todo.";
    }
  },

  initialize: function(){
    console.log('This model has been initialized.');
    this.on("error", function(model, error){
        console.log(error);
    });
  }
});

var myTodo = new Todo();
myTodo.set('completed', false); // logs: Remember to set a title for your todo.
```

**Note**: Backbone passes the `attributes` object (attribs param in above example) by shallow copy to the `validate` function using the Underscore `_.extend` method. This means that it is not possible to change any Number, String or Boolean attribute but it *is* possible to change attributes of objects because they are passed by reference. As shallow copy doesn't copy objects by implicitly copying them, but rather, by reference, one can change the attributes on those objects.

An example of this (by @fivetanley) is available [here](http://jsfiddle.net/2NdDY/7/).


## Views

Views in Backbone don't contain the markup for your application, but rather they are there to support models by defining the logic for how they should be represented to the user. This is usually achieved using JavaScript templating (e.g. Mustache, jQuery-tmpl, etc.). A view's `render()` function can be bound to a model's `change()` event, allowing the view to always be up to date without requiring a full page refresh.


#### Creating new views

Similar to the previous sections, creating a new view is relatively straight-forward. To create a new View, simply extend `Backbone.View`. I'll explain this code in detail below:

```javascript
var TodoView = Backbone.View.extend({

  tagName:  'li',

  // Cache the template function for a single item.
  todoTpl: _.template( $('#item-template').html() ),

  events: {
    'dblclick label': 'edit',
    'keypress .edit': 'updateOnEnter',
    'blur .edit':   'close'
  },

  // Re-render the titles of the todo item.
  render: function() {
    this.$el.html( this.todoTpl( this.model.toJSON() ) );
    this.input = this.$('.edit');
    return this;
  },

  edit: function() {
    // executed when todo label is double clicked
  },

  close: function() {
    // executed when todo loses focus
  },

  updateOnEnter: function( e ) {
    // executed on each keypress when in todo edit mode, 
    // but we'll wait for enter to get in action
  }
});

var todoView = new TodoView();

// logs reference to a DOM element that cooresponds to the view instance
console.log(todoView.el);
```

#### What is `el`?

`el` is basically a reference to a DOM element and all views must have one. It allows for all of the contents of a view to be inserted into the DOM at once, which makes for faster rendering because the browser performs the minimum required reflows and repaints.

There are two ways to attach a DOM element to a view: a new element is created for the view and added manually by the developer or the element already exists in the page.

If you want to create a new element for your view, set any combination of the following view's properties: `tagName`, `id` and `className`. A new element will be created for you by the framework and a reference to it will be available at the `el` property. If nothing is specified `el` defaults to `div`.

```javascript
var TodosView = Backbone.View.extend({
  tagName: 'ul', // required, but defaults to 'div' if not set
  className: 'container', // optional, you can assign multiple classes to this property like so 'container homepage'
  id: 'todos', // optional
});

var todosView = new TodosView();
console.log(todosView.el);
```

The above code creates the ```DOMElement``` below but doesn't append it to the DOM.

```html
<ul id="todos" class="container"></ul>
```

If the element already exists in the page, you can set `el` as a CSS selector that matches the element.

```javascript
el: '#footer'
```

**Understanding `render()`**

`render()` is an optional function that defines the logic for rendering a template. We'll use Underscore's micro-templating in these examples, but remember you can use other templating frameworks if you prefer.

The `_.template` method in Underscore compiles JavaScript templates into functions which can be evaluated for rendering. In the above view, I'm passing the markup from a template with id `item-template` to `_.template()` to be compiled. Next, I set the html of the `el` DOM element to the output of processing a JSON version of the model associated with the view through the compiled template.

Presto! This populates the template, giving you a data-complete set of markup in just a few short lines of code.

**The `events` attribute**

The Backbone `events` attribute allows us to attach event listeners to either custom selectors, or directly to `el` if no selector is provided. An event takes the form `{'eventName selector': 'callbackFunction'}` and a number of DOM event-types are supported, including `click`, `submit`, `mouseover`, `dblclick` and more.

What isn't instantly obvious is that under the bonnet, Backbone uses jQuery's `.delegate()` to provide instant support for event delegation but goes a little further, extending it so that `this` always refers to the current view object. The only thing to really keep in mind is that any string callback supplied to the events attribute must have a corresponding function with the same name within the scope of your view.


## Collections

Collections are sets of Models and are created by extending `Backbone.Collection`.

Normally, when creating a collection you'll also want to pass through a property specifying the model that your collection will contain, as well as any instance properties required.

In the following example, we create a TodoCollection that will contain our Todo models:

```javascript
var Todo = Backbone.Model.extend({
  defaults: {
    title: '',
    completed: false
  }
});

var TodosCollection = Backbone.Collection.extend({
  model: Todo,
  localStorage: new Store('todos-backbone')  
});

var myTodo = new Todo({title:'Read the whole book', id: 2});

// pass array of models on collection instantiation
var todos = new TodosCollection([myTodo]);
console.log("Collection size: " + todos.length);

// Collection's convenience method used to create 
// new model instance within collection itself.
todos.create({title:'Try out code examples', id: 48});
console.log("Collection size: " + todos.length);
```

**Getters and Setters**

There are a few different ways to retrieve a model from a collection. The most straight-forward is to use `Collection.get()` which accepts a single id as follows:

```javascript
// extends on previous example

var todo2 = todos.get(2);

// Models, as objects, are passed by reference
console.log(todo2 === myTodo);
```

Internally `Backbone.Collection` sets an array of models enumerated by their `id` property, if model instances happen to have one. Once `collection.get(id)` is called this array is checked for existence of the model instance with the corresponding `id`.

Sometimes you may also want to get a model based on its client id. The client id is a property that Backbone automatically assigns models that have not yet been saved. You can get a model's client id from its `.cid` property.

```javascript
// extends on previous examples

var todoCid = todos.get(todo2.cid);

// As mentioned in previous example, 
// models are passed by reference
console.log(todoCid === myTodo); 
```

Backbone Collections don't have setters as such, but do support adding new models via `.add()` and removing models via `.remove()`.

```javascript
var Todo = Backbone.Model.extend({
  defaults: {
    title: '',
    completed: false
  }
});

var TodosCollection = Backbone.Collection.extend({
  model: Todo,
  localStorage: new Store('todos-backbone')  
});

var a = new Todo({ title: 'Go to Jamaica.'}),
    b = new Todo({ title: 'Go to China.'}),
    c = new Todo({ title: 'Go to Disneyland.'});

var todos = new TodosCollection([a,b]);
console.log("Collection size: " + todos.length);

todos.add(c);
console.log("Collection size: " + todos.length);

todos.remove([a,b]);
console.log("Collection size: " + todos.length);

todos.remove(c);
console.log("Collection size: " + todos.length);
```

**Listening for events**

As collections represent a group of items, we're also able to listen for `add` and `remove` events for when new models are added or removed from the collection. Here's an example:

```javascript
var TodosCollection = new Backbone.Collection();

TodosCollection.on("add", function(todo) {
  console.log("I should " + todo.get("title") + ". Have I done it before? "  + (todo.get("completed") ? 'Yeah!': 'Not.' ));
});

TodosCollection.add([
  { title: 'go to Jamaica.', completed: false },
  { title: 'go to China.', completed: false },
  { title: 'go to Disneyland.', completed: true }
]);
```

In addition, we're able to bind a `change` event to listen for changes to models in the collection.

```javascript
var TodosCollection = new Backbone.Collection();

TodosCollection.on("change:title", function(model) {
    console.log("Changed my mind where I should go, " + model.get('title'));
});

TodosCollection.add([
  { title: 'go to Jamaica.', completed: false, id: 3 },
]);

var myTodo = TodosCollection.get(3);

myTodo.set('title', 'go fishing');
```

**Fetching models from the server**

`Collections.fetch()` retrieves a default set of models from the server in the form of a JSON array. When this data returns, the current collection's contents will be replaced with the contents of the array.


```javascript
var TodosCollection = new Backbone.Collection;
TodosCollection.url = '/todos';
TodosCollection.fetch();
```

During configuration, Backbone sets a variable to denote if extended HTTP methods are supported by the server. Another setting controls if the server understands the correct MIME type for JSON:

```javascript
Backbone.emulateHTTP = false;
Backbone.emulateJSON = false;
```

The Backbone.sync method that uses these values is actually an integral part of Backbone.js. A jQuery-like ajax method is assumed, so HTTP parameters are organised based on jQuery’s API. Searching through the code for calls to the sync method show it’s used whenever a model is saved, fetched, or deleted (destroyed).

Under the covers, `Backbone.sync` is the function called every time Backbone tries to read or save models to the server. It uses jQuery or Zepto's ajax implementations to make these RESTful requests, however this can be overridden as per your needs.

The sync function may be overriden globally as Backbone.sync, or at a finer-grained level, by adding a sync function to a Backbone collection or to an individual model.

There’s no fancy plugin API for adding a persistence layer – simply override Backbone.sync with the same function signature:

```javascript
Backbone.sync = function(method, model, options) {
};
```

The default methodMap is useful for working out what the method argument does:

```javascript
var methodMap = {
  'create': 'POST',
  'update': 'PUT',
  'delete': 'DELETE',
  'read':   'GET'
};
```

In the above example if we wanted to log an event when `.sync()` was called, we could do this:

```javascript
var id_counter = 1;
Backbone.sync = function(method, model) {
  console.log("I\'ve been passed " + method + " with " + JSON.stringify(model));
  if(method === 'create'){ model.set('id', id_counter++); }
};
```


**Resetting/Refreshing Collections**

Rather than adding or removing models individually, you might occasionally wish to update an entire collection at once. ```Collection.reset()``` allows us to replace an entire collection with new models as follows:

```javascript
var TodosCollection = new Backbone.Collection();

TodosCollection.on("reset", function() {
  console.log("Collection reseted.");
});

TodosCollection.add([
  { title: 'go to Jamaica.', completed: false },
  { title: 'go to China.', completed: false },
  { title: 'go to Disneyland.', completed: true }
]);

console.log('Collection size: ' + TodosCollection.length);

TodosCollection.reset([
  { title: 'go to Cuba.', completed: false }
]);
console.log('Collection size: ' + TodosCollection.length);
```

Note that using `Collection.reset()` doesn't fire any `add` or `remove` events. A `reset` event is fired instead as shown in example.

### Underscore utility functions

As Backbone requires Underscore as a hard dependency, we're able to use many of the utilities it has to offer to aid with our application development. Here's an example of how Underscore's `forEach` method that can be used for iterating over collection and `sortBy()` method that can be used to sort a collection of todos based on a particular attribute.

```javascript
var TodosCollection = new Backbone.Collection();

TodosCollection.on("reset", function() {
  console.log("Collection reseted.");
});

TodosCollection.add([
  { title: 'go to Belgium.', completed: false },
  { title: 'go to China.', completed: false },
  { title: 'go to Austria.', completed: true }
]);

TodosCollection.forEach(function(model){
  console.log(model.get('title'));
});

var sortedByAlphabet = TodosCollection.sortBy(function (todo) {
    return todo.get("title").toLowerCase();
});

console.log("- Now sorted: ");

sortedByAlphabet.forEach(function(model){
  console.log(model.get('title'));
});
```

The complete list of what Underscore can do is beyond the scope of this guide, but can be found in its official [docs](http://documentcloud.github.com/underscore/).


### Chainable API

Speaking of utility methods, another bit of sugar in Backbone is the support for Underscore’s chain method. This works by calling the original method with the current array of models and returning the result. In case you haven’t seen it before, the chainable API looks like this:

```javascript
var collection = new Backbone.Collection([
  { name: 'Tim', age: 5 },
  { name: 'Ida', age: 26 },
  { name: 'Rob', age: 55 }
]);

var filteredNames = collection.chain()
  .filter(function(item) { return item.get('age') > 10; })
  .map(function(item) { return item.get('name'); })
  .value();

console.log(filteredNames); // logs: ['Ida', 'Rob']
```

Some of the Backbone-specific methods will return this, which means they can be chained as well:

```javascript
var collection = new Backbone.Collection();

collection
    .add({ name: 'John', age: 23 })
    .add({ name: 'Harry', age: 33 })
    .add({ name: 'Steve', age: 41 });

var names = collection.pluck('name');

console.log(names); // logs: ['John', 'Harry', 'Steve']
```

## Events


As we've covered, `Backbone.Events` is mixed into the other Backbone "classes", including:

* Backbone.Model
* Backbone.Collection
* Backbone.Router
* Backbone.History
* Backbone.View

Events are the standard way to deal with user interface actions, through the declarative event bindings on views, and also model and collection changes. Mastering events is one of the quickest ways to become more productive with Backbone.

`Backbone.Events` also has the ability to give any object a way to bind and trigger custom events. We can mix this module into any object easily and there isn't a requirement for events to be declared prior to them being bound.

Example:

```javascript
var ourObject = {};

// Mixin
_.extend(ourObject, Backbone.Events);

// Add a custom event
ourObject.on('dance', function(msg){
  console.log('We triggered ' + msg);
});

// Trigger the custom event
ourObject.trigger('dance', 'our event');
```

If you're familiar with jQuery custom events or the concept of Publish/Subscribe, `Backbone.Events` provides a system that is very similar with `on` being analogous to `subscribe` and `trigger` being similar to `publish`.

`on` basically allows us to bind a callback function to any object, as we've done with `dance` in the above example. Whenever the event is fired, our callback is invoked.

The official Backbone.js documentation recommends namespacing event names using colons if you end up using quite a few of these on your page. e.g:

```javascript
var ourObject = {};

// Mixin
_.extend(ourObject, Backbone.Events);

function dancing (msg) { console.log("We started " + msg); }

// Add namespaced custom events
ourObject.on("dance:tap", dancing);
ourObject.on("dance:break", dancing);

// Trigger the custom events
ourObject.trigger("dance:tap", "tap dancing. Yeah!");
ourObject.trigger("dance:break", "break dancing. Yeah!");

// This one triggers nothing as no listener listens for it
ourObject.trigger("dance", "break dancing. Yeah!");
```

A special `all` event is made available in case you would like an event to be triggered when any event occurs (e.g if you would like to screen events in a single location). The `all` event can be used as follows:


```javascript
var ourObject = {};

// Mixin
_.extend(ourObject, Backbone.Events);

function dancing (msg) { console.log("We started " + msg); }

ourObject.on("all", function(eventName){
  console.log("The name of the event passed was " + eventName);
});

// This time each event will be catched with a catch 'all' event listener
ourObject.trigger("dance:tap", "tap dancing. Yeah!");
ourObject.trigger("dance:break", "break dancing. Yeah!");
ourObject.trigger("dance", "break dancing. Yeah!");
```

`off` allows us to remove a callback function that has previously been bound to an object. Going back to our Publish/Subscribe comparison, think of it as an `unsubscribe` for custom events.

To remove the `dance` event we previously bound to `ourObject`, we would simply do:

```javascript
var ourObject = {};

// Mixin
_.extend(ourObject, Backbone.Events);

function dancing (msg) { console.log("We  " + msg); }

// Add a namespaced custom events
ourObject.on("dance:tap", dancing);
ourObject.on("dance:break", dancing);

// Trigger the custom events. Each will be catched and acted upon.
ourObject.trigger("dance:tap", "started tap dancing. Yeah!");
ourObject.trigger("dance:break", "started break dancing. Yeah!");

// Removes event bound to the object
ourObject.off("dance:tap");

// Trigger the custom events again, but one is logged.
ourObject.trigger("dance:tap", "stopped tap dancing."); // won't be logged as it's not listened for
ourObject.trigger("dance:break", "break dancing. Yeah!");
```

To remove all callbacks for the event we pass an event name (e.g `move`) to `off()` method on the object the event is bound to. If we wish to remove just a callback by a specific name, we can pass the callback name as second parameter:

```javascript
var ourObject = {};

// Mixin
_.extend(ourObject, Backbone.Events);

function dancing (msg) { console.log("We are dancing. " + msg); }
function jumping (msg) { console.log("We are jumping. " + msg); }

// Add two listeners to the same event
ourObject.on("move", dancing);
ourObject.on("move", jumping);

// Trigger the events. Both listeners are called.
ourObject.trigger("move", "Yeah!");

// Removes specified listener
ourObject.off("move", dancing);

// Trigger the events again. One listener left.
ourObject.trigger("move", "Yeah, jump, jump!");
```

Finally, `trigger` triggers a callback for a specified event (or a space-separated list of events). e.g:

```javascript
var ourObject = {};

// Mixin
_.extend(ourObject, Backbone.Events);

function doAction (msg) { console.log("We are " + msg); }

// Add event listeners
ourObject.on("dance", doAction);
ourObject.on("jump", doAction);
ourObject.on("skip", doAction);

// Single event
ourObject.trigger("dance", 'just dancing.');

// Multiple events
ourObject.trigger("dance jump skip", 'very tired from so much action.');
```

It is also possible to pass along additional arguments to each (or all) of these events via a second argument supported by `trigger`. e.g:

```javascript
var ourObject = {};

// Mixin
_.extend(ourObject, Backbone.Events);

function doAction (actionObj) {
  console.log("We are " + actionObj.action + ' for ' + actionObj.duration ); 
}

// Add event listeners
ourObject.on("dance", doAction);
ourObject.on("jump", doAction);
ourObject.on("skip", doAction);

// Passing multiple arguments to single event
ourObject.trigger("dance", {duration: "5 minutes", action: 'dancing'});

// Passing multiple arguments to multiple events
ourObject.trigger("dance jump skip", {duration: "15 minutes", action: 'on fire'});
```

## Routers

In Backbone, routers are used to help manage application state and for connecting URLs to application events. This is achieved using hash-tags with URL fragments, or using the browser's pushState and History API. Some examples of routes may be seen below:

```javascript
http://example.com/#about
http://example.com/#search/seasonal-horns/page2
```

Note: An application will usually have at least one route mapping a URL route to a function that determines what happens when a  user reaches that particular route. This relationship is defined as follows:

```javascript
'route' : 'mappedFunction'
```

Let us now define our first router by extending `Backbone.Router`. For the purposes of this guide, we're going to continue pretending we're creating a complex todo application (something like personal organize/planner) that requires a complex TodoRouter.

Note the inline comments in the code example below as they continue the rest of the lesson on routers.

```javascript
var TodoRouter = Backbone.Router.extend({
    /* define the route and function maps for this router */
    routes: {
        "about" : "showAbout",
        /* Sample usage: http://example.com/#about */

        "todo/:id" : "getTodo",
        /* This is an example of using a ":param" variable which allows us to match
        any of the components between two URL slashes */
        /* Sample usage: http://example.com/#todo/5 */

        "search/:query" : "searchTodos",
        /* We can also define multiple routes that are bound to the same map function,
        in this case searchTodos(). Note below how we're optionally passing in a
        reference to a page number if one is supplied */
        /* Sample usage: http://example.com/#search/job */

        "search/:query/p:page" : "searchTodos",
        /* As we can see, URLs may contain as many ":param"s as we wish */
        /* Sample usage: http://example.com/#search/job/p1 */

        "todos/:id/download/*documentPath" : "downloadDocument",
        /* This is an example of using a *splat. Splats are able to match any number of
        URL components and can be combined with ":param"s*/
        /* Sample usage: http://example.com/#todos/5/download/files/Meeting_schedule.doc */

        /* If you wish to use splats for anything beyond default routing, it's probably a good
        idea to leave them at the end of a URL otherwise you may need to apply regular
        expression parsing on your fragment */

        "*other"    : "defaultRoute"
        /* This is a default route that also uses a *splat. Consider the
        default route a wildcard for URLs that are either not matched or where
        the user has incorrectly typed in a route path manually */
        /* Sample usage: http://example.com/# <anything */
    },

    showAbout: function(){
    },

    getTodo: function(id){
        /*
        Note that the id matched in the above route will be passed to this function
        */
        console.log("You are trying to reach todo " + id);
    },

    searchTodos: function(query, page){
        var page_number = page || 1;
        console.log("Page number: " + page_number + " of the results for todos containing the word: " + query);
    },

    downloadDocument: function(id, path){
    },

    defaultRoute: function(other){
        console.log('Invalid. You attempted to reach:' + other);
    }
});

/* Now that we have a router setup, remember to instantiate it */

var myTodoRouter = new TodoRouter();
```

As of Backbone 0.5+, it's possible to opt-in for HTML5 pushState support via `window.history.pushState`. This permits you to define routes such as http://www.scriptjunkie.com/just/an/example. This will be supported with automatic degradation when a user's browser doesn't support pushState. For the purposes of this tutorial, we'll use the hashtag method.

#### Is there a limit to the number of routers I should be using?

Andrew de Andrade has pointed out that DocumentCloud, the creators of Backbone, usually only use a single router in most of their applications. You're very likely to not require more than one or two routers in your own projects; the majority of your application routing can be kept organized in a single router without it getting unwieldy.

#### Backbone.history

Next, we need to initialize `Backbone.history` as it handles `hashchange` events in our application. This will automatically handle routes that have been defined and trigger callbacks when they've been accessed.

The `Backbone.history.start()` method will simply tell Backbone that it's OK to begin monitoring all `hashchange` events as follows:

```javascript
var TodoRouter = Backbone.Router.extend({
  /* define the route and function maps for this router */
  routes: {
    "about" : "showAbout",
    "search/:query" : "searchTodos",
    "search/:query/p:page" : "searchTodos"
  },

  showAbout: function(){},

  searchTodos: function(query, page){
    var page_number = page || 1;
    console.log("Page number: " + page_number + " of the results for todos containing the word: " + query);
  }
});

var myTodoRouter = new TodoRouter();

Backbone.history.start();

// Go to and check console:
// http://localhost/#search/job/p3 logs: Page number: 3 of the results for todos containing the word: job
// http://localhost/#search/job logs: Page number: 1 of the results for todos containing the word: job 
// etc.
```

Note: To test the last example, you'll need to create a local development environment and test project, instructions for which are beyond the scope of what this book seeks to outline.

As an aside, if you would like to save application state to the URL at a particular point you can use the `.navigate()` method to achieve this. It simply updates your URL fragment without the need to trigger the `hashchange` event:

```javascript
// Let's imagine we would like a specific fragment (edit) once a user opens a single todo
var TodoRouter = Backbone.Router.extend({
  routes: {
    "todo/:id": "viewTodo",
    "todo/:id/edit": "editTodo"
    // ... other routes
  },

  viewTodo: function(id){
    console.log("View todo requested.");
    this.navigate("todo/" + id + '/edit'); // updates the fragment for us, but doesn't trigger the route
  },
  editTodo: function(id) {
    console.log("Edit todo openned.");
  }
});

var myTodoRouter = new TodoRouter();

Backbone.history.start();

// Go to:
// http://localhost/#todo/4 URL is updated to: http://localhost/#todo/4/edit
// but editTodo() function is not invoked even though location we end up is mapped to it.
//
// logs: View todo requested.
```

It is also possible for `Router.navigate()` to trigger the route as well as update the URL fragment.

```javascript
var TodoRouter = Backbone.Router.extend({
  routes: {
    "todo/:id": "viewTodo",
    "todo/:id/edit": "editTodo"
    // ... other routes
  },

  viewTodo: function(id){
    console.log("View todo requested.");
    this.navigate("todo/" + id + '/edit', true); // updates the fragment and triggers the route as well
  },
  editTodo: function(id) {
    console.log("Edit todo openned.");
  }
});

var myTodoRouter = new TodoRouter();

Backbone.history.start();

// Go to:
// http://localhost/#todo/4 url is updated to: http://localhost/#todo/4/edit
// but this time editTodo() function is invoked.
// 
// logs: 
// View todo requested.
// Edit todo openned. 
```

### Backbone’s Sync API

The Backbone.sync method is intended to be overridden to support other backends. The built-in method is tailored to a certain breed of RESTful JSON APIs – Backbone was originally extracted from a Ruby on Rails application, which uses HTTP methods like PUT the same way.

This works via the model and collection classes' sync method, which calls Backbone.sync. Both will call this.sync internally when fetching, saving, or deleting items.

The sync method is called with three parameters:

* method: One of create, update, delete, read
* model: The Backbone model object
* options: May include success and error methods

Implementing a new sync method can use the following pattern:

```javascript
Backbone.sync = function(method, model, options) {
  var requestContent = {}, success, error;

  function success(result) {
    // Handle results from MyAPI
    if (options.success) {
      options.success(result);
    }
  }

  function error(result) {
    // Handle results from MyAPI
    if (options.error) {
      options.error(result);
    }
  }

  options || (options = {});

  switch (method) {
    case 'create':
      requestContent['resource'] = model.toJSON();
      return MyAPI.create(model, success, error);

    case 'update':
      requestContent['resource'] = model.toJSON();
      return MyAPI.update(model, success, error);

    case 'delete':
      return MyAPI.destroy(model, success, error);

    case 'read':
      if (model.attributes[model.idAttribute]) {
        return MyAPI.find(model, success, error);
      } else {
        return MyAPI.findAll(model, success, error);
      }
  }
};
```

This pattern delegates API calls to a new object, which could be a Backbone-style class that supports events. This can be safely tested separately, and potentially used with libraries other than Backbone.

There are quite a few sync implementations out there:

* Backbone localStorage
* Backbone offline
* Backbone Redis
* backbone-parse
* backbone-websql
* Backbone Caching Sync

### Conflict Management

Like most client-side projects, Backbone's code is wrapped in an immediately-invoked function expression:

```javascript
(function(){
  // Backbone.js
}).call(this);
```

Several things happen during this configuration stage. A Backbone “namespace” is created, and multiple versions of Backbone on the same page are supported through the noConflict mode:

```javascript
var root = this;
var previousBackbone = root.Backbone;

Backbone.noConflict = function() {
  root.Backbone = previousBackbone;
  return this;
};
```

Multiple versions of Backbone can be used on the same page by calling noConflict like this:

```javascript
var Backbone19 = Backbone.noConflict();
// Backbone19 refers to the most recently loaded version,
// and `window.Backbone` will be restored to the previously
// loaded version
```

This initial configuration code also supports CommonJS modules so Backbone can be used in Node projects:

```javascript
var Backbone;
if (typeof exports !== 'undefined') {
  Backbone = exports;
} else {
  Backbone = root.Backbone = {};
}
```

## Inheritance & Mixins

For its inheritance, Backbone internally uses an `inherits` function inspired by `goog.inherits`, Google’s implementation from the Closure Library. It's basically a function to correctly setup the prototype chain.

```javascript
 var inherits = function(parent, protoProps, staticProps) {
      ...
```

The only major difference here is that Backbone’s API accepts two objects containing “instance” and “static” methods.

Following on from this, for inheritance purposes all of Backbone's objects contain an `extend` method as follows:

```javascript
Model.extend = Collection.extend = Router.extend = View.extend = extend;
```

Most development with Backbone is based around inheriting from these objects, and they’re designed to mimic a classical object-oriented implementation.

If this sounds familiar, it's because `extend` is an Underscore.js utility, although Backbone itself does a lot more with this. See below for Underscore's `extend`:

```javascript
each(slice.call(arguments, 1), function(source) {
  for (var prop in source) {
    obj[prop] = source[prop];
  }
});
return obj;
```

The above isn’t quite the same as ES5’s `Object.create`, as it’s actually copying properties (methods and values) from one object to another. As this isn’t enough to support Backbone’s inheritance and class model, the following steps are performed:

* The instance methods are checked to see if there’s a constructor property. If so, the class’s constructor is used, otherwise the parent’s constructor is used (i.e., Backbone.Model)
* Underscore’s extend method is called to add the parent class’s methods to the new child class
* The `prototype` property of a blank constructor function is assigned with the parent’s prototype, and a new instance of this is set to the child’s `prototype` property
* Underscore’s extend method is called twice to add the static and instance methods to the child class
* The child’s prototype’s constructor and a `__super__` property are assigned
* This pattern is also used for classes in CoffeeScript, so Backbone classes are compatible with CoffeeScript classes.

`extend` can be used for a great deal more and developers who are fans of mixins will like that it can be used for this too. You can define functionality on any custom object, and then quite literally copy & paste all of the methods and attributes from that object to a Backbone one:

For example:

```javascript
var MyMixin = {
  foo: 'bar',
  sayFoo: function(){alert(this.foo);}
};

var MyView = Backbone.View.extend({
 // ...
});

_.extend(MyView.prototype, MyMixin);

var myView = new MyView();
myView.sayFoo(); //=> 'bar'
```

We can take this further and also apply it to View inheritance. The following is an example of how to extend one View using another:

```javascript
var Panel = Backbone.View.extend({
});

var PanelAdvanced = Panel.extend({
});
```

However, if you have an `initialize()` method in Panel, then it won't be called if you also have an `initialize()` method in PanelAdvanced, so you would have to call Panel's initialize method explicitly:

```javascript
var Panel = Backbone.View.extend({
  initialize: function(options){
    console.log('Panel initialized');
    this.foo = 'bar';
  }
});

var PanelAdvanced = Panel.extend({
  initialize: function(options){
    Panel.prototype.initialize.call(this, [options]);
    console.log('PanelAdvanced initialized');
    console.log(this.foo); // Log: bar
  }
});

// We can also inherit PanelAdvaned if needed
var PanelAdvancedExtra = PanelAdvanced.extend({
  initialize: function(options){
    PanelAdvanced.prototype.initialize.call(this, [options]);
    console.log('PanelAdvancedExtra initialized');
  }
});

new Panel();
new PanelAdvanced();
new PanelAdvancedExtra();
```

This isn't the most elegant of solutions because if you have a lot of Views that inherit from Panel, then you'll have to remember to call Panel's initialize from all of them.

It's worth noting that if Panel doesn't have an initialize method now but you choose to add it in the future, then you'll need to go to all of the inherited classes in the future and make sure they call Panel's initialize.

So here's an alternative way to define Panel so that your inherited views don't need to call Panel's initialize method:

```javascript
var Panel = function (options) {
  // put all of Panel's initialization code here
  console.log('Panel initialized');
  this.foo = 'bar';

  Backbone.View.apply(this, [options]);
};

_.extend(Panel.prototype, Backbone.View.prototype, {
  // put all of Panel's methods here. For example:
  sayHi: function () {
    console.log('hello from Panel');
  }
});

Panel.extend = Backbone.View.extend;

// other classes then inherit from Panel like this:
var PanelAdvanced = Panel.extend({
  initialize: function (options) {
    console.log('PanelAdvanced initialized');
    console.log(this.foo);
  }
});

var panelAdvanced = new PanelAdvanced(); //Logs: Panel initialized, PanelAdvanced initialized, bar
panelAdvanced.sayHi(); // Logs: hello from Panel
```

When used appropriately, Underscore's `extend` method can save a great deal of time and effort writing redundant code.

(Thanks to [Alex Young](http://dailyjs.com), [Derick Bailey](http://stackoverflow.com/users/93448/derick-bailey) and [JohnnyO](http://stackoverflow.com/users/188740/johnnyo) for the heads up about these tips).

#### Backbone-Super

[Backbone-Super](https://github.com/lukasolson/Backbone-Super) by Lukas Olson adds a *_super* method to *Backbone.Model* using [John Resig’s Inheritance script](http://ejohn.org/blog/simple-javascript-inheritance/).
Rather than using Backbone.Model.prototype.set.call as per the Backbone.js documentation, _super can be called instead:

```javascript
// This is how we normally do it
var OldFashionedNote = Backbone.Model.extend({
  set: function(attributes, options) {
    // Call parent's method
    Backbone.Model.prototype.set.call(this, attributes, options);
    // some custom code here
    // ...
  }
});
```

After including this plugin, you can do the same thing with the following syntax:

```javascript
// This is how we can do it after using the Backbone-super plugin
var Note = Backbone.Model.extend({
  set: function(attributes, options) {
    // Call parent's method
    this._super(attributes, options);
    // some custom code here
    // ...
  }
});
```

## Dependencies

The official Backbone.js [documentation](http://backbonejs.org/) states:

>Backbone's only hard dependency is Underscore.js ( > 1.3.1). For RESTful persistence, history support via Backbone.Router and DOM manipulation with Backbone.View, include json2.js, and either jQuery ( > 1.4.2) or Zepto.

*[Lo-dash](https://github.com/bestiejs/lodash), a fork of Underscore containing performance enhancements is also compatible with Backbone.*

What this translates to is that if you require working with anything beyond models, you will need to include a DOM manipulation library such as jQuery or Zepto. Underscore is primarily used for it's utility methods (which Backbone relies upon heaviy) and json2.js for legacy browser JSON support if Backbone.sync is used.

### DOM Manipulation

Although most developers won't need it, Backbone does support setting a custom DOM library to be used instead of these options. From the source:

```
// Set the JavaScript library that will be used for DOM manipulation and
// Ajax calls (a.k.a. the `$` variable). By default Backbone will use: jQuery,
// Zepto, or Ender; but the `setDomLibrary()` method lets you inject an
// alternate JavaScript library (or a mock library for testing your views
// outside of a browser).

Backbone.setDomLibrary = function(lib) {
  $ = lib;
};
```

Calling this method will allow you to use any custom DOM-manipulation library. e.g:

```
Backbone.setDomLibrary(aCustomLibrary);
```

### Utilities

Underscore.js is heavily used in Backbone behind the scenes for everything from object extension through to event binding. As the entire library is generally included, we get free access to a number of useful utilities we can use on Collections such as filtering `_.filter()`, sorting `_.sortBy()`, mapping `_.map()` and so on. 

From the source:

```
// Underscore methods that we want to implement on the Collection.
var methods = ['forEach', 'each', 'map', 'reduce', 'reduceRight', 'find',
    'detect', 'filter', 'select', 'reject', 'every', 'all', 'some', 'any',
    'include', 'contains', 'invoke', 'max', 'min', 'sortBy', 'sortedIndex',
    'toArray', 'size', 'first', 'initial', 'rest', 'last', 'without', 'indexOf',
    'shuffle', 'lastIndexOf', 'isEmpty', 'groupBy'];

// Mix in each Underscore method as a proxy to Collection#models.
  _.each(methods, function(method) {
    Collection.prototype[method] = function() {
      return _[method].apply(_, [this.models].concat(_.toArray(arguments)));
    };
```

However, for a complete linked list of methods supported, see the [official documentation](http://backbonejs.org/#Collection-Underscore-Methods).

### RESTFul persistence

Models and collections in Backbone can be "sync"ed with the server using the `fetch`, `save` and `destroy` methods. All of these methods delegate back to the `Backbone.sync` function, which actually wraps jQuery/Zepto's `$.ajax` function, calling GET, POST and DELETE for the respective persistence methods on Backbone models. 

From the the source for `Backbone.sync`:

```
var methodMap = {
    'create': 'POST',
    'update': 'PUT',
    'delete': 'DELETE',
    'read':   'GET'
  };
  
Backbone.sync = function(method, model, options) {
    var type = methodMap[method];
    options || (options = {});
    // ... Followed by lots of Backbone.js configuration, then..
   return $.ajax(_.extend(params, options));
```

### Routing

Calls to `Backbone.History.start` rely on jQuery/Zepto binding `popState` or `hashchange` event listeners back to the window object. 

From the source for `Backbone.history.start`:

```
// Depending on whether we're using pushState or hashes, and whether 'onhashchange' is 
// supported, determine how we check the URL state.
if (this._hasPushState) {
        $(window).bind('popstate', this.checkUrl);
      } else if (this._wantsHashChange && ('onhashchange' in window) && !oldIE) {
        $(window).bind('hashchange', this.checkUrl);
      ...
```

`Backbone.History.stop` similarly uses your DOM manipulation library to unbind these event listeners.



## Namespacing

When learning how to use Backbone, an important and commonly overlooked area by tutorials is namespacing. If you already have experience with namespacing in JavaScript, the following section will provide some advice on how to specifically apply concepts you know to Backbone, however I will also be covering explanations for beginners to ensure everyone is on the same page.

#### What is namespacing?

The basic idea around namespacing is to avoid collisions with other objects or variables in the global namespace. They're important as it's best to safeguard your code from breaking in the event of another script on the page using the same variable names as you are. As a good 'citizen' of the global namespace, it's also imperative that you do your best to similarly not prevent other developer's scripts executing due to the same issues.

JavaScript doesn't really have built-in support for namespaces like other languages, however it does have closures which can be used to achieve a similar effect.

In this section we'll be taking a look shortly at some examples of how you can namespace your models, views, routers and other components specifically. The patterns we'll be examining are:

* Single global variables
* Object Literals
* Nested namespacing

**Single global variables**

One popular pattern for namespacing in JavaScript is opting for a single global variable as your primary object of reference. A skeleton implementation of this where we return an object with functions and properties can be found below:

```javascript
var myApplication = (function(){
    function(){
      // ...
    },
    return {
      // ...
    }
})();
```

You've probably seen this technique before. A Backbone-specific example might look like this:

```javascript
var myViews = (function(){
    return {
        TodoView: Backbone.View.extend({ .. }),
        TodosView: Backbone.View.extend({ .. }),
        AboutView: Backbone.View.extend({ .. });
        //etc.
    };
})();
```

Here we can return a set of views, but the same technique could return an entire collection of models, views and routers depending on how you decide to structure your application. Although this works for certain situations, the biggest challenge with the single global variable pattern is ensuring that no one else has used the same global variable name as you have in the page.

One solution to this problem, as mentioned by Peter Michaux, is to use prefix namespacing. It's a simple concept at heart, but the idea is you select a common prefix name (in this example, `myApplication_`) and then define any methods, variables or other objects after the prefix.

```javascript
var myApplication_todoView = Backbone.View.extend({}),
    myApplication_todosView = Backbone.View.extend({});
```

This is effective from the perspective of trying to lower the chances of a particular variable existing in the global scope, but remember that a uniquely named object can have the same effect. This aside, the biggest issue with the pattern is that it can result in a large number of global objects once your application starts to grow.

For more on Peter's views about the single global variable pattern, read his [excellent post on them](http://michaux.ca/articles/javascript-namespacing).

Note: There are several other variations on the single global variable pattern out in the wild, however having reviewed quite a few, I felt the prefixing approach applied best to Backbone.

**Object Literals**

Object Literals have the advantage of not polluting the global namespace but assist in organizing code and parameters logically. They're beneficial if you wish to create easily readable structures that can be expanded to support deep nesting. Unlike simple global variables, Object Literals often also take into account tests for the existence of a variable by the same name, which helps reduce the chances of collision.

This example demonstrates two ways you can check to see if a namespace already exists before defining it. I commonly use Option 2.

```javascript
/* Doesn't check for existence of myApplication */
var myApplication = {};

/*
Does check for existence. If already defined, we use that instance.
Option 1:   if(!myApplication) myApplication = {};
Option 2:   var myApplication = myApplication || {};
We can then populate our object literal to support models, views and collections (or any data, really):
*/

var myApplication = {
    models : {},
    views : {
        pages : {}
    },
    collections : {}
};
```

One can also opt for adding properties directly to the namespace (such as your views, in the following example):

```javascript
var myTodosViews = myTodosViews || {};
myTodosViews.todoView = Backbone.View.extend({});
myTodosViews.todosView = Backbone.View.extend({});
```

The benefit of this pattern is that you're able to easily encapsulate all of your models, views, routers etc. in a way that clearly separates them and provides a solid foundation for extending your code.

This pattern has a number of benefits. It's often a good idea to decouple the default configuration for your application into a single area that can be easily modified without the need to search through your entire codebase just to alter it. Here's an example of a hypothetical object literal that stores application configuration settings:


```javascript
var myConfig = {
  language: 'english',
  defaults: {
    enableDelegation: true,
    maxTodos: 40
  },
  theme: {
    skin: 'a',
    toolbars: {
      index: 'ui-navigation-toolbar',
      pages: 'ui-custom-toolbar'
    }
  }
}
```

Note that there are really only minor syntactical differences between the Object Literal pattern and a standard JSON data set. If for any reason you wish to use JSON for storing your configurations instead (e.g. for simpler storage when sending to the back-end), feel free to.

For more on the Object Literal pattern, I recommend reading Rebecca Murphey's [excellent article on the topic](http://rmurphey.com/blog/2009/10/15/using-objects-to-organize-your-code).

**Nested namespacing**

An extension of the Object Literal pattern is nested namespacing. It's another common pattern used that offers a lower risk of collision due to the fact that even if a top-level namespace already exists, it's unlikely the same nested children do. For example, Yahoo's YUI uses the nested object namespacing pattern extensively:

```javascript
YAHOO.util.Dom.getElementsByClassName('test');
```

Yahoo's YUI uses the nested object namespacing pattern regularly and even DocumentCloud (the creators of Backbone) use the nested namespacing pattern in their main applications. A sample implementation of nested namespacing with Backbone may look like this:

```javascript
var todoApp =  todoApp || {};

// perform similar check for nested children
todoApp.routers = todoApp.routers || {};
todoApp.model = todoApp.model || {};
todoApp.model.special = todoApp.model.special || {};

// routers
todoApp.routers.Workspace   = Backbone.Router.extend({});
todoApp.routers.TodoSearch = Backbone.Router.extend({});

// models
todoApp.model.Todo   = Backbone.Model.extend({});
todoApp.model.Notes = Backbone.Model.extend({});

// special models
todoApp.model.special.Admin = Backbone.Model.extend({});
```

This is readable, clearly organized, and is a relatively safe way of namespacing your Backbone application. The only real caveat however is that it requires your browser's JavaScript engine to first locate the todoApp object, then dig down until it gets to the function you're calling. However, developers such as Juriy Zaytsev (kangax) have tested and found the performance differences between single object namespacing vs the 'nested' approach to be quite negligible.

**Recommendation**

Reviewing the namespace patterns above, the option that I prefer when writing Backbone applications is nested object namespacing with the object literal pattern.

Single global variables may work fine for applications that are relatively trivial. However, larger codebases requiring both namespaces and deep sub-namespaces require a succinct solution that's both readable and scalable. I feel this pattern achieves both of these objectives and is a good choice for most Backbone development.
