# Fundamentals

In this first chapter, we're going to explore how frameworks like Backbone.js fit in the world of JavaScript application architecture. Classically, developers creating desktop and server-class applications have had a wealth of design patterns available for them to lean on, but it's only been in the past few years that such patterns have come to client-side development.

Before exploring any JavaScript frameworks that assist in structuring applications, it can be useful to gain a basic understanding of architectural design patterns.

### MVC & Backbone.js

Design patterns are proven solutions to common development problems and can help guide us in adding more organization and structure to our applications. Patterns are also of great use as they reflect a set of practices which build upon the collective experience of skilled developers who have repeatedly solved similar problems. 

In this section, we're going to explore the MVC (Model-View-Controller) pattern and how it applies to Backbone.js.

## MVC

MVC is an architectural design pattern that encourages improved application organization through a separation of concerns. It enforces the isolation of business data (Models) from user interfaces (Views), with a third component (Controllers) traditionally managing logic, user-input and the coordination of models and views. The pattern was originally designed by [Trygve Reenskaug](http://en.wikipedia.org/wiki/Trygve_Reenskaug) while working on Smalltalk-80 (1979), where it was initially called Model-View-Controller-Editor. MVC was described in depth in [“Design Patterns: Elements of Reusable Object-Oriented Software”](http://www.amazon.co.uk/Design-patterns-elements-reusable-object-oriented/dp/0201633612) (The "GoF" or “Gang of Four” book) in 1994, which played a role in popularizing its use.


### Smalltalk-80 MVC

It's important to understand what the original MVC pattern was aiming to solve as it has changed quite heavily since the days of its origin. Back in the 70's, graphical user-interfaces were few and far between. An approach known as [Separated Presentation](http://martinfowler.com/eaaDev/uiArchs.html) began to be used as a means to make a clear division between domain objects which modeled concepts in the real world (e.g a photo, a person) and the presentation objects which were rendered to the user's screen.

The Smalltalk-80 implementation of MVC took this concept further and had an objective of separating out the application logic from the user interface. The idea was that decoupling these parts of the application would also allow the reuse of models for other interfaces in the application. There are some interesting points worth noting about Smalltalk-80's MVC architecture:

* A Domain element was known as a Model and were ignorant of the user-interface (Views and Controllers)
* Presentation was taken care of by the View and the Controller, but there wasn't just a single view and controller. A View-Controller pair was required for each element being displayed on the screen and so there was no true separation between them
* The Controller's role in this pair was handling user input (such as key-presses and click events), doing something sensible with them.
* The Observer pattern was relied upon for updating the View whenever the Model changed

Developers are sometimes surprised when they learn that the Observer pattern (nowadays commonly implemented as a Publish/Subscribe system) was included as a part of MVC's architecture decades ago. In Smalltalk-80's MVC, the View and Controller both observe the Model: anytime the Model changes, the Views react. A simple example of this is an application backed by stock market data - for the application to show real-time information, any change to the data in its Models should result in the View being refreshed instantly.

Martin Fowler has done an excellent job of writing about the [origins](http://martinfowler.com/eaaDev/uiArchs.html) of MVC over the years and if you are interested in further historical information about Smalltalk-80's MVC, I recommend reading his work.

### MVC Applied To The Web

The web heavily relies on the HTTP protocol, which is stateless. This means that there is no constantly open connection between the browser and server. Each request instantiates a new communication channel between the two. Once the request initiator (e.g. a browser) gets a response the connection is closed. This fact creates a completely different context when compared to the one of the operating systems on which many of the original MVC ideas were developed. The MVC implementation has to obey the web context. 

A typical server-side MVC implementation has one MVC stack layered behind the singe point of entry. This single point of entry means that all HTTP requests, e.g. `http://www.example.com` or `http://www.example.com/whichever-page/` etc., are routed, by a server configuration, through one point or, to be bold, one file, e.g. `index.php`.

At that point, there would be an implementation of what we call the Front Controller pattern which, once it receives it, analyzes HTTP requests and decides which class (Controller) and method (Action) are to be invoked as a response to the request. Method is the name for a function and member is name for a variable when part of the class/object.

Once invoked the Controller takes over and passes to and/or fetches data from the appropriate Model for the Action in concern. After the Controller receives the data from Model it loads the view, which corresponds to the invoked Action, injects the data into it and returns the response to the user.

For example, let say we have our blog on `www.example.com` and we want to edit an article (with `id=43`) and request `http://www.example.com/article/edit/43`:

On the server side, the Front Controller would analyze the URL and invoke the Article Controller (corresponding to the `/article/` part of the URI) and its Edit Action (corresponding to the `/edit/` part of the URI). Within the Action there would be a call to, lets say, the Articles model and its `Articles::getEntry(43)` method (43 corresponding to `/43/` in URI). This would return the blog article data from database for edit. Afterwards, the Article Controller would load the (`article/edit`) view which would include logic for injecting the article's data into the form for a user to edit its content, title and other (meta) data. Once all of this is done a response is returned to the browser.

As you can imagine, a similar flow is necessary with POST requests after we press a save button in a form. The POST action URI would look like `/article/save/43`. This time the request would go through the same Controller, but the Save Action (due to the `/save/` URI chunk), and invoked Articles Model would save edit article to the database with `Articles::saveEntry(43)` and redirect back to the `/article/edit/43` for further editing.

If the user requested `http://www.example.com/`:

On the server side the Front Controller would invoke default Controller and Action, e.g. Index Controller and its Index action. Within Index Action there would be a call to Articles model and its `Articles::getLastEntries(10)` method which would return last 10 blog posts. Afterwards, the Controller would load blog/index view which would have basic logic for listing last 10 blog posts.

We can see the larger picture of typical HTTP request lifecycle through the server side MVC in the picture below. 

![](img/webmvcflow_bacic.png)

Server receives a request and routes it through a single entry point. At the entry point, the Front Controller analyzes the request and based on it invokes the appropriate Action of the appropriate Controller. This process is called routing. The Action Model is asked to return and/or save submitted data. Model communicates with the data source (e.g. database or API etc.). Once the Model completes its work it returns data to the Controller which then loads the appropriate View. The View executes presentation logic (loops through articles and prints titles, content etc.) with the supplied data. In the end, the response is returned to the user.

Demand for fast, complex and responsive Ajax powered web applications demanded a lot of this logic to be replicated on the client side. This meant a lot more code had to exist there. Eventually it brought us to the point that we needed MVC (or a similar architecture) implemented on the client side to better structure the code and make it easier to maintain and further extend during the application life-cycle.

And, of course, JavaScript and browsers create another context which require that we bend or adjust the traditional MVC paradigm to fit it.

### MVC In The Browser

In complex JavaScript web applications, aka Single Page Applications (SPA), all application responses (e.g. UI updates) to user inputs are done seemlessly on the client-side. Data fetching and persistence (e.g. saving to database on server) are done with Ajax in the background. For silky, slick and smooth user experiences, the code powering these interacions needs to be well thought out.

Through evolution, trial and error, and a lot of spaghetti and not so spaghetti-like code developers in the end developed on ideas of traditional MVC paradigm and brought the solution for structuring JavaScript code to the landscape of the SPAs through JavaScript MVC frameworks. JavaScript now has a number of MVC frameworks, including Ember.js, JavaScriptMVC, and of course Backbone.js.

**The problem**

A typical page in an SPA consists of smaller ingredients which, when looked at at a deeper level, represent logical entities, which involve specific data domains that should be represented in a particular way on the page.

A good example is basket in an e-commerce web application which would typically have a list of items added to it and presented to the user as box in top right corner of the page (see the picture). 

![](img/wireframe_e_commerce.png)

The basket has its data and representation of the data in HTML. This data and associated view in HTML changes over time. There was a time when we used jQuery (or a similar DOM manipulation library) and a bunch of Ajax calls and callbacks to sync the two. That often produced code that was not so well structured and easy to maintain. Bugs were easy to produce and they are even unavoidable.

Eventually, an elegant way to handle it was brought to the client side throught JavaScript MVC libraries.

Now, data is handled with a Model and its HTML representation with View. So, on Model changes the View is updated and vice versa. The Controller is the component that manages this synchronization. It sends update commands both ways, to the View to update itself based on the Model change (e.g. sync with database) and to the Model based on the View changes (e.g. new item dropped into the basket). This way offers a better separation of concerns and improved code structure is accomplished.

### Simple JavaScript MVC Implementation

Let's see a simple implementation of the MVC pattern and its usage to clarify some concepts - we're going to call our little library Cranium.js. 

To simplify a bit we will rely on [Underscore](http://underscorejs.org "Underscore.js") for inheritance and templating (similar to Backbone).

#### Event System

At the heart of our JavaScript MVC implementation is an `Event` system (object) based on the Publisher-Subscriber Pattern which makes it possible for MVC components to intercommunicate in an elegant, decoupled manner. Subscribers 'listen' out for specific events of interest and react when Publishers broadcast these events.

`Event` is inherited by the View and Model components. That way each of them can inform other component that event of the interest to them happened.

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

* for View to notify its subscribers of user interaction, like click or input in a form, to update/re-render its UI etc.
* for Model once its data has changed to notify its Subscribers to update themselves (e.g. view to re-render to show accurate/updated data) etc­.

#### Models

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

#### Views

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

#### Controllers

Controllers are an intermediary between models and views which are classically responsible for two tasks: 

* they both update the view when the model changes and 
* update the model when the user manipulates the view.

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

#### Practical Usage

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

### Imlementation Specifics

#### Notes on Model

* The built-in capabilities of models vary across frameworks, however it's common for them to support validation of attributes, where attributes represent the properties of the model, such as a model identifier.

* When using models in real-world applications we generally also need a way of persisting models. Persistence allows us to edit and update models with the knowledge that their most recent states will be saved somewhere, for example in a web browser's localStorage data-store or synchronized with a database.

* A model may also have single or multiple views observing it. A developer could create a single view that displayed all Model attributes, or might create three separate views to display each attribute. The important detail is that the model doesn't care how these views are organized, it simply announces updates to its data as necessary. It does it through system like the Event System we used in our Cranium Framework.

* It is not uncommon for modern MVC/MV* frameworks to provide a means to group models together. In Backbone, these groups are called "Collections". Managing models in groups allows us to write application logic based on notifications from the group, should any model it contains change. This avoids the need to manually observe individual model instances. We'll see this in action later in the book.

* If you read older texts on MVC, you may come across a description of models as also managing application "state". In JavaScript applications state has a specific meaning, typically referring to the current state of a view or sub-view on a user's screen at a fixed time. State is a topic which is regularly discussed when looking at Single-page applications, where the concept of state needs to be simulated.

#### Notes on View

* Users interact with views, which usually means reading and editing model data. For example, in our todo application example, todo model viewing might happen in a user interface in the list of all todo items. Within it each todo is rendered with their title and completed checkbox. Model editing could be done through an "edit" view where a user who has selected a specific todo could edit its title in a form.

* We define a ```render()``` utility within our view which is responsible for rendering the contents of the ```Model``` using a JavaScript templating engine (provided by Underscore.js) and updating the contents of our view, referenced by ```el```.

* We then add our ```render()``` callback as one of ```Model``` subscribers, so that through Event System it can trigger the view to update when the model changes.

* You may wonder where user interaction comes into play here. When users click on todo element within the view, it's not the view's responsibility to know what to do next. A Controller makes this decision. In our implementation, this is achieved by adding an event listener to todo element which will delegate handling the click behavior back to the controller, passing the model information along with it in case it's needed. The benefit of this architecture is that each component plays its own separate role in making the application function as needed.

**Templating**

In the context of JavaScript frameworks that support MVC/MV*, it is worth looking more closely at JavaScript templating and its relationship to Views.

It has long been considered bad practice (and computationally expensive) to manually create large blocks of HTML markup in-memory through string concatenation. Developers using this technique often find themselves iterating through their data, wrapping it in nested divs and using outdated techniques such as ```document.write``` to inject the 'template' into the DOM. This approach often means keeping scripted markup inline with standard markup, which can quickly become difficult to read and maintain, especially when building large applications.

JavaScript templating libraries (such as Handlebars.js or Mustache) are often used to define templates for views as HTML markup containing template variables. These template blocks can be either stored externally or within script tags with a custom type (e.g 'text/template'). Variables are delimited using a variable syntax (e.g {{title}}). Javascript template libraries typically accept data in JSON, and the grunt work of populating templates with data is taken care of by the framework itself. This has a several benefits, particularly when opting to store templates externally as this can let applications load templates dynamically on an as-needed basis.

Let's compare two examples of HTML templates. One is implemented using the popular Handlebars.js library, and the other uses Underscore's 'microtemplates'.

**Handlebars.js:**

```html
<div class="view">
  <input class="toggle" type="checkbox" {{#if completed}} "checked" {{/if}}>
  <label>{{title}}</label>
  <button class="destroy"></button>
</div>
<input class="edit" value="{{title}}">
```

**Underscore.js Microtemplates:**

```html
<div class="view">
  <input class="toggle" type="checkbox" <%= completed ? 'checked' : '' %>>
  <label><%= title %></label>
  <button class="destroy"></button>
</div>
<input class="edit" value="<%= title %>">
```

You may also use double curly brackets (i.e ```{{}}```) (or any other tag you feel comfortable with) in Microtemplates. In the case of curly brackets, this can be done by setting the Underscore ```templateSettings``` attribute as follows:

```javascript
_.templateSettings = { interpolate : /\{\{(.+?)\}\}/g };
```

**A note on navigation and state**

It is also worth noting that in classical web development, navigating between independent views required the use of a page refresh. In single-page JavaScript applications, however, once data is fetched from a server via Ajax, it can be dynamically rendered in a new view within the same page. Since this doesn't automatically update the URL, the role of navigation thus falls to a "router", which assists in managing application state (e.g allowing users to bookmark a particular view they have navigated to). As routers are however neither a part of MVC nor present in every MVC-like framework, I will not be going into them in greater detail in this section.

#### Notes on Controller


In our Todo application, a controller would be responsible for handling changes the user made in the edit view for a particular todo, updating a specific todo model when a user has finished editing.

It's with controllers that most JavaScript MVC frameworks depart from this interpretation of the MVC pattern. The reasons for this vary, but in my opinion, Javascript framework authors likely initially looked at server-side interpretations of MVC (such as Ruby on Rails), realized that that approach didn't translate 1:1 on the client-side, and so re-interpreted the C in MVC to solve their state management problem. This was a clever approach, but it can make it hard for developers coming to MVC for the first time to understand both the classical MVC pattern and the "proper" role of controllers in other JavaScript frameworks.

So does Backbone.js have Controllers? Not really. Backbone's Views typically contain "controller" logic, and Routers are used to help manage application state, but neither are true Controllers according to classical MVC.

In this respect, contrary to what might be mentioned in the official documentation or in blog posts, Backbone isn't truly an MVC framework. It's in fact better to see it a member of the MV* family which approaches architecture in its own way. There is of course nothing wrong with this, but it is important to distinguish between classical MVC and MV* should you be relying on discussions of MVC to help with your Backbone projects.

## What does MVC give us?

To summarize, the separation of concerns in MVC facilitates modularization of an application's functionality and enables:

* Easier overall maintenance. When updates need to be made to the application it is clear whether the changes are data-centric, meaning changes to models and possibly controllers, or merely visual, meaning changes to views.
* Decoupling models and views means that it's straight-forward to write unit tests for business logic
* Duplication of low-level model and controller code is eliminated across the application
* Depending on the size of the application and separation of roles, this modularity allows developers responsible for core logic and developers working on the user-interfaces to work simultaneously


### Delving deeper

Right now, you likely have a basic understanding of what the MVC pattern provides, but for the curious, we'll explore it a little further.

The GoF (Gang of Four) do not refer to MVC as a design pattern, but rather consider it a "set of classes to build a user interface". In their view, it's actually a variation of three other classical design patterns: the Observer (Pub/Sub), Strategy and Composite patterns. Depending on how MVC has been implemented in a framework, it may also use the Factory and Decorator patterns. I've covered some of these patterns in my other free book, JavaScript Design Patterns For Beginners if you would like to read into them further.

As we've discussed, models represent application data, while views handle what the user is presented on screen. As such, MVC relies on Publish/Subscribe for some of its core communication (something that surprisingly isn't covered in many articles about the MVC pattern). When a model is changed it "publishes" to the rest of the application that it has been updated. The "subscriber"--generally a Controller--then updates the view accordingly. The observer-viewer nature of this relationship is what facilitates multiple views being attached to the same model.

For developers interested in knowing more about the decoupled nature of MVC (once again, depending on the implementation), one of the goals of the pattern is to help define one-to-many relationships between a topic and its observers. When a topic changes, its observers are updated. Views and controllers have a slightly different relationship. Controllers facilitate views to respond to different user input and are an example of the Strategy pattern.


### Summary

Having reviewed the classical MVC pattern, you should now understand how it allows developers to cleanly separate concerns in an application. You should also now appreciate how JavaScript MVC frameworks may differ in their interpretation of MVC, and how they share some of the fundamental concepts of the original pattern.

When reviewing a new JavaScript MVC/MV* framework, remember - it can be useful to step back and consider how it's opted to approach Models, Views, Controllers or other alternatives, as this can better help you grok how the framework expects to be used.

### Further reading

If you are interested in learning more about the variation of MVC which Backbone.js is better categorized under, please see the MVP (Model-View-Presenter) section in the appendix.


## Fast facts

### Backbone.js

* Core components: Model, View, Collection, Router. Enforces its own flavor of MV*
* Used by large companies such as SoundCloud and Foursquare to build non-trivial applications
* Event-driven communication between views and models. As we'll see, it's relatively straight-forward to add event listeners to any attribute in a model, giving developers fine-grained control over what changes in the view
* Supports data bindings through manual events or a separate Key-value observing (KVO) library
* Support for RESTful interfaces out of the box, so models can be easily tied to a backend
* Extensive eventing system. It's [trivial](http://lostechies.com/derickbailey/2011/07/19/references-routing-and-the-event-aggregator-coordinating-views-in-backbone-js/) to add support for pub/sub in Backbone
* Prototypes are instantiated with the ```new``` keyword, which some developers prefer
* Agnostic about templating frameworks, however Underscore's micro-templating is available by default. Backbone works well with libraries like Handlebars
* Doesn't support deeply nested models, though there are Backbone plugins such as [Backbone-relational](https://github.com/PaulUithol/Backbone-relational) which can help
* Clear and flexible conventions for structuring applications. Backbone doesn't force usage of all of its components and can work with only those needed.
