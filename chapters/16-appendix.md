# Appendix

## A Simple JavaScript MVC Implementation

A comprehensive discussion of Backbone's implementation is beyond the scope of this book. We can, however, present a simple MVC library - which we will call Cranium.js - that illustrates how frameworks such as Backbone implement the MVC pattern. 

Like Backbone, we will rely on [Underscore](http://underscorejs.org "Underscore.js") for inheritance and templating.

### Event System

At the heart of our JavaScript MVC implementation is an `Event` system (object) based on the Publisher-Subscriber Pattern which makes it possible for MVC components to communicate in an elegant, decoupled manner. Subscribers 'listen' for specific events of interest and react when Publishers broadcast these events.

`Event` is mixed into both the View and Model components so that instances of either of these components can publish events of interest.

```javascript
// cranium.js - Cranium.Events

var Cranium = Cranium || {};

// Set DOM selection utility
var $ = document.querySelector.bind(document) || this.jQuery || this.Zepto;

// Mix in to any object in order to provide it with custom events.
var Events = Cranium.Events = {
  // Keeps list of events and associated listeners
  channels: {},

  // Counter
  eventNumber: 0,

  // Announce events and passes data to the listeners;
  trigger: function (events, data) {
    for (var topic in Cranium.Events.channels){
      if (Cranium.Events.channels.hasOwnProperty(topic)) {
        if (topic.split("-")[0] == events){
          Cranium.Events.channels[topic](data) !== false || delete Cranium.Events.channels[topic];
        }
      }
    }
  },
  // Registers an event type and its listener
  on: function (events, callback) {
    Cranium.Events.channels[events + --Cranium.Events.eventNumber] = callback;
  },
  // Unregisters an event type and its listener
  off: function(topic) {
    delete Cranium.Events.channels[topic];
  }            
};
```

The Event system makes it possible for:

* a View to notify its subscribers of user interaction (e.g., clicks or input in a form), to update/re-render its presentation, etc.
* a Model whose data has changed to notify its Subscribers to update themselves (e.g., view to re-render to show accurate/updated data), etc.

### Models

Models manage the (domain-specific) data for an application. They are concerned with neither the user-interface nor presentation layers, but instead represent structured data that an application may require. When a model changes (e.g when it is updated), it will typically notify its observers (Subscribers) that a change has occurred so that they may react accordingly.

Let's see a simple implementation of the Model:

```javascript
// cranium.js - Cranium.Model

// Attributes represents data, model's properties.
// These are to be passed at Model instantiation.
// Also we are creating id for each Model instance 
// so that it can identify itself (e.g. on chage 
// announcements)
var Model = Cranium.Model = function (attributes) {
    this.id = _.uniqueId('model');
    this.attributes = attributes || {};
};

// Getter (accessor) method;
// returns named data item
Cranium.Model.prototype.get = function(attrName) {
    return this.attributes[attrName];
};

// Setter (mutator) method;
// Set/mix in into model mapped data (e.g.{name: "John"})
// and publishes the change event
Cranium.Model.prototype.set = function(attrs){
    if (_.isObject(attrs)) {
      _.extend(this.attributes, attrs);
      this.change(this.attributes);
    }
    return this;
};

// Returns clone of the Models data object 
// (used for view template rendering)
Cranium.Model.prototype.toJSON = function(options) {
    return _.clone(this.attributes);
};

// Helper function that announces changes to the Model
// and passes the new data
Cranium.Model.prototype.change = function(attrs){
    this.trigger(this.id + 'update', attrs);
}; 

// Mix in Event system
_.extend(Cranium.Model.prototype, Cranium.Events);
```

### Views

Views are a visual representation of models that present a filtered view of their current state. A view typically observes a model and is notified when the model changes, allowing the view to update itself accordingly. Design pattern literature commonly refers to views as 'dumb', given that their knowledge of models and controllers in an application is limited.

Let's explore Views a little further using a simple JavaScript example:

```javascript
// DOM View
var View = Cranium.View = function (options) {
  // Mix in options object (e.g extending functionallity)
  _.extend(this, options); 
  this.id = _.uniqueId('view');
};

// Mix in Event system
_.extend(Cranium.View.prototype, Cranium.Events);
```

### Controllers

Controllers are an intermediary between models and views which are classically responsible for two tasks: 

* they update the view when the model changes
* they update the model when the user manipulates the view

```javascript
// cranium.js - Cranium.Controller

// Controller tying together a model and view
var Controller = Cranium.Controller = function(options){
  // Mix in options object (e.g extending functionallity)
  _.extend(this, options); 
  this.id = _.uniqueId('controller');
  var parts, selector, eventType;

  // Parses Events object passed during the definition of the 
  // controller and maps it to the defined method to handle it;
  if(this.events){
    _.each(this.events, function(method, eventName){
      parts = eventName.split('.');
      selector = parts[0];
      eventType = parts[1];
      $(selector)['on' + eventType] = this[method];
    }.bind(this));
  }    
};
```

### Practical Usage

HTML template for the primer that follows:

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
  <script type="text/template" class="todo-template">
    <div>
      <input id="todo_complete" type="checkbox" <%= completed %>>
      <%= title %>
    </div>
  </script>
  <script src="underscore-min.js"></script>
  <script src="cranium.js"></script>
  <script src="example.js"></script>
</body>
</html>
```

Cranium.js usage:

```javascript

// example.js - usage of Cranium MVC

// And todo instance
var todo1 = new Cranium.Model({
    title: "",
    completed: ""
});

console.log("First todo title - nothing set: " + todo1.get('title'));
todo1.set({title: "Do something"});
console.log("Its changed now: " + todo1.get('title'));
''
// View instance
var todoView = new Cranium.View({
  // DOM element selector
  el: '#todo',

  // Todo template; Underscore temlating used
  template: _.template($('.todo-template').innerHTML),

  init: function (model) {
    this.render( model.toJSON() );

    this.on(model.id + 'update', this.render.bind(this));
  },
  render: function (data) {
    console.log("View about to render.");
    $(this.el).innerHTML = this.template( data );
  }
});

var todoController = new Cranium.Controller({
  // Specify the model to update
  model: todo1,

  // and the view to observe this model
  view:  todoView,
  
  events: {
    "#todo.click" : "toggleComplete"
  },

  // Initialize everything
  initialize: function () {
    this.view.init(this.model);
    return this;
  },
  // Toggles the value of the todo in the Model
  toggleComplete: function () {
    var completed = todoController.model.get('completed');
    console.log("Todo old 'completed' value?", completed);
    todoController.model.set({ completed: (!completed) ? 'checked': '' });
    console.log("Todo new 'completed' value?", todoController.model.get('completed'));
    return this;
  }
});


// Let's kick start things off
todoController.initialize();

todo1.set({ title: "Due to this change Model will notify View and it will re-render"});

```

## MVP

Model-View-Presenter (MVP) is a derivative of the MVC design pattern which focuses on improving presentation logic. It originated at a company named [Taligent](http://en.wikipedia.org/wiki/Taligent) in the early 1990s while they were working on a model for a C++ CommonPoint environment. Whilst both MVC and MVP target the separation of concerns across multiple components, there are some fundamental differences between them.

For the purposes of this summary we will focus on the version of MVP most suitable for web-based architectures.

### Models, Views & Presenters

The P in MVP stands for presenter. It's a component which contains the user-interface business logic for the view. Unlike MVC, invocations from the view are delegated to the presenter, which are decoupled from the view and instead talk to it through an interface. This allows for all kinds of useful things such as being able to mock views in unit tests.

The most common implementation of MVP is one which uses a Passive View (a view which is for all intents and purposes "dumb"), containing little to no logic. MVP models are almost identical to MVC models and handle application data. The presenter acts as a mediator which talks to both the view and model, however both of these are isolated from each other. They effectively bind models to views, a responsibility held by Controllers in MVC. Presenters are at the heart of the MVP pattern and as you can guess, incorporate the presentation logic behind views.

Solicited by a view, presenters perform any work to do with user requests and pass data back to them. In this respect, they retrieve data, manipulate it and determine how the data should be displayed in the view. In some implementations, the presenter also interacts with a service layer to persist data (models). Models may trigger events but it's the presenter's role to subscribe to them so that it can update the view. In this passive architecture, we have no concept of direct data binding. Views expose setters which presenters can use to set data.

The benefit of this change from MVC is that it increases the testability of your application and provides a more clean separation between the view and the model. This isn't however without its costs as the lack of data binding support in the pattern can often mean having to take care of this task separately.

Although a common implementation of a [Passive View](http://martinfowler.com/eaaDev/PassiveScreen.html) is for the view to implement an interface, there are variations on it, including the use of events which can decouple the View from the Presenter a little more. As we don't have the interface construct in JavaScript, we're using it more and more as a protocol than an explicit interface here. It's technically still an API and it's probably fair for us to refer to it as an interface from that perspective.


There is also a [Supervising Controller](http://martinfowler.com/eaaDev/SupervisingPresenter.html) variation of MVP, which is closer to the MVC and [MVVM - Model-View-ViewModel](http://en.wikipedia.org/wiki/Model_View_ViewModel) patterns as it provides data-binding from the Model directly from the View. Key-value observing (KVO) plugins (such as Derick Bailey's Backbone.ModelBinding plugin) introduce this idea of a Supervising Controller to Backbone.


## MVP or MVC?

MVP is generally used most often in enterprise-level applications where it's necessary to reuse as much presentation logic as possible. Applications with very complex views and a great deal of user interaction may find that MVC doesn't quite fit the bill here as solving this problem may mean heavily relying on multiple controllers. In MVP, all of this complex logic can be encapsulated in a presenter, which can simplify maintenance greatly.

As MVP views are defined through an interface and the interface is technically the only point of contact between the system and the view (other than a presenter), this pattern also allows developers to write presentation logic without needing to wait for designers to produce layouts and graphics for the application.

Depending on the implementation, MVP may be more easy to automatically unit test than MVC. The reason often cited for this is that the presenter can be used as a complete mock of the user-interface and so it can be unit tested independent of other components. In my experience this really depends on the languages you are implementing MVP in (there's quite a difference between opting for MVP for a JavaScript project over one for say, ASP.NET).

At the end of the day, the underlying concerns you may have with MVC will likely hold true for MVP given that the differences between them are mainly semantic. As long as you are cleanly separating concerns into models, views and controllers (or presenters) you should be achieving most of the same benefits regardless of the pattern you opt for.

## MVC, MVP and Backbone.js

There are very few, if any architectural JavaScript frameworks that claim to implement the MVC or MVP patterns in their classical form as many JavaScript developers don't view MVC and MVP as being mutually exclusive (we are actually more likely to see MVP strictly implemented when looking at web frameworks such as ASP.NET or GWT). This is because it's possible to have additional presenter/view logic in your application and yet still consider it a flavor of MVC.

Backbone contributor [Irene Ros](http://ireneros.com/) subscribes to this way of thinking as when she separates Backbone views out into their own distinct components, she needs something to actually assemble them for her. This could either be a controller route (such as a ```Backbone.Router```, covered later in the book) or a callback in response to data being fetched.

That said, some developers do however feel that Backbone.js better fits the description of MVP than it does MVC
. Their view is that:

* The presenter in MVP better describes the ```Backbone.View``` (the layer between View templates and the data bound to it) than a controller does
* The model fits ```Backbone.Model``` (it isn't that different from the classical MVC "Model")
* The views best represent templates (e.g Handlebars/Mustache markup templates)

A response to this could be that the view can also just be a View (as per MVC) because Backbone is flexible enough to let it be used for multiple purposes. The V in MVC and the P in MVP can both be accomplished by ```Backbone.View``` because they're able to achieve two purposes: both rendering atomic components and assembling those components rendered by other views.

We've also seen that in Backbone the responsibility of a controller is shared with both the Backbone.View and Backbone.Router and in the following example we can actually see that aspects of that are certainly true.

Here, our Backbone ```TodoView``` uses the Observer pattern to 'subscribe' to changes to a View's model in the line ```this.model.on('change',...)```. It also handles templating in the ```render()``` method, but unlike some other implementations, user interaction is also handled in the View (see ```events```).


```javascript
// The DOM element for a todo item...
app.TodoView = Backbone.View.extend({

  //... is a list tag.
  tagName:  'li',

  // Pass the contents of the todo template through a templating
  // function, cache it for a single todo
  template: _.template( $('#item-template').html() ),

  // The DOM events specific to an item.
  events: {
    'click .toggle':  'togglecompleted'
  },

  // The TodoView listens for changes to its model, re-rendering. Since there's
  // a one-to-one correspondence between a **Todo** and a **TodoView** in this
  // app, we set a direct reference on the model for convenience.
  initialize: function() {
    this.model.on( 'change', this.render, this );
    this.model.on( 'destroy', this.remove, this );
  },

  // Re-render the titles of the todo item.
  render: function() {
    this.$el.html( this.template( this.model.toJSON() ) );
    return this;
  },

  // Toggle the `"completed"` state of the model.
  togglecompleted: function() {
    this.model.toggle();
  },
});
```

Another (quite different) opinion is that Backbone more closely resembles [Smalltalk-80 MVC](http://martinfowler.com/eaaDev/uiArchs.html#ModelViewController), which we went through earlier.

As MarionetteJS author Derick Bailey has [written](http://lostechies.com/derickbailey/2011/12/23/backbone-js-is-not-an-mvc-framework/), it's ultimately best not to force Backbone to fit any specific design patterns. Design patterns should be considered flexible guides to how applications may be structured and in this respect, Backbone doesn't fit either MVC nor MVP perfectly. Instead, it borrows some of the best concepts from multiple architectural patterns and creates a flexible framework that just works well. Call it **the Backbone way**, MV* or whatever helps reference its flavor of application architecture.

It *is* however worth understanding where and why these concepts originated, so I hope that my explanations of MVC and MVP have been of help. Most structural JavaScript frameworks will adopt their own take on classical patterns, either intentionally or by accident, but the important thing is that they help us develop applications which are organized, clean and can be easily maintained.


## Upgrading to Backbone 0.9.10


*Developing Backbone.js Applications* is currently based on Backbone 0.9.2. If you are transitioning from this version to 0.9.10 or above, the following is a guide of [changes](http://backbonejs.org/#changelog) grouped by classes, where applicable. 

**Note:** We aim to update the entirety of this book to Backbone 1.0 once it has been tagged.

### Model

* Model validation is now only enforced by default in `Model#save` and is no longer enforced by default upon construction or in `Model#set`, unless the `{validate:true}` option is passed:

```javascript
var model = new Backbone.Model({name: "One"});
model.validate = function(attrs) {
  if (!attrs.name) {
    return "No thanks.";
  }
};
model.set({name: "Two"});
console.log(model.get('name'));
// 'Two'
model.unset('name', {validate: true});
// false
```

* Passing `{silent:true}` on change will no longer delay individual `"change:attr"` events, instead they are silenced entirely.

```javascript
var model = new Backbone.Model();
model.set({x: true}, {silent: true});

console.log(!model.hasChanged(0));
// true
console.log(!model.hasChanged(''));
// true
```

* The `Model#change` method has been removed, as delayed attribute changes as no longer available.

* Calling `destroy` on a Model will now return `false` if the model `isNew`.

```javascript
var model = new Backbone.Model();
console.log(model.destroy());
// false
```

* After fetching a model or a collection, all defined parse functions will now be run. So fetching a collection and getting back new models could cause both the collection to parse the list, and then each model to be parsed in turn, if you have both functions defined.

* HTTP PATCH support allows us to send only changed attributes (i.e partial updates) to the server by passing `{patch: true}` i.e `model.save(attrs, {patch: true})`.

```javascript
// Save partial using PATCH
model.clear().set({id: 1, a: 1, b: 2, c: 3, d: 4});
model.save();
model.save({b: 2, d: 4}, {patch: true});
console.log(this.syncArgs.method);
// 'patch'
```

* When using `add` on a collection, passing `{merge: true}` will now cause duplicate models to have their attributes merged in to the existing models, instead of being ignored.

```javascript
var items = new Backbone.Collection;
items.add([{ id : 1, name: "Dog" , age: 3}, { id : 2, name: "cat" , age: 2}]);
items.add([{ id : 1, name: "Bear" }], {merge: true });
items.add([{ id : 2, name: "lion" }]); // merge: false
 
console.log(JSON.stringify(items.toJSON()));
// [{"id":1,"name":"Bear","age":3},{"id":2,"name":"cat","age":2}]

```

### Collection


* While listening to a [reset](http://backbonejs.org/#Collection-reset) event, the list of previous models is now available in `options.previousModels`, for convenience.

```javascript
var model = new Backbone.Model();
var collection = new Backbone.Collection([model])
.on('reset', function(collection, options) {
  console.log(options.previousModels);
  console.log([model]);
  console.log(options.previousModels[0] === model); // true
});
collection.reset([]);
```

* `Collection#sort` now triggers a `sort` event, instead of a `reset` event.

* Removed `getByCid` from Collections. `collection.get` now supports lookup by both id and cid.

* Collections now also proxy Underscore method name aliases (`collect`, `inject`, `foldl`, `foldr`, `head`, `tail`, `take`, and so on...)

* Added `update` (which is also available as an option to fetch) for "smart" updating of sets of models.

The update method attempts to perform smart updating of a collection using a specified list of models. When a model in this list isn't present in the collection, it is added. If it is, its attributes will be merged. Models which are present in the collection but not in the list are removed.

```javascript
var theBeatles = new Collection(['john', 'paul', 'george', 'ringo']);

theBeatles.update(['john', 'paul', 'george', 'pete']);

// Fires a `remove` event for 'ringo', and an `add` event for 'pete'.
// Updates any of john, paul and georges's attributes that may have
// changed over the years.
```

* `collection.indexOf(model)` can be used to retrieve the index of a model as necessary.

```javascript
var col = new Backbone.Collection;

col.comparator = function(a, b) {
  return a.get('name') < b.get('name') ? -1 : 1;
};

var tom = new Backbone.Model({name: 'Tom'});
var rob = new Backbone.Model({name: 'Rob'});
var tim = new Backbone.Model({name: 'Tim'});

col.add(tom);
col.add(rob);
col.add(tim);

console.log(col.indexOf(rob) === 0); // true
console.log(col.indexOf(tim) === 1); // true
console.log(col.indexOf(tom) === 2); // true
  
```

### View
* `View#make` has been removed. You'll need to use `$` directly to construct DOM elements now.
* When declaring a View, `options`, `el`, `tagName`, `id` and `className` may now be defined as functions, if you want their values to be determined at runtime.

### Events

* Backbone events now support jQuery-style event maps `obj.on({click: action})`. This is clearer than needing three separate calls to `.on` and should align better with the events hash used in Views:

```javascript
 model.on({
     ‘change:name’ : this.nameChanged,
     ‘change:age’ : this.ageChanged,
     ‘change:height’ : this.heightChanges
 });
```

* The Backbone object now extends Events so that you can use it as a global event bus, if you like.

* Backbone events now supports [once](http://backbonejs.org/#Events-once), similar to Node's [once](http://nodejs.org/api/events.html#events_emitter_once_event_listener), or jQuery's [one](http://api.jquery.com/one/). A call to `once()` ensures that the callback only fires once when a notification arrives.

```javascript
// Use once rather than having to explicitly unbind
var obj = { counterA: 0, counterB: 0 };
_.extend(obj, Backbone.Events);

var incrA = function(){ obj.counterA += 1; obj.trigger('event'); };
var incrB = function(){ obj.counterB += 1; };

obj.once('event', incrA);
obj.once('event', incrB);
obj.trigger('event');

console.log(obj.counterA === 1); // true
console.log(obj.counterB === 1); // true 

```

`counterA` and `counterB` should only have been incremented once.

* Added [listenTo](http://backbonejs.org/#Events-listenTo) and [stopListening](http://backbonejs.org/#Events-stopListening) to Events. They can be used as inversion-of-control flavors of [on](http://backbonejs.org/#Events-on) and [off](http://backbonejs.org/#Events-off), for convenient unbinding of all events an object is currently listening to. `view.remove()` automatically calls `view.stopListening()`.

If you've had a chance to work on a few Backbone projects by this point, you may know that every `on` called on an object also requires an `off` to be called in order for the garbage collector to do its job. 

This can sometimes be overlooked when Views are binding to Models. In 0.9.10, this can now be done the other way around - Views can bind to Model notifications and unbind from all of them with just one call. We achieve this using `view.listenTo(model, 'eventName', func)` and `view.stopListening()`.

The default `remove()` of Views will call `stopListening()` for you, just in case you don't remember to.

```javascript
var a = _.extend({}, Backbone.Events);
var b = _.extend({}, Backbone.Events);
a.listenTo(b, 'all', function(){ console.log(true); });
b.trigger('anything');
a.listenTo(b, 'all', function(){ console.log(false); });
a.stopListening();
b.trigger('anything');
```

A more complex example (from [Just JSON](http://justjson.blogspot.co.uk/)) might require our Views to respond to "no connection" and "connection resume" events to re-fetch data on demand in an application.

In 0.9.2, we have to do this to achieve what we need:

```javascript
 // In BaseView definition
var BaseView = Backbone.View.extend({
  destroy: function() {
    // Allow child views to hook to this event to unsubscribe
    // anything they may have subscribed to to other objects.
    this.trigger('beforedestroy');
    if (this.model) {
      this.model.off(null, null, this);
    }
                
    if (this.collection) {
      this.collection.off(null, null, this);
    }

    this.remove();
    this.unbind();
  }
});

// In MyView definition. 
// We have a global EventBus that allows elements on the app to subscribe to global events.
// connection/disconnected, connection/resume is two of them.

var MyView = BaseView.extend({
  initialize: function() {
    this.on('beforedestroy', this.onBeforeDestroy, this);
    this.model.on('reset', this.onModelLoaded, this);
    EventBus.on('connection/disconnected', this.onDisconnect, this);
    EventBus.on('connection/resume', this.onConnectionResumed, this);
  },
  onModelLoaded: function() {
    // We only need this to be done once! (Kinda weird...)
    this.model.off('load', this.onModelLoaded, this);
  },
  onDisconnect: function() {
    // Figure out what state we are currently on, display View-specific messaging, etc.
  },
  onConnectionResumed: function() {
    // Re-do previous network request that failed.
  },
  onBeforeDestroy: function() {
    EventBus.off('connection/resume', this.onConnectionResumed, this);
    EventBus.off('connection/disconnected', this.onDisconnect, this);
  }
});
```

However, in 0.9.10, what we need to do is quite simple:

```javascript

// In BaseView definition
var BaseView = Backbone.View.extend({
  destroy: function() {
    this.trigger('beforedestroy');
    this.remove();
  }
});

// In MyView definition. 

var MyView = BaseView.extend({
  initialize: function() {
    this.listenTo(EventBus, 'connection/disconnected', this.onDisconnect);
    this.listenTo(EventBus, 'connection/resume', this.onConnectionResumed);
    this.once(this.model, 'load', this.onModelLoaded);
  },
  onModelLoaded: function() {
    // Don't need to unsubscribe anymore!
  },
  onDisconnect: function() {
    // Figure out the state, display messaging, etc.
  },
  onConnectionResumed: function() {
    // Re-do previous network request that failed.
  }
  // Most importantly, we no longer need onBeforeDestroy() anymore!
});

```


### Routers
* A "route" event is triggered on the router in addition to being fired on Backbone.history.

```javascript
Backbone.history.on('route', onRoute);

// Trigger 'route' event on router instance."
router.on('route', function(name, args) {
  console.log(name === 'routeEvent'); 
});

location.replace('http://example.com#route-event/x');
Backbone.history.checkUrl();
```

* For semantic and cross browser reasons, routes will now ignore search parameters. Routes like `search?query=…&page=3` should become `search/…/3`.
* Bugfix for normalizing leading and trailing slashes in the Router definitions. Their presence (or absence) should not affect behavior.
* Router URLs now support optional parts via parentheses, without having to use a regex. 

```javascript
var Router = Backbone.Router.extend({
  routes: {
    "optional(/:item)":           "optionalItem",
    "named/optional/(y:z)":       "namedOptionalItem",
  },
  …
});
```

### Sync
* For mixed-mode APIs, `Backbone.sync` now accepts [emulateHTTP](http://backbonejs.org/#Sync-emulateHTTP) and [emulateJSON](http://backbonejs.org/#Sync-emulateJSON) as inline options.

```javascript
var Library = Backbone.Collection.extend({
    url : function() { return '/library'; }
});

var attrs = {
    title  : "The Tempest",
    author : "Bill Shakespeare",
    length : 123
  };
  
library = new Library;
library.create(attrs, {wait: false});
  
// update with just emulateHTTP
library.first().save({id: '2-the-tempest', author: 'Tim Shakespeare'}, {
  emulateHTTP: true
});
    
console.log(this.ajaxSettings.url === '/library/2-the-tempest'); // true
console.log(this.ajaxSettings.type === 'POST'); // true
console.log(this.ajaxSettings.contentType === 'application/json'); // true

var data = JSON.parse(this.ajaxSettings.data);
console.log(data.id === '2-the-tempest');
console.log(data.author === 'Tim Shakespeare');
console.log(data.length === 123);

// or update with just emulateJSON

library.first().save({id: '2-the-tempest', author: 'Tim Shakespeare'}, {
  emulateJSON: true
});

console.log(this.ajaxSettings.url === '/library/2-the-tempest'); // true
console.log(this.ajaxSettings.type === 'PUT'); // true
console.log(this.ajaxSettings.contentType ==='application/x-www-form-urlencoded'); // true

var data = JSON.parse(this.ajaxSettings.data.model);
console.log(data.id === '2-the-tempest');
console.log(data.author ==='Tim Shakespeare');
console.log(data.length === 123);

```

* Consolidated `"sync"` and `"error"` events within `Backbone.sync`. They are now triggered regardless of the existence of success or error callbacks.

* Added a `"request"` event to `Backbone.sync`, which triggers whenever a request begins to be made to the server. The natural complement to the `"sync"` event. 


### Other
* Bug fix on change where attribute comparison uses `!==` instead of `_.isEqual`.
* Bug fix where an empty response from the server on save would not call the success function.
* To improve the performance of add, `options.index` will no longer be set in the `add` event callback. 
* Removed the `Backbone.wrapError` helper method. Overriding sync should work better for those particular use cases.
* To set what library Backbone uses for DOM manipulation and Ajax calls, use `Backbone.$ =` ... instead of `setDomLibrary`.
* Added a `Backbone.ajax` hook for more convenient overriding of the default use of `$.ajax`. If AJAX is too passé, set it to your preferred method for server communication.
* Validation now occurs even during `"silent"` changes. This change means that the `isValid` method has been removed. Failed validations also trigger an error, even if an error callback is specified in the options.



---
Where relevant, copyright Addy Osmani, 2012-2013.
