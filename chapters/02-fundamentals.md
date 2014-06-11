# Fundamentals

Design patterns are proven solutions to common development problems that can help us improve the organization and structure of our applications. By using patterns, we benefit from the collective experience of skilled developers who have repeatedly solved similar problems.

Historically, developers creating desktop and server-class applications have had a wealth of design patterns available for them to lean on, but it's only been in the past few years that such patterns have been applied to client-side development.

In this chapter, we're going to explore the evolution of the Model-View-Controller (MVC) design pattern and get our first look at how Backbone.js allows us to apply this pattern to client-side development.

## MVC

MVC is an architectural design pattern that encourages improved application organization through a separation of concerns. It enforces the isolation of business data (Models) from user interfaces (Views), with a third component (Controllers) traditionally managing logic, user-input, and coordination of Models and Views. The pattern was originally designed by [Trygve Reenskaug](http://en.wikipedia.org/wiki/Trygve_Reenskaug) while working on Smalltalk-80 (1979), where it was initially called Model-View-Controller-Editor. MVC was described in depth in [“Design Patterns: Elements of Reusable Object-Oriented Software”](http://www.amazon.co.uk/Design-patterns-elements-reusable-object-oriented/dp/0201633612) (The "GoF" or “Gang of Four” book) in 1994, which played a role in popularizing its use.


### Smalltalk-80 MVC

It's important to understand the issues that the original MVC pattern was aiming to solve as it has changed quite heavily since the days of its origin. Back in the 70's, graphical user-interfaces were few and far between. An approach known as [Separated Presentation](http://martinfowler.com/eaaDev/uiArchs.html) began to be used as a means to make a clear division between domain objects which modeled concepts in the real world (e.g., a photo, a person) and the presentation objects which were rendered to the user's screen.

The Smalltalk-80 implementation of MVC took this concept further and had an objective of separating out the application logic from the user interface. The idea was that decoupling these parts of the application would also allow the reuse of Models for other interfaces in the application. There are some interesting points worth noting about Smalltalk-80's MVC architecture:

* A Domain element was known as a Model and was ignorant of the user-interface (Views and Controllers)
* Presentation was taken care of by the View and the Controller, but there wasn't just a single View and Controller. A View-Controller pair was required for each element being displayed on the screen and so there was no true separation between them
* The Controller's role in this pair was handling user input (such as key-presses and click events) and doing something sensible with them
* The Observer pattern was used to update the View whenever the Model changed

Developers are sometimes surprised when they learn that the Observer pattern (nowadays commonly implemented as a Publish/Subscribe system) was included as a part of MVC's architecture decades ago. In Smalltalk-80's MVC, the View and Controller both observe the Model: anytime the Model changes, the Views react. A simple example of this is an application backed by stock market data - for the application to show real-time information, any change to the data in its Model should result in the View being refreshed instantly.

Martin Fowler has done an excellent job of writing about the [origins](http://martinfowler.com/eaaDev/uiArchs.html) of MVC over the years and if you are interested in further historical information about Smalltalk-80's MVC, I recommend reading his work.

### MVC Applied To The Web

The web heavily relies on the HTTP protocol, which is stateless. This means that there is not a constantly open connection between the browser and server; each request instantiates a new communication channel between the two. Once the request initiator (e.g. a browser) gets a response the connection is closed. This fact creates a completely different context when compared to the one of the operating systems on which many of the original MVC ideas were developed. The MVC implementation has to conform to the web context.

An example of a server-side web application framework which tries to apply MVC to the web context is [Ruby On Rails](http://guides.rubyonrails.org/).

![](img/rails_mvc.png)

At its core are the three MVC components we would expect - the Model, View and Controller architecture. In Rails:

* Models represent the data in an application and are typically used to manage rules for interacting with a specific database table. You generally have one table corresponding to one model with much of your application's business logic living within these models.
* Views represent your user interface, often taking the form of HTML that will be sent down to the browser. They're used to present application data to anything making requests from your application.
* Controllers offer the glue between models and views. Their responsibility is to process requests from the browser, ask your models for data and then supply this data to views so that they may be presented to the browser.

Although there's a clear separation of concerns that is MVC-like in Rails, it is actually using a different pattern called [Model2](http://en.wikipedia.org/wiki/Model2). One reason for this is that Rails does not notify views from the model or controllers - it just passes model data directly to the view.

That said, even for the server-side workflow of receiving a request from a URL, baking out an HTML page as a response and separating your business logic from your interface has many benefits. In the same way that keeping your UI cleanly separate from your database records is useful in server-side frameworks, it's equally as useful to keep your UI cleanly separated from your data models in JavaScript (as we will read more about shortly).

Other server-side implementations of MVC (such as the PHP [Zend](http://zend.com) framework) also implement the [Front Controller](http://en.wikipedia.org/wiki/Front_Controller_pattern) design pattern. This pattern layers an MVC stack behind a single point of entry. This single point of entry means that all HTTP requests (e.g., `http://www.example.com`, `http://www.example.com/whichever-page/`, etc.) are routed by the server's configuration to the same handler, independent of the URI.

When the Front Controller receives an HTTP request it analyzes it and decides which class (Controller) and method (Action) to invoke.  The selected Controller Action takes over and interacts with the appropriate Model to fulfill the request. The Controller receives data back from the Model, loads an appropriate View, injects the Model data into it, and returns the response to the browser.

For example, let's say we have our blog on `www.example.com` and we want to edit an article (with `id=43`) and request `http://www.example.com/article/edit/43`:

On the server side, the Front Controller would analyze the URL and invoke the Article Controller (corresponding to the `/article/` part of the URI) and its Edit Action (corresponding to the `/edit/` part of the URI). Within the Action there would be a call to, let's say, the Articles Model and its `Articles::getEntry(43)` method (43 corresponding to the `/43` at the end of the URI). This would return the blog article data from the database for editing. The Article Controller would then load the (`article/edit`) View which would include logic for injecting the article's data into a form suitable for editing its content, title, and other (meta) data. Finally, the resulting HTML response would be returned to the browser.

As you can imagine, a similar flow is necessary with POST requests after we press a save button in a form. The POST action URI would look like `/article/save/43`. The request would go through the same Controller, but this time the Save Action would be invoked (due to the `/save/` URI chunk), the Articles Model would save the edited article to the database with `Articles::saveEntry(43)`, and the browser would be redirected to the `/article/edit/43` URI for further editing.

Finally, if the user requested `http://www.example.com/` the Front Controller would invoke the default Controller and Action; e.g., the Index Controller and its Index action. Within Index Action there would be a call to the Articles model and its `Articles::getLastEntries(10)` method which would return the last 10 blog posts. The Controller would load the blog/index View which would have basic logic for listing the blog posts.

The picture below shows this typical HTTP request/response lifecycle for server-side MVC:

![](img/webmvcflow_bacic.png)

The Server receives an HTTP request and routes it through a single entry point. At that entry point, the Front Controller analyzes the request and based on it invokes an Action of the appropriate Controller. This process is called routing. The Action Model is asked to return and/or save submitted data. The Model communicates with the data source (e.g., database or API). Once the Model completes its work it returns data to the Controller which then loads the appropriate View. The View executes presentation logic (loops through articles and prints titles, content, etc.) using the supplied data. In the end, an HTTP response is returned to the browser.

### Client-Side MVC & Single Page Apps

Several [studies](http://radar.oreilly.com/2009/07/velocity-making-your-site-fast.html) have confirmed that improvements to latency can have a positive impact on the usage and user engagement of sites and apps. This is at odds with the traditional approach to web app development which is very server-centric, requiring a complete page reload to move from one page to the next. Even with heavy caching in place, the browser still has to parse the CSS, JavaScript, and HTML and render the interface to the screen.

In addition to resulting in a great deal of duplicated content being served back to the user, this approach affects both latency and the general responsiveness of the user experience. A trend to improve perceived latency in the past few years has been to move towards building Single Page Applications (SPAs) - apps which after an initial page load are able to handle subsequent navigations and requests for data without the need for a complete reload.

When a user navigates to a new view, additional content required for the view is requested using an XHR (XMLHttpRequest), typically communicating with a server-side REST API or endpoint. [Ajax](https://en.wikipedia.org/wiki/Ajax_(programming)) (Asynchronous JavaScript and XML) makes communication with the server asynchronous so that data is transferred and processed in the background, allowing the user to interact with other parts of a page without interruption. This improves usability and responsiveness.

SPAs can also take advantage of browser features like the [History API](http://diveintohtml5.info/history.html) to update the address seen in the location bar when moving from one view to another. These URLs also make it possible to bookmark and share a particular application state, without the need to navigate to completely new pages.

The typical SPA consists of smaller pieces of interface representing logical entities, all of which have their own UI, business logic and data. A good example is a basket in a shopping web application which can have items added to it. This basket might be presented to the user in a box in the top right corner of the page (see the picture below):

![](img/wireframe_e_commerce.png)

The basket and its data are presented in HTML. The data and its associated View in HTML changes over time. There was a time when we used jQuery (or a similar DOM manipulation library) and a bunch of Ajax calls and callbacks to keep the two in sync. That often produced code that was not well-structured or easy to maintain. Bugs were frequent and perhaps even unavoidable.

The need for fast, complex, and responsive Ajax-powered web applications demands replication of a lot of this logic on the client side, dramatically increasing the size and complexity of the code residing there. Eventually this has brought us to the point where we need MVC (or a similar architecture) implemented on the client side to better structure the code and make it easier to maintain and further extend during the application life-cycle.

Through evolution and trial and error, JavaScript developers have harnessed the power of the traditional MVC pattern, leading to the development of several MVC-inspired JavaScript frameworks, such as Backbone.js.


### Client-Side MVC - Backbone Style

Let's take our first look at how Backbone.js brings the benefits of MVC to client-side development using a Todo application as our example. We will build on this example in the coming chapters when we explore Backbone's features but for now we will just focus on the core components' relationships to MVC.

Our example will need a div element to which we can attach a list of Todo's. It will also need an HTML template containing a placeholder for a Todo item title and a completion checkbox which can be instantiated for Todo item instances. These are provided by the following HTML:

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
      <input id="todo_complete" type="checkbox" <%= completed ? 'checked="checked"' : '' %> />
      <%= title %>
    </div>
  </script>
  <script src="jquery.js"></script>
  <script src="underscore.js"></script>
  <script src="backbone.js"></script>
  <script src="demo.js"></script>
</body>
</html>
```

In our Todo application (demo.js), Backbone Model instances are used to hold the data for each Todo item:

```javascript
// Define a Todo Model
var Todo = Backbone.Model.extend({
  // Default todo attribute values
  defaults: {
    title: '',
    completed: false
  }
});

// Instantiate the Todo Model with a title, with the completed attribute
// defaulting to false
var myTodo = new Todo({
  title: 'Check attributes property of the logged models in the console.'
});
```

Our Todo Model extends Backbone.Model and simply defines default values for two data attributes. As you will discover in the upcoming chapters, Backbone Models provide many more features but this simple Model illustrates that first and foremost a Model is a data container.

Each Todo instance will be rendered on the page by a TodoView:

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

  // Called when the view is first created
  initialize: function() {
    this.$el = $('#todo');
    // Later we'll look at:
    // this.listenTo(someCollection, 'all', this.render);
    // but you can actually run this example right now by
    // calling todoView.render();
  },

  // Re-render the titles of the todo item.
  render: function() {
    this.$el.html( this.todoTpl( this.model.attributes ) );
    // $el here is a reference to the jQuery element
    // associated with the view, todoTpl is a reference
    // to an Underscore template and model.attributes
    // contains the attributes of the model.
    // Altogether, the statement is replacing the HTML of
    // a DOM element with the result of instantiating a
    // template with the model's attributes.
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

// create a view for a todo
var todoView = new TodoView({model: myTodo});
```

TodoView is defined by extending Backbone.View and is instantiated with an associated Model. In our example, the ```render()``` method uses a template to construct the HTML for the Todo item which is placed inside an li element. Each call to ```render()``` will replace the content of the li element using the current Model data. Thus, a View instance renders the content of a DOM element using the attributes of an associated Model. Later we will see how a View can bind its ```render()``` method to Model change events, causing the View to re-render whenever the Model changes.

So far, we have seen that Backbone.Model implements the Model aspect of MVC and Backbone.View implements the View. However, as we noted earlier, Backbone departs from traditional MVC when it comes to Controllers - there is no Backbone.Controller!

Instead, the Controller responsibility is addressed within the View. Recall that Controllers respond to requests and perform appropriate actions which may result in changes to the Model and updates to the View. In a single-page application, rather than having requests in the traditional sense, we have events. Events can be traditional browser DOM events (e.g., clicks) or internal application events such as Model changes.

In our TodoView, the ```events``` attribute fulfills the role of the Controller configuration, defining how events occurring within the View's DOM element are to be routed to event-handling methods defined in the View.

While in this instance events help us relate Backbone to the MVC pattern, we will see them playing a much larger role in our SPA applications. Backbone.Event is a fundamental Backbone component which is mixed into both Backbone.Model and Backbone.View, providing them with rich event management capabilities. Note that the traditional controller role (Smalltalk-80 style) is performed by the template, not by the Backbone.View.

This completes our first encounter with Backbone.js. The remainder of this book will explore the many features which build on these simple constructs. Before moving on, let's take a look at common features of JavaScript MV* libraries and frameworks.

### Implementation Specifics

A SPA is loaded into the browser using a normal HTTP request and response. The page may simply be an HTML file, as in our example above, or it could be a view constructed by a server-side MVC implementation.

Once loaded, a client-side Router intercepts URLs and invokes client-side logic in place of sending a new request to the server. The picture below shows typical request handling for client-side MVC as implemented by Backbone:

![](img/backbone_mvc.png)

URL routing, DOM events (e.g., mouse clicks), and Model events (e.g., attribute changes) all trigger handling logic in the View. The handlers update the DOM and Models, which may trigger additional events. Models are synced with Data Sources which may involve communicating with back-end servers.

#### Models

* The built-in capabilities of Models vary across frameworks; however, it's common for them to support validation of attributes, where attributes represent the properties of the Model, such as a Model identifier.

* When using Models in real-world applications we generally also need a way of persisting Models. Persistence allows us to edit and update Models with the knowledge that their most recent states will be saved somewhere, for example in a web browser's localStorage data-store or synchronized with a database.

* A Model may have multiple Views observing it for changes. By *observing* we mean that a View has registered an interest in being informed whenever an update is made to the Model. This allows the View to ensure that what is displayed on screen is kept in sync with the data contained in the model. Depending on your requirements, you might create a single View displaying all Model attributes, or create separate Views displaying different attributes. The important point is that the Model doesn't care how these Views are organized, it simply announces updates to its data as necessary through the framework's event system.

* It is not uncommon for modern MVC/MV* frameworks to provide a means of grouping Models together. In Backbone, these groups are called Collections. Managing Models in groups allows us to write application logic based on notifications from the group when a Model within the group changes. This avoids the need to manually observe individual Model instances. We'll see this in action later in the book. Collections are also useful for performing any aggregate computations across more than one model.


#### Views

* Users interact with Views, which usually means reading and editing Model data. For example, in our Todo application, Todo Model viewing happens in the user interface in the list of all Todo items. Within it, each Todo is rendered with its title and completed checkbox. Model editing is done through an "edit" View where a user who has selected a specific Todo edits its title in a form.

* We define a ```render()``` utility within our View which is responsible for rendering the contents of the ```Model``` using a JavaScript templating engine (provided by Underscore.js) and updating the contents of our View, referenced by ```this.$el```.

* We then add our ```render()``` callback as a Model subscriber, so the View can be triggered to update when the Model changes.

* You may wonder where user interaction comes into play here. When users click on a Todo element within the View, it's not the View's responsibility to know what to do next. A Controller makes this decision. In Backbone, this is achieved by adding an event listener to the Todo's element which delegates handling of the click to an event handler.

**Templating**

In the context of JavaScript frameworks that support MVC/MV*, it is worth looking more closely at JavaScript templating and its relationship to Views.

It has long been considered bad practice (and computationally expensive) to manually create large blocks of HTML markup in-memory through string concatenation. Developers using this technique often find themselves iterating through their data, wrapping it in nested divs and using outdated techniques such as ```document.write``` to inject the 'template' into the DOM. This approach often means keeping scripted markup inline with standard markup, which can quickly become difficult to read and maintain, especially when building large applications.

JavaScript templating libraries (such as Mustache or Handlebars.js) are often used to define templates for Views as HTML markup containing template variables. These template blocks can be either stored externally or within script tags with a custom type (e.g 'text/template'). Variables are delimited using a variable syntax (e.g `<%= title %>` for Underscore and `{{title}}` for Handlebars).

JavaScript template libraries typically accept data in a number of formats, including JSON; a serialisation format that is always a string. The grunt work of populating templates with data is generally taken care of by the framework itself. This has several benefits, particularly when opting to store templates externally which enables applications to load templates dynamically on an as-needed basis.

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

**A note on Navigation and State**

It is also worth noting that in classical web development, navigating between independent views required the use of a page refresh. In single-page JavaScript applications, however, once data is fetched from a server via Ajax, it can be dynamically rendered in a new view within the same page. Since this doesn't automatically update the URL, the role of navigation thus falls to a "router", which assists in managing application state (e.g., allowing users to bookmark a particular view they have navigated to). As routers are neither a part of MVC nor present in every MVC-like framework, I will not be going into them in greater detail in this section.

#### Controllers


In our Todo application, a Controller would be responsible for handling changes the user made in the edit View for a particular Todo, updating a specific Todo Model when a user has finished editing.

It's with Controllers that most JavaScript MVC frameworks depart from the traditional interpretation of the MVC pattern. The reasons for this vary, but in my opinion, JavaScript framework authors likely initially looked at server-side interpretations of MVC (such as Ruby on Rails), realized that the approach didn't translate 1:1 on the client-side, and so re-interpreted the C in MVC to solve their state management problem. This was a clever approach, but it can make it hard for developers coming to MVC for the first time to understand both the classical MVC pattern and the "proper" role of Controllers in other JavaScript frameworks.

So does Backbone.js have Controllers? Not really. Backbone's Views typically contain "Controller" logic, and Routers are used to help manage application state, but neither are true Controllers according to classical MVC.

In this respect, contrary to what might be mentioned in the official documentation or in blog posts, Backbone isn't truly an MVC library. It's in fact better to see it a member of the MV* family which approaches architecture in its own way. There is of course nothing wrong with this, but it is important to distinguish between classical MVC and MV* should you be relying on discussions of MVC to help with your Backbone projects.

## What does MVC give us?

To summarize, the MVC pattern helps you keep your application logic separate from your user interface, making it easier to change and maintain both. Thanks to this separation of logic, it is more clear where changes to your data, interface, or business logic need to be made and for what your unit tests should be written.

### Delving Deeper into MVC

Right now, you likely have a basic understanding of what the MVC pattern provides, but for the curious, we'll explore it a little further.

The GoF (Gang of Four) do not refer to MVC as a design pattern, but rather consider it a "set of classes to build a user interface." In their view, it's actually a variation of three other classical design patterns: the Observer (Publish/Subscribe), Strategy, and Composite patterns. Depending on how MVC has been implemented in a framework, it may also use the Factory and Decorator patterns. I've covered some of these patterns in my other free book, "JavaScript Design Patterns For Beginners" if you would like to read about them further.

As we've discussed, Models represent application data, while Views handle what the user is presented on screen. As such, MVC relies on Publish/Subscribe for some of its core communication (something that surprisingly isn't covered in many articles about the MVC pattern). When a Model is changed it "publishes" to the rest of the application that it has been updated. The "subscriber," generally a Controller, then updates the View accordingly. The observer-viewer nature of this relationship is what facilitates multiple Views being attached to the same Model.

For developers interested in knowing more about the decoupled nature of MVC (once again, depending on the implementation), one of the goals of the pattern is to help define one-to-many relationships between a topic and its observers. When a topic changes, its observers are updated. Views and Controllers have a slightly different relationship. Controllers facilitate Views' responses to different user input and are an example of the Strategy pattern.

### Summary

Having reviewed the classical MVC pattern, you should now understand how it allows developers to cleanly separate concerns in an application. You should also now appreciate how JavaScript MVC frameworks may differ in their interpretation of MVC, and how they share some of the fundamental concepts of the original pattern.

When reviewing a new JavaScript MVC/MV* framework, remember - it can be useful to step back and consider how it's opted to approach Models, Views, Controllers or other alternatives, as this can better help you understand how the framework is intended to be used.

### Further reading

If you are interested in learning more about the variation of MVC which Backbone.js uses, please see the MVP (Model-View-Presenter) section in the appendix.

## Fast facts

### Backbone.js

* Core components: Model, View, Collection, Router. Enforces its own flavor of MV*
* Event-driven communication between Views and Models. As we'll see, it's relatively straight-forward to add event listeners to any attribute in a Model, giving developers fine-grained control over what changes in the View
* Supports data bindings through manual events or a separate Key-value observing (KVO) library
* Support for RESTful interfaces out of the box, so Models can be easily tied to a backend
* Extensive eventing system. It's [trivial](http://lostechies.com/derickbailey/2011/07/19/references-routing-and-the-event-aggregator-coordinating-views-in-backbone-js/) to add support for pub/sub in Backbone
* Prototypes are instantiated with the ```new``` keyword, which some developers prefer
* Agnostic about templating frameworks, however Underscore's micro-templating is available by default
* Clear and flexible conventions for structuring applications. Backbone doesn't force usage of all of its components and can work with only those needed

### Used by

*Disqus*

Disqus chose Backbone.js to power the latest version of its commenting widget. The Disqus team felt it was the right choice for their distributed web app, given Backbone's small footprint and ease of extensibility.

![](img/disqus.png)


*Khan Academy*

Offering a web app that aims to provide free world-class education to anyone anywhere, Khan Academy uses Backbone to keep its frontend code both modular and organized.

![](img/khan-academy.png)


*MetaLab*

MetaLab created Flow, a task management app for teams using Backbone. Its workspace uses Backbone to create task views, activities, accounts, tags and more.

![](img/flow.png)

*Walmart Mobile*

Walmart chose Backbone to power its mobile web applications, creating two new extension frameworks in the process - Thorax and Lumbar. We'll be discussing both of these later in the book.

![](img/walmart-mobile.png)


*Airbnb*

Airbnb developed its mobile web app using Backbone and now uses it across many of its products.

![](img/airbnb.png)


*Code School*

Code School's course challenge app is built from the ground up using Backbone, taking advantage of all the pieces it has to offer: routers, collections, models and complex event handling.

![](img/code-school.png)

