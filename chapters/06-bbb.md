# Backbone Boilerplate And Grunt-BBB

[Backbone Boilerplate](https://github.com/tbranyen/backbone-boilerplate/) is an excellent set of best practices and utilities for building Backbone.js applications, created by Backbone contributor [Tim Branyen](https://github.com/tbranyen). He organized this boilerplate out of the gotchas, pitfalls and common tasks he ran into over a year of heavily using Backbone to build apps at Bocoup. This includes apps such [StartupDataTrends.com](http://startupdatatrends.com).

With scaffolding and built in build tasks that take care of minification, concatentation, server, template compilation and more, Backbone Boilerplate (and sister project [Grunt-BBB](https://github.com/backbone-boilerplate/grunt-bbb)) are an excellent choice for developers of all levels. I heavily recommend using them as they will give you an enormous start when it comes to getting setup for development. They also have some great inline documentation which is also another excellent time-saver.

By default, Backbone Boilerplate provides you with:

* Backbone, [Lodash](https://github.com/bestiejs/lodash) (an [Underscore.js](http://underscorejs.org/) alternative) and [jQuery](http://jquery.com) with an [HTML5 Boilerplate](http://html5boilerplate.com) foundation
* Boilerplate module code
* A Windows/Mac/Linux build tool for template precompilation and, concatenation & minification of all your libraries, application code and CSS
* Scaffolding support (via grunt-bbb - [B]ackbone [B]oilerplate [B]uild) so you have to spend minimal time writing boilerplate for modules, collections and so on.
* A Lightweight node.js webserver
* Numerous other Backbone.js snippets for making your life easier

## Getting Started

### Backbone Boilerplate

We can use Boilerplate to easily begin creating an application, but first, we'll need to install it. This can be done by grabbing the latest version of it by cloning the Boilerplate repo directly:

```shell
$ git clone git://github.com/tbranyen/backbone-boilerplate.git
```

or alternatively, just fetching the latest tarball as follows:

```shell
$ curl -C - -O https://github.com/tbranyen/backbone-boilerplate/zipball/master
```

### Grunt-BBB

As Tim covers in the Boilerplate docs, we have to install [Grunt](http://gruntjs.com) if we want to use the build tools and grunt-bbb helpers he recommends.

Grunt is an excellent Node-based JavaScript build tool by another [Bocoup](http://bocoup.com) developer ([Ben Alman](http://benalman.com)). Think of it as similar to [Ant](http://ant.apache.org/) or [Rake](https://github.com/jimweirich/rake). The grunt-bbb helper is also useful to have as it provides several Backbone-specific utilities for scaffolding out your project, without the need to write boilerplate yourself.

To install grunt and grunt-bbb via NPM:

```shell
# first run
$ npm install -g grunt

# followed by
$ npm install -g bbb

# finally create a new project
$ bbb init
```

That's it. We should now be good to go.

A typical workflow for using grunt-bbb, which we can use later on is:

* Initialize a new project (`bbb init`)
* Add new modules and templates (`bbb init:module`)
* Develop using the built in server (`bbb server`)
* Run the build tool (`bbb build`)
* Deploy and map to production assets (using `bbb release`)

## Creating a new project

Let's create a new folder for our project and run `bbb init` to kick things off. If everything has been correctly installed, this will sub out some project directories and files for us. Let's review what is generated.

### index.html

This is a fairly standard stripped-down HTML5 Boilerplate foundation with the notable exception of including [RequireJS](http://requirejs.org) at the bottom of the page.

```html
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
  <meta name="viewport" content="width=device-width,initial-scale=1">

  <title>Backbone Boilerplate</title>

  <!-- Application styles. -->
  <link rel="stylesheet" href="/assets/css/index.css">
</head>

<body>
  <!-- Main container. -->
  <div role="main" id="main"></div>

  <!-- Application source. -->
  <script data-main="/app/config" src="/assets/js/libs/require.js"></script>
</body>
</html>
```

RequireJS is an [AMD](https://github.com/amdjs/amdjs-api/wiki/AMD) (Asynchronous Module Definition) module and script loader, which will assist us with managing the modules in our application. We'll be covering it in a lot more detail later on in the book, but for now, let's cover at a high-level what this particular block does:

```
<script data-main="app/config" src="/assets/js/libs/require.js"></script>
```

The `data-main` attribute is used to inform RequireJS to load `app/config.js` (a configuration object) after it has finished loading itself. You'll notice that we've omitted the `.js` extension here as RequireJS can automatically add this for us, however it will respect your paths if we do choose to include it regardless. Let's now look at the config file being referenced.

### config.js

A RequireJS configuration object allows us to specify aliases and paths for dependencies we're likely to reference often (e.g jQuery), bootstrap properties like our base application URL and `shim` libraries that don't support AMD natively.

This is what the config file in Backbone Boilerplate looks like:

```javascript
// Set the RequireJS configuration for your application.
require.config({

  // Initialize the application with the main application file.
  deps: ['main'],

  paths: {
    // JavaScript folders.
    libs: '../assets/js/libs',
    plugins: '../assets/js/plugins',

    // Libraries.
    jquery: '../assets/js/libs/jquery',
    lodash: '../assets/js/libs/lodash',
    backbone: '../assets/js/libs/backbone'
  },

  shim: {
    // Backbone library depends on lodash and jQuery.
    backbone: {
      deps: ['lodash', 'jquery'],
      exports: 'Backbone'
    },

    // Backbone.LayoutManager depends on Backbone.
    'plugins/backbone.layoutmanager': ['backbone']
  }

});
```

The first option defined in the above config is `deps: ['main']`. This informs RequireJS to load up our main.js file, which is considered the entry point for our application. You may notice that we haven't specified any other path information for `main`.

This is because as we haven't overridden the path to our scripts using the `baseUrl` option, Require will infer this using the path from our `data-main` attribute in index.html. In other words, our `baseUrl` is `app/` and any scripts we require will be loaded relative to this location.

The next block is `paths`, which we can use to specify paths relative to the `baseUrl` as well as the paths/aliases to dependencies we're likely to regularly reference.

```javascript
  paths: {
    // JavaScript folders.
    libs: '../assets/js/libs',
    plugins: '../assets/js/plugins',

    // Libraries.
    jquery: '../assets/js/libs/jquery',
    lodash: '../assets/js/libs/lodash',
    backbone: '../assets/js/libs/backbone'
  },
```

Next we have the `shim` config:

```javascript
  shim: {
    // Backbone library depends on lodash and jQuery.
    backbone: {
      deps: ['lodash', 'jquery'],
      exports: 'Backbone'
    },

    // Backbone.LayoutManager depends on Backbone.
    'plugins/backbone.layoutmanager': ['backbone']
  }
```

`shim` is an important part of our RequireJS configuration which allows us to load libraries which are not AMD compliant. The basic idea here is that rather than requiring all libraries to implement support for AMD, the `shim` takes care of the hard work for us.

For example, in the block below, we state that Backbone.js is dependent on Lodash (a fork of Underscore.js) and jQuery being loaded before it. Once they've been loaded, we then use the global export `Backbone` as the module value.

```javascript
    backbone: {
      deps: ['lodash', 'jquery'],
      exports: 'Backbone'
    }
```

Finally, we inform RequireJS that the Backbone [LayoutManager](https://github.com/tbranyen/backbone.layoutmanager) plugin (a template and layout manager, also included) requires that Backbone be loaded before it should be.

```javascript
    // Backbone.LayoutManager depends on Backbone.
    'plugins/backbone.layoutmanager': ['backbone']
```

This entire setup ensures that our scripts correctly get loaded in the order in which we expect.

### main.js

Next, we have `main.js`, which defines the entry point for our application. We use a global `require()` method to load an array any other scripts needed, such as our application `app.js` and our main router `router.js`. Note that most of the time, we will only use `require()` for bootstrapping an application and a similar method called `define()` for all other purposes.

The function defined after our array of dependencies is a callback which doesn't fire until these scripts have loaded. Notice how we're able to locally alias references to "app" and "router" as `app` and `Router` for convenience.

```javascript
require([
  // Application.
  'app',

  // Main Router.
  'router'
],

function(app, Router) {

  // Define your master router on the application namespace and trigger all
  // navigation from this instance.
  app.router = new Router();

  // Trigger the initial route and enable HTML5 History API support, set the
  // root folder to '/' by default.  Change in app.js.
  Backbone.history.start({ pushState: true, root: app.root });

  // All navigation that is relative should be passed through the navigate
  // method, to be processed by the router. If the link has a `data-bypass`
  // attribute, bypass the delegation completely.
  $(document).on('click', 'a:not([data-bypass])', function(evt) {
    // Get the absolute anchor href.
    var href = $(this).attr('href');

    // If the href exists and is a hash route, run it through Backbone.
    if (href && href.indexOf('#') === 0) {
      // Stop the default event to ensure the link will not cause a page
      // refresh.
      evt.preventDefault();

      // `Backbone.history.navigate` is sufficient for all Routers and will
      // trigger the correct events. The Router's internal `navigate` method
      // calls this anyways.  The fragment is sliced from the root.
      Backbone.history.navigate(href, true);
    }
  });

});
```

Inline, Backbone Boilerplate includes boilerplate code for initializing our router with HTML5 History API support and handling other navigation scenarios, so we don't have to.

### app.js

Let us now look at our `app.js` module. Typically, in non-Backbone Boilerplate applications, an `app.js` file may contain the core logic or module references needed to kick start an app.

In this case however, this file is used to define templating and layout configuration options as well as utilities for consuming layouts. To a beginner, this might look like a lot of code to comprehend, but the good news is that for basic apps, you're unlikely to need to heavily modify this. Instead, you'll be more concerned with modules for your app, which we'll look at next.

```javascript
define([

  // Libraries.
  'jquery',
  'lodash',
  'backbone',

  // Plugins.
  'plugins/backbone.layoutmanager'

],

function($, _, Backbone) {

  // Provide a global location to place configuration settings and module
  // creation.
  var app = {
    // The root path to run the application.
    root: '/'
  };

  // Localize or create a new JavaScript Template object.
  var JST = window.JST = window.JST || {};

  // Configure LayoutManager with Backbone Boilerplate defaults.
  Backbone.LayoutManager.configure({
    paths: {
      layout: 'app/templates/layouts/',
      template: 'app/templates/'
    },

    fetch: function(path) {
      path = path + '.html';

      if (!JST[path]) {
        $.ajax({ url: app.root + path, async: false }).then(function(contents) {
          JST[path] = _.template(contents);
        });
      }

      return JST[path];
    }
  });

  // Mix Backbone.Events, modules, and layout management into the app object.
  return _.extend(app, {
    // Create a custom object with a nested Views object.
    module: function(additionalProps) {
      return _.extend({ Views: {} }, additionalProps);
    },

    // Helper for using layouts.
    useLayout: function(name) {
      // If already using this Layout, then don't re-inject into the DOM.
      if (this.layout && this.layout.options.template === name) {
        return this.layout;
      }

      // If a layout already exists, remove it from the DOM.
      if (this.layout) {
        this.layout.remove();
      }

      // Create a new Layout.
      var layout = new Backbone.Layout({
        template: name,
        className: 'layout ' + name,
        id: 'layout'
      });

      // Insert into the DOM.
      $('#main').empty().append(layout.el);

      // Render the layout.
      layout.render();

      // Cache the reference.
      this.layout = layout;

      // Return the reference, for chainability.
      return layout;
    }
  }, Backbone.Events);

});
```

### Creating Backbone Boilerplate Modules

Not to be confused with simply being just an AMD module, a Backbone Boilerplate `module` is a script composed of a:

* Model
* Collection
* Views (optional)

We can easily create a new Boilerplate module using `grunt-bbb` once again using `init`:

```shell
# Create a new module
$ bbb init:module

# Grunt prompt
Please answer the following:
[?] Module Name foo
[?] Do you need to make any changes to the above before continuing? (y/N) n

Writing app/modules/foo.js...OK

Initialized from template "module".
```

This will generate a module `foo.js` as follows:

```javascript
define([
  // Application.
  'app'
],

// Map dependencies from above array.
function(app) {

  // Create a new module.
  var Foo = app.module();

  // Default model.
  Foo.Model = Backbone.Model.extend({

  });

  // Default collection.
  Foo.Collection = Backbone.Collection.extend({
    model: Foo.Model
  });

  // Return the module for AMD compliance.
  return Foo;

});

```

Notice how boilerplate code for our model and collection has already been written for us, as well as code for consuming the layout utilities defined in `app.js`.

Now, you may be wondering where or how Views fit into this setup. Although Backbone Boilerplate doesn't include Views in its generated modules by default, we can easily add them ourselves as needed.

e.g:

```javascript
define([
  // Application.
  'app',

  // Views
  'modules/foo/views'
],

// Map dependencies from above array.
function(app, Views) {

  // Create a new module.
  var Foo = app.module();

  // Default model.
  Foo.Model = Backbone.Model.extend({

  });

  // Default collection.
  Foo.Collection = Backbone.Collection.extend({
    model: Foo.Model
  });

  // Default views
  Foo.Views = Views;

  // Return the module for AMD compliance.
  return Foo;

});
```

Optionally, we may also wish to include references to plugins such as the Backbone LocalStorage or Offline adapters. One clean way of including a plugin in the above boilerplate could be:

```javascript
define([
  'app',

  // Libs
  'backbone',

  // Views
  'modules/foo/views',

  // Plugins
  'plugins/backbone-localstorage'
],

function(app, Backbone, Views) {
  // Create a new module.
  var Foo = app.module();

  // Default model.
  Foo.Model = Backbone.Model.extend({

  });

  // Default collection.
  Foo.Collection = Backbone.Collection.extend({
    model: Foo.Model,

    // Save all of the items under the `"foo"` namespace.
    localStorage: new Store('foo-backbone'),
  });

  // Default views
  Foo.Views = Views;

  // Return the module for AMD compliance.
  return Foo;
});
```

You may have spotted that in our module sample we're using the plural, "Views", rather than just View. This is because a View module can contain references to as many Views as needed. In the above, our `/modules/foo/views.js` file may look as follows:

```javascript
define([
  'app',

  // Libs
  'backbone'
],

function(app, Backbone) {

  var Views = {};

  Views.Bar = Backbone.View.extend({
    template: 'foo/bar',
    tagName: 'li',
    ...
   });

  Views.Baz = Backbone.View.extend({
    template: 'foo/baz',
    tagName: 'li',
    ...
   });

   return Views;

});
```

Where the `template` references in our Views, correspond to files in the `app/templates` directory. e.g `foo/bar` is located at `app/templates/foo/bar.html` and is a HTML template that can contain Lodash/Underscore.js Micro-templating logic.


### router.js

Finally, let's look at our application router, used for handling navigation. The default router Backbone Boilerplate generates for us inclues sane defaults for no routes being specified.

```javascript
define([
  // Application.
  'app'
],

function(app) {

  // Defining the application router, you can attach sub routers here.
  var Router = Backbone.Router.extend({
    routes: {
      '': 'index'
    },

    index: function() {

    }
  });

  return Router;

});
```

If however we would like to execute some module-specific logic, when the page loads (i.e when a user hits the default route), we can pull in a module as a dependency and optionally use the Backbone LayoutManager to attach Views to our layout as follows:

```javascript
define([
  // Application.
  'app',

  // Modules
  'modules/foo'
],

function(app, Foo) {

  // Defining the application router, you can attach sub routers here.
  var Router = Backbone.Router.extend({
    routes: {
      '': 'index'
    },

    index: function() {
            // Create a new Collection
            var collection = new Foo.Collection();

            // Use and configure a 'main' layout
            app.useLayout('main').setViews({
                    // Attach the bar View into the content View
                    '.bar': new Foo.Views.Bar({
                            collection: collection
                    })
             }).render();
    }
  });

  // Fetch data (e.g from localStorage)
  collection.fetch();

  return Router;

});
```

## Conclusions

In this section we reviewed Backbone Boilerplate and learned how to use the BBB tool to help us scaffold out our application.

If you would like to learn more about how this project helps structure your app, BBB includes some built-in boilerplate sample apps that can be easily generated for review.

These include a boilerplate tutorial project (`bbb init:tutorial`) and an implementation of my [TodoMVC](http://todomvc) project (`bbb init:todomvc`). I recommend checking these out as they'll provide you with a more complete picture of how Backbone Boilerplate, its templates and so on fit into the overall setup for a web app.

For more about Grunt-BBB, remember to take a look at the official project [repository](https://github.com/backbone-boilerplate/grunt-bbb). There is also a related [slide-deck](https://dl.dropbox.com/u/79007/talks/Modern_Web_Applications/slides/index.html) available for those interested in reading more.

## Related Tools & Projects

As we've seen, scaffolding tools can assist in expediting how quickly you can begin a new application by creating the basic files required for a project automatically. If you appreciate such tools, I'm happy to also recommend checking out [Yeoman](http://yeoman.io) (one of my upcoming projects) and [Brunch](https://github.com/brunch/brunch).

Brunch works very well with Backbone, Underscore, jQuery and CoffeeScript and is even used by companies such as Red Bull and Jim Beam. You may have to update any third party dependencies (e.g. latest jQuery or Zepto) when using it, but other than that it should be fairly stable to use right out of the box.

Brunch can be installed via the nodejs package manager and is easy to get started with. If you happen to use Vim or Textmate as your editor of choice, you'll be happy to know that there are Brunch bundles available for both.
