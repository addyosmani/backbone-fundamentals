# Introduction

Frank Lloyd Wright once said “You can’t make an architect. You can however open the doors and windows toward the light as you see it”. In this book, I hope to shed some light on how to improve the structure of your web applications, opening doors to what will hopefully be more maintainable, readable applications in your future.

The goal of all architecture is to build something well - in our case, to craft code that is enduring and delights both ourselves and the developers who will maintain our code long after we are gone. We all want our architecture to be simple, yet beautiful.

When writing a web application from scratch it can be easy to feel like we can get by simply relying on a DOM manipulation library (such as jQuery) and a handful of plugins. The challenge with this approach is that it doesn’t take long to get lost in a nested pile of callbacks and DOM elements without any real structure in place.

In short, you can end up with a pile of spaghetti code - code that is disorganized and difficult to follow. This type of code has no simple panacea, short of a rewrite that may end up costing both time and money to alleviate. Fortunately, there are ways to avoid this problem.

Modern JavaScript frameworks and libraries can assist with bringing structure and organization to our projects, improving how maintainable they are from the start. They build on the trials and tribulations of developers who have had to work around similar callback chaos as you have, providing solutions to many common problems by default.

With that, in "Developing Backbone.js Applications:, I and a number of other experienced authors will take you through a journey of learning how to improve your application structure using one such library - Backbone.js.

### What Is MVC?

The modern JavaScript frameworks I mentioned a moment ago provide developers an easy path to organizing their code using variations of a pattern known as MVC (Model-View-Controller). MVC separates the concerns in an application down into three parts:

* Models represent the domain-specific knowledge and data in an application. Think of this as being a ‘type’ of data you can model — like a User, Photo or Todo note. Models should notify anyone observing them about their current state (e.g Views).
* Views are typically considered the User-interface in an application (e.g your markup and templates), but don’t have to be. They should know about the existence of Models in order to observe them, but don’t directly communicate with them.
* Controllers handle the input (e.g clicks, user actions) in an application and Views can be considered as handling the output. When a Controller updates the state of a model (such as editing Todo note content), it doesn’t directly tell the View. This is what the observing nature of the View and Model relationship is for.

JavaScript ‘MVC’ frameworks that can help us structure our code don’t always strictly follow the above pattern. Some solutions will include the responsibility of the Controller in the View (e.g Backbone.js) whilst others add their own opinionated components into the mix as they feel this is more effective.

For this reason we refer to such frameworks as following the MV* pattern, that is, you’re likely to have a View and a Model, but more likely to have something else also included.

### What is Backbone.js?

![](img/backbonejsorg.png)

Backbone.js is a lightweight JavaScript library for adding structure to your client-side code. It makes it easy to manage and decouple concerns in your application, leaving you with code that is more maintainable in the long term.

Developers commonly use libraries like Backbone.js to create single-page applications or SPAs. To put it simply, these apps enable the browser to react to changes in data on the client-side without the need to completely load up all your markup from the server, meaning no complete page-refreshes are necessary.

Backbone.js is a mature, popular library at the time of writing and has both a large development community online as well as a wealth of plugins and extensions available to build upon it. It has been used to create non-trivial applications by companies such as
Disqus, Walmart, SoundCloud and Foursquare.

### When Do I Need A JavaScript MVC Framework?

When building a single-page application using JavaScript, whether it involves a complex user interface or is simply trying to reduce the number of HTTP requests required for new Views, you will likely find yourself inventing many of the pieces that make up an MV* framework, such as Backbone.js.

At the outset, it isn’t terribly difficult to write an application framework that offers some opinionated way to avoid spaghetti code, however to say that it is equally as trivial to write something of the standard of Backbone would be a grossly incorrect assumption.

There’s a lot more that goes into structuring an application than tying together a DOM manipulation library, templating and routing. Mature MV* frameworks typically not only include many of the pieces you would find yourself writing, but also include solutions to problems you’ll find yourself running into later on down the road. This is a time-saver that you shouldn’t underestimate the value of.

So, where will you likely need an MV* framework and where won’t you?

If you’re writing an application that will likely only be communicating with an API or back-end data service, where much of the heavy lifting for viewing or manipulating that data will be occurring in the browser, you may find a JavaScript MV* framework useful.
Good examples of applications that fall into this category are GMail and Google Docs.

These applications typically download a single payload containing all the scripts, stylesheets and markup users need for common tasks and then perform a lot of additional behavior in the background. It’s trivial to switch between reading an email or document to writing one and you don’t need to ask the application to render the whole page again at all.

If, however, you’re building an application that still relies on the server for most of the heavy-lifting of Views/pages and you’re just using a little JavaScript or jQuery to make things a little more interactive, an MV framework may be overkill. There certainly are complex Web applications where the partial rendering of views can* be coupled with a single-page application effectively, but for everything else, you may find yourself better sticking to a simpler setup.

Maturity in software (framework) development isn't simply about how long a framework has been around. It's about how solid the framework is and more importantly how well it's evolved to fill its role. Has it become more effective at solving common problems? Does it continue to improve as developers build larger and more complex applications with it?


### Why Consider Backbone.js?

Does the following describe you?:

"I want something flexible which offers a minimalist solution to separating concerns in my application. It should support a persistence layer and RESTful sync, models, views (with controllers), event-driven communication, templating and routing. It should be imperative, allowing one to update the View when a model changes. I’d like some decisions about the architecture left up to me. Ideally, many large companies have used the solution to build non-trivial applications.

As I may be building something complex, I’d like there to be an active extension community around the framework that have already tried addressing larger problems (Marionette, Chaplin, Aura, Thorax). Ideally, there are also scaffolding tools (grunt-bbb, brunch) available for the solution."

If so, continue reading.

Backbone's main benefits, regardless of your target platform or device, include helping:

* Organize the structure to your application
* Simplify server-side persistence
* Decouple the DOM from your page's data
* Model data, views and routers in a succinct manner
* Provide DOM, model and collection synchronization


### Setting Expectations

The goal of this book is to create an authoritative and centralized repository of information that can help those developing real-world apps with Backbone. If you come across a section or topic which you think could be improved or expanded on, please feel free to submit a pull-request. It won't take long and you'll be helping other developers avoid problems you've run into before.

Topics will include MVC theory and how to build applications using Backbone's models, views, collections and routers. I'll also be taking you through advanced topics like modular development with Backbone.js and AMD (via RequireJS), solutions to common problems like nested views, how to solve the routing problems with Backbone and jQuery Mobile and a lot more.
