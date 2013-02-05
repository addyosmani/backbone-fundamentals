# Modular Development

## Introduction

When we say an application is modular, we generally mean it's composed of a set of highly decoupled, distinct pieces of functionality stored in modules. As you probably know, loose coupling facilitates easier maintainability of apps by removing dependencies where possible. When this is implemented efficiently, its quite easy to see how changes to one part of a system may affect another.

Unlike some more traditional programming languages however, the current iteration of JavaScript (ECMA-262) doesn't provide developers with the means to import such modules of code in a clean, organized manner. It's one of the concerns with specifications that haven't required great thought until more recent years where the need for more organized JavaScript applications became apparent.

Instead, developers at present are left to fall back on variations of the module or object literal patterns. With many of these, module scripts are strung together in the DOM with namespaces being described by a single global object where it's still possible to incur naming collisions in your architecture. There's also no clean way to handle dependency management without some manual effort or third party tools.

Whilst native solutions to these problems will be arriving in ES Harmony, the good news is that writing modular JavaScript has never been easier and you can start doing it today.

In this next part of the book, we're going to look at how to use AMD modules and RequireJS for cleanly wrapping units of code in your application into manageable modules, and an alternate approach using routes to determine when modules are loaded.

## Organizing modules with RequireJS and AMD

*Partly Contributed by [Jack Franklin](https://github.com/jackfranklin)*


[RequireJS](http://requirejs.org) is a popular script loader written by James Burke - a developer who has been quite instrumental in helping shape the AMD module format, which we'll discuss more shortly. Some of RequireJS's capabilities include helping to load multiple script files, helping define modules with or without dependencies and loading in non-script dependencies such as text files.

### Maintainability problems with multiple script files

You might be thinking that there is little benefit to RequireJS. After all, you can simply load in your JavaScript files through multiple `<script>` tags, which is very straightforward. However, doing it that way has a lot of drawbacks, namely the HTTP overhead.

Everytime the browser loads in a file you've referenced in a `<script>` tag, it makes a HTTP request to load the file's contents. It has to make a new HTTP request for each file you want to load, which causes problems.

- Browsers are limited in how many parallel requests they can make, so often it's slow to load multiple files, as it can only do a certain number at a time. This number depends on the user's settings and browser, but is usually around 4-8. When working on Backbone applications it's good to split your app into multiple JS files, so it's easy to hit that limit quickly. This can be negated by minifying your code into one file as part of a build process, but does not help with the next point.
- When a script is loaded it is done so synchronously. This means that while it's being loaded, the browser cannot continue rendering the page.

What tools like RequireJS do is load in scripts asynchronously. This means we have to adjust our code slightly, you can't just swap out `<script>` elements for a small piece of RequireJS code, but the benefits are very worthwhile.

- Loading in the scripts asychrnously means they are non-blocking. The browser can continue to render the rest of the page as the scripts are being loaded, speeding up the initial load time.
- We can load modules in more intelligently, having more control over when they are loaded; along with making sure modules that have dependencies are loaded in the right order so all dependencies are met.

### Need for better dependency management

Dependency management is a challenging subject, in particular when writing JavaScript in the browser. The closest thing we have to dependency management by default is simply making sure we order our `<script>` tags such that code that depends on code in another file is loaded after. This is not a good approach. As I've already discussed, loading multiple files in that way is bad for performance; needing them to be loaded in a certain order is very brittle.

Being able to load different code in only when needed is something RequireJS is very good at. Rather than load all our JavaScript code in at run-time, often a better approach is to dynamically load modules at run-time, only when that code is required. This avoids loading all the code in when the user first hits your application, consequently speeding up the initial load times.

Think about the GMail web-client for a moment. When users initially load up the page on their first visit, Google can simply hide widgets such as the chat module until a user has indicated (by clicking 'expand') that they wish to use it. Through dynamic dependency loading, Google could load up the chat module only then, rather than forcing all users to load it when the page first initializes. This can improve performance and load times and can definitely prove useful when building larger applications. As the codebase for an application grows this becomes even more important.

The important thing to note here is that while it's absolutely fine to develop applications without a script loader, there are significant benefits to utilising tools like RequireJS in your application.

### Asynchrous Module Definition (AMD)

RequireJS implements the [AMD Specification](https://github.com/amdjs/amdjs-api/wiki/AMD) which defines a method for writing modular code and managing dependencies. The RequireJS website also has a section [documenting the reasons behind implementing AMD](http://requirejs.org/docs/whyamd.html):

> The AMD format comes from wanting a module format that was better than today's "write a bunch of script tags with implicit dependencies that you have to manually order" and something that was easy to use directly in the browser. Something with good debugging characteristics that did not require server-specific tooling to get started.

### Writing AMD modules with RequireJS

As discussed above, the overall goal for the AMD format is to provide a solution for modular JavaScript that developers can use today. The two key concepts you need to be aware of when using it with a script-loader are a `define()` method for facilitating module definition and a `require()` method for handling dependency loading. `define()` is used to define named or unnamed modules based on the proposal using the following signature:

```javascript
define(
    module_id /*optional*/,
    [dependencies] /*optional*/,
    definition function /*function for instantiating the module or object*/
);
```

As you can tell by the inline comments, the `module_id` is an optional argument which is typically only required when non-AMD concatenation tools are being used (there may be some other edge cases where it's useful too). When this argument is left out, we call the module 'anonymous'. When working with anonymous modules, the idea of a module's identity is DRY, making it trivial to avoid duplication of filenames and code.

Back to the define signature, the dependencies argument represents an array of dependencies which are required by the module you are defining and the third argument ('definition function') is a function that's executed to instantiate your module. A barebone module (compatible with RequireJS) could be defined using `define()` as follows:

```javascript
// A module ID has been omitted here to make the module anonymous

define(['foo', 'bar'],
    // module definition function
    // dependencies (foo and bar) are mapped to function parameters
    function ( foo, bar ) {
        // return a value that defines the module export
        // (i.e the functionality we want to expose for consumption)

        // create your module here
        var myModule = {
            doStuff:function(){
                console.log('Yay! Stuff');
            }
        }

        return myModule;
});
```

#### Alternate syntax
There is also a [sugared version](http://requirejs.org/docs/whyamd.html#sugar) of `define()` available that allows you to declare your dependencies as local variables using `require()`. This will feel familiar to anyone who's used node, and can be easier to add or remove dependencies.
Here is the previous snippet using the alternate syntax:

```javascript
// A module ID has been omitted here to make the module anonymous

define(function(require){
        // module definition function
    // dependencies (foo and bar) are defined as local vars
    var foo = require('foo'),
        bar = require('bar');

    // return a value that defines the module export
    // (i.e the functionality we want to expose for consumption)

    // create your module here
    var myModule = {
        doStuff:function(){
            console.log('Yay! Stuff');
        }
    }

    return myModule;
});
```

The `require()` method is typically used to load code in a top-level JavaScript file or within a module should you wish to dynamically fetch dependencies. An example of its usage is:

```javascript
// Consider 'foo' and 'bar' are two external modules
// In this example, the 'exports' from the two modules loaded are passed as
// function arguments to the callback (foo and bar)
// so that they can similarly be accessed

require( ['foo', 'bar'], function ( foo, bar ) {
    // rest of your code here
    foo.doSomething();
});
```


Addy's post on [Writing Modular JS](http://addyosmani.com/writing-modular-js/) covers the AMD specification in much more detail. Defining and using modules will be covered in this book shortly when we look at more structured examples of using RequireJS.

### Getting Started with RequireJS
Before using RequireJS and Backbone we will first set up a very basic RequireJS project to demonstrate how it works. The first thing to do is to [Download RequireJS](http://requirejs.org/docs/download.html#requirejs). When you load in the RequireJS script in your HTML file, you need to also tell it where your main JavaScript file exists. Typically this will be called something like "app.js", and is the main entry point for your application. You do this by passing in a `data-main` attribute:

```html
<script data-main="app.js" src="lib/require.js"></script>
```

Now, RequireJS will automatically load in `app.js` for you.

#### RequireJS Configuration

In your main JavaScript file that you pass in through the `data-main` attribute, you can configure RequireJS. This is done by calling `require.config`, and passing in an object:

```javascript
require.config({
    // your configuration key/values here
    baseUrl: "app", // generally the same directory as the script used in a data-main attribute for the top level script
    paths: {}, // set up custom paths to libraries, or paths to RequireJS plugins
    shim: {}, // used for setting up all Shims (see below for more detail)
});
```

The main reason you'd want to configure RequireJS is for shims, which is used to allow RequireJS to work with libraries that don't use `define()` in the code, but expose themselves globally. We'll cover this shortly. To see other configuration options available to you, I recommend checking out the [RequireJS documentation](http://requirejs.org/docs/api.html#config).


##### RequireJS Shims
To use a library with RequireJS, ideally that library should come with AMD support. That is, it uses the `define` method to define the library as a module. However, some libraries - including Backbone and one of its dependencies, Underscore - don't do this. Fortunately RequireJS comes with a way to bypass this.

To demonstrate this, first let's shim Underscore, and then we'll shim Backbone too. Shims are very simple to implement:

```javascript
require.config({
    shim: {
        'lib/underscore': {
          exports: '_'
        }
    }
});
```

The important line there is `exports: '_'`. That tells RequireJS that whenever we require the file `'lib/underscore'`, it would return the global `_` object, as that's where Underscore exports itself too, it adds the `_` property to the global object. Now when we require Underscore, RequireJS will return that `_` property. Now we can set up a shim for Backbone too:

```javascript
require.config({
    shim: {
        'lib/underscore': {
          exports: '_'
        },
        'lib/backbone': {
            deps: ['lib/underscore', 'jquery'],
            exports: 'Backbone'
        }
    }
});
```

Again, that configuration tells RequireJS to return the global `Backbone` property that Backbone exports, but also this time you'll notice that Backbone's dependencies are defined. This means whenever this:

```javascript
require( 'lib/backbone', function( Backbone ) {...} );
```

Is run, it will first make sure the dependencies are met, and then pass the global `Backbone` object into the callback function. You don't need to do this with every library, only the ones that don't support AMD. For example, jQuery does support it, as of jQuery 1.7.

If you'd like to read more about general RequireJS usage, the [RequireJS API docs](http://requirejs.org/docs/api.html) are incredibly thorough and easy to read.

#### Custom Paths

Typing long paths to file names like `lib/backbone` can get tedious. RequireJS lets us set up custom paths in our configuration object. Here, whenever I refer to "underscore", RequireJS will look for the file `lib/underscore.js`:

```javascript
require.config({
    paths: {
        'underscore': 'lib/underscore'
    }
});
```

Of course, this can be combined with a shim:

```javascript
require.config({
    paths: {
        'underscore': 'lib/underscore'
    },
    shim: {
        'underscore': {
          exports: '_'
        }
    }
});
```

Just make sure that in your shim settings, you refer to the custom path too. Now you can do:

```javascript
require( ['underscore'], function(_) {
// code here
});
```

To shim Underscore but still use a custom path.


### Require.js and Backbone Examples

Now that we've taken a look at how to define AMD modules, let's review how to go about wrapping components like views and collections so that they can also be easily loaded as dependencies for any parts of your application that require them. At it's simplest, a Backbone model may just require Backbone and Underscore.js. These are dependencies, so we can define those when defining the new modules. Note that the following examples presume you have configured RequireJS to shim Backbone and Underscore, as discussed previously.

#### Wrapping modules, views and other components with AMD

For example, here is how a model is defined.

```javascript
define(['underscore', 'backbone'], function(_, Backbone) {
  var myModel = Backbone.Model.extend({

    // Default attributes
    defaults: {
      content: 'hello world',
    },

    // A dummy initialization method
    initialize: function() {
    },

    clear: function() {
      this.destroy();
      this.view.remove();
    }

  });
  return myModel;
});
```

Note how we alias Underscore.js's instance to `_` and Backbone to just `Backbone`, making it very trivial to convert non-AMD code over to using this module format. For a view which might require other dependencies such as jQuery, this can similarly be done as follows:

```javascript
define([
  'jquery',
  'underscore',
  'backbone',
  'collections/mycollection',
  'views/myview'
  ], function($, _, Backbone, myCollection, myView){

  var AppView = Backbone.View.extend({
  ...
```

Aliasing to the dollar-sign (`$`), once again makes it very easy to encapsulate any part of an application you wish using AMD.

Doing it this way makes it easy to organise your Backbone application as you like. It's recommended to separate modules into folders. For example, individual folders for models, collections, views and so on. RequireJS doesn't care about what folder structure you use; as long as you use the correct path when using `require`, it will happily pull in the file.

As part of this chapter I've made a very simple [Backbone application with RequireJS that you can find on Github](https://github.com/javascript-playground/backbone-require-example). This application is incredibly simple. It is a stock application for a manager of a shop. They can add new items and filter down the items based on price, but nothing more. Because it's so simple it's easier to focus purely on the RequireJS part of the implementation, rather than deal with complex JavaScript and Backbone logic too.

At the base of this application is the `Item` model, which describes a single item in the stock. Its implementation is very straight forward:

```javascript
define( ["lib/backbone"], function ( Backbone ) {
  var Item = Backbone.Model.extend({
    defaults: {
      price: 35,
      photo: "http://www.placedog.com/100/100"
    }
  });
  return Item;
});
```

Converting an individual model, collection, view or similar into an AMD, RequireJS compliant one is typically very straight forward. Usually all that's needed is the first line, calling `define`, and to make sure that once you've defined your object - in this case, the `Item` model, to return it.

Lets now set up a view for that individual item:

```javascript
define( ["lib/backbone"], function ( Backbone ) {
  var ItemView = Backbone.View.extend({
    tagName: "div",
    className: "item-wrap",
    template: _.template($("#itemTemplate").html()),

    render: function() {
      this.$el.html(this.template(this.model.toJSON()));
      return this;
    }
  });
  return ItemView;
});
```

This view doesn't actually depend on the model it will be used with, so again the only dependency is Backbone. Other than that it's just a regular Backbone view. There's nothing special going on here, other than returning the object and using `define` so RequireJS can pick it up. Now lets make a collection to view a list of items. This time we will need to reference the `Item` model, so we add it as a dependency:

```javascript
define(["lib/backbone", "models/item"], function(Backbone, Item) {
  var Cart = Backbone.Collection.extend({
    model: Item,
    initialize: function() {
      this.on("add", this.updateSet, this);
    },
    updateSet: function() {
      items = this.models;
    }
  });
  return Cart;
});
```

I've called this collection `Cart`, as it's a group of items. As the `Item` model is the second dependency, I can bind the variable `Item` too it by declaring it as the second argument to the callback function. I can then refer to this within my collection implementation.

Finally, let's have a look at the view for this collection. (This file is much bigger in the application, but I've taken some bits out so it's easier to examine).

```javascript
define(["lib/backbone", "models/item", "views/itemview"], function(Backbone, Item, ItemView) {
  var ItemCollectionView = Backbone.View.extend({
    el: '#yourcart',
    initialize: function(collection) {
      this.collection = collection;
      this.render();
      this.collection.on("reset", this.render, this);
    },
    render: function() {
      this.$el.html("");
      this.collection.each(function(item) {
        this.renderItem(item);
      }, this);
    },
    renderItem: function(item) {
      var itemView = new ItemView({model: item});
      this.$el.append(itemView.render().el);
    },
    // more methods here removed
  });
  return ItemCollectionView;
});
```

There really is nothing to it once you've got the general pattern. Define each "object" (a model, view, collection, router or otherwise) through RequireJS, and then specify them as dependencies to other objects that need them. Again, you can find this entire application [on Github](https://github.com/javascript-playground/backbone-require-example).

If you'd like to take a look at how others do it, [Pete Hawkins' Backbone Stack repository](https://github.com/phawk/Backbone-Stack) is a good example of structuring a Backbone application, using RequireJS. Greg Franko has also written [an overview of how he uses Backbone and Require](http://gregfranko.com/blog/using-backbone-dot-js-with-require-dot-js/), and [Jeremy Kahn's post](http://jeremyckahn.github.com/blog/2012/08/18/keeping-it-sane-backbone-views-and-require-dot-js/) neatly describes his approach. For a full look at a sample application, the [Backbone and Require example](https://github.com/addyosmani/todomvc/tree/gh-pages/dependency-examples/backbone_require) of the TodoMVC repository should be your starting point.



### Keeping Your Templates External Using RequireJS And The Text Plugin

Moving your [Underscore/Mustache/Handlebars] templates to external files is actually quite straight-forward. As this application makes use of RequireJS, I'll discuss how to implement external templates using this specific script loader.

RequireJS has a special plugin called text.js which is used to load in text file dependencies. To use the text plugin, simply follow these simple steps:

1. Download the plugin from http://requirejs.org/docs/download.html#text and place it in either the same directory as your application's main JS file or a suitable sub-directory.

2. Next, include the text.js plugin in your initial RequireJS configuration options. In the code snippet below, we assume that RequireJS is being included in our page prior to this code snippet being executed.

```javascript
require.config( {
    paths: {
        'text': 'libs/require/text',
    },
    baseUrl: 'app'
} );
```

3. When the `text!` prefix is used for a dependency, RequireJS will automatically load the text plugin and treat the dependency as a text resource. A typical example of this in action may look like:

```javascript
require(['js/app', 'text!templates/mainView.html'],
    function( app, mainView ) {
        // the contents of the mainView file will be
        // loaded into mainView for usage.
    }
);
```

4. Finally we can use the text resource that's been loaded for templating purposes. You're probably used to storing your HTML templates inline using a script with a specific identifier.

With Underscore.js's micro-templating (and jQuery) this would typically be:

HTML:

```html
<script type="text/template" id="mainViewTemplate">
    <% _.each( person, function( person_item ){ %>
        <li><%= person_item.get('name') %></li>
    <% }); %>
</script>
```


JS:

```javascript
var compiled_template = _.template( $('#mainViewTemplate').html() );
```

With RequireJS and the text plugin however, it's as simple as saving your template into an external text file (say, `mainView.html`) and doing the following:

```javascript
require(['js/app', 'text!templates/mainView.html'],
    function(app, mainView){
        var compiled_template = _.template( mainView );
    }
);
```

That's it! Now you can apply your template to a view in Backbone with something like:

```javascript
collection.someview.$el.html( compiled_template( { results: collection.models } ) );
```


All templating solutions will have their own custom methods for handling template compilation, but if you understand the above, substituting Underscore's micro-templating for any other solution should be fairly trivial.

**Note:** You may also be interested in looking at [RequireJS tpl](https://github.com/ZeeAgency/requirejs-tpl). It's an AMD-compatible version of the Underscore templating system that also includes support for optimization (pre-compiled templates) which can lead to better performance and no evals. I have yet to use it myself, but it comes as a recommended resource.


### Optimizing Backbone apps for production with the RequireJS Optimizer

As experienced developers may know, an essential final step when writing both small and large JavaScript web applications is the build process.  The majority of non-trivial apps are likely to consist of more than one or two scripts and so optimizing, minimizing and concatenating your scripts prior to pushing them to production will require your users to download a reduced number (if not just one) script file.

If this is your first time looking at build scripts, [Addy Osmani's screencast on build scripts](http://addyosmani.com/blog/client-side-build-process/) may be useful.

Another benefit to using RequireJS is its command line optimization tool, R.js. This has a number of capabilities, including:

* Concatenating specific scripts and minifying them using external tools such as UglifyJS (which is used by default) or Google's Closure Compiler for optimal browser delivery, whilst preserving the ability to dynamically load modules
* Optimizing CSS and stylesheets by inlining CSS files imported using @import, stripping out comments etc.
* The ability to run AMD projects in both Node and Rhino (more on this later)

You'll notice that I mentioned the word 'specific' in the first bullet point. The RequireJS optimizer only concatenates module scripts that have been specified in arrays of string literals passed to top-level (i.e non-local) require and define calls. As clarified by the [optimizer docs](http://requirejs.org/docs/optimization.html) this means that Backbone modules defined like this:

```javascript
define(['jquery','backbone','underscore', 'collections/sample','views/test'],
    function($,Backbone, _, Sample, Test){
        //...
    });
```

will combine fine, however inline dependencies such as:

```javascript
var models = someCondition ? ['models/ab','models/ac'] : ['models/ba','models/bc'];
```

will be ignored. This is by design as it ensures that dynamic dependency/module loading can still take place even after optimization.

Although the RequireJS optimizer works fine in both Node and Java environments, it's strongly recommended to run it under Node as it executes significantly faster there. In my experience, it's a piece of cake to get setup with either environment, so go for whichever you feel most comfortable with.

To get started with r.js, grab it from the [RequireJS download page](http://requirejs.org/docs/download.html#rjs) or [through NPM](http://requirejs.org/docs/optimization.html#download). Now, the RequireJS optimizer works absolutely fine for single script and CSS files, but for most cases you'll want to actually optimize an entire Backbone project. You *could* do this completely from the command-line, but a cleaner option is using build profiles.

Below is an example of a build file taken from the modular jQuery Mobile app referenced later in this book. A **build profile** (commonly named `app.build.js`) informs RequireJS to copy all of the content of `appDir` to a directory defined by `dir` (in this case `../release`). This will apply all of the necessary optimizations inside the release folder. The `baseUrl` is used to resolve the paths for your modules. It should ideally be relative to `appDir`.

Near the bottom of this sample file, you'll see an array called `modules`. This is where you specify the module names you wish to have optimized. In this case we're optimizing the main application called 'app', which maps to `appDir/app.js`. If we had set the `baseUrl` to 'scripts', it would be mapped to `appDir/scripts/app.js`.

```javascript
({
    appDir: './',
    baseUrl: './',
    dir: '../release',
    paths: {
       'backbone':          'libs/AMDbackbone-0.5.3',
        'underscore':       'libs/underscore-1.2.2',
        'jquery':           'libs/jQuery-1.7.1',
        'json2':            'libs/json2',
        'datepicker':       'libs/jQuery.ui.datepicker',
        'datepickermobile': 'libs/jquery.ui.datepicker.mobile',
        'jquerymobile':     'libs/jquery.mobile-1.0'
    },
    optimize: 'uglify',
    modules: [
        {
            name: 'app',
            exclude: [
                // If you prefer not to include certain libs exclude them here
            ]
        }
    ]
})
```

The way the build system in r.js works is that it traverses app.js (whatever modules you've passed) and resolved dependencies, concatenating them into the final `release`(dir) folder. CSS is treated the same way.

The build profile is usually placed inside the 'scripts' or 'js' directory of your project. As per the docs, this file can however exist anywhere you wish, but you'll need to edit the contents of your build profile accordingly.

Finally, to run the build, execute the following command once inside your `appDir` or `appDir/scripts` directory:

```javascript
node ../../r.js -o app.build.js
```

That's it. As long as you have UglifyJS/Closure tools setup correctly, r.js should be able to easily optimize your entire Backbone project in just a few key-strokes. If you would like to learn more about build profiles, James Burke has a [heavily commented sample file](https://github.com/jrburke/r.js/blob/master/build/example.build.js) with all the possible options available.


## Optimize and Build a Backbone.js JavaScript application with RequireJS using Packages

*Contributed by [Bill Heaton](https://github.com/pixelhandler)*

When a JavaScript application is too complex or large to build in a single file, grouping the application’s components into packages allows for script dependencies to download in parallel, and facilitates only loading **packaged** and other modular code as the site experience requires the specific set of dependencies.

RequireJS, the (JavaScript) module loading library, has an [optimizer](http://requirejs.org/docs/optimization.html 'RequireJS optimizer') to build a JavaScript-based application and provides various options. A build profile is the recipe for your build, much like a build.xml file is used to build a project with ANT. The benefit of building with **r.js** not only results in speedy script loading with minified code, but also provides a way to package components of your application.

* [Optimizing one JavaScript file](http://requirejs.org/docs/optimization.html#onejs 'Optimizing one JavaScript file')
* [Optimizing a whole project](http://requirejs.org/docs/optimization.html#wholeproject 'Optimizing a whole project')
* [Optimizing a project in layers or packages](http://requirejs.org/docs/faq-optimization.html#priority 'Optimizing a project in layers or packages')

In a complex application, organizing code into *packages* is an attractive build strategy. The build profile in this article is based on an test application currently under development (files list below). The application framework is built with open source libraries. The main objective in this build profile is to optimize an application developed with [Backbone.js](http://documentcloud.github.com/backbone/ 'Backbone.js') using modular code, following the [Asynchronous Module Definition (AMD)](https://github.com/amdjs/amdjs-api/wiki/AMD 'Asynchronous Module Definition (AMD) wiki page') format. AMD and RequireJS provide the structure for writing modular code with dependencies. Backbone.js provides the code organization for developing models, views and collections and also interactions with a RESTful API.

Below is an outline of the applications file organization, followed by the build profile to build modular (or packaged) layers a JavaScript driven application.

#### File organization

Assume the following directories and file organization, with app.build.js as the build profile (a sibling to both source and release directories). Note that the files in the list below named *section* can be any component of the application, e.g. *header*, *login*)

```text
.-- app.build.js
|-- app-release
`-- app-src
    |-- collections
    |   |-- base.js
    |   |-- sections-segments.js
    |   `-- sections.js
    |-- docs
    |   `--docco.css
    |-- models
    |   |-- base.js
    |   |-- branding.js
    |   `-- section.js
    |-- packages
    |   |-- header
    |   |   |-- models
    |   |   |   |-- nav.js
    |   |   |   `-- link.js
    |   |   |-- templates
    |   |   |   |-- branding.js
    |   |   |   |-- nav.js
    |   |   |   `-- links.js
    |   |   `-- views
    |   |       |-- nav.js
    |   |       |-- branding.js
    |   |       `-- link.js
    |   |-- header.js
    |   `-- ... more packages here e.g. cart, checkout ...
    |-- syncs
    |   |-- rest
    |   |   `-- sections.js
    |   |-- factory.js
    |   `-- localstorage.js
    |-- test
    |   |-- fixtures
    |   |   `-- sections.json
    |   |-- header
    |   |   |-- index.html
    |   |   `-- spec.js
    |   |-- lib
    |   |   `-- Jasmine
    |   |-- models
    |   |-- utils
    |   |-- global-spec.js
    |-- utils
    |   |-- ajax.js
    |   |-- baselib.js
    |   |-- debug.js
    |   |-- localstorage.js
    |   `-- shims.js
    |-- vendor
    |-- |-- backbone-min.js
    |   |-- jquery.min.js
    |   |-- jquery.mobile-1.0.min.js
    |   |-- json2.js
    |   |-- modernizr.min.js
    |   |-- mustache.js
    |   |-- require.js
    |   |-- text.js
    |   `-- underscore.js
    |-- views
    |   |-- base.js
    |   `-- collection.js
    |-- application.js
    |-- collections.js
    |-- index.html
    |-- main.js
    |-- models.js
    |-- syncs.js
    |-- utils.js
    |-- vendor.js
    `-- views.js
```

#### Build profile to optimize modular dependencies with code organized in packages

The build profile can be organized to [divide parallel downloads for various sections of the application](http://requirejs.org/docs/faq-optimization.html#priority 'optimize modular dependencies in packages').

This strategy demonstrated builds common or site-wide groups of (core) *models*, *views*, collections which are extended from a base.js constructor which extends the appropriate backbone method, e.g. Backbone.Model. The *packages* directory organizes code by section / responsibility, e.g. cart, checkout, etc. Notice that within the example *header* package the directory structure is similar to the app root directory file structure. A *package* (of modularized code) has dependencies from the common libraries in your application and also has specific code for the packages execution alone; other packages should not require another packages dependencies. A *utils* directory has shims, helpers, and common library code to support the application. A *syncs* directory to define persistence with your RESTful api and/or localStorage. The *vendor* libraries folder will not be built, there is no need to do so, you may decide to use a CDN (then set these paths to : *[empty:](http://requirejs.org/docs/optimization.html#empty 'empty:')*). And finally a *test* directory for [Jasmine](http://pivotal.github.com/jasmine/ 'Jasmine is a behavior-driven development framework for testing your JavaScript code') unit test specs, which may be ignored in the build as well if you choose.

Also notice the there are .js files named the same as the directories, these are the files listed in the paths. these are strategic to group sets of files to build, examples follow the build profile below.

```javascript
({
    appDir: './app-src',
    baseUrl: './',
    dir: './app-build',
    optimize: 'uglify',
    paths: {
        // will not build 3rd party code, it's already built
        'text'         : 'vendor/text',
        'json2'        : 'vendor/json2.min',
        'modernizr'    : 'vendor/modernizr.min',
        'jquery'       : 'vendor/jquery-1.7.1',
        'jquerymobile' : 'vendor/jquery.mobile.min.js',
        'underscore'   : 'vendor/underscore',
        'mustache'     : 'vendor/mustache',
        'backbone'     : 'vendor/backbone',
        // files that define dependencies...
        // ignore vendor libraries, but need a group to do so
        'vendor'       : 'vendor',
        // application modules/packages these files define dependencies
        // and may also group modules into objects if needed to require
        // by groups rather than individual files
        'utils'        : 'utils',
        'models'       : 'models',
        'views'        : 'views',
        'collections'  : 'collections',
        // packages to build
        'header'       : 'packages/header'
        //... more packages
    },
    modules: [
        // Common libraries, Utilities, Syncs, Models, Views, Collections
        {
            name: 'utils',
            exclude: ['vendor']
        },
        {
            name: 'syncs',
            exclude: ['vendor', 'utils']
        },
        {
            name: 'models',
            exclude: ['vendor', 'utils', 'syncs']
        },
        {
            name: 'views',
            exclude: ['vendor', 'utils', 'syncs', 'models']
        },
        {
            name: 'collections',
            exclude: ['vendor', 'utils', 'syncs', 'models', 'views']
        },
        // Packages
        {
            name: 'header',
            exclude: ['vendor', 'utils', 'syncs', 'models', 'views', 'collections']
        }
        // ... and so much more ...
    ]
})
```

The above build profile is designed for balancing scalability and performance.

**Examples of the grouped sets of code dependencies**

The contents of the vendor.js which is not built into a package may use some *no conflict* calls as well.

```javascript
// List of vendor libraries, e.g. jQuery, Underscore, Backbone, etc.
// this module is used with the r.js optimizer tool during build
// @see <http://requirejs.org/docs/faq-optimization.html>
define([ 'jquery', 'underscore', 'backbone', 'modernizr', 'mustache' ],
function ($,        _,            Backbone,   Modernizr,   Mustache) {
    // call no conflicts so if needed you can use multiple versions of $
    $.noConflict();
    _.noConflict();
    Backbone.noConflict();
});
```

For your application common library code.

```javascript
// List of utility libraries,
define([ 'utils/ajax', 'utils/baselib', 'utils/localstorage', 'utils/debug', 'utils/shims' ],
function (ajax,         baselib,         localstorage,         debug) {
    return {
        'ajax' : ajax,
        'baselib' : baselib,
        'localstorage' : localstorage,
        'debug' : debug
    };
    // the shim only extend JavaScript when needed, e.g. Object.create
});
```

An example where you intend to use require the common models in another package file.

```javascript
// List of models
// models in this directory are intended for site-wide usage
// grouping site-wide models in this module (object)
// optimizes the performance and keeps dependencies organized
// when the (build) optimizer is run.
define([ 'models/branding', 'models/section' ],
function (Branding,          Section) {
    return {
        'Branding' : Branding,
        'Section'  : Section
    };
});
```

#### A quick note on code standards

Notice that in the above examples the parameters may begin with lower or upper case characters. The variable names uses in the parameters that begin with *Uppercase* are *Constructors* and the *lowercase* variable names are not, they may be instances created by a constructor, or perhaps an object or function that is not meant to used with *new*.

The convention recommended is to use Upper CamelCase for constructors and lower camelCase for others.

#### Common Pitfall when organizing code in modules

Be careful not define circular dependencies. For example, in a common *models* package (models.js) dependencies are listed for the files in your models directory

    define([ 'models/branding', 'models/section' ], function (branding, section)
    // ...
    return { 'branding' : branding, 'section', section }

Then when another packages requires a common model you can access the models objects returned from your common models.js file like so...

    define([ 'models', 'utils' ], function (models, utils) {
    var branding = models.branding, debug = utils.debug;

Perhaps after using the model a few times you get into the habit of requiring "model". Later you need add another common model with extends a model you already defined. So the pitfall begins, you add a new model inside your models directory and add a reference this same model in the model.js:

    define([ 'models/branding', 'models/section', 'models/section-b' ], function (branding, section)
    // ...
    return { 'branding' : branding, 'section', section, 'section-b' : section-b }

However in your *models/section-b.js* file you define a dependency using the model.js which returns the models in an object like so...

    define([ 'models' ], function (models, utils) {
    var section = models.section;

Above is the mistake in models.js a dependency was added for models/section-b and in section-b a dependency is defined for model. The new models/section-b.js requires *model* and model.js requires *models/section-b.js* - a circular dependency. This should result in a load timeout error from RequireJS, but not tell you about the circular dependency.

For other common mistakes see the [COMMON ERRORS](http://requirejs.org/docs/errors.html 'RequireJS common errors page') page on the RequireJS site.

#### Executing the Build with r.js

If you intalled r.js with Node's npm (package manager) like so...

    > npm install requirejs

...you can execute the build on the command line:

    > r.js -o app.build.js


## Exercise: Building a modular Backbone app with AMD & RequireJS

In this chapter, we'll look at our first practical Backbone & RequireJS project - how to build a modular Todo application. The application will allow us to add new todos, edit new todos and clear todo items that have been marked as completed. For a more advanced practical, see the section on mobile Backbone development.

The complete code for the application can can be found in the `practicals/modular-todo-app` folder of this repo (thanks to Thomas Davis and J&eacute;r&ocirc;me Gravel-Niquet). Alternatively grab a copy of my side-project [TodoMVC](https://github.com/addyosmani/todomvc) which contains the sources to both AMD and non-AMD versions.

**Note:** Thomas may be covering a practical on this exercise in more detail on [backbonetutorials.com](http://backbonetutorials.com) at some point soon, but for this section I'll be covering what I consider the core concepts.

### Overview

Writing a 'modular' Backbone application can be a straight-forward process. There are however, some key conceptual differences to be aware of if opting to use AMD as your module format of choice:

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
      this.$el.html(this.template(this.model.toJSON()));
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

To see how everything ties together, feel free to grab the source by cloning this repo or browse it [online](https://github.com/addyosmani/backbone-fundamentals/tree/master/practicals/modular-todo-app) to learn more. I hope you find it helpful!.

**Note:** While this first practical doesn't use a build profile as outlined in the chapter on using the RequireJS optimizer, we will be using one in the section on building mobile Backbone applications.


## Route based module loading

This section will discuss a route based approach to module loading as implemented in [Lumbar](http://walmartlabs.github.com/lumbar) by Kevin Decker. Like RequireJS, Lumbar is also a modular build system, but the pattern it implements for loading routes may be used with any build system.

The specifics of the Lumbar build tool are not discussed in this book. To see a complete Lumbar based project with the loader and build system see [Thorax](http://thoraxjs.org) which provides boilerplate projects for various environments including Lumbar.

### JSON based module configuration

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

A little used feature of `Backbone.Router` is it's ability to create multiple routers that listen to the same set of routes. Lumbar uses this feature to create a router that listens to all routes in the application. When a route is matched, this master router checks to see if the needed module is loaded. If the module is already loaded, then the master router takes no action and the router defined by the module will handle the route. If the needed module has not yet been loaded, it will be loaded, then `Backbone.history.loadUrl` will be called. This reloads the route, causes the master router to take no further action and the router defined in the freshly loaded module to respond.

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

`window.history.pushState` support (serving Backbone routes without a hashtag) requires that the server be aware of what URLs your Backbone application will handle, since the user should be able to enter the app at any of those routes (or hit reload after navigating to a pushState URL).

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

## Decoupling Backbone with the Mediator and Facade Patterns

In this section we'll discuss applying some of the concepts I cover in my article on [Large-scale JavaScript Application development](http://addyosmani.com/largescalejavascript) to Backbone.

*After, you may be interested in taking a look At [Aura](http://github.com/aurajs/aura) - my popular widget-based Backbone.js extension framework based on many of the concepts we will be covering in this section.*

### Summary

At a high-level, one architecture that works for such applications is something which is:

* **Highly decoupled**: encouraging modules to only publish and subscribe to events of interest rather than directly communicating with each other. This helps us to build applications who's units of code aren't highly tied (coupled) together and can thus be reused more easily.
* **Supports module-level security**: whereby modules are only able to execute behavior they've been permitted to. Application security is an area which is often overlooked in JavaScript applications, but can be quite easily implemented in a flexible manner.
* **Supports failover**: allowing an application continuing to function even if particular modules fail. The typical example I give of this is the GMail chat widget. Imagine being able to build applications in a way that if one widget on the page fails (e.g chat), the rest of your application (mail) can continue to function without being affected.

This is an architecture which has been implemented by a number of different companies in the past, including Yahoo! (for their modularized homepage.

The three design patterns that make this architecture possible are the:

* **Module pattern**: used for encapsulating unique blocks of code, where functions and variables can be kept either public or private. ('private' in the simulation of privacy sense, as of course don't have true privacy in JavaScript)
* **Mediator pattern**: used when the communication between modules may be complex, but is still well defined. If it appears a system may have too many relationships between modules in your code, it may be time to have a central point of control, which is where the pattern fits in.
* **Facade pattern**: used for providing a convenient higher-level interface to a larger body of code, hiding its true underlying complexity

Their specific roles in this architecture can be found below.

* **Modules**: There are almost two concepts of what defines a module. As AMD is being used as a module wrapper, technically each model, view and collection can be considered a module. We then have the concept of modules being distinct blocks of code outside of just MVC/MV*. For the latter, these types of 'modules' are primarily concerned with broadcasting and subscribing to events of interest rather than directly communicating with each other.They are made possible through the Mediator pattern.
* **Mediator**: The mediator has a varying role depending on just how you wish to implement it. In my article, I mention using it as a module manager with the ability to start and stop modules at will, however when it comes to Backbone, I feel that simplifying it down to the role of a central 'controller' that provides pub/sub capabilities should suffice. One can of course go all out in terms of building a module system that supports module starting, stopping, pausing etc, however the scope of this is outside of this chapter.
* **Facade**: This acts as a secure middle-layer that both abstracts an application core (Mediator) and relays messages from the modules back to the Mediator so they don't touch it directly. The Facade also performs the duty of application security guard; it checks event notifications from modules against a configuration (permissions.js, which we will look at later) to ensure requests from modules are only processed if they are permitted to execute the behavior passed.


### Exercise

For the practical section of this chapter, we'll be extending the well-known Backbone Todo application using the three patterns mentioned above.

The application is broken down into AMD modules that cover everything from Backbone models through to application-level modules. The views publish events of interest to the rest of the application and modules can then subscribe to these event notifications.

All subscriptions from modules go through a facade (or sandbox). What this does is check against the subscriber name and the 'channel/notification' it's attempting to subscribe to. If a channel *doesn't* have permissions to be subscribed to (something established through permissions.js), the subscription isn't permitted.


**Mediator**

Found in `aura/mediator.js`

Below is a very simple AMD-wrapped implementation of the mediator pattern, based on prior work by Ryan Florence. It accepts as its input an object, to which it attaches `publish()` and `subscribe()` methods. In a larger application, the mediator can contain additional utilities, such as handlers for initializing, starting and stopping modules, but for demonstration purposes, these two methods should work fine for our needs.

```javascript
define([], function(obj){

  var channels = {};
  if (!obj) obj = {};

  obj.subscribe = function (channel, subscription) {
    if (!channels[channel]) channels[channel] = [];
    channels[channel].push(subscription);
  };

  obj.publish = function (channel) {
    if (!channels[channel]) return;
    var args = [].slice.call(arguments, 1);
    for (var i = 0, l = channels[channel].length; i < l; i++) {
      channels[channel][i].apply(this, args);
    }
  };

  return obj;

});
```


**Facade**

Found in `aura/facade.js`

Next, we have an implementation of the facade pattern. Now the classical facade pattern applied to JavaScript would probably look a little like this:

```javascript

var module = (function() {
    var _private = {
        i:5,
        get : function() {
            console.log('current value:' + this.i);
        },
        set : function( val ) {
            this.i = val;
        },
        run : function() {
            console.log('running');
        },
        jump: function(){
            console.log('jumping');
        }
    };
    return {
        facade : function( args ) {
            _private.set(args.val);
            _private.get();
            if ( args.run ) {
                _private.run();
            }
        }
    }
}());

module.facade({run: true, val:10});
//outputs current value: 10, running
```

It's effectively a variation of the module pattern, where instead of simply returning an interface of supported methods, your API can completely hide the true implementation powering it, returning something simpler. This allows the logic being performed in the background to be as complex as necessary, whilst all the end-user experiences is a simplified API they pass options to (note how in our case, a single method abstraction is exposed). This is a beautiful way of providing APIs that can be easily consumed.

That said, to keep things simple, our implementation of an AMD-compatible facade will act a little more like a proxy. Modules will communicate directly through the facade to access the mediator's `publish()` and `subscribe()` methods, however, they won't as such touch the mediator directly.This enables the facade to provide application-level validation of any subscriptions and publications made.

It also allows us to implement a simple, but flexible, permissions checker (as seen below) which will validate subscriptions made against a permissions configuration to see whether it's permitted or not.


```javascript
define([ '../aura/mediator' , '../aura/permissions' ], function (mediator, permissions) {

    var facade = facade || {};

    facade.subscribe = function(subscriber, channel, callback){

        // Note: Handling permissions/security is optional here
        // The permissions check can be removed
        // to just use the mediator directly.

        if(permissions.validate(subscriber, channel)){
            mediator.subscribe( channel, callback );
        }
    }

    facade.publish = function(channel){
        mediator.publish( channel );
    }
    return facade;

});
```

**Permissions**

Found in `aura/permissions.js`

In our simple permissions configuration, we support checking against subscription requests to establish whether they are allowed to clear. This enforces a flexible security layer for the application.

To visually see how this works, consider changing say, permissions -> renderDone -> todoCounter to be false. This will completely disable the application from from rendering or displaying the counts component for Todo items left (because they aren't allowed to subscribe to that event notification). The rest of the Todo app can still however be used without issue.

It's a very dumbed down example of the potential for application security, but imagine how powerful this might be in a large app with a significant number of visual widgets.

```javascript
define([], function () {

    // Permissions

    // A permissions structure can support checking
    // against subscriptions prior to allowing them
    // to clear. This enforces a flexible security
    // layer for your application.

    var permissions = {

        newContentAvailable: {
            contentUpdater:true
        },

        endContentEditing:{
            todoSaver:true
        },

        beginContentEditing:{
            editFocus:true
        },

        addingNewTodo:{
            todoTooltip:true
        },

        clearContent:{
            garbageCollector:true
        },

        renderDone:{
            todoCounter:true //switch to false to see what happens :)
        },

        destroyContent:{
            todoRemover:true
        },

        createWhenEntered:{
            keyboardManager:true
        }

    };

    permissions.validate = function(subscriber, channel){
        var test = permissions[channel][subscriber];
        return test===undefined? false: test;
    };

    return permissions;

});
```



**Subscribers**

Found in `subscribers.js`

Subscriber 'modules' communicate through the facade back to the mediator and perform actions when a notification event of a particular name is published.

For example, when a user enters in a new piece of text for a Todo item and hits 'enter' the application publishes a notification saying two things: a) a new Todo item is available and b) the text content of the new item is X. It's then left up to the rest of the application to do with this information whatever it wishes.

In order to update your Backbone application to primarily use pub/sub, a lot of the work you may end up doing will be moving logic coupled inside of specific views to modules outside of it which are reactionary.

Take the `todoSaver` for example - its responsibility is saving new Todo items to models once the a `notificationName` called 'newContentAvailable' has fired. If you take a look at the permissions structure in the last code sample, you'll notice that 'newContentAvailable' is present there. If I wanted to prevent subscribers from being able to subscribe to this notification, I simply set it to a boolean value of `false`.

Again, this is a massive oversimplification of how advanced your permissions structures could get, but it's certainly one way of controlling what parts of your application can or can't be accessed by specific modules at any time.

```javascript
define(['jquery', 'underscore', 'aura/facade'],
function ($, _, facade) {

    // Subscription 'modules' for our views. These take the
    // the form facade.subscribe( subscriberName, notificationName , callBack )

    // Update view with latest todo content
    // Subscribes to: newContentAvailable

    facade.subscribe('contentUpdater', 'newContentAvailable', function (context) {
        var content = context.model.get('content');
        context.$('.todo-content').text(content);
        context.input = context.$('.todo-input');
        context.input.bind('blur', context.close);
        context.input.val(content);
    });


    // Save models when a user has finishes editing
    // Subscribes to: endContentEditing
    facade.subscribe('todoSaver','endContentEditing', function (context) {
        try {
            context.model.save({
                content: context.input.val()
            });
            context.$el.removeClass('editing');
        } catch (e) {
            //console.log(e);
        }
    });


    // Delete a todo when the user no longer needs it
    // Subscribes to: destroyContent
    facade.subscribe('todoRemover','destroyContent', function (context) {
        try {
            context.model.clear();
        } catch (e) {
            //console.log(e);
        }
    });


    // When a user is adding a new entry, display a tooltip
    // Subscribes to: addingNewTodo
    facade.subscribe('todoTooltip','addingNewTodo', function (context, todo) {
        var tooltip = context.$('.ui-tooltip-top');
        var val = context.input.val();
        tooltip.fadeOut();
        if (context.tooltipTimeout) clearTimeout(context.tooltipTimeout);
        if (val == '' || val == context.input.attr('placeholder')) return;
        var show = function () {
                tooltip.show().fadeIn();
            };
        context.tooltipTimeout = _.delay(show, 1000);
    });


    // Update editing UI on switching mode to editing content
    // Subscribes to: beginContentEditing
    facade.subscribe('editFocus','beginContentEditing', function (context) {
        context.$el.addClass('editing');
        context.input.focus();
    });


    // Create a new todo entry
    // Subscribes to: createWhenEntered
    facade.subscribe('keyboardManager','createWhenEntered', function (context, e, todos) {
        if (e.keyCode != 13) return;
        todos.create(context.newAttributes());
        context.input.val('');
    });



    // A Todo and remaining entry counter
    // Subscribes to: renderDone
    facade.subscribe('todoCounter','renderDone', function (context, Todos) {
        var done = Todos.done().length;
        context.$('#todo-stats').html(context.statsTemplate({
            total: Todos.length,
            done: Todos.done().length,
            remaining: Todos.remaining().length
        }));
    });


    // Clear all completed todos when clearContent is dispatched
    // Subscribes to: clearContent
    facade.subscribe('garbageCollector','clearContent', function (Todos) {
        _.each(Todos.done(), function (todo) {
            todo.clear();
        });
    });


});
```

That's it for this section. If you've been intrigued by some of the concepts covered, I encourage you to consider taking a look at my [slides](http://addyosmani.com/blog/large-scale-javascript-application-architecture/) on Large-scale JS from the jQuery Summit or my longer post on the topic [here](http://addyosmani.com/largescalejavascript) for more information.


## Paginating Backbone.js Requests & Collections

Pagination is a ubiquitous problem we often find ourselves needing to solve on the web. Perhaps most predominantly when working with back-end APIs and JavaScript-heavy clients which consume them.

On this topic, we're going to go through a set of **pagination components** I wrote for Backbone.js, which should hopefully come in useful if you're working on applications which need to tackle this problem. They're part of an extension called [Backbone.Paginator](http://github.com/addyosmani/backbone.paginator).

When working with a structural framework like Backbone.js, the three types of pagination we are most likely to run into are:

**Requests to a service layer (API)**- e.g query for results containing the term 'Brendan' - if 5,000 results are available only display 20 results per page (leaving us with 250 possible result pages that can be navigated to).

This problem actually has quite a great deal more to it, such as maintaining persistence of other URL parameters (e.g sort, query, order) which can change based on a user's search configuration in a UI. One also had to think of a clean way of hooking views up to this pagination so you can easily navigate between pages (e.g First, Last, Next, Previous, 1,2,3), manage the number of results displayed per page and so on.

**Further client-side pagination of data returned -** e.g we've been returned a JSON response containing 100 results. Rather than displaying all 100 to the user, we only display 20 of these results within a navigatable UI in the browser.

Similar to the request problem, client-pagination has its own challenges like navigation once again (Next, Previous, 1,2,3), sorting, order, switching the number of results to display per page and so on.

**Infinite results** - with services such as Facebook, the concept of numeric pagination is instead replaced with a 'Load More' or 'View More' button. Triggering this normally fetches the next 'page' of N results but rather than replacing the previous set of results loaded entirely, we simply append to them instead.

A request pager which simply appends results in a view rather than replacing on each new fetch is effectively an 'infinite' pager.

**Let's now take a look at exactly what we're getting out of the box:**

*Paginator is a set of opinionated components for paginating collections of data using Backbone.js. It aims to provide both solutions for assisting with pagination of requests to a server (e.g an API) as well as pagination of single-loads of data, where we may wish to further paginate a collection of N results into M pages within a view.*


### Paginator's pieces

Backbone.Paginator supports two main pagination components:

* **Backbone.Paginator.requestPager**: For pagination of requests between a client and a server-side API
* **Backbone.Paginator.clientPager**: For pagination of data returned from a server which you would like to further paginate within the UI (e.g 60 results are returned, paginate into 3 pages of 20)


### Live Examples

Live previews of both pagination components using the Netflix API can be found below. Fork the repository to experiment with these examples further.

* [Backbone.Paginator.requestPager()](http://addyosmani.github.com/backbone.paginator/examples/netflix-request-paging/index.html)
* [Backbone.Paginator.clientPager()](http://addyosmani.github.com/backbone.paginator/examples/netflix-client-paging/index.html)
* [Infinite Pagination (Backbone.Paginator.requestPager())](http://addyosmani.github.com/backbone.paginator/examples/netflix-infinite-paging/index.html)
* [Diacritic Plugin](http://addyosmani.github.com/backbone.paginator/examples/google-diacritic/index.html)

### Paginator.requestPager

In this section we're going to walkthrough actually using the requestPager.

#### 1. Create a new Paginated collection

First, we define a new Paginated collection using `Backbone.Paginator.requestPager()` as follows:

```javascript
var PaginatedCollection = Backbone.Paginator.requestPager.extend({
```

#### 2: Set the model for the collection as normal

Within our collection, we then (as normal) specify the model to be used with this collection followed by the URL (or base URL) for the service providing our data (e.g the Netflix API).

```javascript
        model: model,
```

#### 3. Configure the base URL and the type of the request

We need to set a base URL. The `type` of the request is `GET` by default, and the `dataType` is `jsonp` in order to enable cross-domain requests.

```javascript
    paginator_core: {
      // the type of the request (GET by default)
      type: 'GET',

      // the type of reply (jsonp by default)
      dataType: 'jsonp',

      // the URL (or base URL) for the service
      url: 'http://odata.netflix.com/Catalog/People(49446)/TitlesActedIn?'
    },
```

#### 4. Configure how the library will show the results

We need to tell the library how many items per page would we like to see, etc...

```javascript
    paginator_ui: {
      // the lowest page index your API allows to be accessed
      firstPage: 0,

      // which page should the paginator start from
      // (also, the actual page the paginator is on)
      currentPage: 0,

      // how many items per page should be shown
      perPage: 3,

      // a default number of total pages to query in case the API or
      // service you are using does not support providing the total
      // number of pages for us.
      // 10 as a default in case your service doesn't return the total
      totalPages: 10
    },
```

#### 5. Configure the parameters we want to send to the server

Only the base URL won't be enough for most cases, so you can pass more parameters to the server.
Note how you can use functions insead of hardcoded values, and you can also reffer to the values you specified in `paginator_ui`.

```javascript
    server_api: {
      // the query field in the request
      '$filter': '',

      // number of items to return per request/page
      '$top': function() { return this.perPage },

      // how many results the request should skip ahead to
      // customize as needed. For the Netflix API, skipping ahead based on
      // page * number of results per page was necessary.
      '$skip': function() { return this.currentPage * this.perPage },

      // field to sort by
      '$orderby': 'ReleaseYear',

      // what format would you like to request results in?
      '$format': 'json',

      // custom parameters
      '$inlinecount': 'allpages',
      '$callback': 'callback'
    },
```

#### 6. Finally, configure Collection.parse() and we're done

The last thing we need to do is configure our collection's `parse()` method. We want to ensure we're returning the correct part of our JSON response containing the data our collection will be populated with, which below is `response.d.results` (for the Netflix API).

You might also notice that we're setting `this.totalPages` to the total page count returned by the API. This allows us to define the maximum number of (result) pages available for the current/last request so that we can clearly display this in the UI. It also allows us to infuence whether clicking say, a 'next' button should proceed with a request or not.

```javascript
        parse: function (response) {
            // Be sure to change this based on how your results
            // are structured (e.g d.results is Netflix specific)
            var tags = response.d.results;
            //Normally this.totalPages would equal response.d.__count
            //but as this particular NetFlix request only returns a
            //total count of items for the search, we divide.
            this.totalPages = Math.floor(response.d.__count / this.perPage);
            return tags;
        }
    });

});
```

#### Convenience methods:

For your convenience, the following methods are made available for use in your views to interact with the `requestPager`:

* **Collection.goTo( n, options )** - go to a specific page
* **Collection.requestNextPage( options )** - go to the next page
* **Collection.requestPreviousPage( options )** - go to the previous page
* **Collection.howManyPer( n )** - set the number of items to display per page

**requestPager** collection's methods `.goTo()`, `.requestNextPage()` and `.requestPreviousPage()` are all extension of the original [Backbone Collection.fetch() method](http://documentcloud.github.com/backbone/#Collection-fetch). As so, they all can take the same option object as parameter.

This option object can use `success` and `error` parameters to pass a function to be executed after server answer.

```javascript
Collection.goTo(n, {
  success: function( collection, response ) {
    // called is server request success
  },
  error: function( collection, response ) {
    // called if server request fail
  }
});
```

To manage callback, you could also use the [jqXHR](http://api.jquery.com/jQuery.ajax/#jqXHR) returned by these methods to manage callback.

```javascript
Collection
  .requestNextPage()
  .done(function( data, textStatus, jqXHR ) {
    // called is server request success
  })
  .fail(function( data, textStatus, jqXHR ) {
    // called if server request fail
  })
  .always(function( data, textStatus, jqXHR ) {
    // do something after server request is complete
  });
});
```

If you'd like to add the incoming models to the current collection, instead of replacing the collection's contents, pass `{add: true}` as an option to these methods.

```javascript
Collection.requestPreviousPage({ add: true });
```

### Paginator.clientPager

The `clientPager` works similar to the `requestPager`, except that our configuration values influence the pagination of data already returned at a UI-level. Whilst not shown (yet) there is also a lot more UI logic that ties in with the `clientPager`. An example of this can be seen in 'views/clientPagination.js'.

#### 1. Create a new paginated collection with a model and URL
As with `requestPager`, let's first create a new Paginated `Backbone.Paginator.clientPager` collection, with a model:

```javascript
    var PaginatedCollection = Backbone.Paginator.clientPager.extend({

        model: model,
```

#### 2. Configure the base URL and the type of the request

We need to set a base URL. The `type` of the request is `GET` by default, and the `dataType` is `jsonp` in order to enable cross-domain requests.

```javascript
    paginator_core: {
      // the type of the request (GET by default)
      type: 'GET',

      // the type of reply (jsonp by default)
      dataType: 'jsonp',

      // the URL (or base URL) for the service
      url: 'http://odata.netflix.com/v2/Catalog/Titles?&'
    },
```

#### 3. Configure how the library will show the results

We need to tell the library how many items per page would we like to see, etc...

```javascript
    paginator_ui: {
      // the lowest page index your API allows to be accessed
      firstPage: 1,

      // which page should the paginator start from
      // (also, the actual page the paginator is on)
      currentPage: 1,

      // how many items per page should be shown
      perPage: 3,

      // a default number of total pages to query in case the API or
      // service you are using does not support providing the total
      // number of pages for us.
      // 10 as a default in case your service doesn't return the total
      totalPages: 10
    },
```

#### 4. Configure the parameters we want to send to the server

Only the base URL won't be enough for most cases, so you can pass more parameters to the server.
Note how you can use functions insead of hardcoded values, and you can also reffer to the values you specified in `paginator_ui`.

```javascript
    server_api: {
      // the query field in the request
      '$filter': 'substringof(\'america\',Name)',

      // number of items to return per request/page
      '$top': function() { return this.perPage },

      // how many results the request should skip ahead to
      // customize as needed. For the Netflix API, skipping ahead based on
      // page * number of results per page was necessary.
      '$skip': function() { return this.currentPage * this.perPage },

      // field to sort by
      '$orderby': 'ReleaseYear',

      // what format would you like to request results in?
      '$format': 'json',

      // custom parameters
      '$inlinecount': 'allpages',
      '$callback': 'callback'
    },
```

#### 5. Finally, configure Collection.parse() and we're done

And finally we have our `parse()` method, which in this case isn't concerned with the total number of result pages available on the server as we have our own total count of pages for the paginated data in the UI.

```javascript
    parse: function (response) {
            var tags = response.d.results;
            return tags;
        }

    });
```

#### Convenience methods:

As mentioned, your views can hook into a number of convenience methods to navigate around UI-paginated data. For `clientPager` these include:

* **Collection.goTo(n)** - go to a specific page
* **Collection.previousPage()** - go to the previous page
* **Collection.nextPage()** - go to the next page
* **Collection.howManyPer(n)** - set how many items to display per page
* **Collection.setSort(sortBy, sortDirection)** - update sort on the current view. Sorting will automatically detect if you're trying to sort numbers (even if they're strored as strings) and will do the right thing.
* **Collection.setFilter(filterFields, filterWords)** - filter the current view. Filtering supports multiple words without any specific order, so you'll basically get a full-text search ability. Also, you can pass it only one field from the model, or you can pass an array with fields and all of them will get filtered. Last option is to pass it an object containing a comparison method and rules. Currently, only ```levenshtein``` method is available.

```javascript
  this.collection.setFilter(
    {'Name': {cmp_method: 'levenshtein', max_distance: 7}}
    , 'Amreican P' // Note the switched 'r' and 'e', and the 'P' from 'Pie'
  );
```

Also note that the levenshtein plugin should be loaded and enabled using the ```useLevenshteinPlugin``` variable.

Last but not less important: Performing Levenshtein comparison returns the ```distance``` between to strings. It won't let you *search* lenghty text.

The distance between two strings means the number of characters that should be added, removed or moved to the left or to the right so the strings get equal.

That means that comparing "Something" in "This is a test that could show something" will return 32, which is bigger than comparing "Something" and "ABCDEFG" (9).

Use levenshtein only for short texts (titles, names, etc).

* **Collection.doFakeFilter(filterFields, filterWords)** - returns the models count after fake-applying a call to ```Collection.setFilter```.

* **Collection.setFieldFilter(rules)** - filter each value of each model according to `rules` that you pass as argument. Example: You have a collection of books with 'release year' and 'author'. You can filter only the books that were released between 1999 and 2003. And then you can add another `rule` that will filter those books only to authors who's name start with 'A'. Possible rules: function, required, min, max, range, minLength, maxLength, rangeLength, oneOf, equalTo, pattern.


```javascript

  my_collection.setFieldFilter([
    {field: 'release_year', type: 'range', value: {min: '1999', max: '2003'}},
    {field: 'author', type: 'pattern', value: new RegExp('A*', 'igm')}
  ]);

  //Rules:
  //
  //var my_var = 'green';
  //
  //{field: 'color', type: 'equalTo', value: my_var}
  //{field: 'color', type: 'function', value: function(field_value){ return field_value == my_var; } }
  //{field: 'color', type: 'required'}
  //{field: 'number_of_colors', type: 'min', value: '2'}
  //{field: 'number_of_colors', type: 'max', value: '4'}
  //{field: 'number_of_colors', type: 'range', value: {min: '2', max: '4'} }
  //{field: 'color_name', type: 'minLength', value: '4'}
  //{field: 'color_name', type: 'maxLength', value: '6'}
  //{field: 'color_name', type: 'rangeLength', value: {min: '4', max: '6'}}
  //{field: 'color_name', type: 'oneOf', value: ['green', 'yellow']}
  //{field: 'color_name', type: 'pattern', value: new RegExp('gre*', 'ig')}

```

* **Collection.doFakeFieldFilter(rules)** - returns the models count after fake-applying a call to ```Collection.setFieldFilter```.

#### Implementation notes:

You can use some variables in your ```View``` to represent the actual state of the paginator.

```totalUnfilteredRecords``` - Contains the number of records, including all records filtered in any way. (Only available in ```clientPager```)

```totalRecords``` - Contains the number of records

```currentPage``` - The actual page were the paginator is at.

```perPage``` - The number of records the paginator will show per page.

```totalPages``` - The number of total pages.

```startRecord``` - The posicion of the first record shown in the current page (eg 41 to 50 from 2000 records) (Only available in ```clientPager```)

```endRecord``` - The posicion of the last record shown in the current page (eg 41 to 50 from 2000 records) (Only available in ```clientPager```)

### Plugins

**Diacritic.js**

A plugin for Backbone.Paginator that replaces diacritic characters (ă,ş,ţ etc) with characters that match them most closely. This is particularly useful for filtering.

To enable the plugin, set `this.useDiacriticsPlugin` to true, as can be seen in the example below:

```javascript
Paginator.clientPager = Backbone.Collection.extend({

    // Default values used when sorting and/or filtering.
    initialize: function(){
      this.useDiacriticsPlugin = true; // use diacritics plugin if available
    ...
```

 [1]: http://github.com/addyosmani/backbone.paginator
 [2]: http://addyosmani.github.com/backbone.paginator/
 [3]: http://addyosmani.github.com/backbone.paginator/examples/netflix-request-paging/index.html
 [4]: http://addyosmani.github.com/backbone.paginator/examples/netflix-client-paging/index.html
 [5]: http://addyosmani.github.com/backbone.paginator/examples/netflix-infinite-paging/index.html
 [6]: http://github.com/addyosmani/backbone.paginator/issues
 [7]: https://github.com/cowboy/grunt

## <a name="thorax">Thorax</a>

*By Ryan Eastridge & Addy Osmani*


### view helper

### Creating new View helpers

Note that this differs from `Handlebars.registerHelper`. Registers a helper that will create and append a new `HelperView` instance, with it's `template` attribute set to the value of the captured block. `callback` will recieve any arguments passed to the helper followed by a `HelperView` instance. Named arguments to the helper will be present on `options` attribute of the `HelperView` instance.

A `HelperView` instance differs from a regular view instance in that it has a `parent` attribute which is always set to the declaring view, and a `context` which always returns the value of the `parent`'s context method. The `collection`, `empty` and other built in block view helpers are created with `registerViewHelper`.

A helper that re-rendered a `HelperView` every time an event was triggered on the declaring / parent view could be implemented as:

    Handlebars.registerViewHelper('on', function(eventName, helperView) {
      // register a handler on the parent view, which will be automatically
      // unregistered when helperView is destroyed
      helperView.on(helperView.parent, eventName, function() {
        helperView.render();
      });
    });

An example use of this would be to have a counter that would incriment each time a button was clicked. In Handlebars:

    {{#on "incrimented"}}{{i}}{/on}}
    {{#button trigger="incrimented"}}Add{{/button}}

And the corresponding view class:

    new Thorax.View({
        events: {
            incrimented: function() {
                ++this.i;
            }
        },
        initialize: function() {
            this.i = 0;
        },
        template: ...
    });


- $.view, $.model, $.collection


--------

- registry
- template helper
  - straight embed
  - yield
- view helper
- registerViewHelper
- collection helper
- layout and view lifecycle
- bindToRoute
- mobile
