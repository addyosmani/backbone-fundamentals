# Introduction

Frank Lloyd Wright once said “You can’t make an architect. You can however open the doors and windows toward the light as you see it.” In this book, I hope to shed some light on how to improve the structure of your web applications, opening doors to what will hopefully be more maintainable, readable applications in your future.

The goal of all architecture is to build something well; in our case, to craft code that is enduring and delights both ourselves and the developers who will maintain our code long after we are gone. We all want our architecture to be simple, yet beautiful.

Modern JavaScript frameworks and libraries can bring structure and organization to your projects, establishing a maintainable foundation right from the start. They build on the trials and tribulations of developers who have had to work around callback chaos similar to that which you are facing now or may in the near future.

When developing applications using just jQuery, the piece missing is a way to structure and organize your code. It's very easy to create a JavaScript app that ends up a tangled mess of jQuery selectors and callbacks, all desperately trying to keep data in sync between the HTML for your UI, the logic in your JavaScript, and calls to your API for data.

Without something to help tame the mess, you're likely to string together a set of independent plugins and libraries to make up the functionality or build everything yourself from scratch and have to maintain it yourself. Backbone solves this problem for you, providing a way to cleanly organize code, separating responsibilities into recognizable pieces that are easy to maintain.

In "Developing Backbone.js Applications," I and a number of other experienced authors will show you how to improve your web application structure using the popular JavaScript library, Backbone.js

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

Backbone is mature, popular, and has both a vibrant developer community as well as a wealth of plugins and extensions available that build upon it. It has been used to create non-trivial applications by companies such as Disqus, Walmart, SoundCloud and LinkedIn.

Backbone focuses on giving you helpful methods for querying and manipulating your data rather than re-inventing the JavaScript object model. It's a library, rather than a framework, that plays well with others and scales well, from embedded widgets to large-scale applications.

As it's small, there is also less your users have to download on mobile or slower connections. The entire Backbone source can be read and understood in just a few hours.

### When Do I Need A JavaScript MVC Framework?

When building a single-page application using JavaScript, whether it involves a complex user interface or is simply trying to reduce the number of HTTP requests required for new Views, you will likely find yourself inventing many of the pieces that make up an MV* framework.

At the outset, it isn’t terribly difficult to write your own application framework that offers some opinionated way to avoid spaghetti code; however, to say that it is equally as trivial to write something as robust as Backbone would be a grossly incorrect assumption.

There’s a lot more that goes into structuring an application than tying together a DOM manipulation library, templating, and routing. Mature MV* frameworks typically include not only the pieces you would find yourself writing, but also include solutions to problems you’ll find yourself running into later on down the road. This is a time-saver that you shouldn’t underestimate the value of.

So, where will you likely need an MV* framework and where won’t you?

If you’re writing an application where much of the heavy lifting for view rendering and data manipulation will be occurring in the browser, you may find a JavaScript MV* framework useful. Examples of applications that fall into this category are GMail, NewsBlur and the LinkedIn mobile app.

These types of applications typically download a single payload containing all the scripts, stylesheets, and markup users need for common tasks and then perform a lot of additional behavior in the background. For instance, it’s trivial to switch between reading an email or document to writing one without sending a new page request to the server.

If, however, you’re building an application that still relies on the server for most of the heavy-lifting of page/view rendering and you’re just using a little JavaScript or jQuery to make things more interactive, an MV* framework may be overkill. There certainly are complex Web applications where the partial rendering of views can be coupled with a single-page application effectively, but for everything else, you may find yourself better sticking to a simpler setup.

Maturity in software (framework) development isn't simply about how long a framework has been around. It's about how solid the framework is and more importantly how well it's evolved to fill its role. Has it become more effective at solving common problems? Does it continue to improve as developers build larger and more complex applications with it?


### Why Consider Backbone.js?

Backbone provides a minimal set of data-structuring (Models, Collections) and user interface (Views, URLs) primitives that are helpful when building dynamic applications using JavaScript. It's not opinionated, meaning you have the freedom and flexibility to build the best experience for your web application how you see fit. You can either use the prescribed architecture it offers out of the box or extend it to meet your requirements.

The library doesn't focus on widgets or replacing the way you structure objects - it just supplies you with utilities for manipulating and querying data in your application. It also doesn't prescribe a specific template engine - while you are free to use the Micro-templating offered by Underscore.js (one of it's dependencies), views can bind to HTML constructed using your templating solution of choice.

Looking at the [large](http://backbonejs.org/#examples) number of applications built with Backbone, it's clear that it scales well. Backbone also works quite well with other libraries, meaning you can embed Backbone widgets in an application written with AngularJS, use it with TypeScript, or just use an individual class (like Models) as a data backer for simpler apps.

There are no performance drawbacks to using Backbone to structure your application. It avoids run loops, two-way binding, and constant polling of your data structures for updates and tries to keep things simple where possible. That said, should you wish to go against the grain, you can of course implement such things on top of it. Backbone won't stop you.

With a vibrant community of plugin and extension authors, there's a liklihood that if you're looking to achieve some behavior Backbone is lacking, a complementary project exists that works well with it. This is made simpler by Backbone offering literate documentation of its source code, allowing anyone an opportunity to easily understand what is going on behind the scenes.

Having been refined over two and a half years of development, Backbone is a mature library that will continue to offer a minimalist solution for building better web applications. I regularly use it and hope that you find it as useful an addition to your toolbelt as I have.


### Setting Expectations

The goal of this book is to create an authoritative and centralized repository of information that can help those developing real-world apps with Backbone. If you come across a section or topic which you think could be improved or expanded on, please feel free to submit an issue (or better yet, a pull-request) on the book's [GitHub site](https://github.com/addyosmani/backbone-fundamentals). It won't take long and you'll be helping other developers avoid the problems you ran into.

Topics will include MVC theory and how to build applications using Backbone's Models, Views, Collections, and Routers. I'll also be taking you through advanced topics like modular development with Backbone.js and AMD (via RequireJS), solutions to common problems like nested views, how to solve routing problems with Backbone and jQuery Mobile, and much more.

Here is a peek at what you will be learning in each chapter:

<i>Chapter 2, Fundamentals</i> traces the history of the MVC design pattern and introduces how it is implemented by Backbone.js and other JavaScript frameworks.

<i>Chapter 3, Backbone Basics</i> covers the major features of the Backbone.js core and the technologies and techniques you will need to know in order to apply it.

<i>Chapter 4, Exercise 1: Todos - Your First Backbone.js App</i> takes you step-by-step through development of a simple client-side Todo List application.

<i>Chapter 5, Exercise 2: Book Library - Your First RESTful Backbone.js App</i> walks you through development of a Book Library application which persists its model to a server using a REST API.

<i>Chapter 6, Backbone Extensions</i> describes Backbone.Marionette and Thorax, two extension frameworks which add features to Backbone.js that are useful for developing large-scale applications.

<i>Chapter 7, Common Problems and Solutions</i> reviews common issues you may encounter when using Backbone.js and ways of addressing them.

<i>Chapter 8, Modular Development</i> looks at how AMD modules and RequireJS can be used to modularize your code.

<i>Chapter 9, Exercise 3: Todos - Your First Modular Backbone + RequireJS App</i> takes you through rewriting the app created in Exercise 1 to be more modular with the help of RequireJS.

<i>Chapter 10, Paginating Backbone Requests & Collections</i> walks through how to use the Backbone.Paginator plugin to paginate data for your Collections.

<i>Chapter 11, Backbone Boilerplate And Grunt BBB</i> introduces powerful tools you can use to bootstrap a new Backbone.js application with boilerplate code.

<i>Chapter 12, Mobile Applications</i> addresses the issues that arise when using Backbone with jQuery Mobile.

<i>Chapter 13, Jasmine</i> covers how to unit test Backbone code using the Jasmine test framework.

<i>Chapter 14, QUnit</i> discusses how to use QUnit for unit testing.

<i>Chapter 15, SinonJS</i> discusses how to use SinonJS for unit testing your Backbone apps.

<i>Chapter 16, Resources</i> provides references to additional Backbone-related resources.

<i>Chapter 17, Conclusions</i> wraps up the our tour through the world of Backbone.js development.

<i>Chapter 18, Appendix</i> returns to our design pattern discussion by contrasting MVC with the Model-View-Presenter (MVP) pattern and examines how Backbone.js relates to both. A walkthrough of writing a Backbone-like library from scratch and other topics are also covered.
