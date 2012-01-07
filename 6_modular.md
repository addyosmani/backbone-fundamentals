

##Modular JavaScript

When we say an application is modular, we generally mean it's composed of a set of highly decoupled, distinct pieces of functionality stored in modules. As you probably know, loose coupling facilitates easier maintainability of apps by removing dependencies where possible. When this is implemented efficiently, its quite easy to see how changes to one part of a system may affect another.

Unlike some more traditional programming languages however, the current iteration of JavaScript (ECMA-262) doesn't provide developers with the means to import such modules of code in a clean, organized manner. It's one of the concerns with specifications that haven't required great thought until more recent years where the need for more organized JavaScript applications became apparent.

Instead, developers at present are left to fall back on variations of the module or object literal patterns. With many of these, module scripts are strung together in the DOM with namespaces being described by a single global object where it's still possible to incur naming collisions in your architecture. There's also no clean way to handle dependency management without some manual effort or third party tools.

Whilst native solutions to these problems will be arriving in ES Harmony, the good news is that writing modular JavaScript has never been easier and you can start doing it today.

In this next part of the book, we're going to look at how to use AMD modules and RequireJS for cleanly wrapping units of code in your application into managable modules.


##Organizing modules with RequireJS and AMD

In case you haven't used it before, RequireJS is a popular script loader written by James Burke - a developer who has been quite instrumental in helping shape the AMD module format, which we'll discuss more shortly. Some of RequireJS's capabilities include helping to load multiple script files, helping define modules with or without dependencies and loading in non-script dependencies such as text files.

So, why use RequireJS with Backbone? Although Backbone is excellent when it comes to providing a sanitary structure to your applications, there are a few key areas where some additional help could be used:

1) Backbone doesn't endorse a particular approach to modular-development. Although this means it's quite open-ended for developers to opt for classical patterns like the module-pattern or Object Literals for structuring their apps (which both work fine), it also means developers aren't sure of what works best when other concerns come into play, such as dependency management.

RequireJS is compatible with the AMD (Asynchronous Module Definition) format, a format which was born from a desire to write something better than the 'write lots of script tags with implicit dependencies and manage them manually' approach to development. In addition to allowing you to clearly declare dependencies, AMD works well in the browser, supports string IDs for dependencies, declaring multiple modules in the same file and gives you easy-to-use tools to avoid polluting the global namespace.

2) Let's discuss dependency management a little more as it can actually be quite challenging to get right if you're doing it by hand. When we write modules in JavaScript, we ideally want to be able to handle the reuse of code units intelligently and sometimes this will mean pulling in other modules at run-time whilst at other times you may want to do this dynamically to avoid a large pay-load when the user first hits your application.

Think about the GMail web-client for a moment. When users initially load up the page on their first visit, Google can simply hide widgets such as the chat module until a user has indicated (by clicking 'expand') that they wish to use it. Through dynamic dependency loading, Google could load up the chat module only then, rather than forcing all users to load it when the page first initializes. This can improve performance and load times and can definitely prove useful when building larger applications.

I've previously written [a detailed article](http://addyosmani.com/writing-modular-js) covering both AMD and other module formats and script loaders in case you'd like to explore this topic further. The takeaway is that although it's perfectly fine to develop applications without a script loader or clean module format in place, it can be of significant benefit to consider using these tools in your application development.




###Writing AMD modules with RequireJS

As discussed above, the overall goal for the AMD format is to provide a solution for modular JavaScript that developers can use today. The two key concepts you need to be aware of when using it with a script-loader are a `define()` method for facilitating module definition and a `require()` method for handling dependency loading. `define()` is used to define named or unnamed modules based on the proposal using the following signature:

  define(
      module_id /*optional*/, 
      [dependencies] /*optional*/, 
      definition function /*function for instantiating the module or object*/
  );

As you can tell by the inline comments, the `module_id` is an optional argument which is typically only required when non-AMD concatenation tools are being used (there may be some other edge cases where it's useful too). When this argument is left out, we call the module 'anonymous'. When working with anonymous modules, the idea of a module's identity is DRY, making it trivial to avoid duplication of filenames and code.

Back to the define signature, the dependencies argument represents an array of dependencies which are required by the module you are defining and the third argument ('definition function') is a function that's executed to instantiate your module. A barebone module (compatible with RequireJS) could be defined using `define()` as follows:

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


####Alternate syntax
There is also a [sugared version](http://requirejs.org/docs/whyamd.html#sugar) of `define()` available that allows you to declare your dependencies as local variables using `require()`. This will feel familiar to anyone who's used node, and can be easier to add or remove dependencies.
Here is the previous snippet using the alternate syntax:

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


The `require()` method is typically used to load code in a top-level JavaScript file or within a module should you wish to dynamically fetch dependencies. An example of its usage is:

  // Consider 'foo' and 'bar' are two external modules
  // In this example, the 'exports' from the two modules loaded are passed as
  // function arguments to the callback (foo and bar)
  // so that they can similarly be accessed

  require(['foo', 'bar'], function ( foo, bar ) {
          // rest of your code here
          foo.doSomething();
  });


**Wrapping modules, views and other components with AMD**

Now that we've taken a look at how to define AMD modules, let's review how to go about wrapping components like views and collections so that they can also be easily loaded as dependencies for any parts of your application that require them. At it's simplest, a Backbone model may just require Backbone and Underscore.js. These are considered it's dependencies and so, to write an AMD model module, we would simply do this:

  define(['underscore', 'backbone'], function(_, Backbone) {
    var myModel = Backbone.Model.extend({

      // Default attributes 
      defaults: {
        content: "hello world",
      },

      // A dummy initialization method
      initialize: function() {
        if (!this.get("content")) {
          this.set({"content": this.defaults.content});
        }
      },

      clear: function() {
        this.destroy();
        this.view.remove();
      }

    });
    return myModel;
  });


Note how we alias Underscore.js's instance to `_` and Backbone to just `Backbone`, making it very trivial to convert non-AMD code over to using this module format. For a view which might require other dependencies such as jQuery, this can similarly be done as follows:

  define([
    'jquery',
    'underscore', 
    'backbone',
    'collections/mycollection',
    'views/myview'
    ], function($, _, Backbone, myCollection, myView){

    var AppView = Backbone.View.extend({
    ...


Aliasing to the dollar-sign (`$`), once again makes it very easy to encapsulate any part of an application you wish using AMD.


##External [Underscore/Handlebars/Mustache] templates using RequireJS

Moving your [Underscore/Mustache/Handlebars] templates to external files is actually quite straight-forward. As this application makes use of RequireJS, I'll discuss how to implement external templates using this specific script loader.

RequireJS has a special plugin called text.js which is used to load in text file dependencies. To use the text plugin, simply follow these simple steps:

1. Download the plugin from http://requirejs.org/docs/download.html#text and place it in either the same directory as your application's main JS file or a suitable sub-directory.

2. Next, include the text.js plugin in your initial RequireJS configuration options. In the code snippet below, we assume that RequireJS is being included in our page prior to this code snippet being executed. Any of the other scripts being loaded are just there for the sake of example.

  require.config( {
      paths: {
          'backbone':         'libs/AMDbackbone-0.5.3',
          'underscore':       'libs/underscore-1.2.2',
          'text':             'libs/require/text',
          'jquery':           'libs/jQuery-1.7.1',
          'json2':            'libs/json2',
          'datepicker':       'libs/jQuery.ui.datepicker',
          'datepickermobile': 'libs/jquery.ui.datepicker.mobile',
          'jquerymobile':     'libs/jquery.mobile-1.0'
      },
      baseUrl: 'app'
  });


3. When the `text!` prefix is used for a dependency, RequireJS will automatically load the text plugin and treat the dependency as a text resource. A typical example of this in action may look like..

  require(['js/app', 'text!templates/mainView.html'],
      function(app, mainView){
          // the contents of the mainView file will be
          // loaded into mainView for usage.
      }
  );


4. Finally we can use the text resource that's been loaded for templating purposes. You're probably used to storing your HTML templates inline using a script with a specific identifier.

With Underscore.js's micro-templating (and jQuery) this would typically be:

HTML:

  <script type="text/template" id="mainViewTemplate">
      <% _.each( person, function( person_item ){ %>
          <li><%= person_item.get("name") %></li>  
      <% }); %>
  </script>


JS:

  var compiled_template = _.template( $('#mainViewTemplate').html() );


With RequireJS and the text plugin however, it's as simple as saving your template into an external text file (say, `mainView.html`) and doing the following:

  require(['js/app', 'text!templates/mainView.html'],
      function(app, mainView){
          
          var compiled_template = _.template( mainView );
      }
  );

That's it!. You can then go applying your template to a view in Backbone doing something like:

  collection.someview.el.html( compiled_template( { results: collection.models } ) );


All templating solutions will have their own custom methods for handling template compilation, but if you understand the above, substituting Underscore's micro-templating for any other solution should be fairly trivial.

**Note:** You may also be interested in looking at [Require.js tpl](https://github.com/ZeeAgency/requirejs-tpl). It's an AMD-compatible version of the Underscore templating system that also includes support for optimization (pre-compiled templates) which can lead to better performance and no evals. I have yet to use it myself, but it comes as a recommended resource.


##Optimizing Backbone apps for production with the RequireJS Optimizer

As experienced developers may know, an essential final step when writing both small and large JavaScript web applications is the build process.  The majority of non-trivial apps are likely to consist of more than one or two scripts and so optimizing, minimizing and concatenating your scripts prior to pushing them to production will require your users to download a reduced number (if not just one) script file.

Note: If you haven't looked at build processes before and this is your first time hearing about them, you might find [my post and screencast on this topic](http://addyosmani.com/blog/client-side-build-process/) useful.

With some other structural JavaScript frameworks, my recommendation would normally be to implicitly use YUI Compressor or Google's closure compiler tools, but we have a slightly more elegant method available, when it comes to Backbone if you're using RequireJS. RequireJS has a command line optimization tool called r.js which has a number of capabilities, including:

* Concatenating specific scripts and minifiying them using external tools such as UglifyJS (which is used by default) or Google's Closure Compiler for optimal browser delivery, whilst preserving the ability to dynamically load modules
* Optimizing CSS and stylesheets by inlining CSS files imported using @import, stripping out comments etc.
* The ability to run AMD projects in both Node and Rhino (more on this later)

You'll notice that I mentioned the word 'specific' in the first bullet point. The RequireJS optimizer only concatenates module scripts that have been specified in arrays of string literals passed to top-level (i.e non-local) require and define calls. As clarified by the [optimizer docs](http://requirejs.org/docs/optimization.html) this means that Backbone modules defined like this:

  define(['jquery','backbone','underscore', 'collections/sample','views/test'], 
      function($,Backbone, _, Sample, Test){
          //...
      });

will combine fine, however inline dependencies such as:


  var models = someCondition ? ['models/ab','models/ac'] : ['models/ba','models/bc'];

will be ignored. This is by design as it ensures that dynamic dependency/module loading can still take place even after optimization. 

Although the RequireJS optimizer works fine in both Node and Java environments, it's strongly recommended to run it under Node as it executes significantly faster there. In my experience, it's a piece of cake to get setup with either environment, so go for whichever you feel most comfortable with. 

To get started with r.js, grab it from the [RequireJS download page](http://requirejs.org/docs/download.html#rjs) or [through NPM](http://requirejs.org/docs/optimization.html#download). Now, the RequireJS optimizer works absolutely fine for single script and CSS files, but for most cases you'll want to actually optimize an entire Backbone project. You *could* do this completely from the command-line, but a cleaner option is using build profiles.

Below is an example of a build file taken from the modular jQuery Mobile app referenced later in this book. A **build profile** (commonly named `app.build.js`) informs RequireJS to copy all of the content of `appDir` to a directory defined by `dir` (in this case `../release`). This will apply all of the necessary optimizations inside the release folder. The `baseUrl` is used to resolve the paths for your modules. It should ideally be relative to `appDir`.

Near the bottom of this sample file, you'll see an array called `modules`. This is where you specify the module names you wish to have optimized. In this case we're optimizing the main application called 'app', which maps to `appDir/app.js`. If we had set the `baseUrl` to 'scripts', it would be mapped to `appDir/scripts/app.js`.

  ({
      appDir: "./",
      baseUrl: "./",
      dir: "../release",
      paths: {
         'backbone':          'libs/AMDbackbone-0.5.3',
          'underscore':       'libs/underscore-1.2.2',
          'jquery':           'libs/jQuery-1.7.1',
          'json2':            'libs/json2',
          'datepicker':       'libs/jQuery.ui.datepicker',
          'datepickermobile': 'libs/jquery.ui.datepicker.mobile',
          'jquerymobile':     'libs/jquery.mobile-1.0'
      },
      optimize: "uglify",
      modules: [
          {
              name: "app",
              exclude: [
                  // If you prefer not to include certain libs exclude them here
              ]
          }
      ]
  })

The way the build system in r.js works is that it traverses app.js (whatever modules you've passed) and resolved dependencies, concatenating them into the final `release`(dir) folder. CSS is treated the same way.

The build profile is usually placed inside the 'scripts' or 'js' directory of your project. As per the docs, this file can however exist anywhere you wish, but you'll need to edit the contents of your build profile accordingly. 

Finally, to run the build, execute the following command once insice your `appDir` or `appDir/scripts` directory:

  node ../../r.js -o app.build.js

That's it. As long as you have UglifyJS/Closure tools setup correctly, r.js should be able to easily optimize your entire Backbone project in just a few key-strokes. If you would like to learn more about build profiles, James Burke has a [heavily commented sample file](https://github.com/jrburke/r.js/blob/master/build/example.build.js) with all the possible options available.


##[Practical: Building a modular Backbone Todo app with AMD & RequireJS](#practical_modularapp)

In this chapter, we'll look at our first practical Backbone & RequireJS project - how to build a modular Todo application. The application will allow us to add new todos, edit new todos and clear todo items that have been marked as completed. For a more advanced practical, see the section on mobile Backbone development.

The complete code for the application can can be found in the `practicals/modular-todo-app` folder of this repo (thanks to Thomas Davis and J&eacute;r&ocirc;me Gravel-Niquet). Alternatively grab a copy of my side-project [TodoMVC](https://github.com/addyosmani/todomvc) which contains the sources to both AMD and non-AMD versions.

**Note:** Thomas may be covering a practical on this exercise in more detail on [backbonetutorials.com](http://backbonetutorials.com) at some point soon, but for this section I'll be covering what I consider the core concepts.

###Overview

Writing a 'modular' Backbone application can be a straight-forward process. There are however, some key conceptual differences to be aware of if opting to use AMD as your module format of choice:

* As AMD isn't a standard native to JavaScript or the browser, it's necessary to use a script loader (such as RequireJS or curl.js) in order to support defining components and modules using this module format. As we've already reviewed, there are a number of advantages to using the AMD as well as RequireJS to assist here.
* Models, views, controllers and routers need to be encapsulated *using* the AMD-format. This allows each component of our Backbone application to cleanly manage dependencies (e.g collections required by a view) in the same way that AMD allows non-Backbone modules to.
* Non-Backbone components/modules (such as utilities or application helpers) can also be encapsulated using AMD. I encourage you to try developing these modules in such a way that they can both be used and tested independent of your Backbone code as this will increase their ability to be re-used elsewhere.

Now that we've reviewed the basics, let's take a look at developing our application. For reference, the structure of our app is as follows:

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


###Markup

The markup for the application is relatively simple and consists of three primary parts: an input section for entering new todo items (`create-todo`), a list section to display existing items (which can also be edited in-place) (`todo-list`) and finally a section summarizing how many items are left to be completed (`todo-stats`).

<pre>
&lt;div id=&quot;todoapp&quot;&gt;

      &lt;div class=&quot;content&quot;&gt;

        &lt;div id=&quot;create-todo&quot;&gt;
          &lt;input id=&quot;new-todo&quot; placeholder=&quot;What needs to be done?&quot; type=&quot;text&quot; /&gt;
          &lt;span class=&quot;ui-tooltip-top&quot;&gt;Press Enter to save this task&lt;/span&gt;
        &lt;/div&gt;

        &lt;div id=&quot;todos&quot;&gt;
          &lt;ul id=&quot;todo-list&quot;&gt;&lt;/ul&gt;
        &lt;/div&gt;

        &lt;div id=&quot;todo-stats&quot;&gt;&lt;/div&gt;

      &lt;/div&gt;

&lt;/div&gt;
</pre>

The the rest of the tutorial will now focus on the JavaScript side of the practical.

###Configuration options

If you've read the earlier chapter on AMD, you may have noticed that explicitly needing to define each dependency a Backbone module (view, collection or other module) may require with it can get a little tedious. This can however be improved.

In order to simplify referencing common paths the modules in our application may use, we use a RequireJS [configuration object](http://requirejs.org/docs/api.html#config), which is typically defined as a top-level script file. Configuration objects have a number of useful capabilities, the most useful being mode name-mapping. Name-maps are basically a key:value pair, where the key defines the alias you wish to use for a path and key represents the true location of the path.

In the code-sample below, you can see some typical examples of common name-maps which include: `backbone`, `underscore`, `jquery` and depending on your choice, the RequireJS `text` plugin, which assists with loading text assets like templates.

**main.js**

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


The `require()` at the end of our main.js file is simply there so we can load and instantiation the primary view for our application (`views/app.js`). You'll commonly see both this and the configuration object included the most top-level script file for a project.

In addition to offering name-mapping, the configuration object can be used to define additional properties such as `waitSeconds` - the number of seconds to wait before script loading times out and `locale`, should you wish to load up i18n bundles for custom languages. The `baseUrl` is simply the path to use for module lookups.

For more information on configuration objects, please feel free to check out the excellent guide to them in the [RequireJS docs](http://requirejs.org/docs/api.html#config).


###Modularizing our models, views and collections

Before we dive into AMD-wrapped versions of our Backbone components, let's review a sample of a non-AMD view. The following view listens for changes to it's model (a Todo item) and re-renders if a user edits the value of the item.   

  var TodoView = Backbone.View.extend({

      //... is a list tag.
      tagName:  "li",

      // Cache the template function for a single item.
      template: _.template($('#item-template').html()),

      // The DOM events specific to an item.
      events: {
        "click .check"              : "toggleDone",
        "dblclick div.todo-content" : "edit",
        "click span.todo-destroy"   : "clear",
        "keypress .todo-input"      : "updateOnEnter"
      },

      // The TodoView listens for changes to its model, re-rendering. Since there's
      // a one-to-one correspondence between a **Todo** and a **TodoView** in this
      // app, we set a direct reference on the model for convenience.
      initialize: function() {
        _.bindAll(this, 'render', 'close');
        this.model.bind('change', this.render);
        this.model.view = this;
      },
      ...

Note how for templating the common practice of referencing a script by an ID (or other selector) and obtaining it's value is used. This of course requires that the template being accessed is implicitly defined in our markup. The following is the 'embedded' version of our template being referenced above:

<pre>
&lt;script type=&quot;text/template&quot; id=&quot;item-template&quot;&gt;
      &lt;div class=&quot;todo &lt;%= done ? &#39;done&#39; : &#39;&#39; %&gt;&quot;&gt;
        &lt;div class=&quot;display&quot;&gt;
          &lt;input class=&quot;check&quot; type=&quot;checkbox&quot; &lt;%= done ? &#39;checked=&quot;checked&quot;&#39; : &#39;&#39; %&gt; /&gt;
          &lt;div class=&quot;todo-content&quot;&gt;&lt;/div&gt;
          &lt;span class=&quot;todo-destroy&quot;&gt;&lt;/span&gt;
        &lt;/div&gt;
        &lt;div class=&quot;edit&quot;&gt;
          &lt;input class=&quot;todo-input&quot; type=&quot;text&quot; value=&quot;&quot; /&gt;
        &lt;/div&gt;
      &lt;/div&gt;
&lt;/script&gt;
</pre>

Whilst there is nothing wrong with the template itself, once we begin to develop larger applications requiring multiple templates, including them all in our markup on page-load can quickly become both unmanageable and come with performance costs. We'll look at solving this problem in a minute.

Let's now take a look at the AMD-version of our view. As discussed earlier, the 'module' is wrapped using AMD's `define()` which allows us to specify the dependencies our view requires. Using the mapped paths to 'jquery' etc. simplifies referencing common dependencies and instances of dependencies are themselves mapped to local variables that we can access (e.g 'jquery' is mapped to `$`).

**views/todos.js**

  define([
    'jquery', 
    'underscore', 
    'backbone',
    'text!templates/todos.html'
    ], function($, _, Backbone, todosTemplate){
    var TodoView = Backbone.View.extend({

      //... is a list tag.
      tagName:  "li",

      // Cache the template function for a single item.
      template: _.template(todosTemplate),

      // The DOM events specific to an item.
      events: {
        "click .check"              : "toggleDone",
        "dblclick div.todo-content" : "edit",
        "click span.todo-destroy"   : "clear",
        "keypress .todo-input"      : "updateOnEnter"
      },

      // The TodoView listens for changes to its model, re-rendering. Since there's
      // a one-to-one correspondence between a **Todo** and a **TodoView** in this
      // app, we set a direct reference on the model for convenience.
      initialize: function() {
        _.bindAll(this, 'render', 'close');
        this.model.bind('change', this.render);
        this.model.view = this;
      },

      // Re-render the contents of the todo item.
      render: function() {
        $(this.el).html(this.template(this.model.toJSON()));
        this.setContent();
        return this;
      },

      // Use `jQuery.text` to set the contents of the todo item.
      setContent: function() {
        var content = this.model.get('content');
        this.$('.todo-content').text(content);
        this.input = this.$('.todo-input');
        this.input.bind('blur', this.close);
        this.input.val(content);
      },
      ...

 From a maintenance perspective, there's nothing logically different in this version of our view, except for how we approach templating. 

Using the RequireJS text plugin (the dependency marked `text`), we can actally store all of the contents for the template we looked at earlier in an external file (todos.html).

**templates/todos.html**

<pre>
&lt;div class=&quot;todo &lt;%= done ? &#39;done&#39; : &#39;&#39; %&gt;&quot;&gt;
    &lt;div class=&quot;display&quot;&gt;
      &lt;input class=&quot;check&quot; type=&quot;checkbox&quot; &lt;%= done ? &#39;checked=&quot;checked&quot;&#39; : &#39;&#39; %&gt; /&gt;
      &lt;div class=&quot;todo-content&quot;&gt;&lt;/div&gt;
      &lt;span class=&quot;todo-destroy&quot;&gt;&lt;/span&gt;
    &lt;/div&gt;
    &lt;div class=&quot;edit&quot;&gt;
      &lt;input class=&quot;todo-input&quot; type=&quot;text&quot; value=&quot;&quot; /&gt;
    &lt;/div&gt;
&lt;/div&gt;
</pre>

There's no longer a need to be concerned with IDs for the template as we can map it's contents to a local variable (in this case `todosTemplate`). We then simply pass this to the Underscore.js templating function `_.template()` the same way we normally would have the value of our template script.

Next, let's look at how to define models as dependencies which can be pulled into collections. Here's an AMD-compatible model module, which has two default values: a `content` attribute for the content of a Todo item and a boolean `done` state, allowing us to trigger whether the item has been completed or not.

**models/todo.js**

  define(['underscore', 'backbone'], function(_, Backbone) {
    var TodoModel = Backbone.Model.extend({

      // Default attributes for the todo.
      defaults: {
        content: "empty todo...",
        done: false
      },

      // Ensure that each todo created has `content`.
      initialize: function() {
        if (!this.get("content")) {
          this.set({"content": this.defaults.content});
        }
      },

      // Toggle the `done` state of this todo item.
      toggle: function() {
        this.save({done: !this.get("done")});
      },

      // Remove this Todo from *localStorage* and delete its view.
      clear: function() {
        this.destroy();
        this.view.remove();
      }

    });
    return TodoModel;
  });


As per other types of dependencies, we can easily map our model module to a local variable (in this case `Todo`) so it can be referenced as the model to use for our `TodosCollection`. This collection also supports a simple `done()` filter for narrowing down Todo items that have been completed and a `remaining()` filter for those that are still outstanding.

**collections/todos.js**

  define([
    'underscore', 
    'backbone', 
    'libs/backbone/localstorage', 
    'models/todo'
    ], function(_, Backbone, Store, Todo){

      var TodosCollection = Backbone.Collection.extend({

      // Reference to this collection's model.
      model: Todo,

      // Save all of the todo items under the `"todos"` namespace.
      localStorage: new Store("todos"),

      // Filter down the list of all todo items that are finished.
      done: function() {
        return this.filter(function(todo){ return todo.get('done'); });
      },

      // Filter down the list to only todo items that are still not finished.
      remaining: function() {
        return this.without.apply(this, this.done());
      },
      ...

In addition to allowing users to add new Todo items from views (which we then insert as models in a collection), we ideally also want to be able to display how many items have been completed and how many are remaining. We've already defined filters that can provide us this information in the above collection, so let's use them in our main application view.

**views/app.js**

  define([
    'jquery',
    'underscore', 
    'backbone',
    'collections/todos',
    'views/todos',
    'text!templates/stats.html'
    ], function($, _, Backbone, Todos, TodoView, statsTemplate){

    var AppView = Backbone.View.extend({

      // Instead of generating a new element, bind to the existing skeleton of
      // the App already present in the HTML.
      el: $("#todoapp"),

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


Above, we map the second template for this project, `templates/stats.html` to `statsTemplate` which is used for rendering the overall `done` and `remaining` states. This works by simply passing our template the length of our overall Todos collection (`Todos.length` - the number of Todo items created so far) and similarly the length (counts) for items that have been completed (`Todos.done().length`) or are remaining (`Todos.remaining().length`).

The contents of our `statsTemplate` can be seen below. It's nothing too complicated, but does use ternary conditions to evaluate whether we should state there's "1 item" or "2 item<i>s</i>" in a particular state.

<pre>
 &lt;% if (total) { %&gt;
        &lt;span class=&quot;todo-count&quot;&gt;
          &lt;span class=&quot;number&quot;&gt;&lt;%= remaining %&gt;&lt;/span&gt;
          &lt;span class=&quot;word&quot;&gt;&lt;%= remaining == 1 ? &#39;item&#39; : &#39;items&#39; %&gt;&lt;/span&gt; left.
        &lt;/span&gt;
      &lt;% } %&gt;
      &lt;% if (done) { %&gt;
        &lt;span class=&quot;todo-clear&quot;&gt;
          &lt;a href=&quot;#&quot;&gt;
            Clear &lt;span class=&quot;number-done&quot;&gt;&lt;%= done %&gt;&lt;/span&gt;
            completed &lt;span class=&quot;word-done&quot;&gt;&lt;%= done == 1 ? &#39;item&#39; : &#39;items&#39; %&gt;&lt;/span&gt;
          &lt;/a&gt;
        &lt;/span&gt;
      &lt;% } %&gt;
</pre>



The rest of the source for the Todo app mainly consists of code for handling user and application events, but that rounds up most of the core concepts for this practical. 

To see how everything ties together, feel free to grab the source by cloning this repo or browse it [online](https://github.com/addyosmani/backbone-fundamentals/tree/master/practicals/modular-todo-app) to learn more. I hope you find it helpful!.

**Note:** While this first practical doesn't use a build profile at outlined in the chapter on using the RequireJS optimizer, we will be using one in the section on building mobile Backbone applications.


##Decoupling Backbone with the Mediator and Facade patterns

In this section we'll discuss applying some of the concepts I cover in my article on [Large-scale JavaScript Application development](http://addyosmani.com/largescalejavascript) to Backbone. 

### Summary

At a high-level, one architecture that works for such applications is something which is:

* **Highly decoupled**: encouraging modules to only publish and subscribe to events of interest rather than directly communicating with each other. This helps us to build applications which who's units of code aren't highly tied (coupled) together and can thus be reused more easily.
* **Supports module-level security**: whereby modules are only able to execute behaviour they've been permitted to. Application security is an area which is often overlooked in JavaScript applications, but can be quite easily implemented in a flexible manner.
* **Supports failover**: allowing an application continuing to function even if particular modules fail. The typical example I give of this is the GMail chat widget. Imagine being able to build applications in a way that if one widget on the page fails (e.g chat), the rest of your application (mail) can continue to function without being affected.

This is an architecture which has been implemented by a number of different companies in the past, including Yahoo! (for their modularized homepage - which Nicholas Zakas has [spoken](http://www.youtube.com/watch?v=vXjVFPosQHw) about) and AOL for some of our upcoming projects. 

The three design patterns that make this architecture possible are the:

* **Module pattern**: used for encapsulating unique blocks of code, where functions and variables can be kept either public or private. ('private' in the simulation of privacy sense, as of course don't have true privacy in JavaScript)
* **Mediator pattern**: used when the communication between modules may be complex, but is still well defined. If it appears a system may have too many relationships between modules in your code, it may be time to have a central point of control, which is where the pattern fits in.
* **Facade pattern**: used for providing a convenient higher-level interface to a larger body of code, hiding its true underlying complexity

Their specific roles in this architecture can be found below.

* **Modules**: There are almost two concepts of what defines a module. As AMD is being used as a module wrapper, technically each model, view and collection can be considered a module. We then have the concept of modules being distinct blocks of code outside of just MVC/MV*. For the latter, these types of 'modules' are primarily concerned with broadcasting and subscribing to events of interest rather than directly communicating with each other.They are made possible through the Mediator pattern.
* **Mediator**: The mediator has a varying role depending on just how you wish to implement it. In my article, I mention using it as a module manager with the ability to start and stop modules at will, however when it comes to Backbone, I feel that simplifying it down to the role of a central 'controller' that provides pub/sub capabilities should suffice. One can of course go all out in terms of building a module system that supports module starting, stopping, pausing etc, however the scope of this is outside of this chapter.
* **Facade**: This acts as a secure middle-layer that both abstracts an application core (Mediator) and relays messages from the modules back to the Mediator so they don't touch it directly. The Facade also performs the duty of application security guard; it checks event notifications from modules against a configuration (permissions.js, which we will look at later) to ensure requests from modules are only processed if they are permitted to execute the behaviour passed.

For ease of reference, I sometimes refer to these three patterns grouped together as Aura (a word that means subtle, luminous light).

### Practical

For the practical section of this chapter, we'll be extending the well-known Backbone Todo application using the three patterns mentioned above. The complete code for this section can be found here: https://github.com/addyosmani/backbone-aura and should ideally be run on at minimum, a local HTTP server.

The application is broken down into AMD modules that cover everything from Backbone models through to application-level modules. The views publish events of interest to the rest of the application and modules can then subscribe to these event notifications. 

All subscriptions from modules go through a facade (or sandbox). What this does is check against the subscriber name and the 'channel/notification' it's attempting to subscribe to. If a channel *doesn't* have permissions to be subscribed to (something established through permissions.js), the subscription isn't permitted. 


**Mediator**

Found in `aura/mediator.js`

Below is a very simple AMD-wrapped implementation of the mediator pattern, based on prior work by Ryan Florence. It accepts as it's input an object, to which it attaches `publish()` and `subscribe()` methods. In a larger application, the mediator can contain additional utilities, such as handlers for initialising, starting and stopping modules, but for demonstration purposes, these two methods should work fine for our needs.

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


**Facade**

Found in `aura/facade.js`

Next, we have an implementation of the facade pattern. Now the classical facade pattern applied to JavaScript would probably look a little like this: 

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


It's effectively a variation of the module pattern, where instead of simply returning an interface of supported methods, your API can completely hide the true implementation powering it, returning something simpler. This allows the logic being performed in the background to be as complex as necessary, whilst all the end-user experiences is a simplified API they pass options to (note how in our case, a single method abstraction is exposed). This is a beautiful way of providing APIs that can be easily consumed.

That said, to keep things simple, our implementation of an AMD-compatible facade will act a little more like a proxy. Modules will communicate directly through the facade to access the mediator's `publish()` and `subscribe()` methods, however, they won't as such touch the mediator directly.This enables the facade to provide application-level validation of any subscriptions and publications made.

It also allows us to implement a simple, but flexible, permissions checker (as seen below) which will validate subscriptions made against a permissions configuration to see whether it's permitted or not. 

  define([ "../aura/mediator" , "../aura/permissions" ], function (mediator, permissions) {

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


**Permissions**

Found in `aura/permissions.js`

In our simple permissions configuration, we support checking against subscription requests to establish whether they are allowed to clear. This enforces a flexible security layer for the application.

To visually see how this works, consider changing say, permissions -> renderDone -> todoCounter to be false. This will completely disable the application from from rendering or displaying the counts component for Todo items left (because they aren't allowed to subscribe to that event notification). The rest of the Todo app can still however be used without issue. 

It's a very dumbed down example of the potential for application security, but imagine how powerful this might be in a large app with a significant number of visual widgets.


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


**Subscribers**

Found in `subscribers.js`

Subscriber 'modules' communicate through the facade back to the mediator and perform actions when a notification event of a particular name is published. 

For example, when a user enters in a new piece of text for a Todo item and hits 'enter' the application publishes a notification saying two things: a) a new Todo item is available and b) the text content of the new item is X. It's then left up to the rest of the application to do with this information whatever it wishes. 

In order to update your Backbone application to primarily use pub/sub, a lot of the work you may end up doing will be moving logic coupled inside of specific views to modules outside of it which are reactionary. 

Take the `todoSaver` for example - it's responsibility is saving new Todo items to models once the a `notificationName` called 'newContentAvailable' has fired. If you take a look at the permissions structure in the last code sample, you'll notice that 'newContentAvailable' is present there. If I wanted to prevent subscribers from being able to subscribe to this notification, I simply set it to a boolean value of `false`.

Again, this is a massive oversimplification of how advanced your permissions structures could get, but it's certainly one way of controlling what parts of your application can or can't be accessed by specific modules at any time.


  define(["jquery", "underscore", "aura/facade"], 
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
              $(context.el).removeClass("editing");
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
          var tooltip = context.$(".ui-tooltip-top");
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
          $(context.el).addClass("editing");
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


That's it for this section. If you've been intrigued by some of the concepts covered, I encourage you to consider taking a look at my [slides](http://addyosmani.com/blog/large-scale-javascript-application-architecture/) on Large-scale JS from the jQuery Summit or my longer post on the topic [here](http://addyosmani.com/largescalejavascript) for more information. 