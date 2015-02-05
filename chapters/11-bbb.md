# Backbone Boilerplate And Grunt-BBB

Boilerplates provide us a starting point for working on projects. They're a base for building upon using the minimum required code to get something functional put together. When you're working on a new Backbone application, a new Model typically only takes a few lines of code to get working.

That alone probably isn't enough however, as you'll need a Collection to group those models, a View to render them and perhaps a router if you're looking to making specific views of your Collection data bookmarkable. If you're starting on a completely fresh project, you may also need a build process in place to produce an optimized version of your app that can be pushed to production.

This is where boilerplate solutions are useful. Rather than having to manually write out the initial code for each piece of your Backbone app, a boilerplate could do this for you, also ideally taking care of the build process.

[Backbone Boilerplate](https://github.com/tbranyen/backbone-boilerplate/) (or just BB) provides just this. It is an excellent set of best practices and utilities for building Backbone.js applications, created by Backbone contributor [Tim Branyen](https://github.com/tbranyen). He took the the gotchas, pitfalls and common tasks he ran into while heavily using Backbone to build apps and crafted BB as a result of this experience.

[Grunt-BBB or Boilerplate Build Buddy](https://github.com/backbone-boilerplate/grunt-bbb) is the companion tool to BB, which offers scaffolding, file watcher and build capabilities. Used together with BB it provides an excellent base for quickly starting new Backbone applications.

![](img/bbb.png)

Out of the box, BB and Grunt-BBB provide us with:

* Backbone, [Lodash](https://github.com/bestiejs/lodash) (an [Underscore.js](http://underscorejs.org/) alternative) and [jQuery](http://jquery.com) with an [HTML5 Boilerplate](http://html5boilerplate.com) foundation
* Boilerplate and scaffolding support, allowing us to spend minimal time writing boilerplate for modules, collections and so on.
* A build tool for template pre-compilation and, concatenation & minification of all our libraries, application code and stylesheets
* A Lightweight node.js webserver

Notes on build tool steps:

* Template pre-compilation: using a template library such as Underscore micro-templating or Handlebars.js generally involves three steps: (1) reading a raw template, (2) compiling it into a JavaScript function and (3) running the compiled template with your desired data. Precompiling eliminates the second step from runtime, by moving this process into a build step.
* Concatenation is the process of combining a number of assets (in our case, script files) into a single (or fewer number) of files to reduce the number of HTTP requests required to obtain them.
* Minification is the process of removing unnecessary characters (e.g white space, new lines, comments) from code and compressing it to reduce the overall size of the scripts being served.

## Getting Started

### Backbone Boilerplate and Grunt-BBB

To get started we're going to install Grunt-BBB, which will include Backbone Boilerplate and any third-party dependencies it might need such as the Grunt build tool.

We can install Grunt-bBB via NPM by running:

```shell
npm install -g bbb
```

That's it. We should now be good to go.

A typical workflow for using grunt-bbb, which we will use later on is:

* Initialize a new project (`bbb init`)
* Add new modules and templates (`bbb init:module`)
* Preview changes using the built in server (`bbb server`)
* Run the build tool (`bbb build`)
* Lint JavaScript, compile templates, build your application using r.js, minify CSS and JavaScript (using `bbb release`)

## Creating a new project

Let's create a new directory for our project and run `bbb init` to kick things off. A number of project sub-directories and files will be stubbed out for us, as shown below:

```shell
$ bbb init
Running "init" task
This task will create one or more files in the current directory, based on the
environment and the answers to a few questions. Note that answering "?" to any
question will show question-specific help and answering "none" to most questions
will leave its value blank.

"bbb" template notes:
This tool will help you install, configure, build, and maintain your Backbone
Boilerplate project.
Writing app/app.js...OK
Writing app/config.js...OK
Writing app/main.js...OK
Writing app/router.js...OK
Writing app/styles/index.css...OK
Writing favicon.ico...OK
Writing grunt.js...OK
Writing index.html...OK
Writing package.json...OK
Writing readme.md...OK
Writing test/jasmine/index.html...OK
Writing test/jasmine/spec/example.js...OK
Writing test/jasmine/vendor/jasmine-html.js...OK
Writing test/jasmine/vendor/jasmine.css...OK
Writing test/jasmine/vendor/jasmine.js...OK
Writing test/jasmine/vendor/jasmine_favicon.png...OK
Writing test/jasmine/vendor/MIT.LICENSE...OK
Writing test/qunit/index.html...OK
Writing test/qunit/tests/example.js...OK
Writing test/qunit/vendor/qunit.css...OK
Writing test/qunit/vendor/qunit.js...OK
Writing vendor/h5bp/css/main.css...OK
Writing vendor/h5bp/css/normalize.css...OK
Writing vendor/jam/backbone/backbone.js...OK
Writing vendor/jam/backbone/package.json...OK
Writing vendor/jam/backbone.layoutmanager/backbone.layoutmanager.js...OK
Writing vendor/jam/backbone.layoutmanager/package.json...OK
Writing vendor/jam/jquery/jquery.js...OK
Writing vendor/jam/jquery/package.json...OK
Writing vendor/jam/lodash/lodash.js...OK
Writing vendor/jam/lodash/lodash.min.js...OK
Writing vendor/jam/lodash/lodash.underscore.min.js...OK
Writing vendor/jam/lodash/package.json...OK
Writing vendor/jam/require.config.js...OK
Writing vendor/jam/require.js...OK
Writing vendor/js/libs/almond.js...OK
Writing vendor/js/libs/require.js...OK

Initialized from template "bbb".

Done, without errors.
```

Let's review what has been generated.

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
  <!--(if target dummy)><!-->
  <link rel="stylesheet" href="/app/styles/index.css">
  <!--<!(endif)-->
</head>
<body>
  <!-- Application container. -->
  <main role="main" id="main"></main>

  <!-- Application source. -->
  <!--(if target dummy)><!-->
  <script data-main="/app/config" src="/vendor/js/libs/require.js"></script>
  <!--<!(endif)-->

</body>
</html>
```

RequireJS - the [AMD](https://github.com/amdjs/amdjs-api/wiki/AMD) (Asynchronous Module Definition) module and script loader - will assist us with managing the modules in our application. We've already covered it in the last chapter, but let's recap what this particular block does in terms of the Boilerplate:

```
<script data-main="/app/config" src="/vendor/js/libs/require.js"></script>
```

The `data-main` attribute is used to inform RequireJS to load `app/config.js` (a configuration object) after it has finished loading itself. You'll notice that we've omitted the `.js` extension here as RequireJS can automatically add this for us, however it will respect your paths if we do choose to include it regardless. Let's now look at the config file being referenced.

### config.js

A RequireJS configuration object allows us to specify aliases and paths for dependencies we're likely to reference often (e.g., jQuery), bootstrap properties like our base application URL, and `shim` libraries that don't support AMD natively.

This is what the config file in Backbone Boilerplate looks like:

```javascript
// Set the require.js configuration for your application.
require.config({

  // Initialize the application with the main application file and the JamJS
  // generated configuration file.
  deps: ["../vendor/jam/require.config", "main"],

  paths: {
    // Put paths here.
  },

  shim: {
    // Put shims here.
  }

});
```

The first option defined in the above config is `deps: ["../vendor/jam/require.config", "main"]`. This informs RequireJS to load up additional RequireJS configuration as well a a main.js file, which is considered the entry point for our application.

 You may notice that we haven't specified any other path information for `main`. Require will infer the default `baseUrl` using the path from our `data-main` attribute in index.html. In other words, our `baseUrl` is `app/` and any scripts we require will be loaded relative to this location. We could use the `baseUrl` option to override this default if we wanted to use a different location.

The next block is `paths`, which we can use to specify paths relative to the `baseUrl` as well as the paths/aliases to dependencies we're likely to regularly reference.

After this comes `shim`, an important part of our RequireJS configuration which allows us to load libraries which are not AMD compliant. The basic idea here is that rather than requiring all libraries to implement support for AMD, the `shim` takes care of the hard work for us.

Going back to `deps`, the contents of our `require.config` file can be seen below.

```javascript
var jam = {
    "packages": [
        {
            "name": "backbone",
            "location": "../vendor/jam/backbone",
            "main": "backbone.js"
        },
        {
            "name": "backbone.layoutmanager",
            "location": "../vendor/jam/backbone.layoutmanager",
            "main": "backbone.layoutmanager.js"
        },
        {
            "name": "jquery",
            "location": "../vendor/jam/jquery",
            "main": "jquery.js"
        },
        {
            "name": "lodash",
            "location": "../vendor/jam/lodash",
            "main": "./lodash.js"
        }
    ],
    "version": "0.2.11",
    "shim": {
        "backbone": {
            "deps": [
                "jquery",
                "lodash"
            ],
            "exports": "Backbone"
        },
        "backbone.layoutmanager": {
            "deps": [
                "jquery",
                "backbone",
                "lodash"
            ],
            "exports": "Backbone.LayoutManager"
        }
    }
};

```

The `jam` object is to support configuration of [Jam](http://jamjs.org/) - a package manager for the front-end which helps install, upgrade and configure the dependencies used by your project. It is currently the package manager of choice for Backbone Boilerplate.

Under the `packages` array, a number of dependencies are specified for inclusion, such as Backbone, the Backbone.LayoutManager plugin, jQuery and Lo-dash.

For those curious about [Backbone.LayoutManager](https://github.com/tbranyen/backbone.layoutmanager), it's a Backbone plugin that provides a foundation for assembling layouts and views within Backbone.

Additional packages you install using Jam will have a corresponding entry added to `packages`.

### main.js

Next, we have `main.js`, which defines the entry point for our application. We use a global `require()` method to load an array containing any other scripts needed, such as our application `app.js` and our main router `router.js`. Note that most of the time, we will only use `require()` for bootstrapping an application and a similar method called `define()` for all other purposes.

The function defined after our array of dependencies is a callback which doesn't fire until these scripts have loaded. Notice how we're able to locally alias references to "app" and "router" as `app` and `Router` for convenience.

```javascript
require([
  // Application.
  "app",

  // Main Router.
  "router"
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
  $(document).on("click", "a[href]:not([data-bypass])", function(evt) {
    // Get the absolute anchor href.
    var href = { prop: $(this).prop("href"), attr: $(this).attr("href") };
    // Get the absolute root.
    var root = location.protocol + "//" + location.host + app.root;

    // Ensure the root is part of the anchor href, meaning it's relative.
    if (href.prop.slice(0, root.length) === root) {
      // Stop the default event to ensure the link will not cause a page
      // refresh.
      evt.preventDefault();

      // `Backbone.history.navigate` is sufficient for all Routers and will
      // trigger the correct events. The Router's internal `navigate` method
      // calls this anyways.  The fragment is sliced from the root.
      Backbone.history.navigate(href.attr, true);
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
  "backbone.layoutmanager"
], function() {

  // Provide a global location to place configuration settings and module
  // creation.
  var app = {
    // The root path to run the application.
    root: "/"
  };

  // Localize or create a new JavaScript Template object.
  var JST = window.JST = window.JST || {};

  // Configure LayoutManager with Backbone Boilerplate defaults.
  Backbone.LayoutManager.configure({
    // Allow LayoutManager to augment Backbone.View.prototype.
    manage: true,

    prefix: "app/templates/",

    fetch: function(path) {
      // Concatenate the file extension.
      path = path + ".html";

      // If cached, use the compiled template.
      if (JST[path]) {
        return JST[path];
      }

      // Put fetch into `async-mode`.
      var done = this.async();

      // Seek out the template asynchronously.
      $.get(app.root + path, function(contents) {
        done(JST[path] = _.template(contents));
      });
    }
  });

  // Mix Backbone.Events, modules, and layout management into the app object.
  return _.extend(app, {
    // Create a custom object with a nested Views object.
    module: function(additionalProps) {
      return _.extend({ Views: {} }, additionalProps);
    },

    // Helper for using layouts.
    useLayout: function(name, options) {
      // Enable variable arity by allowing the first argument to be the options
      // object and omitting the name argument.
      if (_.isObject(name)) {
        options = name;
      }

      // Ensure options is an object.
      options = options || {};

      // If a name property was specified use that as the template.
      if (_.isString(name)) {
        options.template = name;
      }

      // Create a new Layout with options.
      var layout = new Backbone.Layout(_.extend({
        el: "#main"
      }, options));

      // Cache the reference.
      return this.layout = layout;
    }
  }, Backbone.Events);

});

```

Note: JST stands for JavaScript templates and generally refers to templates which have been (or will be) precompiled as part of a build step. When running `bbb release` or `bbb debug`, Underscore/Lo-dash templates will be precompiled to avoid the need to compile them at runtime within the browser.

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
[?] Do you need to make any changes to the above before continuing? (y/N)

Writing app/modules/foo.js...OK
Writing app/styles/foo.styl...OK
Writing app/templates/foo.html...OK

Initialized from template "module".

Done, without errors.
```

This will generate a module `foo.js` as follows:

```javascript
// Foo module
define([
  // Application.
  "app"
],

// Map dependencies from above array.
function(app) {

  // Create a new module.
  var Foo = app.module();

  // Default Model.
  Foo.Model = Backbone.Model.extend({

  });

  // Default Collection.
  Foo.Collection = Backbone.Collection.extend({
    model: Foo.Model
  });

  // Default View.
  Foo.Views.Layout = Backbone.Layout.extend({
    template: "foo"
  });

  // Return the module for AMD compliance.
  return Foo;

});
```

Notice how boilerplate code for a model, collection and view have been scaffolded out for us.

Optionally, we may also wish to include references to plugins such as the Backbone LocalStorage or Offline adapters. One clean way of including a plugin in the above boilerplate could be:

```javascript
// Foo module
define([
  // Application.
  "app",
  // Plugins
  'plugins/backbone-localstorage'
],

// Map dependencies from above array.
function(app, Store) {

  // Create a new module.
  var Foo = app.module();

  // Default Model.
  Foo.Model = Backbone.Model.extend({
    // Save all of the items under the `"foo"` namespace.
    localStorage: new Store('foo-backbone'),
  });

  // Default Collection.
  Foo.Collection = Backbone.Collection.extend({
    model: Foo.Model
  });

  // Default View.
  Foo.Views.Layout = Backbone.Layout.extend({
    template: "foo"
  });

  // Return the module for AMD compliance.
  return Foo;

});
```

### router.js

Finally, let's look at our application router which is used for handling navigation. The default router Backbone Boilerplate generates for us includes sane defaults out of the box and can be easily extended.

```javascript
define([
  // Application.
  "app"
],

function(app) {

  // Defining the application router, you can attach sub routers here.
  var Router = Backbone.Router.extend({
    routes: {
      "": "index"
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

  // Fetch data (e.g., from localStorage)
  collection.fetch();

  return Router;

});
```

## Other Useful Tools & Projects

When working with Backbone, you usually need to write a number of different classes and files for your application. Scaffolding tools such as Grunt-BBB can help automate this process by generating basic boilerplates for the files you need for you.

### Yeoman

If you appreciated Grunt-BBB but would like to explore a tool for assisting with your broader development workflow, I'm happy to recommend a tool I've been helping with called [Yeoman](http://yeoman.io).

![](img/yeoman.png)

Yeoman is a workflow comprised of a collection of tools and best practices for helping you develop more efficiently. It's comprised of yo (a scaffolding tool), [Grunt](http://gruntjs.com)(a build tool) and [Bower](http://bower.io) (a client-side package manager).

Where Grunt-BBB focuses on offering an opionated start for Backbone projects, Yeoman allows you to scaffold apps using Backbone (or other libraries and frameworks), get Backbone plugins directly from the command-line and compile your CoffeeScript, Sass or other abstractions without additional effort.

![](img/bower.png)

You may also be interested in [Brunch](http://brunch.io/), a similar project which uses skeleton boilerplates to generate new applications.

### Backbone DevTools

When building an application with Backbone, there's some additional tooling available for your day-to-day debugging workflow.

Backbone DevTools was created to help with this and is a Chrome DevTools extension allowing you to inspect events, syncs, View-DOM bindings and what objects have been instantiated.

A useful View hierarchy is displayed in the Elements panel. Also, when you inspect a DOM element the closest View will be exposed via $view in the console.

![](img/bbdevtools.jpg)

At the time of writing, the project is currently available on [GitHub](https://github.com/spect88/backbone-devtools).

## Conclusions

In this section we reviewed Backbone Boilerplate and learned how to use the `bbb` tool to help us scaffold out our application.

If you would like to learn more about how this project helps structure your app, BBB includes some built-in boilerplate sample apps that can be easily generated for review.

These include a boilerplate tutorial project (`bbb init:tutorial`) and an implementation of my [TodoMVC](http://todomvc) project (`bbb init:todomvc`). I recommend checking these out as they'll provide you with a more complete picture of how Backbone Boilerplate, its templates, and so on fit into the overall setup for a web app.

For more about Grunt-BBB, remember to take a look at the official project [repository](https://github.com/backbone-boilerplate/grunt-bbb). There is also a related [slide-deck](https://dl.dropbox.com/u/79007/talks/Modern_Web_Applications/slides/index.html) available for those interested in reading more.




