# Backbone Basics

In this section, you'll learn the essentials of Backbone's models, views, collections, events, and routers. This isn't by any means a replacement for the official documentation, but it will help you understand many of the core concepts behind Backbone before you start building applications using it.

### Getting setup

Before we dive into more code examples, let's define some boilerplate markup you can use to specify the dependencies Backbone requires. This boilerplate can be reused in many ways with little to no alteration and will allow you to run code from examples with ease.

You can paste the following into your text editor of choice, replacing the commented line between the script tags with the JavaScript from any given example:

```html
<!DOCTYPE HTML>
<html>
<head>
    <meta charset="UTF-8">
    <title>Title</title>
</head>
<body>

<script src="https://ajax.googleapis.com/ajax/libs/jquery/1.9.1/jquery.min.js"></script>
<script type="text/javascript" src="http://documentcloud.github.com/underscore/underscore-min.js"></script>
<script type="text/javascript" src="http://documentcloud.github.com/backbone/backbone-min.js"></script>
<script type="text/javascript">
  // Your code goes here
</script>
</body>
</html>
```

You can then save and run the file in our browser of choice, such as Chrome or Firefox. Alternatively, if you prefer working with an online code editor, [jsFiddle](http://jsfiddle.net/jnf8B/) and [jsBin](http://jsbin.com/iwiwox/1/edit) versions of this boilerplate are also available.

Most examples can also be run directly from within the console in your browser's developer tools, assuming you've loaded the boilerplate HTML page so that Backbone and it's dependencies are available for use.

For Chrome, you can open up the DevTools via the Chrome menu in the top right hand corner: select "Tools > Developer Tools" or alternatively use the Control + Shift + I shortcut on Windows/Linux or Command + Option + I on Mac. 

![](img/devtools.png)

Next, switch to the Console tab, from where you can enter in and run any piece of JavaScript code by hitting the return key. You can also use the Console as a multi-line editor using the Ctrl + Enter shortcut to move from the end of one line to the start of another.
 

## Models

Backbone models contain data for an application as well as the logic around this data. For example, we can use a model to represent the concept of a todo item including its attributes like title (todo content) and completed (current state of the todo).

Models can be created by extending `Backbone.Model` as follows:

```javascript
var Todo = Backbone.Model.extend({});

// We can then create our own concrete instance of a (Todo) model
// with no values at all:
var todo1 = new Todo();
// Following logs: {}
console.log(JSON.stringify(todo1));

// or with some arbitrary data:
var todo2 = new Todo({
  title: 'Check the attributes of both model instances in the console.',
  completed: true
});

// Following logs: {"title":"Check the attributes of both model instances in the console.","completed":true}
console.log(JSON.stringify(todo2));
```

#### Initialization

The `initialize()` method is called when a new instance of a model is created. Its use is optional; however you'll see why it's good practice to use it below.

```javascript
var Todo = Backbone.Model.extend({
  initialize: function(){
      console.log('This model has been initialized.');
  }
});

var myTodo = new Todo();
// Logs: This model has been initialized.
```

**Default values**

There are times when you want your model to have a set of default values (e.g., in a scenario where a complete set of data isn't provided by the user). This can be set using a property called `defaults` in your model.

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

// Following logs: {"title":"","completed":false}
console.log(JSON.stringify(todo1));

// Or we could instantiate it with some of the attributes (e.g., with custom title):
var todo2 = new Todo({
  title: 'Check attributes of the logged models in the console.'
});

// Following logs: {"title":"Check attributes of the logged models in the console.","completed":false}
console.log(JSON.stringify(todo2));

// Or override all of the default attributes:
var todo3 = new Todo({
  title: 'This todo is done, so take no action on this one.',
  completed: true
});

// Following logs: {"title":"This todo is done, so take no action on this one.","completed":true} 
console.log(JSON.stringify(todo3));
```

#### Getters & Setters

**Model.get()**

`Model.get()` provides easy access to a model's attributes.

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
  title: "Retrieved with model's get() method.",
  completed: true
});
console.log(todo2.get('title')); // Retrieved with model's get() method.
console.log(todo2.get('completed')); // true
```

If you need to read or clone all of a model's data attributes, use its `toJSON()` method. This method returns a copy of the attributes as an object (not a JSON string despite its name). (When `JSON.stringify()` is passed an object with a `toJSON()` method, it stringifies the return value of `toJSON()` instead of the original object. The examples in the previous section took advantage of this feature when they called `JSON.stringify()` to log model instances.)

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

// logs: {"title":"Try these examples and check results in console.","completed":true}
console.log(todo2.toJSON());
```

**Model.set()**

`Model.set()` allows us to pass attributes into an instance of our model. Backbone uses Model.set() to know when to broadcast to its listeners that the model's data has changed.

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
console.log('Todo title: ' + myTodo.get('title')); // Todo title: Set through instantiation.
console.log('Completed: ' + myTodo.get('completed')); // Completed: false

// Set single attribute value at a time through Model.set():
myTodo.set("title", "Title attribute set through Model.set().");
console.log('Todo title: ' + myTodo.get('title')); // Todo title: Title attribute set through Model.set().
console.log('Completed: ' + myTodo.get('completed')); // Completed: false

// Set map of attributes through Model.set():
myTodo.set({
  title: "Both attributes set through Model.set().",
  completed: true
});
console.log('Todo title: ' + myTodo.get('title')); // Todo title: Both attributes set through Model.set().
console.log('Completed: ' + myTodo.get('completed')); // Completed: true
```

**Direct access**

Models expose an `.attributes` attribute which represents an internal hash containing the state of that model. This is generally in the form of a JSON object similar to the model data you might find on the server but can take other forms.

Setting values through the `.attributes` attribute on a model bypasses triggers bound to the model. Additionally you can pass `{silent: true}`, which silences individual "change:attr" events entirely, but can build up and then hit in unexpected ways later on.

Remember where possible it is best practice to use `Model.set()`, or direct instantiation as explained earlier.


#### Listening for changes to your model

If you want to receive a notification when a Backbone model changes you can bind a listener to the model for its change event. A convenient place to add listeners is in the `initialize()` function as shown below:

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

myTodo.set('title', 'The listener is triggered whenever an attribute value changes.');
console.log('Title has changed: ' + myTodo.get('title'));


myTodo.set('completed', true);
console.log('Completed has changed: ' + myTodo.get('completed'));

myTodo.set({
  title: 'Changing more than one attribute at the same time only triggers the listener once.',
  completed: true
});

// Above logs:
// This model has been initialized.
// - Values for this model have changed.
// Title has changed: The listener is triggered whenever an attribute value changes.
// - Values for this model have changed.
// Completed has changed: true
// - Values for this model have changed.
```

You can also listen for changes to individual attributes in a Backbone model. In the following example, we log a message whenever a specific attribute (the title of our Todo model) is altered.

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
        console.log('Title value for this model has changed.');
    });
  },

  setTitle: function(newTitle){
    this.set({ title: newTitle });
  }
});

var myTodo = new Todo();

// Both of the following changes trigger the listener:
myTodo.set('title', 'Check what\'s logged.');
myTodo.setTitle('Go fishing on Sunday.');

// But, this change type is not observed, so no listener is triggered:
myTodo.set('completed', true);
console.log('Todo set as completed: ' + myTodo.get('completed'));

// Above logs:
// This model has been initialized.
// Title value for this model has changed.
// Title value for this model has changed.
// Todo set as completed: true
```


#### Validation

Backbone supports model validation through `Model.validate()`, which allows checking the attribute values for a model prior to setting them.

Validation functions can be as simple or complex as necessary. If the attributes provided are valid, nothing should be returned from `.validate()`. If they are invalid, an error value should be returned instead. If an error is returned, an `invalid` event will occur and listeners will be passed the unmodified model and the error value.

Validation automatically occurs when the model is persisted using the `.save()` method or when `.set()` is called with the validate option set to true. A basic validation example can be seen below:

```javascript
var Todo = Backbone.Model.extend({
  defaults: {
    completed: false
  },

  validate: function(attribs){
    if(attribs.title === undefined){
        return "Remember to set a title for your todo.";
    }
  },

  initialize: function(){
    console.log('This model has been initialized.');
    this.on("invalid", function(model, error){
        console.log(error);
    });
  }
});

var myTodo = new Todo();
myTodo.set('completed', true, {validate: true}); // logs: Remember to set a title for your todo.
console.log('completed: ' + myTodo.get('completed')); // completed: false
```

**Note**: the `attributes` object passed to the `validate` function represents what the attributes would be after completing the current `set()` or `save()`. This object is distinct from the current attributes of the model and from the parameters passed to the operation. Since it is created by shallow copy, it is not possible to change any Number, String, or Boolean attribute of the input within the function, but it *is* possible to change attributes in nested objects.

An example of this (by @fivetanley) is available [here](http://jsfiddle.net/2NdDY/7/).


## Views

Views in Backbone don't contain the HTML markup for your application; they contain the logic behind the presentation of the model's data to the user. This is usually achieved using JavaScript templating (e.g., Underscore Microtemplates, Mustache, jQuery-tmpl, etc.). A view's `render()` method can be bound to a model's `change()` event, enabling the view to instantly reflect model changes without requiring a full page refresh.


#### Creating new views

Creating a new view is relatively straight-forward and similar to creating new models. To create a new View, simply extend `Backbone.View`. We introduced the sample TodoView below in the previous chapter; now let's take a closer look at how it works.

```javascript
var TodoView = Backbone.View.extend({

  tagName:  'li',

  // Cache the template function for a single item.
  todoTpl: _.template( "An example template" ),

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

// log reference to a DOM element that corresponds to the view instance
console.log(todoView.el); // logs <li></li>
```

#### What is `el`?

The central property of a view is `el` (the value logged in the last statement of the example). What is `el` and how is it defined? 

`el` is basically a reference to a DOM element and all views must have one. Views can use `el` to compose their element's content and then insert it into the DOM all at once, which makes for faster rendering because the browser performs the minimum required number of reflows and repaints.

There are two ways to associate a DOM element with a view: a new element can be created for the view and subsequently added to the DOM or a reference can be made to an element which already exists in the page.

If you want to create a new element for your view, set any combination of the following properties on the view: `tagName`, `id`, and `className`. A new element will be created for you by the framework and a reference to it will be available at the `el` property. If nothing is specified `tagName` defaults to `div`.

In the example above, `tagName` is set to 'li', resulting in creation of an li element. The following example creates a ul element with id and class attributes:

```javascript
var TodosView = Backbone.View.extend({
  tagName: 'ul', // required, but defaults to 'div' if not set
  className: 'container', // optional, you can assign multiple classes to this property like so: 'container homepage'
  id: 'todos', // optional
});

var todosView = new TodosView();
console.log(todosView.el); // logs <ul id="todos" class="container"></ul>
```

The above code creates the DOM element below but doesn't append it to the DOM.

```html
<ul id="todos" class="container"></ul>
```

If the element already exists in the page, you can set `el` as a CSS selector that matches the element.

```javascript
el: '#footer'
```

Alternatively, you can set `el` to an existing element when creating the view:

```javascript
var todosView = new TodosView({el: $('#footer')});
```

**$el and $()**

View logic often needs to invoke jQuery or Zepto functions on the `el` element and elements nested within it. Backbone makes it easy to do so by defining the `$el` property and `$()` function. The `view.$el` property is equivalent to `$(view.el)` and `view.$(selector)` is equivalent to `$(view.el).find(selector)`. In our TodosView example's render method, we see `this.$el` used to set the HTML of the element and `this.$()` used to find subelements of class 'edit'.

**setElement**

If you need to apply an existing Backbone view to a different DOM element `setElement` can be used for this purpose. Overriding this.el needs to both change the DOM reference and re-bind events to the new element (and unbind from the old). 

`setElement` will create a cached `$el` reference for you, moving the delegated events for a view from the old element to the new one.

```javascript

// We create two DOM elements representing buttons
// which could easily be containers or something else
var button1 = $('<button></button>');
var button2 = $('<button></button>');

// Define a new view
var View = Backbone.View.extend({
      events: {
        click: function(e) {
          console.log(view.el === e.target);
        }
      }
    });

// Create a new instance of the view, applying it
// to button1
var view = new View({el: button1});

// Apply the view to button2 using setElement
view.setElement(button2);

button1.trigger('click'); 
button2.trigger('click'); // returns true
```

The "el" property represents the markup portion of the view that will be rendered; to get the view to actually render to the page, you need to add it as a new element or append it to an existing element.

```javascript

// We can also provide raw markup to setElement
// as follows (just to demonstrate it can be done):
var view = new Backbone.View;
view.setElement('<p><a><b>test</b></a></p>');
view.$('a b').html(); // outputs "test"
```

**Understanding `render()`**

`render()` is an optional function that defines the logic for rendering a template. We'll use Underscore's micro-templating in these examples, but remember you can use other templating frameworks if you prefer. Our example will reference the following HTML markup:

```html
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <title></title>
  <meta name="description" content="">
</head>
<body>
  <div id="todo">
  </div>
  <script type="text/template" id="item-template">
    <div>
      <input id="todo_complete" type="checkbox" <%= completed ? 'checked="checked"' : '' %>>
      <%= title %>
    </div>
  </script>
  <script src="underscore-min.js"></script>
  <script src="backbone-min.js"></script>
  <script src="jquery-min.js"></script>
  <script src="example.js"></script>
</body>
</html>
```

The `_.template` method in Underscore compiles JavaScript templates into functions which can be evaluated for rendering. In the TodoView, I'm passing the markup from the template with id `item-template` to `_.template()` to be compiled and stored in the todoTpl property when the view is created.

The `render()` method uses this template by passing it the `toJSON()` encoding of the attributes of the model associated with the view. The template returns its markup after using the model's title and completed flag to evaluate the expressions containing them. I then set this markup as the HTML content of the `el` DOM element using the `$el` property.

Presto! This populates the template, giving you a data-complete set of markup in just a few short lines of code.



A common Backbone convention is to return `this` at the end of `render()`. This is useful for a number of reasons, including:

* Making views easily reusable in other parent views
* Creating a list of elements without rendering and painting each of them individually, only to be drawn once the entire list is populated.

Let's try to implement the latter of these. The `render`method of a simple ListView which doesn't use an ItemView for each item could be written:

```javascript

var ListView = Backbone.View.extend({
  render: function(){
    this.$el.html(this.model.toJSON());
  }
});
```

Simple enough. Let's now assume a decision is made to construct the items using an ItemView to provide enhanced behaviour to our list. The ItemView could be written:

```javascript

var ItemView = Backbone.View.extend({
  events: {},
  render: function(){
    this.$el.html(this.model.toJSON());
    return this;
  }
});

```

Note the usage of `return this;` at the end of `render`. This common pattern enables us to reuse the view as a sub-view. We can also use it to pre-render the view prior to rendering. Using this requires that we make a change to our ListView's `render` method as follows:

```javascript

var ListView = Backbone.View.extend({
  render: function(){

    // Assume our model exposes the items we will
    // display in our list
    var items = this.model.get('items');

    // Loop through each our items using the Underscore
    // _.each iterator
    _.each(items, function(item){

      // Create a new instance of the ItemView, passing 
      // it a specific model item
      var itemView = new ItemView({ model: item });
      // The itemView's DOM element is appended after it
      // has been rendered. Here, the 'return this' is helpful
      // as the itemView renders its model. Later, we ask for 
      // it's output ("el")
      this.$el.append( itemView.render().el );
    }, this);
  }
});
```

**The `events` hash**


The Backbone `events` hash allows us to attach event listeners to either `el`-relative custom selectors, or directly to `el` if no selector is provided. An event takes the form of a key-value pair `'eventName selector': 'callbackFunction'` and a number of DOM event-types are supported, including `click`, `submit`, `mouseover`, `dblclick` and more.

```javascript

// A sample view
var TodoView = Backbone.View.extend({
  tagName:  'li',

  // with an events hash containing DOM events
  // specific to an item:
  events: {
    'click .toggle': 'toggleCompleted',
    'dblclick label': 'edit',
    'click .destroy': 'clear',
    'blur .edit': 'close'
  },
```

What isn't instantly obvious is that while Backbone uses jQuery's `.delegate()` underneath, it goes further by extending it so that `this` always refers to the current view object within callback functions. The only thing to really keep in mind is that any string callback supplied to the events attribute must have a corresponding function with the same name within the scope of your view. 

The declarative, delegated jQuery events means that you don't have to worry about whether a particular element has been rendered to the DOM yet or not. Usually with jQuery you have to worry about "presence or absence in the DOM" all the time when binding events.

In our TodoView example, the edit callback is invoked when the user double-clicks a label element within the `el` element, updateOnEnter is called for each keypress in an element with class 'edit', and close executes when an element with class 'edit' loses focus. Each of these callback functions can use `this` to refer to the TodoView object.

Note that you can also bind methods yourself using `_.bind(this.viewEvent, this)`, which is effectively what the value in each event's key-value pair is doing. Below we use `_.bind` to re-render our view when a model changes.

```javascript

var TodoView = Backbone.View.extend({
  initialize: function() {
    this.model.bind('change', _.bind(this.render, this));
  }
});
```

`_.bind` only works on one method at a time, but supports currying and as it returns the bound function means that you can use `_.bind` on an anonymous function.


## Collections

Collections are sets of Models and are created by extending `Backbone.Collection`.

Normally, when creating a collection you'll also want to define a property specifying the type of model that your collection will contain, along with any instance properties required.

In the following example, we create a TodoCollection that will contain our Todo models:

```javascript
var Todo = Backbone.Model.extend({
  defaults: {
    title: '',
    completed: false
  }
});

var TodosCollection = Backbone.Collection.extend({
  model: Todo
});

var myTodo = new Todo({title:'Read the whole book', id: 2});

// pass array of models on collection instantiation
var todos = new TodosCollection([myTodo]);
console.log("Collection size: " + todos.length); // Collection size: 1
```

#### Adding and Removing Models

The preceding example populated the collection using an array of models when it was instantiated. After a collection has been created, models can be added and removed using the `add()` and `remove()` methods:

```javascript
var Todo = Backbone.Model.extend({
  defaults: {
    title: '',
    completed: false
  }
});

var TodosCollection = Backbone.Collection.extend({
  model: Todo,
});

var a = new Todo({ title: 'Go to Jamaica.'}),
    b = new Todo({ title: 'Go to China.'}),
    c = new Todo({ title: 'Go to Disneyland.'});

var todos = new TodosCollection([a,b]);
console.log("Collection size: " + todos.length);
// Logs: Collection size: 2

todos.add(c);
console.log("Collection size: " + todos.length);
// Logs: Collection size: 3

todos.remove([a,b]);
console.log("Collection size: " + todos.length);
// Logs: Collection size: 1

todos.remove(c);
console.log("Collection size: " + todos.length);
// Logs: Collection size: 0
```

Note that `add()` and `remove()` accept both individual models and lists of models.

#### Retrieving Models

There are a few different ways to retrieve a model from a collection. The most straight-forward is to use `Collection.get()` which accepts a single id as follows:

```javascript
var myTodo = new Todo({title:'Read the whole book', id: 2});

// pass array of models on collection instantiation
var todos = new TodosCollection([myTodo]);

var todo2 = todos.get(2);

// Models, as objects, are passed by reference
console.log(todo2 === myTodo); // true
```

In client-server applications, collections contain models obtained from the server. Anytime you're exchanging data between the client and a server, you will need a way to uniquely identify models. In Backbone, this is done using the `id`, `cid`, and `idAttribute` properties.

Each model in Backbone has an `id`, which is a unique identifier that is either an integer or string (e.g., a UUID). Models also have a `cid` (client id) which is automatically generated by Backbone when the model is created. Either identifier can be used to retrieve a model from a collection. 

The main difference between them is that the `cid` is generated by Backbone; it is helpful when you don't have a true id - this may be the case if your model has yet to be saved to the server or you aren't saving it to a database.

The `idAttribute` is the identifying attribute of the model returned from the server (i.e., the `id` in your database). This tells Backbone which data field from the server should be used to populate the `id` property (think of it as a mapper). By default, it assumes `id`, but this can be customized as needed. For instance, if your server sets a unique attribute on your model named "userId" then you would set `idAttribute` to "userId" in your model definition.

The value of a model's idAttribute should be set by the server when the model is saved. After this point you shouldn't need to set it manually, unless further control is required. 

Internally, `Backbone.Collection` contains an array of models enumerated by their `id` property, if the model instances happen to have one. When `collection.get(id)` is called, this array is checked for existence of the model instance with the corresponding `id`.

```javascript
// extends the previous example

var todoCid = todos.get(todo2.cid);

// As mentioned in previous example, 
// models are passed by reference
console.log(todoCid === myTodo); // true
```

#### Listening for events

As collections represent a group of items, we can listen for `add` and `remove` events which occur when models are added to or removed from a collection. Here's an example:

```javascript
var TodosCollection = new Backbone.Collection();

TodosCollection.on("add", function(todo) {
  console.log("I should " + todo.get("title") + ". Have I done it before? "  + (todo.get("completed") ? 'Yeah!': 'No.' ));
});

TodosCollection.add([
  { title: 'go to Jamaica', completed: false },
  { title: 'go to China', completed: false },
  { title: 'go to Disneyland', completed: true }
]);

// The above logs:
// I should go to Jamaica. Have I done it before? No.
// I should go to China. Have I done it before? No.
// I should go to Disneyland. Have I done it before? Yeah!
```

In addition, we're also able to bind to a `change` event to listen for changes to any of the models in the collection.

```javascript
var TodosCollection = new Backbone.Collection();

// log a message if a model in the collection changes
TodosCollection.on("change:title", function(model) {
    console.log("Changed my mind! I should " + model.get('title'));
});

TodosCollection.add([
  { title: 'go to Jamaica.', completed: false, id: 3 },
]);

var myTodo = TodosCollection.get(3);

myTodo.set('title', 'go fishing');
// Logs: Changed my mind! I should go fishing
```

#### Resetting/Refreshing Collections

Rather than adding or removing models individually, you might want to update an entire collection at once. `Collection.set()` takes an array of models and performs the necessary add, remove, and change operations required to update the collection.

```javascript
var TodosCollection = new Backbone.Collection();

TodosCollection.add([
    { id: 1, title: 'go to Jamaica.', completed: false },
    { id: 2, title: 'go to China.', completed: false },
    { id: 3, title: 'go to Disneyland.', completed: true }
]);

// we can listen for add/change/remove events
TodosCollection.on("add", function(model) {
  console.log("Added " + model.get('title'));
});

TodosCollection.on("remove", function(model) {
  console.log("Removed " + model.get('title'));
});

TodosCollection.on("change:completed", function(model) {
  console.log("Completed " + model.get('title'));
});

TodosCollection.set([
    { id: 1, title: 'go to Jamaica.', completed: true },
    { id: 2, title: 'go to China.', completed: false },
    { id: 4, title: 'go to Disney World.', completed: false }
]);

// Above logs:
// Removed go to Disneyland.
// Completed go to Jamaica.
// Added go to Disney World.
```

If you need to simply replace the entire content of the collection then `Collection.reset()` can be used:

```javascript
var TodosCollection = new Backbone.Collection();

// we can listen for reset events
TodosCollection.on("reset", function() {
  console.log("Collection reset.");
});

TodosCollection.add([
  { title: 'go to Jamaica.', completed: false },
  { title: 'go to China.', completed: false },
  { title: 'go to Disneyland.', completed: true }
]);

console.log('Collection size: ' + TodosCollection.length); // Collection size: 3

TodosCollection.reset([
  { title: 'go to Cuba.', completed: false }
]);
// Above logs 'Collection reset.'

console.log('Collection size: ' + TodosCollection.length); // Collection size: 1
```

Another useful tip is to use `reset` with no arguments to clear out a collection completely. This is handy when dynamically loading a new page of results where you want to blank out the current page of results.

```javascript
myCollection.reset();
```

Note that using `Collection.reset()` doesn't fire any `add` or `remove` events. A `reset` event is fired instead as shown in the previous example. The reason you might want to use this is to perform super-optimized rendering in extreme cases where individual events are too expensive.

#### Underscore utility functions

Backbone takes full advantage of its hard dependency on Underscore by making many of its utilities directly available on collections:

**`forEach`: iterate over collections**

```javascript
var Todos = new Backbone.Collection();

Todos.add([
  { title: 'go to Belgium.', completed: false },
  { title: 'go to China.', completed: false },
  { title: 'go to Austria.', completed: true }
]);

// iterate over models in the collection
Todos.forEach(function(model){
  console.log(model.get('title'));
});
// Above logs:
// go to Belgium.
// go to China.
// go to Austria.
```

**`sortBy()`: sort a collection on a specific attribute**

```javascript
// sort collection
var sortedByAlphabet = Todos.sortBy(function (todo) {
    return todo.get("title").toLowerCase();
});

console.log("- Now sorted: ");

sortedByAlphabet.forEach(function(model){
  console.log(model.get('title'));
});
// Above logs:
// go to Austria.
// go to Belgium.
// go to China.
```

**`map()`: iterate through a collection, mapping each value through a transformation function**

```javascript
var count = 1;
console.log(Todos.map(function(model){
  return count++ + ". " + model.get('title');;
}));
// Above logs:
//1. go to Belgium.
//2. go to China.
//3. go to Austria.
```

**`min()`/`max()`: retrieve item with the min or max value of an attribute**

```javascript
Todos.max(function(model){
  return model.id;
}).id;

Todos.min(function(model){
  return model.id;
}).id;
```

**`pluck()`: extract a specific attribute**

```javascript
var captions = Todos.pluck('caption');
// returns list of captions
```

**`filter()`: filter a collection**

*Filter by an array of model IDs*

```javascript
var Todos = Backbone.Collection.extend({
  model: Todo,
  filterById: function(ids){
    return this.models.filter(
      function(c) { 
        return _.contains(ids, c.id); 
      })
  }
});
```

**`indexOf()`: return the item at a particular index within a collection**

```javascript
Todos.indexOf(5);
```

**`any()`: Confirm if any of the values in a collection pass an iterator truth test**

```javascript
Todos.any(function(model){
  return model.id === 100;
});

// or
Todos.some(function(model){
  return model.id === 100;
});
```

**`size()`: return the size of a collection**

```javascript
Todos.size();

// equivalent to
Todos.length;
```

**`isEmpty()`: determine whether a collection is empty**

```javascript
var isEmpty = Todos.isEmpty();
```

**`groupBy()`: group a collection into groups of like items**

```javascript
var Todos = new Backbone.Collection();

Todos.add([
  { title: 'go to Belgium.', completed: false },
  { title: 'go to China.', completed: false },
  { title: 'go to Austria.', completed: true }
]);

// create groups of completed and incomplete models
var byCompleted = Todos.groupBy('completed');
var completed = new Backbone.Collection(byCompleted[true]);
console.log(completed.pluck('title'));
// logs: ["go to Austria."]
```

In addition, several of the Underscore operations on objects are available as methods on Models.

**`pick()`: extract a set of attributes from a model**

```javascript
var Todo = Backbone.Model.extend({
  defaults: {
    title: '',
    completed: false
  }
});

var todo = new Todo({title: 'go to Austria.'});
console.log(todo.pick('title'));
// logs {title: "go to Austria"}
```

**`omit()`: extract all attributes from a model except those listed**

```javascript
var todo = new Todo({title: 'go to Austria.'});
console.log(todo.omit('title'));
// logs {completed: false}
```

**`keys()` and `values()`: get lists of attribute names and values**

```javascript
var todo = new Todo({title: 'go to Austria.'});
console.log(todo.keys());
// logs: ["title", "completed"]

console.log(todo.values());
//logs: ["go to Austria.", false]
```

**`pairs()`: get list of attributes as [key, value] pairs**

```javascript
var todo = new Todo({title: 'go to Austria.'});
var pairs = todo.pairs();

console.log(pairs[0]);
// logs: ["title", "go to Austria."]
console.log(pairs[1]);
// logs: ["completed", false]
```

**`invert()`: create object in which the values are keys and the attributes are values**

```javascript
var todo = new Todo({title: 'go to Austria.'});
console.log(todo.invert());

// logs: {go to Austria.: "title", false: "completed"}
```

The complete list of what Underscore can do can be found in its official [docs](http://documentcloud.github.com/underscore/).

#### Chainable API

Speaking of utility methods, another bit of sugar in Backbone is its support for Underscore’s `chain()` method. Chaining is a common idiom in object-oriented languages; a chain is a sequence of method calls on the same object that are performed in a single statement. While Backbone makes Underscore's array manipulation operations available as methods of Collection objects, they cannot be directly chained since they return arrays rather than the original Collection.

Fortunately, the inclusion of Underscore's `chain()` method enables you to chain calls to these methods on Collections.

The `chain()` method returns an object that has all of the Underscore array operations attached as methods which return that object. The chain ends with a call to the `value()` method which simply returns the resulting array value. In case you haven’t seen it before, the chainable API looks like this:

```javascript
var collection = new Backbone.Collection([
  { name: 'Tim', age: 5 },
  { name: 'Ida', age: 26 },
  { name: 'Rob', age: 55 }
]);

var filteredNames = collection.chain() // start chain, returns wrapper around collection's models
  .filter(function(item) { return item.get('age') > 10; }) // returns wrapped array excluding Tim
  .map(function(item) { return item.get('name'); }) // returns wrapped array containing remaining names
  .value(); // terminates the chain and returns the resulting array

console.log(filteredNames); // logs: ['Ida', 'Rob']
```

Some of the Backbone-specific methods do return `this`, which means they can be chained as well:

```javascript
var collection = new Backbone.Collection();

collection
    .add({ name: 'John', age: 23 })
    .add({ name: 'Harry', age: 33 })
    .add({ name: 'Steve', age: 41 });

var names = collection.pluck('name');

console.log(names); // logs: ['John', 'Harry', 'Steve']
```

## RESTful Persistence

Thus far, all of our example data has been created in the browser. For most single page applications, the models are derived from a data set residing on a server. This is an area in which Backbone dramatically simplifies the code you need to write to perform RESTful synchronization with a server through a simple API on its models and collections.

**Fetching models from the server**

`Collections.fetch()` retrieves a set of models from the server in the form of a JSON array by sending an HTTP GET request to the URL specified by the collection's `url` property (which may be a function). When this data is received, a `set()` will be executed to update the collection.

```javascript
var Todo = Backbone.Model.extend({
  defaults: {
    title: '',
    completed: false
  }
});

var TodosCollection = Backbone.Collection.extend({
  model: Todo,
  url: '/todos'
});

var todos = new TodosCollection();
todos.fetch(); // sends HTTP GET to /todos
```

**Saving models to the server**

While Backbone can retrieve an entire collection of models from the server at once, updates to models are performed individually using the model's `save()` method. When `save()` is called on a model that was fetched from the server, it constructs a URL by appending the model's id to the collection's URL and sends an HTTP PUT to the server. If the model is a new instance that was created in the browser (i.e., it doesn't have an id) then an HTTP POST is sent to the collection's URL. `Collections.create()` can be used to create a new model, add it to the collection, and send it to the server in a single method call.

```javascript
var Todo = Backbone.Model.extend({
  defaults: {
    title: '',
    completed: false
  }
});

var TodosCollection = Backbone.Collection.extend({
  model: Todo,
  url: '/todos'
});

var todos = new TodosCollection();
todos.fetch();

var todo2 = todos.get(2);
todo2.set('title', 'go fishing');
todo2.save(); // sends HTTP PUT to /todos/2

todos.create({title: 'Try out code samples'}); // sends HTTP POST to /todos and adds to collection
```

As mentioned earlier, a model's `validate()` method is called automatically by `save()` and will trigger an `invalid` event on the model if validation fails.

**Deleting models from the server**

A model can be removed from the containing collection and the server by calling its `destroy()` method. Unlike `Collection.remove()` which only removes a model from a collection, `Model.destroy()` will also send an HTTP DELETE to the collection's URL.

```javascript
var Todo = Backbone.Model.extend({
  defaults: {
    title: '',
    completed: false
  }
});

var TodosCollection = Backbone.Collection.extend({
  model: Todo,
  url: '/todos'
});

var todos = new TodosCollection();
todos.fetch();

var todo2 = todos.get(2);
todo2.destroy(); // sends HTTP DELETE to /todos/2 and removes from collection
```

**Options**

Each RESTful API method accepts a variety of options. Most importantly, all methods accept success and error callbacks which can be used to customize the handling of server responses. Specifying the `{patch: true}` option to `Model.save()` will cause it to use HTTP PATCH to send only the changed attributes to the server instead of the entire model. Similarly, passing the `{reset: true}` option to `Collection.fetch()` will result in the collection being updated using `reset()` rather than `set()`.

See the Backbone.js documentation for full descriptions of the supported options.

## Events

Events are a basic inversion of control, where instead of having a function call another function by name, the second function registers interest in being called whenever the first "event" occurs.

The part of your application that has to know how to call the other part of your app has been inverted. This is the core thing that makes it possible for your business logic to not have to know about how your user interface works and is the most powerful thing about the Backbone Events system.

Mastering events is one of the quickest ways to become more productive with Backbone, so let's take a closer look at Backbone's event model.

`Backbone.Events` is mixed into the other Backbone "classes", including:

* Backbone
* Backbone.Model
* Backbone.Collection
* Backbone.Router
* Backbone.History
* Backbone.View

Note that `Backbone.Events` is mixed into the `Backbone` object. Since `Backbone` is globally visible, it can be used as a simple event bus:

```javascript
Backbone.on('event', function() {console.log('Handled Backbone event');});
Backbone.trigger('event'); // logs: Handled Backbone event
```

#### on(), off(), and trigger()

`Backbone.Events` can give any object the ability to bind and trigger custom events. We can mix this module into any object easily and there isn't a requirement for events to be declared before being bound to a callback handler.

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

`on` binds a callback function to an object, as we've done with `dance` in the above example. The callback is invoked whenever the event is triggered.

The official Backbone.js documentation recommends namespacing event names using colons if you end up using quite a few of these on your page. e.g.:

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

A special `all` event is made available in case you would like notifications for every event that occurs on the object (e.g., if you would like to screen events in a single location). The `all` event can be used as follows:


```javascript
var ourObject = {};

// Mixin
_.extend(ourObject, Backbone.Events);

function dancing (msg) { console.log("We started " + msg); }

ourObject.on("all", function(eventName){
  console.log("The name of the event passed was " + eventName);
});

// This time each event will be caught with a catch 'all' event listener
ourObject.trigger("dance:tap", "tap dancing. Yeah!");
ourObject.trigger("dance:break", "break dancing. Yeah!");
ourObject.trigger("dance", "break dancing. Yeah!");
```

`off` removes callback functions that were previously bound to an object. Going back to our Publish/Subscribe comparison, think of it as an `unsubscribe` for custom events.

To remove the `dance` event we previously bound to `ourObject`, we would simply do:

```javascript
var ourObject = {};

// Mixin
_.extend(ourObject, Backbone.Events);

function dancing (msg) { console.log("We " + msg); }

// Add namespaced custom events
ourObject.on("dance:tap", dancing);
ourObject.on("dance:break", dancing);

// Trigger the custom events. Each will be caught and acted upon.
ourObject.trigger("dance:tap", "started tap dancing. Yeah!");
ourObject.trigger("dance:break", "started break dancing. Yeah!");

// Removes event bound to the object
ourObject.off("dance:tap");

// Trigger the custom events again, but one is logged.
ourObject.trigger("dance:tap", "stopped tap dancing."); // won't be logged as it's not listened for
ourObject.trigger("dance:break", "break dancing. Yeah!");
```

To remove all callbacks for the event we pass an event name (e.g., `move`) to the `off()` method on the object the event is bound to. If we wish to remove a specific callback, we can pass that callback as the second parameter:

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

Finally, as we have seen in our previous examples, `trigger` triggers a callback for a specified event (or a space-separated list of events). e.g.:

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

`trigger` can pass multiple arguments to the callback function:

```javascript
var ourObject = {};

// Mixin
_.extend(ourObject, Backbone.Events);

function doAction (action, duration) {
  console.log("We are " + action + ' for ' + duration ); 
}

// Add event listeners
ourObject.on("dance", doAction);
ourObject.on("jump", doAction);
ourObject.on("skip", doAction);

// Passing multiple arguments to single event
ourObject.trigger("dance", 'dancing', "5 minutes");

// Passing multiple arguments to multiple events
ourObject.trigger("dance jump skip", 'on fire', "15 minutes");
```

#### listenTo() and stopListening()

While `on()` and `off()` add callbacks directly to an observed object, `listenTo()` tells an object to listen for events on another object, allowing the listener to keep track of the events for which it is listening. `stopListening()` can subsequently be called on the listener to tell it to stop listening for events:

```javascript
var a = _.extend({}, Backbone.Events);
var b = _.extend({}, Backbone.Events);
var c = _.extend({}, Backbone.Events);

// add listeners to A for events on B and C
a.listenTo(b, 'anything', function(event){ console.log("anything happened"); });
a.listenTo(c, 'everything', function(event){ console.log("everything happened"); });

// trigger an event
b.trigger('anything'); // logs: anything happened

// stop listening
a.stopListening();

// A does not receive these events
b.trigger('anything');
c.trigger('everything');
```

`stopListening()` can also be used to selectively stop listening based on the event, model, or callback handler.

If you use `on` and `off` and remove views and their corresponding models at the same time, there are generally no problems. But a problem arises when you remove a view that had registered to be notified about events on a model, but you don't remove the model or call `off` to remove the view's event handler. Since the model has a reference to the view's callback function, the JavaScript garbage collector cannot remove the view from memory. This is called a "ghost view" and is a form of memory leak which is common since the models generally tend to outlive the corresponding views during an application's lifecycle. For details on the topic and a solution, check this [excellent article](http://lostechies.com/derickbailey/2011/09/15/zombies-run-managing-page-transitions-in-backbone-apps/) by Derick Bailey. 

Practically, every `on` called on an object also requires an `off` to be called in order for the garbage collector to do its job. `listenTo()` changes that, allowing Views to bind to Model notifications and unbind from all of them with just one call - `stopListening()`.

The default implementation of `View.remove()` makes a call to `stopListening()`, ensuring that any listeners bound using `listenTo()` are unbound before the view is destroyed.

```javascript
var view = new Backbone.View();
var b = _.extend({}, Backbone.Events);

view.listenTo(b, 'all', function(){ console.log(true); });
b.trigger('anything');

view.listenTo(b, 'all', function(){ console.log(false); });
view.remove(); // stopListening() implicitly called
b.trigger('anything');
// logs: true
```

#### Events and Views

Within a View, there are two types of events you can listen for: DOM events and events triggered using the Event API. It is important to understand the differences in how views bind to these events and the context in which their callbacks are invoked.

DOM events can be bound to using the View's `events` property or using `jQuery.on()`. Within callbacks bound using the `events` property, `this` refers to the View object; whereas any callbacks bound directly using jQuery will have `this` set to the handling DOM element by jQuery. All DOM event callbacks are passed an `event` object by jQuery. See `delegateEvents()` in the Backbone documentation for additional details.

Event API events are bound as described in this section. If the event is bound using `on()` on the observed object, a context parameter can be passed as the third argument. If the event is bound using `listenTo()` then within the callback `this` refers to the listener. The arguments passed to Event API callbacks depends on the type of event. See the Catalog of Events in the Backbone documentation for details.

The following example illustrates these differences:

```html
<div id="todo">
    <input type='checkbox' />
</div>
```

```javascript
var View = Backbone.View.extend({

    el: '#todo',

    // bind to DOM event using events property
    events: {
        'click [type="checkbox"]': 'clicked',
    },

    initialize: function () {
        // bind to DOM event using jQuery
        this.$el.click(this.jqueryClicked);

        // bind to API event
        this.on('apiEvent', this.callback);
    },

    // 'this' is view
    clicked: function(event) {
        console.log("events handler for " + this.el.outerHTML);
        this.trigger('apiEvent', event.type);
    },

    // 'this' is handling DOM element
    jqueryClicked: function(event) {
        console.log("jQuery handler for " + this.outerHTML);
    },

    callback: function(eventType) {
        console.log("event type was " + eventType);
    }

});

var view = new View();
```

## Routers

In Backbone, routers provide a way for you to connect URLs (either hash fragments, or
real) to parts of your application. Any piece of your application that you want
to be bookmarkable, shareable, and back-button-able, needs a URL.

Some examples of routes using the hash mark may be seen below:

```javascript
http://example.com/#about
http://example.com/#search/seasonal-horns/page2
```

An application will usually have at least one route mapping a URL route to a function that determines what happens when a user reaches that route. This relationship is defined as follows:

```javascript
'route' : 'mappedFunction'
```

Let's define our first router by extending `Backbone.Router`. For the purposes of this guide, we're going to continue pretending we're creating a complex todo application (something like a personal organizer/planner) that requires a complex TodoRouter.

Note the inline comments in the code example below as they continue our lesson on routers.

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
        /* Sample usage: http://example.com/# <anything> */
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

/* Now that we have a router setup, we need to instantiate it */

var myTodoRouter = new TodoRouter();
```

Backbone offers an opt-in for HTML5 pushState support via `window.history.pushState`. This permits you to define routes such as http://backbonejs.org/just/an/example. This will be supported with automatic degradation when a user's browser doesn't support pushState. Note that it is vastly preferred that you're capable of also supporting pushState on the server side, although it is a little more difficult to implement.

**Is there a limit to the number of routers I should be using?**

Andrew de Andrade has pointed out that DocumentCloud, the creators of Backbone, usually only use a single router in most of their applications. You're very likely to not require more than one or two routers in your own projects; the majority of your application routing can be kept organized in a single router without it getting unwieldy.

#### Backbone.history

Next, we need to initialize `Backbone.history` as it handles `hashchange` events in our application. This will automatically handle routes that have been defined and trigger callbacks when they've been accessed.

The `Backbone.history.start()` method will simply tell Backbone that it's okay to begin monitoring all `hashchange` events as follows:

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
// http://localhost/#search/job/p3   logs: Page number: 3 of the results for todos containing the word: job
// http://localhost/#search/job      logs: Page number: 1 of the results for todos containing the word: job 
// etc.
```

Note: To run the last example, you'll need to create a local development environment and test project, which we will cover later on in the book.

If you would like to update the URL to reflect the application state at a particular point, you can use the router's `.navigate()` method. By default, it simply updates your URL fragment without triggering the `hashchange` event:

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
    console.log("Edit todo opened.");
  }
});

var myTodoRouter = new TodoRouter();

Backbone.history.start();

// Go to: http://localhost/#todo/4
//
// URL is updated to: http://localhost/#todo/4/edit
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
    console.log("Edit todo opened.");
  }
});

var myTodoRouter = new TodoRouter();

Backbone.history.start();

// Go to: http://localhost/#todo/4
//
// URL is updated to: http://localhost/#todo/4/edit
// and this time editTodo() function is invoked.
//
// logs:
// View todo requested.
// Edit todo opened.
```

## Backbone’s Sync API

We previously discussed how Backbone supports RESTful persistence via its `fetch()` and `create()` methods on Collections and `save()`, and `delete()` methods on Models. Now we are going to take a closer look at Backbone's sync method which underlies these operations.

The Backbone.sync method is an integral part of Backbone.js. It assumes a jQuery-like `$.ajax()` method, so HTTP parameters are organized based on jQuery’s API. Since some legacy servers may not support JSON-formatted requests and HTTP PUT and DELETE operations, Backbone can be configured to emulate these capabilities using the two configuration variables shown below with their default values:

```javascript
Backbone.emulateHTTP = false; // set to true if server cannot handle HTTP PUT or HTTP DELETE
Backbone.emulateJSON = false; // set to true if server cannot handle application/json requests
```

The Backbone.emulateHTTP variable should be set to true if extended HTTP methods are not supported by the server. The Backbone.emulateJSON variable should be set to true if the server does not understand the MIME type for JSON.

`Backbone.sync` is called every time Backbone tries to read, save, or delete models. It uses jQuery or Zepto's `$.ajax()` implementations to make these RESTful requests, however this can be overridden as per your needs.

**Overriding Backbone.sync**

The `sync` function may be overridden globally as Backbone.sync, or at a finer-grained level, by adding a sync function to a Backbone collection or to an individual model.

Since all persistence is handled by the Backbone.sync function, an alternative persistence layer can be used by simply overriding Backbone.sync with a function that has the same signature:

```javascript
Backbone.sync = function(method, model, options) {
};
```

The methodMap below is used by the standard sync implementation to map the method parameter to an HTTP operation and illustrates the type of action required by each method argument:

```javascript
var methodMap = {
  'create': 'POST',
  'update': 'PUT',
  'patch':  'PATCH',
  'delete': 'DELETE',
  'read':   'GET'
};
```

If we wanted to replace the standard `sync` implementation with one that simply logged the calls to sync, we could do this:

```javascript
var id_counter = 1;
Backbone.sync = function(method, model) {
  console.log("I've been passed " + method + " with " + JSON.stringify(model));
  if(method === 'create'){ model.set('id', id_counter++); }
};
```

Note that we assign a unique id to any created models.

The Backbone.sync method is intended to be overridden to support other persistence backends. The built-in method is tailored to a certain breed of RESTful JSON APIs - Backbone was originally extracted from a Ruby on Rails application, which uses HTTP methods like PUT in the same way.

The sync method is called with three parameters:

* method: One of create, update, patch, delete, or read
* model: The Backbone model object
* options: May include success and error methods

Implementing a new sync method can use the following pattern:

```javascript
Backbone.sync = function(method, model, options) {

  function success(result) {
    // Handle successful results from MyAPI
    if (options.success) {
      options.success(result);
    }
  }

  function error(result) {
    // Handle error results from MyAPI
    if (options.error) {
      options.error(result);
    }
  }

  options || (options = {});

  switch (method) {
    case 'create':
      return MyAPI.create(model, success, error);

    case 'update':
      return MyAPI.update(model, success, error);

    case 'patch':
      return MyAPI.patch(model, success, error);

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

This pattern delegates API calls to a new object (MyAPI), which could be a Backbone-style class that supports events. This can be safely tested separately, and potentially used with libraries other than Backbone.

There are quite a few sync implementations out there. The following examples are all available on GitHub:

* Backbone localStorage: persists to the browser's local storage
* Backbone offline: supports working offline
* Backbone Redis: uses Redis key-value store
* backbone-parse: integrates Backbone with Parse.com
* backbone-websql: stores data to WebSQL
* Backbone Caching Sync: uses local storage as cache for other sync implementations


## Dependencies

The official Backbone.js [documentation](http://backbonejs.org/) states:

>Backbone's only hard dependency is either Underscore.js ( >= 1.4.3) or Lo-Dash. For RESTful persistence, history support via Backbone.Router and DOM manipulation with Backbone.View, include json2.js, and either jQuery ( >= 1.7.0) or Zepto.

What this translates to is that if you require working with anything beyond models, you will need to include a DOM manipulation library such as jQuery or Zepto. Underscore is primarily used for its utility methods (which Backbone relies upon heavily) and json2.js for legacy browser JSON support if Backbone.sync is used.

## Summary

In this chapter we have introduced you to the components you will be using to build applications with Backbone: Models, Views, Collections, and Routers. We've also explored the Events mix-in that Backbone uses to enhance all components with publish-subscribe capabilities and seen how it can be used with arbitrary objects. Finally, we saw how Backbone leverages the Underscore.js and jQuery/Zepto APIs to add rich manipulation and persistence features to Backbone Collections and Models.

Backbone has many operations and options beyond those we have covered here and is always evolving, so be sure to visit the official [documentation](http://backbonejs.org/) for more details and the latest features. In the next chapter you will start to get your hands dirty as we walk you through implementation of your first Backbone application.


