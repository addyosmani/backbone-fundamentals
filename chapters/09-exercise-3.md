# Exercise 3: Your First Modular Backbone + RequireJS App

In this chapter, we'll look at our first practical Backbone & RequireJS project - how to build a modular Todo application. Similar to exercise 1, the application will allow us to add new todos, edit new todos and clear todo items that have been marked as completed. For a more advanced practical, see the section on mobile Backbone development.

The complete code for the application can can be found in the `practicals/modular-todo-app` folder of this repo (thanks to Thomas Davis and J&eacute;r&ocirc;me Gravel-Niquet). Alternatively grab a copy of my side-project [TodoMVC](https://github.com/addyosmani/todomvc) which contains the sources to both AMD and non-AMD versions.

### Overview

Writing a modular Backbone application can be a straight-forward process. There are however, some key conceptual differences to be aware of if opting to use AMD as your module format of choice:

* As AMD isn't a standard native to JavaScript or the browser, it's necessary to use a script loader (such as RequireJS or curl.js) in order to support defining components and modules using this module format. As we've already reviewed, there are a number of advantages to using the AMD as well as RequireJS to assist here.
* Models, views, controllers and routers need to be encapsulated *using* the AMD-format. This allows each component of our Backbone application to cleanly manage dependencies (e.g collections required by a view) in the same way that AMD allows non-Backbone modules to.
* Non-Backbone components/modules (such as utilities or application helpers) can also be encapsulated using AMD. I encourage you to try developing these modules in such a way that they can both be used and tested independent of your Backbone code as this will increase their ability to be re-used elsewhere.

Now that we've reviewed the basics, let's take a look at developing our application. For reference, the structure of our app is as follows:

```
index.html
...js/
    main.js
    .../models
            todo.js
    .../views
            app.js
            todos.js
    .../collections
            todos.js
    .../templates
            stats.html
            todos.html
    ../libs
        .../backbone
        .../jquery
        .../underscore
        .../require
                require.js
                text.js
...css/
```

### Markup

The markup for the application is relatively simple and consists of three primary parts: an input section for entering new todo items (`create-todo`), a list section to display existing items (which can also be edited in-place) (`todo-list`) and finally a section summarizing how many items are left to be completed (`todo-stats`).

```
<div id="todoapp">

      <div class="content">

        <div id="create-todo">
          <input id="new-todo" placeholder="What needs to be done?" type="text" />
          <span class="ui-tooltip-top">Press Enter to save this task</span>
        </div>

        <div id="todos">
          <ul id="todo-list"></ul>
        </div>

        <div id="todo-stats"></div>

      </div>

</div>
```

The rest of the tutorial will now focus on the JavaScript side of the practical.

### Configuration options

If you've read the earlier chapter on AMD, you may have noticed that explicitly needing to define each dependency a Backbone module (view, collection or other module) may require with it can get a little tedious. This can however be improved.

In order to simplify referencing common paths the modules in our application may use, we use a RequireJS [configuration object](http://requirejs.org/docs/api.html#config), which is typically defined as a top-level script file. Configuration objects have a number of useful capabilities, the most useful being mode name-mapping. Name-maps are basically a key:value pair, where the key defines the alias you wish to use for a path and the value represents the true location of the path.

In the code-sample below, you can see some typical examples of common name-maps which include: `backbone`, `underscore`, `jquery` and depending on your choice, the RequireJS `text` plugin, which assists with loading text assets like templates.

**main.js**

```javascript
require.config({
  baseUrl:'../',
  paths: {
    jquery: 'libs/jquery/jquery-min',
    underscore: 'libs/underscore/underscore-min',
    backbone: 'libs/backbone/backbone-optamd3-min',
    text: 'libs/require/text'
  }
});

require(['views/app'], function(AppView){
  var app_view = new AppView;
});
```

The `require()` at the end of our main.js file is simply there so we can load and instantiate the primary view for our application (`views/app.js`). You'll commonly see both this and the configuration object included in most top-level script files for a project.

In addition to offering name-mapping, the configuration object can be used to define additional properties such as `waitSeconds` - the number of seconds to wait before script loading times out and `locale`, should you wish to load up i18n bundles for custom languages. The `baseUrl` is simply the path to use for module lookups.

For more information on configuration objects, please feel free to check out the excellent guide to them in the [RequireJS docs](http://requirejs.org/docs/api.html#config).


### Modularizing our models, views and collections

Before we dive into AMD-wrapped versions of our Backbone components, let's review a sample of a non-AMD view. The following view listens for changes to its model (a Todo item) and re-renders if a user edits the value of the item.

```javascript
var TodoView = Backbone.View.extend({

    //... is a list tag.
    tagName:  'li',

    // Cache the template function for a single item.
    template: _.template($('#item-template').html()),

    // The DOM events specific to an item.
    events: {
      'click .check'              : 'toggleDone',
      'dblclick div.todo-content' : 'edit',
      'click span.todo-destroy'   : 'clear',
      'keypress .todo-input'      : 'updateOnEnter'
    },

    // The TodoView listens for changes to its model, re-rendering. Since there's
    // a one-to-one correspondence between a **Todo** and a **TodoView** in this
    // app, we set a direct reference on the model for convenience.
    initialize: function() {
      this.model.on('change', this.render, this);
      this.model.view = this;
    },
    ...
```

Note how for templating the common practice of referencing a script by an ID (or other selector) and obtaining its value is used. This of course requires that the template being accessed is implicitly defined in our markup. The following is the 'embedded' version of our template being referenced above:

```
<script type="text/template" id="item-template">
      <div class="todo <%= done ? 'done' : '' %>">
        <div class="display">
          <input class="check" type="checkbox" <%= done ? 'checked="checked"' : '' %> />
          <div class="todo-content"></div>
          <span class="todo-destroy"></span>
        </div>
        <div class="edit">
          <input class="todo-input" type="text" value="" />
        </div>
      </div>
</script>
```

Whilst there is nothing wrong with the template itself, once we begin to develop larger applications requiring multiple templates, including them all in our markup on page-load can quickly become both unmanageable and come with performance costs. We'll look at solving this problem in a minute.

Let's now take a look at the AMD-version of our view. As discussed earlier, the 'module' is wrapped using AMD's `define()` which allows us to specify the dependencies our view requires. Using the mapped paths to 'jquery' etc. simplifies referencing common dependencies and instances of dependencies are themselves mapped to local variables that we can access (e.g 'jquery' is mapped to `$`).

**views/todo.js**

```javascript
define([
  'jquery',
  'underscore',
  'backbone',
  'text!templates/todos.html'
  ], function($, _, Backbone, todosTemplate){
  var TodoView = Backbone.View.extend({

    //... is a list tag.
    tagName:  'li',

    // Cache the template function for a single item.
    template: _.template(todosTemplate),

    // The DOM events specific to an item.
    events: {
      'click .check'              : 'toggleDone',
      'dblclick div.todo-content' : 'edit',
      'click span.todo-destroy'   : 'clear',
      'keypress .todo-input'      : 'updateOnEnter'
    },

    // The TodoView listens for changes to its model, re-rendering. Since there's
    // a one-to-one correspondence between a **Todo** and a **TodoView** in this
    // app, we set a direct reference on the model for convenience.
    initialize: function() {
      this.model.on('change', this.render, this);
      this.model.view = this;
    },

    // Re-render the contents of the todo item.
    render: function() {
      this.$el.html(this.template(this.model.attributes));
      this.setContent();
      return this;
    },

    // Use `jQuery.text` to set the contents of the todo item.
    setContent: function() {
      var content = this.model.get('content');
      this.$('.todo-content').text(content);
      this.input = this.$('.todo-input');
      this.input.on('blur', this.close);
      this.input.val(content);
    },
    ...
```

 From a maintenance perspective, there's nothing logically different in this version of our view, except for how we approach templating.

Using the RequireJS text plugin (the dependency marked `text`), we can actually store all of the contents for the template we looked at earlier in an external file (todos.html).

**templates/todos.html**

```html
<div class="todo <%= done ? 'done' : '' %>">
    <div class="display">
      <input class="check" type="checkbox" <%= done ? 'checked="checked"' : '' %> />
      <div class="todo-content"></div>
      <span class="todo-destroy"></span>
    </div>
    <div class="edit">
      <input class="todo-input" type="text" value="" />
    </div>
</div>
```

There's no longer a need to be concerned with IDs for the template as we can map its contents to a local variable (in this case `todosTemplate`). We then simply pass this to the Underscore.js templating function `_.template()` the same way we normally would have the value of our template script.

Next, let's look at how to define models as dependencies which can be pulled into collections. Here's an AMD-compatible model module, which has two default values: a `content` attribute for the content of a Todo item and a boolean `done` state, allowing us to trigger whether the item has been completed or not.

**models/todo.js**

```javascript
define(['underscore', 'backbone'], function(_, Backbone) {
  var TodoModel = Backbone.Model.extend({

    // Default attributes for the todo.
    defaults: {
      // Ensure that each todo created has `content`.
      content: 'empty todo...',
      done: false
    },

    initialize: function() {
    },

    // Toggle the `done` state of this todo item.
    toggle: function() {
      this.save({done: !this.get('done')});
    },

    // Remove this Todo from *localStorage* and delete its view.
    clear: function() {
      this.destroy();
      this.view.remove();
    }

  });
  return TodoModel;
});
```

As per other types of dependencies, we can easily map our model module to a local variable (in this case `Todo`) so it can be referenced as the model to use for our `TodosCollection`. This collection also supports a simple `done()` filter for narrowing down Todo items that have been completed and a `remaining()` filter for those that are still outstanding.

**collections/todos.js**

```javascript
define([
  'underscore',
  'backbone',
  'libs/backbone/localstorage',
  'models/todo'
  ], function(_, Backbone, Store, Todo){

    var TodosCollection = Backbone.Collection.extend({

    // Reference to this collection's model.
    model: Todo,

    // Save all of the todo items under the `todos` namespace.
    localStorage: new Store('todos'),

    // Filter down the list of all todo items that are finished.
    done: function() {
      return this.filter(function(todo){ return todo.get('done'); });
    },

    // Filter down the list to only todo items that are still not finished.
    remaining: function() {
      return this.without.apply(this, this.done());
    },
    ...
```

In addition to allowing users to add new Todo items from views (which we then insert as models in a collection), we ideally also want to be able to display how many items have been completed and how many are remaining. We've already defined filters that can provide us this information in the above collection, so let's use them in our main application view.

**views/app.js**

```javascript
define([
  'jquery',
  'underscore',
  'backbone',
  'collections/todos',
  'views/todo',
  'text!templates/stats.html'
  ], function($, _, Backbone, Todos, TodoView, statsTemplate){

  var AppView = Backbone.View.extend({

    // Instead of generating a new element, bind to the existing skeleton of
    // the App already present in the HTML.
    el: $('#todoapp'),

    // Our template for the line of statistics at the bottom of the app.
    statsTemplate: _.template(statsTemplate),

    // ...events, initialize() etc. can be seen in the complete file

    // Re-rendering the App just means refreshing the statistics -- the rest
    // of the app doesn't change.
    render: function() {
      var done = Todos.done().length;
      this.$('#todo-stats').html(this.statsTemplate({
        total:      Todos.length,
        done:       Todos.done().length,
        remaining:  Todos.remaining().length
      }));
    },
    ...
```

Above, we map the second template for this project, `templates/stats.html` to `statsTemplate` which is used for rendering the overall `done` and `remaining` states. This works by simply passing our template the length of our overall Todos collection (`Todos.length` - the number of Todo items created so far) and similarly the length (counts) for items that have been completed (`Todos.done().length`) or are remaining (`Todos.remaining().length`).

The contents of our `statsTemplate` can be seen below. It's nothing too complicated, but does use ternary conditions to evaluate whether we should state there's "1 item" or "2 item<i>s</i>" in a particular state.

```
<% if (total) { %>
        <span class="todo-count">
          <span class="number"><%= remaining %></span>
          <span class="word"><%= remaining == 1 ? 'item' : 'items' %></span> left.
        </span>
      <% } %>
      <% if (done) { %>
        <span class="todo-clear">
          <a href="#">
            Clear <span class="number-done"><%= done %></span>
            completed <span class="word-done"><%= done == 1 ? 'item' : 'items' %></span>
          </a>
        </span>
      <% } %>
```



The rest of the source for the Todo app mainly consists of code for handling user and application events, but that rounds up most of the core concepts for this practical.

To see how everything ties together, feel free to grab the source by cloning this repo or browse it [online](https://github.com/addyosmani/backbone-fundamentals/tree/master/practicals/modular-todo-app) to learn more. I hope you find it helpful!

**Note:** While this first practical doesn't use a build profile as outlined in the chapter on using the RequireJS optimizer, we will be using one in the section on building mobile Backbone applications.

## Route-based module loading

This section will discuss a route based approach to module loading as implemented in [Lumbar](http://walmartlabs.github.com/lumbar) by Kevin Decker. Like RequireJS, Lumbar is also a modular build system, but the pattern it implements for loading routes may be used with any build system.

The specifics of the Lumbar build tool are not discussed in this book. To see a complete Lumbar based project with the loader and build system see [Thorax](http://thoraxjs.org) which provides boilerplate projects for various environments including Lumbar.

### JSON-based module configuration

RequireJS defines dependencies per file, while Lumbar defines a list of files for each module in a central JSON configuration file, outputting a single JavaScript file for each defined module. Lumbar requires that each module (except the base module) define a single router and a list of routes. An example file might look like:

     {
        "modules": {
            "base": {
                "scripts": [
                    "js/lib/underscore.js",
                    "js/lib/backbone.js",
                    "etc"
                ]
            },
            "pages": {
                "scripts": [
                    "js/routers/pages.js",
                    "js/views/pages/index.js",
                    "etc"
                ],
                "routes": {
                    "": "index",
                    "contact": "contact"
                }
            }
        }
    }

Every JavaScript file defined in a module will have a `module` object in scope which contains the `name` and `routes` for the module. In `js/routers/pages.js` we could define a Backbone router for our `pages` module like so:

    new (Backbone.Router.extend({
        routes: module.routes,
        index: function() {},
        contact: function() {}
    }));

### Module loader Router

A little used feature of `Backbone.Router` is its ability to create multiple routers that listen to the same set of routes. Lumbar uses this feature to create a router that listens to all routes in the application. When a route is matched, this master router checks to see if the needed module is loaded. If the module is already loaded, then the master router takes no action and the router defined by the module will handle the route. If the needed module has not yet been loaded, it will be loaded, then `Backbone.history.loadUrl` will be called. This reloads the route, causes the master router to take no further action and the router defined in the freshly loaded module to respond.

A sample implementation is provided below. The `config` object would need to contain the data from our sample configuration JSON file above, and the `loader` object would need to implement `isLoaded` and `loadModule` methods. Note that Lumbar provides all of these implementations, the examples are provided to create your own implementation.

    // Create an object that will be used as the prototype
    // for our master router
    var handlers = {
        routes: {}
    };

    _.each(config.modules, function(module, moduleName) {
        if (module.routes) {
            // Generate a loading callback for the module
            var callbackName = "loader_" moduleName;
            handlers[callbackName] = function() {
                if (loader.isLoaded(moduleName)) {
                    // Do nothing if the module is loaded
                    return;
                } else {
                    //the module needs to be loaded
                    loader.loadModule(moduleName, function() {
                        // Module is loaded, reloading the route
                        // will trigger callback in the module's
                        // router
                        Backbone.history.loadUrl();
                    });
                }
            };
            // Each route in the module should trigger the
            // loading callback
            _.each(module.routes, function(methodName, route) {
                handlers.routes[route] = callbackName;
            });
        }
    });

    // Create the master router
    new (Backbone.Router.extend(handlers));

### Using NodeJS to handle pushState

`window.history.pushState` support (serving Backbone routes without a hash mark) requires that the server be aware of what URLs your Backbone application will handle, since the user should be able to enter the app at any of those routes (or hit reload after navigating to a pushState URL).

Another advantage to defining all routes in a single location is that the same JSON configuration file provided above could be loaded by the server, listening to each route. A sample implementation in NodeJS and Express:

    var fs = require('fs'),
        _ = require('underscore'),
        express = require('express'),
        server = express(),
        config = JSON.parse(fs.readFileSync('path/to/config.json'));

    _.each(config.modules, function(module, moduleName) {
        if (module.routes) {
            _.each(module.routes, function(methodName, route) {
                server.get(route, function(req, res) {
                      res.sendFile('public/index.html');
                });
            });
        }
    });

This assumes that index.html will be serving out your Backbone application. The `Backbone.History` object can handle the rest of the routing logic as long as a `root` option is specified. A sample configuration for a simple application that lives at the root might look like:

    Backbone.history || (Backbone.history = new Backbone.History());
    Backbone.history.start({
      pushState: true,
      root: '/'
    });


## An asset package alternative for dependency management

For more than trivial views, DocumentCloud have a home-built asset packager called [Jammit](https://github.com/documentcloud/jammit), which has easy integration with Underscore.js templates and can also be used for dependency management.

Jammit expects your JavaScript templates (JST) to live alongside any ERB templates you're using in the form of .jst files. It packages the templates into a global JST object which can be used to render templates into strings. Making Jammit aware of your templates is straight-forward - just add an entry for something like `views/**/*.jst` to your app package in assets.yml.

To provide Jammit dependencies you simply write out an assets.yml file that either listed the dependencies in order or used a combination of free capture directories (for example: /**/*.js, templates/*.js, and specific files). 

A template using Jammit can derive it's data from the collection object that is passed to it:

```
this.$el.html(JST.myTemplate({ collection: this.collection }));
```
