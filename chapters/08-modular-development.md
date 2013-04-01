# Modular Development

## Introduction

When we say an application is modular, we generally mean it's composed of a set of highly decoupled, distinct pieces of functionality stored in modules. As you probably know, loose coupling facilitates easier maintainability of apps by removing dependencies where possible. When this is implemented efficiently, it's quite easy to see how changes to one part of a system may affect another.

Unlike some more traditional programming languages, the current iteration of JavaScript (ECMA-262) doesn't provide developers with the means to import such modules of code in a clean, organized manner. 

Instead, developers are left to fall back on variations of the module or object literal patterns combined with script tags or a script loader. With many of these, module scripts are strung together in the DOM with namespaces being described by a single global object where it's still possible to have name collisions. There's also no clean way to handle dependency management without some manual effort or third party tools.

Whilst native solutions to these problems may be arriving via [ES6](http://wiki.ecmascript.org/doku.php?id=harmony:specification_drafts) (the next version of the official JavaScript specification) [modules proposal](http://wiki.ecmascript.org/doku.php?id=harmony:modules), the good news is that writing modular JavaScript has never been easier and you can start doing it today.

In this next part of the book, we're going to look at how to use AMD modules and RequireJS to cleanly wrap units of code in your application into manageable modules. We'll also cover an alternate approach called Lumbar which uses routes to determine when modules are loaded.

## Organizing modules with RequireJS and AMD

*Partly Contributed by [Jack Franklin](https://github.com/jackfranklin)*

[RequireJS](http://requirejs.org) is a popular script loader written by James Burke - a developer who has been quite instrumental in helping shape the AMD module format, which we'll discuss shortly. Amongst other things RequireJS helps you to load multiple script files, define modules with or without dependencies, and load in non-script dependencies such as text files.

### Maintainability problems with multiple script files

You might be thinking that there is little benefit to RequireJS. After all, you can simply load in your JavaScript files through multiple `<script>` tags, which is very straightforward. However, doing it that way has a lot of drawbacks, including increasing the HTTP overhead.

Every time the browser loads in a file you've referenced in a `<script>` tag, it makes an HTTP request to load the file's contents. It has to make a new HTTP request for each file you want to load, which causes problems.

- Browsers are limited in how many parallel requests they can make, so often it's slow to load multiple files, as it can only do a certain number at a time. This number depends on the user's settings and browser, but is usually around 4-8. When working on Backbone applications it's good to split your app into multiple JS files, so it's easy to hit that limit quickly. This can be negated by minifying your code into one file as part of a build process, but does not help with the next point.
- Scripts are loaded synchronously. This means that the browser cannot continue page rendering while the script is loading, .

What tools like RequireJS do is load scripts asynchronously. This means we have to adjust our code slightly, you can't just swap out `<script>` elements for a small piece of RequireJS code, but the benefits are very worthwhile:

- Loading the scripts asynchronously means the load process is non-blocking. The browser can continue to render the rest of the page as the scripts are being loaded, speeding up the initial load time.
- We can load modules in more intelligently, having more control over when they are loaded and ensuring that modules which have dependencies are loaded in the right order.

### Need for better dependency management

Dependency management is a challenging subject, in particular when writing JavaScript in the browser. The closest thing we have to dependency management by default is simply making sure we order our `<script>` tags such that code that depends on code in another file is loaded after the file it depends on. This is not a good approach. As I've already discussed, loading multiple files in that way is bad for performance; needing them to be loaded in a certain order is very brittle.

Being able to load code on an as-needed basis is something RequireJS is very good at. Rather than load all our JavaScript code in during initial page load, a better approach is to dynamically load modules when that code is required. This avoids loading all the code when the user first hits your application, consequently speeding up initial load times.

Think about the GMail web client for a moment. When a user initially loads the page on their first visit, Google can simply hide widgets such as the chat module until the user has indicated (by clicking 'expand') that they wish to use it. Through dynamic dependency loading, Google could load up the chat module at that time, rather than forcing all users to load it when the page first initializes. This can improve performance and load times and can definitely prove useful when building larger applications. As the codebase for an application grows this becomes even more important.

The important thing to note here is that while it's absolutely fine to develop applications without a script loader, there are significant benefits to utilizing tools like RequireJS in your application.

### Asynchronous Module Definition (AMD)

RequireJS implements the [AMD Specification](https://github.com/amdjs/amdjs-api/wiki/AMD) which defines a method for writing modular code and managing dependencies. The RequireJS website also has a section [documenting the reasons behind implementing AMD](http://requirejs.org/docs/whyamd.html):

> The AMD format comes from wanting a module format that was better than today's "write a bunch of script tags with implicit dependencies that you have to manually order" and something that was easy to use directly in the browser. Something with good debugging characteristics that did not require server-specific tooling to get started.

### Writing AMD modules with RequireJS

As discussed above, the overall goal for the AMD format is to provide a solution for modular JavaScript that developers can use today. The two key concepts you need to be aware of when using it with a script-loader are the `define()` method for defining modules and the `require()` method for loading dependencies. `define()` is used to define named or unnamed modules using the following signature:

```javascript
define(
    module_id /*optional*/,
    [dependencies] /*optional*/,
    definition function /*function for instantiating the module or object*/
);
```

As you can tell by the inline comments, the `module_id` is an optional argument which is typically only required when non-AMD concatenation tools are being used (there may be some other edge cases where it's useful too). When this argument is left out, we call the module 'anonymous'. When working with anonymous modules, RequireJS will use a module's file path as its module id, so the adage Don't Repeat Yourself (DRY) should be applied by omitting the module id in the `define()` invocation.

The dependencies argument is an array representing all of the other modules that this module depends on and the third argument is a factory that can either be a function that should be executed to instantiate the module or an object. 

A barebones module (compatible with RequireJS) could be defined using `define()` as follows:

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

*Note: RequireJS is intelligent enough to automatically infer the '.js' extension to your script file names. As such, this extension is generally omitted when specifying dependencies.*

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

Before using RequireJS and Backbone we will first set up a very basic RequireJS project to demonstrate how it works. The first thing to do is to [Download RequireJS](http://requirejs.org/docs/download.html#requirejs). When you load in the RequireJS script in your HTML file, you need to also tell it where your main JavaScript file is located. Typically this will be called something like "app.js", and is the main entry point for your application. You do this by adding in a `data-main` attribute to the `script` tag:

```html
<script data-main="app.js" src="lib/require.js"></script>
```

Now, RequireJS will automatically load `app.js` for you.

#### RequireJS Configuration

In the main JavaScript file that you load with the `data-main` attribute you can configure how RequireJS loads the rest of your application. This is done by calling `require.config`, and passing in an object:

```javascript
require.config({
    // your configuration key/values here
    baseUrl: "app", // generally the same directory as the script used in a data-main attribute for the top level script
    paths: {}, // set up custom paths to libraries, or paths to RequireJS plugins
    shim: {}, // used for setting up all Shims (see below for more detail)
});
```

The main reason you'd want to configure RequireJS is to add shims, which we'll cover next. To see other configuration options available to you, I recommend checking out the [RequireJS documentation](http://requirejs.org/docs/api.html#config).


##### RequireJS Shims

Ideally, each library that we use with RequireJS will come with AMD support. That is, it uses the `define` method to define the library as a module. However, some libraries - including Backbone and one of its dependencies, Underscore - don't do this. Fortunately RequireJS comes with a way to work around this.

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

Note that when specifying paths for RequireJS you should omit the `.js` from the end of script names.

The important line here is `exports: '_'`. This line tells RequireJS that the script in `'lib/underscore.js'` creates a global variable called `_` instead of defining a module. Now when we list Underscore as a dependency RequireJS will know to give us the `_` global variable as though it was the module defined by that script. We can set up a shim for Backbone too:

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

Again, that configuration tells RequireJS to return the global `Backbone` variable that Backbone exports, but this time you'll notice that Backbone's dependencies are defined. This means whenever this:

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

Just make sure that you refer to the custom path in your shim settings, too. Now you can do

```javascript
require( ['underscore'], function(_) {
// code here
});
```

to shim Underscore but still use a custom path.


### Require.js and Backbone Examples

Now that we've taken a look at how to define AMD modules, let's review how to go about wrapping components like views and collections so that they can also be easily loaded as dependencies for any parts of your application that require them. At its simplest, a Backbone model may just require Backbone and Underscore.js. These are dependencies, so we can define those when defining the new modules. Note that the following examples presume you have configured RequireJS to shim Backbone and Underscore, as discussed previously.

#### Wrapping models, views, and other components with AMD

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

Aliasing to the dollar-sign (`$`) once again makes it very easy to encapsulate any part of an application you wish using AMD.

Doing it this way makes it easy to organize your Backbone application as you like. It's recommended to separate modules into folders. For example, individual folders for models, collections, views and so on. RequireJS doesn't care about what folder structure you use; as long as you use the correct path when using `require`, it will happily pull in the file.

As part of this chapter I've made a very simple [Backbone application with RequireJS that you can find on Github](https://github.com/javascript-playground/backbone-require-example). It is a stock application for a manager of a shop. They can add new items and filter down the items based on price, but nothing more. Because it's so simple it's easier to focus purely on the RequireJS part of the implementation, rather than deal with complex JavaScript and Backbone logic too.

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

Let's now set up a view for that individual item:

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

This view doesn't actually depend on the model it will be used with, so again the only dependency is Backbone. Other than that it's just a regular Backbone view. There's nothing special going on here, other than returning the object and using `define` so RequireJS can pick it up. Now let's make a collection to view a list of items. This time we will need to reference the `Item` model, so we add it as a dependency:

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

I've called this collection `Cart`, as it's a group of items. As the `Item` model is the second dependency, I can bind the variable `Item` to it by declaring it as the second argument to the callback function. I can then refer to this within my collection implementation.

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

If you'd like to take a look at how others do it, [Pete Hawkins' Backbone Stack repository](https://github.com/phawk/Backbone-Stack) is a good example of structuring a Backbone application using RequireJS. Greg Franko has also written [an overview of how he uses Backbone and Require](http://gregfranko.com/blog/using-backbone-dot-js-with-require-dot-js/), and [Jeremy Kahn's post](http://jeremyckahn.github.com/blog/2012/08/18/keeping-it-sane-backbone-views-and-require-dot-js/) neatly describes his approach. For a look at a full sample application, the [Backbone and Require version](https://github.com/addyosmani/todomvc/tree/gh-pages/dependency-examples/backbone_require) of the TodoMVC application is a good starting point.

### Keeping Your Templates External Using RequireJS And The Text Plugin

Moving your templates to external files is actually quite straight-forward, whether they are Underscore, Mustache, Handlebars or any other text-based template format. Let's look at how we do that with RequireJS.

RequireJS has a special plugin called text.js which is used to load in text file dependencies. To use the text plugin, follow these steps:

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

With RequireJS and the text plugin however, it's as simple as saving the same template into an external text file (say, `mainView.html`) and doing the following:

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


### Optimizing Backbone apps for production with the RequireJS Optimizer

Once you're written your application, the next important step is to prepare it for deployment to production. The majority of non-trivial apps are likely to consist of several scripts and so optimizing, minimizing, and concatenating your scripts prior to pushing can reduce the number of scripts your users need to download.

A command-line optimization tool for RequireJS projects called r.js is available to help with this workflow. It offers a number of capabilities, including:

* Concatenating specific scripts and minifying them using external tools such as UglifyJS (which is used by default) or Google's Closure Compiler for optimal browser delivery, whilst preserving the ability to dynamically load modules
* Optimizing CSS and stylesheets by inlining CSS files imported using @import, stripping out comments, etc.
* The ability to run AMD projects in both Node and Rhino (more on this later)

If you find yourself wanting to ship a single file with all dependencies included, r.js can help with this too. Whilst RequireJS does support lazy-loading, your application may be small enough that reducing HTTP requests to a single script file is feasible.

You'll notice that I mentioned the word 'specific' in the first bullet point. The RequireJS optimizer only concatenates module scripts that have been specified as string literals in `require` and `define` calls (which you've probably used). As clarified by the [optimizer docs](http://requirejs.org/docs/optimization.html) this means that Backbone modules defined like this:

```javascript
define(['jquery', 'backbone', 'underscore', 'collections/sample', 'views/test'],
    function($, Backbone, _, Sample, Test){
        //...
    });
```

will combine fine, however dynamic dependencies such as:

```javascript
var models = someCondition ? ['models/ab', 'models/ac'] : ['models/ba', 'models/bc'];
define(['jquery', 'backbone', 'underscore'].concat(models),
    function($, Backbone, _, firstModel, secondModel){
        //...
    });
```

will be ignored. This is by design as it ensures that dynamic dependency/module loading can still take place even after optimization.

Although the RequireJS optimizer works fine in both Node and Java environments, it's strongly recommended to run it under Node as it executes significantly faster there.

To get started with r.js, grab it from the [RequireJS download page](http://requirejs.org/docs/download.html#rjs) or [through NPM](http://requirejs.org/docs/optimization.html#download). To begin getting our project to build with r.js, we will need to create a new build profile.

Assuming the code for our application and external dependencies are in `app/libs`, our build.js build profile could simply be:

```
({
  baseUrl: 'app',
  out: 'dist/main.js',
```

The paths above are relative to the `baseUrl` for our project and in our case it would make sense to make this the `app` folder. The `out` parameter informs r.js that we want to concatenate everything into a single file called `main.js` under the `dist/` directory. Note that here we do need to add the `.js` extension to the filename. Earlier, we saw that when referencing modules by filenames, you don't need to use the `.js` extension, however this is one case in which you do.

Alternatively, we can specify `dir`, which will ensure the contents of our `app` directory are copied into this directory. e.g:

```
({
  baseUrl: 'app',
  dir: 'release',
  out: 'dist/main.js'
```

Additional options that can be specified such as `modules` and `appDir` are not compatible with `out`, however let's briefly discuss them in case you do wish to use them. 

`modules` is an array where we can explicitly specify the module names we would like to have optimized.


```
    modules: [
        {
            name: 'app',
            exclude: [
                // If you prefer not to include certain 
                // libs exclude them here
            ]
        }
```

`appDir` - when specified, our`baseUrl` is relative to this parameter. If `appDir` is not defined, `baseUrl` is simply relative to the `build.js` file.

```
    appDir: './',
```

Back to our build profile, the `main` parameter is used to specify our main module - we are making use of `include` here as we're going to take advantage of [Almond](https://github.com/jrburke/almond) - a stripped down loader for RequireJS modules which is useful should you not need to load modules in dynamically. 

```
  include: ['libs/almond', 'main'],
  wrap: true,
```

`include` is another array which specifies the modules we want to include in the build. By specifying "main", r.js will trace over all modules main depends on and will include them. `wrap` wraps modules which RequireJS needs into a closure so that only what we export is included in the global environment. 

```
  paths: {
    backbone: 'libs/backbone',
    underscore: 'libs/underscore',
    jquery: 'libs/jquery',
    text: 'libs/text'
  }
})
```
The remainder of the build.js file would be a regular paths configuration object. We can compile our project into a target file by running:

```
node r.js -o build.js
```

which should place our compiled project into dist/main.js.

The build profile is usually placed inside the 'scripts' or 'js' directory of your project. As per the docs, this file can however exist anywhere you wish, but you'll need to edit the contents of your build profile accordingly.

That's it. As long as you have UglifyJS/Closure tools setup correctly, r.js should be able to easily optimize your entire Backbone project in just a few key-strokes. 

If you would like to learn more about build profiles, James Burke has a [heavily commented sample file](https://github.com/jrburke/r.js/blob/master/build/example.build.js) with all the possible options available.






