# Introduction

Frank Lloyd Wright once said “You can’t make an architect. You can however open the doors and windows toward the light as you see it.” In this book, I hope to shed some light on how to improve the structure of your web applications, opening doors to what will hopefully be more maintainable, readable applications in your future.

The goal of all architecture is to build something well; in our case, to craft code that is enduring and delights both ourselves and the developers who will maintain our code long after we are gone. We all want our architecture to be simple, yet beautiful.

When writing a web application from scratch it can be easy to feel like you can get by simply relying on a DOM manipulation library (such as jQuery) and a handful of plugins. The challenge with this approach is that it doesn’t take long to get lost in a nested pile of callbacks and DOM elements without any real structure in place.

In short, you can end up with a pile of spaghetti code - code that is disorganized and difficult to follow. This type of code has no simple panacea, short of a rewrite that may end up costing both time and money to complete. Fortunately, there are ways to avoid this problem.

Modern JavaScript frameworks and libraries can bring structure and organization to your projects, establishing a maintainable foundation right from the start. They build on the trials and tribulations of developers who have had to work around callback chaos similar to that which you are facing now or may in the near future.

In "Developing Backbone.js Applications," I and a number of other experienced authors will show you how to improve your web application structure using one such library - Backbone.js.

### What Is MVC?

A number of modern JavaScript frameworks provide developers an easy path to organizing their code using variations of a pattern known as MVC (Model-View-Controller). MVC separates the concerns in an application into three parts:

* Models represent the domain-specific knowledge and data in an application. Think of this as being a ‘type’ of data you can model — like a User, Photo, or Todo note. Models can notify observers when their state changes.
* Views typically constitute the user interface in an application (e.g., markup and templates), but don’t have to be. They observe Models, but don’t directly communicate with them.
* Controllers handle input (e.g., clicks, user actions) and update Models. 

Thus, in an MVC application, user input is acted upon by Controllers which update Models. Views observe Models and update the user interface when changes occur.

JavaScript MVC frameworks don’t always strictly follow the above pattern. Some solutions (including Backbone.js) merge the responsibility of the Controller into the View, while other approaches add additional components into the mix.

For this reason we refer to such frameworks as following the MV* pattern; that is, you’re likely to have a Model and a View, but a distinct Controller might not be present and other components may come into play.

### What is Backbone.js?

![](img/backbonejsorg.jpg)

Backbone.js is a lightweight JavaScript library that adds structure to your client-side code. It makes it easy to manage and decouple concerns in your application, leaving you with code that is more maintainable in the long term.

Developers commonly use libraries like Backbone.js to create single-page applications (SPAs). SPAs are web applications that load into the browser and then react to data changes on the client side without requiring complete page refreshes from the server.
Backbone.js is a mature, popular library at the time of writing and has both a large development community online as well as a wealth of plugins and extensions available that build upon it. It has been used to create non-trivial applications by companies such as Disqus, Walmart, SoundCloud and Foursquare.

### When Do I Need A JavaScript MVC Framework?

When building a single-page application using JavaScript, whether it involves a complex user interface or is simply trying to reduce the number of HTTP requests required for new Views, you will likely find yourself inventing many of the pieces that make up an MV* framework.

At the outset, it isn’t terribly difficult to write your own application framework that offers some opinionated way to avoid spaghetti code; however, to say that it is equally as trivial to write something as robust as Backbone would be a grossly incorrect assumption.

There’s a lot more that goes into structuring an application than tying together a DOM manipulation library, templating, and routing. Mature MV* frameworks typically include not only the pieces you would find yourself writing, but also include solutions to problems you’ll find yourself running into later on down the road. This is a time-saver that you shouldn’t underestimate the value of.

So, where will you likely need an MV* framework and where won’t you?

If you’re writing an application where much of the heavy lifting for view rendering and data manipulation will be occurring in the browser, you may find a JavaScript MV* framework useful. Examples of applications that fall into this category are GMail and Google Docs.

These types of applications typically download a single payload containing all the scripts, stylesheets, and markup users need for common tasks and then perform a lot of additional behavior in the background. For instance, it’s trivial to switch between reading an email or document to writing one without sending a new page request to the server.

If, however, you’re building an application that still relies on the server for most of the heavy-lifting of page/view rendering and you’re just using a little JavaScript or jQuery to make things more interactive, an MV* framework may be overkill. There certainly are complex Web applications where the partial rendering of views can be coupled with a single-page application effectively, but for everything else, you may find yourself better sticking to a simpler setup.

Maturity in software (framework) development isn't simply about how long a framework has been around. It's about how solid the framework is and more importantly how well it's evolved to fill its role. Has it become more effective at solving common problems? Does it continue to improve as developers build larger and more complex applications with it?


### Why Consider Backbone.js?

Does the following describe you?:

"I want a flexible library which allows me to cleanly separate concerns in my application. It should support a persistence layer and RESTful sync, models, views, event-driven communication and routing. I’d like some decisions about the architecture left up to me."

As I may be building something complex, I’d like there to be an active extension community around the framework that is already addressing issues I may run into down the road. Ideally, there are also scaffolding tools available for the solution."

If so, continue reading.

Backbone's main benefits, regardless of your target platform or device, include helping:

* Organize the structure to your application
* Simplify server-side persistence
* Decouple the DOM from your page's data
* Model data, views, and routers in a succinct manner
* Provide DOM, model, and collection synchronization


### Setting Expectations

The goal of this book is to create an authoritative and centralized repository of information that can help those developing real-world apps with Backbone. If you come across a section or topic which you think could be improved or expanded on, please feel free to submit an issue (or better yet, a pull-request) on the book's [GitHub site](https://github.com/addyosmani/backbone-fundamentals). It won't take long and you'll be helping other developers avoid the problems you ran into.

Topics will include MVC theory and how to build applications using Backbone's Models, Views, Collections, and Routers. I'll also be taking you through advanced topics like modular development with Backbone.js and AMD (via RequireJS), solutions to common problems like nested views, how to solve routing problems with Backbone and jQuery Mobile, and much more.

Here is a peek at what you will be learning in each chapter:

<i>Chapter 2, Fundamentals</i> traces the history of the MVC design pattern and introduces how it is implemented by Backbone.js and other JavaScript frameworks.

<i>Chapter 3, Internals</i> covers the major features of the core Backbone.js framework and technologies and techniques you will need to know in order to apply it.

<i>Chapter 4, Exercise 1: Todos - Your First Backbone.js App</i> takes you step-by-step through development of a simple client-side Todo List application.

<i>Chapter 5, Exercise 2: Book Library - Your First RESTful Backbone.js App</i> walks you through development of a Book Library application which persists its model to a server using a REST API.

<i>Chapter 6, Backbone Extensions</i> describes Backbone.Marionette and Thorax, two extension frameworks which add features to Backbone.js that are useful for developing large-scale applications.

<i>Chapter 7, Common Problems and Solutions</i> reviews common issues you may encounter when using Backbone.js and ways of addressing them.

<i>Chapter 8, Modular Development</i> looks at how AMD modules and RequireJS can be used to modularize your code.

<i>Chapter 9, Backbone Boilerplate And Grunt BBB</i> introduces powerful tools you can use to bootstrap a new Backbone.js application with boilerplate code.


<i>Chapter 10, Mobile Applications</i> addresses the issues that arise when using Backbone with jQuery Mobile.

<i>Chapter 11, Jasmine</i> covers how to unit test Backbone code using the Jasmine test framework.

<i>Chapter 12, QUnit</i> discusses how to use the QUnit for unit testing.

<i>Chapter 13, SinonJS</i> discusses how to use SinonJS for unit testing your Backbone apps.

<i>Chapter 14, Resources</i> provides references to additional Backbone-related resources.

<i>Chapter 15, Conclusions</i> wraps up the our tour through the world of Backbone.js development.

<i>Chapter 16, Appendix</i> returns to our design pattern discussion by contrasting MVC with the Model-View-Presenter (MVP) pattern and examines how Backbone.js relates to the two patterns. It also provides useful information for existing Backbone users who may be upgrading from Backbone 0.9.2 to version 0.9.10 and beyond.
