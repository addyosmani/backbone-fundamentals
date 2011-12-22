#Backbone.js Fundamentals: 
How to write modular JavaScript apps for desktop and mobile
<p>A work-in-progress by <a href="http://twitter.com/addyosmani">Addy Osmani</a> released for free under a <a href="http://creativecommons.org/licenses/by-nc-sa/3.0/">CC</a>-license. My extended thanks to <a href="https://github.com/addyosmani/backbone-fundamentals/contributors">these</a> members of the community for their tweaks and contributions.</p>

##Index

* ####[Introduction](#introduction)
* [What is Backbone?](#what-is-backbone)
* [Why should you consider using it?](#why-should-you-use-it)

* ####[The Basics](#the-basics)
* [Models](#models)
    * [Initialization](#models-initialization)
    * [Getters & Setters](#models-getters-setters)
    * [Model.set()](#models-model-set)
* [Views](#views)
    * [Creating new views](#views-creating)
    * [What is <code>el</code>?](#views-what-is-el)
* [Collections](#collections)
    * [Underscore utility functions](#collections-underscore)
* [Routers](#routers)
    * [Backbone.history](#routers-backbone-history)
* [Namespacing](#namespacing)
    * [What is namespacing](#namespacing-what-is-it)
* [Additional tips](#tips)
    * [Automated Backbone Scaffolding](#tips-automated-scaffolding)
    * [Clarifications on Backbone's MVC](#tips-backbone-mvc)
    * [Is there a limit to the number of routers](#tips-router-limot)
    * [Is Backbone too small for my application's needs](#tips-is-backbone-too-small)

* ####[Advanced](#advanced)
* [Modular JavaScript](#modular-javascript)
* [Organizing modules with RequireJS and AMD](#organizing-modules)
    * [Writing AMD Modules with RequireJS](#writing-amd-modules)
* [Keeping your templates external](#external-templates)
* [Optimizing Backbone apps for production with the RequireJS Optimizer](#optimizing-backbone-with-requirejs)
* [Backbone & jQuery Mobile](#backbone-and-jquery-mobile)
    * [Resolving routing conflicts](#bajm-resolving-routing-conflicts)

* ####[Practical](#practical)
* [A sample modular app](#sample-module-app)
* [A sample jQuery mobile app](#sample-mobile-app)
    * [Getting Started](#mobile-app-getting-started)
    * [jQuery Mobile: Going beyond mobile application development](#jquery-mobile)


##<a name="introduction">Introduction</a>

In this mini-book, I'll be covering a complete run-down of Backbone.js; including models, views, collections and routers. I'll also be taking you through advanced topics like modular development with Backbone.js and AMD (with RequireJS), how to solve the routing problems with Backbone and jQuery Mobile, tips about scaffolding tools that can save time setting up your initial application and more.

If this is your first time looking at Backbone.js and you're still unsure whether to give it a try or not, why not take a look at how a Todo application might be implemented in Backbone and most of the other alternatives before reading further? http://github.com/addyosmani/todomvc.

The goal of this mini-book is to create a free, centralized repository of information that can help those developing real-world apps with Backbone. If you come across a section or topic which you think could be improved or expanded on, please feel free to submit a pull-request. It won't take long and you'll be helping other developers avoid problems you've run into before.

###<a name="what-is-backbone">What is Backbone?</a>

Backbone.js is one of a number of JavaScript frameworks for creating MVC-like web applications. On the front-end, it's my architectural framework of choice as it's both mature, relatively lightweight and can be easily tested using third-party toolkits such as Jasmine or QUnit. Other MVC options you may be familiar with include Ember.js (SproutCore 2.0), Spine, YUILibrary and JavaScriptMVC.

Backbone is maintained by a number of contributors, most notably: Jeremy Ashkenas, creator of CoffeeScript, Docco and Underscore.js. As Jeremy is a believer in detailed documentation, there's a level of comfort in knowing you're unlikely to run into issues which are either not explained in the official docs or which can't be nailed down with some assistance from the #documentcloud IRC channel. I strongly recommend using the latter if you find yourself getting stuck.

###<a name="why-should-you-use-it">Why should you consider using it?</a>

Backbone's main benefits, regardless of your target platform or device, include helping:

* Organize the structure to your application
* Simplify server-side persistence
* Decouple the DOM from your page's data
* Model data, views and routers in a succinct manner
* Provide DOM, model and collection synchronization

In ways, the real question is why you should consider applying the MVC-pattern to your JavaScript projects and the one word answer is simply structure.

If opting to use jQuery, zepto or another qSA-based selection library to produce a non-trivial application it can become very easy to produce an unwieldy amount of code; that is - unless you have a plan for how you're going to structure and organize your application. Separating concerns into models, views and controllers (or routers) is one way of solving this.

Remember that if you have experience with structuring applications using the MVVM (Model-View ViewModel) pattern, modules or otherwise, these are also equally as valid but do require you to know what you're doing. For most single-page applications, I find that the MVC pattern works well so Backbone is a perfect fit for our needs.

##<a name="the-basics">The Basics<a>

In this section, you'll learn the essentials about Backbone's models, views, collections and routers. Whilst this isn't meant as a replacement for the official documentation, it will help you understand many of the core concepts behind Backbone before we build mobile applications with it. I will also be covering some tips on effective namespacing.

* Models
* Collections
* Routers
* Views
* Namespacing

###<a name="models">Models</a>

Backbone models contain interactive data for an application as well as the logic around this data. For example, we can use a model to represent the concept of a photo object including its attributes like tags, titles and a location.

Models are quite straight-forward to create and can be constructed by extending Backbone.Model as follows:

```javascript
Photo = Backbone.Model.extend({
    defaults: {
        src: 'placeholder.jpg',
        title: 'an image placeholder',
        coordinates: [0,0]
    },
    initialize: function(){
        this.bind("change:src", function(){
            var src = this.get("src"); 
            console.log('Image source updated to ' + src)
        });
    },
    changeSrc: function( source ){
        this.set({ src: source });
    }
});
 
var somePhoto = new Photo({ src: "test.jpg", title:"testing"});

```

####<a name="models-initialization">Initialization</a>

The initialize() method is called when creating a new instance of a model. It's considered optional, however we'll be reviewing some reasons it might come in useful very shortly.

```javascript
Photo = new Backbone.Model.extend({
    initialize: function(){
        console.log('this model has been initialized');
    }
});
 
/*We can then create our own instance of a photo as follows:*/
var myPhoto = new Photo;
```

####<a name="models-getters-setters">Getters & Setters</a>

<strong>Model.get()</strong>

Model.get() provides easy access to a model's attributes. Attributes below which are passed through to the model on instantiation are then instantly available for retrieval.

```javascript
var myPhoto = new Photo({ title: "My awesome photo", 
                          src:"boston.jpg", 
                          location: "Boston", 
                          tags:['the big game', 'vacation']}),
                          
    title   = myPhoto.get("title"), //my awesome photo
    location = myPhoto.get("location"), //Boston
    tags = myPhoto.get("tags"), // ['the big game','vacation']
    photoSrc = myPhoto.get("src"); //boston.jpg
```

Alternatively, if you wish to directly access all of the attributes in an model's instance directly, you can achieve this as follows:

```javascript
var myAttributes = myPhoto.attributes;
console.log(myAttributes);
```

Note: It is best practice to use Model.set() or direct instantiation to set the values of a model's attributes. Accessing Model.attributes directly is fine for reading or cloning data, but ideally shouldn't be used to for attribute manipulation.

Finally, if you would like to copy a model's attributes for JSON stringification (e.g. for serialization prior to being passed to a view), this can be achieved using Model.toJSON():

```javascript
var myAttributes = myPhoto.toJSON();
console.log(myAttributes);
/* this returns { title: "My awesome photo", 
             src:"boston.jpg", 
             location: "Boston", 
             tags:['the big game', 'vacation']}*/
```

####<a name="models-model-set">Model.set()</a>

Model.set() allows us to pass parameters into an instance of our model. Attributes can either be set during initialization or later on.

```javascript
Photo = new Backbone.Model.extend({
    initialize: function(){
        console.log('this model has been initialized');
    }
});
 
/*Setting the value of attributes via instantiation*/
var myPhoto = new Photo({ title: 'My awesome photo', location: 'Boston' });
 
var myPhoto2 = new Photo();

/*Setting the value of attributes through Model.set()*/
myPhoto2.set({ title:'Vacation in Florida', location: 'Florida' });
```

<strong>Default values</strong>

There are times when you want your model to have a set of default values (e.g. in scenario where a complete set of data isn't provided by the user). This can be set using a property called 'defaults' in your model.

```javascript
Photo = new Backbone.Model.extend({
    defaults:{
        title: 'Another photo!',
        tags:  ['untagged'],
        location: 'home',
        src: 'placeholder.jpg'
    },
    initialize: function(){
    }
});
 
var myPhoto = new Photo({ location: "Boston", 
                          tags:['the big game', 'vacation']}),
    title   = myPhoto.get("title"), //Another photo!
    location = myPhoto.get("location"), //Boston
    tags = myPhoto.get("tags"), // ['the big game','vacation']
    photoSrc = myPhoto.get("src"); //placeholder.jpg
```

<strong>Listening for changes to your model</strong>

Any and all of the attributes in a Backbone model can have listeners bound to them which detect when their values change. This can be easily added to the initialize() function as follows:

```javascript
this.bind('change', function(){
    console.log('values for this model have changed');
});
```

In the following example, we can also log a message whenever a specific attribute (the title of our Photo model) is altered.

```javascript
Photo = new Backbone.Model.extend({
    defaults:{
        title: 'Another photo!',
        tags:  ['untagged'],
        location: 'home',
        src: 'placeholder.jpg'
    },
    initialize: function(){
        console.log('this model has been initialized');
        this.bind("change:title", function(){
            var title = this.get("title");
            console.log("My title has been changed to.." + title);
        });
    },
    
    setTitle: function(newTitle){
        this.set({ title: newTitle });
    }
});
 
var myPhoto = new Photo({ title:"Fishing at the lake", src:"fishing.jpg")});
myPhoto.setTitle('Fishing at sea'); 
//logs my title has been changed to.. Fishing at sea
```

<strong>Validation</strong>

Backbone supports model validation through Model.validate(), which allows checking the attribute values for a model prior to them being set.

It supports including as complex or terse validation rules against attributes and is quite straight-forward to use. If the attributes provided are valid, nothing should be returned from .validate(), however if they are invalid a custom error can be returned instead.

A basic example for validation can be seen below:

```javascript
Photo = new Backbone.Model.extend({
    validate: function(attribs){
        if(attribs.src == "placeholder.jpg"){
            return "Remember to set a source for your image!";
        }
    },
    
    initialize: function(){
        console.log('this model has been initialized');
        this.bind("error", function(model, error){
            console.log(error);
        });
    }
});
 
var myPhoto = new Photo();
myPhoto.set({ title: "On the beach" });
```


###<a name="views">Views</a>

Views in Backbone don't contain the markup for your application, but rather they are there to support models by defining how they should be visually represented to the user. This is usually achieved using JavaScript templating (e.g. Mustache, jQuery tmpl etc). When a model updates, rather than the entire page needing to be refreshed, we can simply bind a view's render() function to a model's change() event, allowing the view to always be up to date.

####<a name="views-creating">Creating new views</a>

Similar to the previous sections, creating a new view is relatively straight-forward. We simply extend Backbone.View. Here's an example of a one possible implementation of this, which I'll explain shortly:

```javascript
var PhotoSearch = Backbone.View.extend({
    el: $('#results'),
    render: function( event ){
        var compiled_template = _.template( $("#results-template").html());
        this.el.html( compiled_template(this.model.toJSON()) );
        return this; //recommended as this enables calls to be chained.
    },
    events: {
        "submit #searchForm":  "search",
        "click .reset": "reset",
        "click .advanced": "switchContext"
    },
    search: function( event ){
        //executed when a form '#searchForm' has been submitted
    },
    reset: function( event ){
        //executed when an element with class "reset" has been clicked.
    },
    //etc
});
```

####<a name="views-what-is-el">What is '<code>el</code>'?</a>

<code>el</code> is basically a reference to a DOM element and all views must have one, however Backbone allows you to specify this in four different ways. You can either directly use an id, a tagName, className or if you don't state anything <code>el</code> will simply default to a plain div element without any id or class. Here are some quick examples of how these may be used:

```javascript
el: $('#results')  //select based on ID or jQuery selector.
tagName: 'li' //select based on a specific tag. Here el itself will be derived from the tagName
className: 'items' //similar to the above, this will also result in el being derived from it
el: '' //defaults to a div without an id, name or class.
```

<strong>Understanding render()</strong>

render() is a function that should always be overridden to define how you would like a template to be rendered. Backbone allows you to use any JavaScript templating solution of your choice for this but for the purposes of this example, we'll opt for underscore's micro-templating.

The _.template method in underscore compiles JavaScript templates into functions which can be evaluated for rendering. Here, I'm passing the markup from a template with id 'results-template' to be compiled. Next, I set the html for <code>el</code> (our DOM element for this view) the output of processing a JSON version of the model associated with the view through the compiled template.

Presto! This populates the template, giving you a data-complete set of markup in just a few short lines of code.

<strong>The 'events' attribute</strong>

The Backbone events attribute allows us to attach event listeners to either custom selectors or <code>el</code> if no selector is provided. An event takes the form {"eventName selector": "callbackFunction"} and a number of event-types are supported, including 'click', 'submit', 'mouseover', 'dblclick' and more.

What isn't instantly obvious is that under the bonnet, Backbone uses jQuery's .delegate() to provide instant support for event delegation but goes a little further, extending it so that 'this' always refers to the current view object. The only thing to really keep in mind is that any string callback supplied to the events attribute must have a corresponding function with the same name within the scope of your view otherwise you may incur exceptions.

###[Collections](#collections)

Collections are basically sets of models and can be easily created by extending Backbone.Collection.

Normally, when creating a collection you'll also want to pass through a property specifying the model that your collection will contain as well as any instance properties required.

In the following example, we're creating a PhotoCollection containing the Photo models we previously defined.

```javascript
PhotoCollection = Backbone.Collection.extend({
    model: Photo
});
```

<strong>Getters and Setters</strong>

There are a few different options for retrieving a model from a collection. The most straight-forward is using Collection.get() which accepts a single id as follows:

```javascript
var skiingEpicness = PhotoCollection.get(2);
```

Sometimes you may also want get a model based on something called the client id. This is an id that is internally assigned automatically when creating models that have not yet been saved, should you need to reference them. You can find out what a model's client id is by accessing its .cid property.

```javascript
var mySkiingCrash = PhotoCollection.getByCid(456);
```

Backbone Collections don't have setters as such, but do support adding new models via .add() and removing models via .remove().

```javascript
var a = new Backbone.Model({ title: 'my vacation'}),
    b = new Backbone.Model({ title: 'my holiday'});

var photoCollection = new Backbone.Collection([a,b]);
photoCollection.remove([a,b]);
```

<strong>Listening for events</strong>

As collections represent a group of items, we're also able to listen for add and remove events for when new models are added or removed from the collection. Here's an example:

```javascript
PhotoCollection = new Backbone.Collection;
PhotoCollection.bind("add", function(photo) {
  console.log("I liked " + photo.get("title") + ' its this one, right? '  + photo.src);
});
 
PhotoCollection.add([
  {title: "My trip to Bali", src: "bali-trip.jpg"},
  {title: "The flight home", src: "long-flight-oofta.jpg"},
  {title: "Uploading pix", src: "too-many-pics.jpg"}
]);
```

In addition, we're able to bind a 'change' event to listen for changes to models in the collection.

```javascript
PhotoCollection.bind("change:title", function(){
    console.log('there have been updates made to this collections titles');    
});
```

<strong>Fetching models from the server</strong>

Collections.fetch() provides you with a simple way to fetch a default set of models from the server in the form of a JSON array. When this data returns, the current collection will refresh.

```javascript
var PhotoCollection = new Backbone.Collection;
PhotoCollection.url = '/photos';
PhotoCollection.fetch();
```

Under the covers, Backbone.sync is the function called every time Backbone tries to read (or save) models to the server. It uses jQuery or Zepto's ajax implementations to make these RESTful requests, however this can be overridden as per your needs.

In the above fetch example if we wish to log an event when .sync() gets called, we can simply achieve this as follows:

```javascript
Backbone.sync = function(method, model) {
  console.log("I've been passed " + method + " with " + JSON.stringify(model));
};
```

<strong>Resetting/Refreshing Collections</strong>

Rather than adding or removing models individually, you occasionally wish to update an entire collection at once. Collection.reset() allows us to replace an entire collection with new models as follows:

```javascript
PhotoCollection.reset([
  {title: "My trip to Scotland", src: "scotland-trip.jpg"},
  {title: "The flight from Scotland", src: "long-flight.jpg"},
  {title: "Latest snap of lock-ness", src: "lockness.jpg"}]);
```

####<a name="collections-underscore">Underscore utility functions</a>

As Backbone requires Underscore as a hard dependency, we're able to use many of the utilities it has to offer to aid with our application development. Here's an example of how Underscore's sortBy() method can be used to sort a collection of photos based on a particular attribute.

```javascript
var sortedByAlphabet = PhotoCollection.sortBy(function(photo)){
    return photo.get("title").toLowerCase();
});
```
The complete list of what it can do is beyond the scope of this guide, but can be found in the official docs.

###<a name="routers">Routers</a>

In Backbone, routers are used to handle routing for your application. This is achieved using hash-tags with URL fragments which you can read more about if you wish. Some examples of valid routes may be seen below:

```javascript
http://unicorns.com/#/whatsup
http://unicorns.com/#/search/seasonal-horns/page2
```

Note: A router will usually have at least one URL route defined as well as a function that maps what happens when you reach that particular route. This type of key/value pair may resemble:

```javascript
"/route" : "mappedFunction"
```

Let us now define our first controller by extending Backbone.Router. For the purposes of this guide, we're going to continue pretending we're creating a photo gallery application that requires a GalleryController.

Note the inline comments in the code example below as they continue the rest of the lesson on routers.

```javascript
GalleryController = Backbone.Router.extend({
    /* define the route and function maps for this controller */
    routes:{
        "/about" : "showAbout",
        /*Sample usage: http://unicorns.com/#/about"*/
        
        "/photos/:id" : "getPhoto",
        /*This is an example of using a ":param" variable which allows us to match 
        any of the components between two URL slashes*/
        /*Sample usage: http://unicorns.com/#/photos/5*/
        
        "/search/:query" : "searchPhotos"
        /*We can also define multiple routes that are bound to the same map function,
        in this case searchPhotos(). Note below how we're optionally passing in a 
        reference to a page number if one is supplied*/
        /*Sample usage: http://unicorns.com/#/search/lolcats*/
         
        "/search/:query/p:page" : "searchPhotos",
        /*As we can see, URLs may contain as many ":param"s as we wish*/
        /*Sample usage: http://unicorns.com/#/search/lolcats/p1*/
        
        "/photos/:id/download/*imagePath" : "downloadPhoto",
        /*This is an example of using a *splat. splats are able to match any number of 
        URL components and can be combined with ":param"s*/
        /*Sample usage: http://unicorns.com/#/photos/5/download/files/lolcat-car.jpg*/
        
        /*If you wish to use splats for anything beyond default routing, it's probably a good 
        idea to leave them at the end of a URL otherwise you may need to apply regular
        expression parsing on your fragment*/
        
        
        "*other"    : "defaultRoute"
        //This is a default route with that also uses a *splat. Consider the
        //default route a wildcard for URLs that are either not matched or where
        //the user has incorrectly typed in a route path manually
        /*Sample usage: http://unicorns.com/#/anything*/
 
    },
    
    showAbout: function(){
    },
    
    getPhoto: function(id){
        /* 
        in this case, the id matched in the above route will be passed through
        to our function getPhoto and we can then use this as we please.
        */
        console.log("You are trying to reach photo " + id);
    },
    
    searchPhotos: function(query, page){
        console.log("Page number: " + page + " of the results for " + query);
    },
    
    downloadPhoto: function(id, path){
    },
    
    defaultRoute(other){
        console.log("Invalid. You attempted to reach:" + other);
    }
});
 
/* Now that we have a controller setup, remember to instantiate it*/
 
var myGalleryController = new GalleryController;
```

Note: In Backbone 0.5+, it's possible to opt-in for HTML5 pushState support via window.history.pushState. This effectively permits non-hashtag routes such as http://www.scriptjunkie.com/just/an/example to be supported with automatic degradation should your browser not support it. For the purposes of this tutorial, we won't be relying on this newer functionality as there have been reports about issues with it under iOS/Mobile Safari. Backbone's hash-based routes should however suffice for our needs.

####<a name="routers-backbone-history">Backbone.history</a>

Next, we need to initialize Backbone.history as it handles <code>hashchange</code> events in our application. This will automatically handle routes that have been defined and trigger callbacks when they've been accessed.

The Backbone.history.start() method will simply tell Backbone that it's OK to begin monitoring all <code>hashchange</code> events as follows:

```javascript
Backbone.history.start();
Controller.saveLocation();
```

As an aside, if you would like to save application state to the URL at a particular point you can use the .saveLocation() method to achieve this. It simply updates your URL fragment without the need to trigger the <code>hashchange</code> event.

```javascript
/*Lets imagine we would like a specific fragment for when a user zooms into a photo*/
zoomPhoto: function(factor){
    this.zoom(factor); //imagine this zooms into the image
    this.saveLocation("zoom/" + factor); //updates the fragment for us
}
```


###<a name="namespacing">Namespacing</a>

When learning how to use Backbone, one important area that that is very commonly overlooked in tutorials is namespacing. If you already have experience with how to namespace in JavaScript, the following section will provide some advice on how to specifically apply concepts you know to Backbone, however I will also be covering explanations for beginners to ensure everyone is on the same page.

####<a name="namespacing-what-is-it">What is namespacing?</a>

The basic idea around namespacing is to avoid collisions with other objects or variables in the global namespace. They're important as it's best to safeguard your code from breaking in the event of another script on the page using the same variable names as you are. As a good 'citizen' of the global namespace, it's also imperative that you do your best to similarly not prevent other developer's scripts executing due to the same issues.

JavaScript doesn't really have built-in support for namespaces like other languages, however it does have closures which can be used to achieve a similar effect.

In this section we'll be taking a look shortly at some examples of how you can namespace your models, views, routers and other components specifically. The patterns we'll be examining are:

* Single global variables
* Object Literals
* Nested namespacing
* Single global variables

<strong>Single global variables</strong>

One popular pattern for namespacing in JavaScript is opting for a single global variable as your primary object of reference. A skeleton implementation of this where we return an object with functions and properties can be found below:

```javascript
var myApplication =  (function(){ 
        function(){
            ...
        },
        return{
            ...
        }
})();
```

which you're likely to have seen before. A Backbone-specific example which may be more useful is:

```javascript
var myViews = (function(){
    return {
        PhotoView: Backbone.View.extend({ .. }),
        GalleryView: Backbone.View.extend({ .. }),
        AboutView: Backbone.View.extend({ .. });
        //etc.
    };
})();
```

Here we can return a set of views or even an entire collection of models, views and routers depending on how you decide to structure your application. Although this works for certain situations, the biggest challenge with the single global variable pattern is ensuring that no one else has used the same global variable name as you have in the page.

One solution to this problem, as mentioned by Peter Michaux, is to use prefix namespacing. It's a simple concept at heart, but the idea is you select a basic prefix namespace you wish to use (in this example, myApplication_) and then define any methods, variables or other objects after the prefix.

```javascript
var myApplication_photoView = Backbone.View.extend({}),
myApplication_galleryView = Backbone.View.extend({});
```

This is effective from the perspective of trying to lower the chances of a particular variable existing in the global scope, but remember that a uniquely named object can have the same effect. This aside, the biggest issue with the pattern is that it can result in a large number of global objects once your application starts to grow.

For more on Peter's views about the single global variable pattern, read his excellent post on them here: http://michaux.ca/articles/javascript-namespacing

Note: There are several other variations on the single global variable pattern out in the wild, however having reviewed quite a few, I felt these applied best to Backbone.

<strong>Object Literals</strong>

Object Literals have the advantage of not polluting the global namespace but assist in organizing code and parameters logically. They're beneficial if you wish to create easily-readable structures that can be expanded to support deep nesting. Unlike simple global variables, Object Literals often also take into account tests for the existence of a variable by the same name so the chances of collision occurring are significantly reduced.

The code at the very top of the next sample demonstrates the different ways in which you can check to see if a namespace already exists before defining it. I commonly use Option 3.

```javascript
/*Doesn't check for existence of myApplication*/
var myApplication = {};
 
/*
Does check for existence. If already defined, we use that instance.
Option 1:   if(!MyApplication) MyApplication = {};
Option 2:   var myApplication = myApplication = myApplication || {}
Option 3:   var myApplication = myApplication || {};
We can then populate our object literal to support models, views and collections (or any data, really):
*/
 
var myApplication = {
    models : {},
    views : {
        pages : {}
    },
    collections : {}
};
```

One can also opt for adding properties directly to the namespace (such as your views, in the following example):

```javascript
var myGalleryViews = myGalleryViews || {};
myGalleryViews.photoView = Backbone.View.extend({});
myGalleryViews.galleryView = Backbone.View.extend({});
```

The benefit of this pattern is that you're able to easily encapsulate all of your models, views, routers etc. in a way that clearly separates them and provides a solid foundation for extending your code.

This pattern has a number of useful applications. It's often of benefit to decouple the default configuration for your application into a single area that can be easily modified without the need to search through your entire codebase just to alter them - Object Literals work great for this purpose. Here's an example of a hypothetical Object Literal for configuration:

```javascript
var myConfig = {
    language: 'english',
    defaults: {
        enableGeolocation: true,
        enableSharing: false,
        maxPhotos: 20
    },
    theme: {
        skin: 'a',
        toolbars: {
            index: 'ui-navigation-toolbar',
            pages: 'ui-custom-toolbar'    
        }
    }
}
```

Note that there are really only minor syntactical differences between the Object Literal pattern and a standard JSON data set. If for any reason you wish to use JSON for storing your configurations instead (e.g. for simpler storage when sending to the back-end), feel free to.

For more on the Object Literal pattern, I recommend reading Rebecca Murphey's excellent article on the topic: (http://blog.rebeccamurphey.com/2009/10/15/using-objects-to-organize-your-code).

<strong>Nested namespacing</strong>

An extension of the Object Literal pattern is nested namespacing. It's another common pattern used that offers a lower risk of collision due to the fact that even if a namespace already exists, it's unlikely the same nested children do.

Does this look familiar?

```javascript
YAHOO.util.Dom.getElementsByClassName('test');
```

Yahoo's YUI uses the nested object namespacing pattern regularly and even DocumentCloud (the creators of Backbone) use the nested namespacing pattern in their main applications. A sample implementation of nested namespacing with Backbone may look like this:

```javascript
var galleryApp =  galleryApp || {};
 
/*perform similar check for nested children*/
galleryApp.routers = galleryApp.routers || {};
galleryApp.model = galleryApp.model || {};
galleryApp.model.special = galleryApp.model.special || {};
 
/*routers*/
galleryApp.routers.Workspace   = Backbone.Router.extend({}); 
galleryApp.routers.PhotoSearch = Backbone.Router.extend({}); 
 
/*models*/
galleryApp.model.Photo      = Backbone.Model.extend({}); 
galleryApp.model.Comment = Backbone.Model.extend({}); 
 
/*special models*/
galleryApp.model.special.Admin = Backbone.Model.extend({});
```

This is both readable, organized and is a relatively safe way of namespacing your Backbone application in a similar fashion to what you may be used to in other languages.

The only real caveat however is that it requires your browser's JavaScript engine first locating the galleryApp object and then digging down until it gets to the function you actually wish to use.

This can mean an increased amount of work to perform lookups, however developers such as Juriy Zaytsev (kangax) have previously tested and found the performance differences between single object namespacing vs the 'nested' approach to be quite negligible.

<strong>Recommendation</strong>

Reviewing the namespace patterns above, the option that I would personally use with Backbone is nested object namespacing with the object literal pattern.

Single global variables may work fine for applications that are relatively trivial, however, larger codebases requiring both namespaces and deep sub-namespaces require a succinct solution that promotes readability and scales. I feel this pattern achieves all of these objectives well and is a perfect companion for Backbone development.

###<a name="tips">Additional Tips</a>

####<a name="tips-automated-scaffolding">Automated Backbone Scaffolding</a>

Scaffolding can assist in expediting how quickly you can begin a new application by creating the basic files required for a project automatically. If you enjoy the idea of automated MVC scaffolding using Backbone, I'm happy to recommend checking out a tool called Brunch.

It works very well with Backbone, Underscore, jQuery and CoffeeScript and is even used by companies such as Red Bull and Jim Bean. You may have to update any third party dependencies (e.g. latest jQuery or Zepto) when using it, but other than that it should be fairly stable to use right out of the box.

Brunch can easily be installed via the nodejs package manager and takes just little to no time to get started with. If you happen to use Vim or Textmate as your editor of choice, you may be happy to know that there are also Brunch bundles available for both.

####<a name="tips-backbone-mvc">Clarifications on Backbone's MVC</a>

As Thomas Davis has previously noted, Backbone.js's MVC is a loose interpretation of traditional MVC, something common to many client-side MVC solutions. Backbone's views are what could be considered a wrapper for templating solutions such as the Mustache.js and Backbone.View is the equivalent of a controller in traditional MVC. Backbone.Model is however the same as a classical 'model'.

Whilst Backbone is not the only client-side MVC solution that could use some improvements in it's naming conventions, Backbone.Controller was probably the most central source of some confusion but has been renamed a router in more recent versions. This won't prevent you from using Backbone effectively, however this is being pointed out just to help avoid any confusion if for any reason you opt to use an older version of the framework.

The official Backbone docs do attempt to clarify that their routers aren't really the C in MVC, but it's important to understand where these fit rather than considering client-side MVC a 1:1 equivalent to the pattern you've probably seen in server-side development.

####<a name="tips-router-limit">Is there a limit to the number of routers I should be using?</a>

Andrew de Andrade has pointed out that DocumentCloud themselves usually only use a single controller in most of their applications. You're very likely to not require more than one or two routers in your own projects as the majority of your application routing can be kept organized in a single controller without it getting unwieldy.

####<a name="tips-is-backbone-too-small">Is Backbone too small for my application's needs?</a>

If you find yourself unsure of whether or not your application is too large to use Backbone, I recommend reading my post on building large-scale jQuery & JavaScript applications or reviewing my slides on client-side MVC architecture options. In both, I cover alternative solutions and my thoughts on the suitability of current MVC solutions for scaled application development.

Backbone can be used for building both trivial and complex applications as demonstrated by the many examples Ashkenas has been referencing in the Backbone documentation. As with any MVC framework however, it's important to dedicate time towards planning out what models and views your application really needs. Diving straight into development without doing this can result in either spaghetti code or a large refactor later on and it's best to avoid this where possible.

At the end of the day, the key to building large applications is not to build large applications in the first place. If you however find Backbone doesn't cut it for your requirements I strongly recommend checking out JavaScriptMVC or SproutCore as these both offer a little more than Backbone out of the box. Dojo and Dojo Mobile may also be of interest as these have also been used to build significantly complex apps by other developers.

##<a name="advanced">Advanced</a>
###<a name="modular-javascript">Modular JavaScript</a>

When we say an application is modular, we generally mean it's composed of a set of highly decoupled, distinct pieces of functionality stored in modules. As you probably know, loose coupling facilitates easier maintainability of apps by removing dependencies where possible. When this is implemented efficiently, its quite easy to see how changes to one part of a system may affect another.

Unlike some more traditional programming languages however, the current iteration of JavaScript (ECMA-262) doesn't provide developers with the means to import such modules of code in a clean, organized manner. It's one of the concerns with specifications that haven't required great thought until more recent years where the need for more organized JavaScript applications became apparent.

Instead, developers at present are left to fall back on variations of the module or object literal patterns. With many of these, module scripts are strung together in the DOM with namespaces being described by a single global object where it's still possible to incur naming collisions in your architecture. There's also no clean way to handle dependency management without some manual effort or third party tools.

Whilst native solutions to these problems will be arriving in ES Harmony, the good news is that writing modular JavaScript has never been easier and you can start doing it today.

In this next part of the book, we're going to look at how to use the AMD module format and RequireJS for cleanly wrapping units of code in your application into managable modules.


###<a name="organizing-modules">Organizing modules with RequireJS and AMD</a>

In case you haven't used it before, RequireJS is a popular script loader written by James Burke - a developer who has been quite instrumental in helping shape the AMD module format, which we'll discuss more shortly. Some of RequireJS's capabilities include helping to load multiple script files, helping define modules with or without dependencies and loading in non-script dependencies such as text files.

So, why use RequireJS with Backbone?. Although Backbone is excellent when it comes to providing a sanitary structure to your applications, there are a few key areas where some additional help could be used:

1) Backbone doesn't endorse a particular approach to modular-development. Although this means it's quite open-ended for developers to opt for classical patterns like the module-pattern or Object Literals for structuring their apps (which both work fine), it also means developers aren't sure of what works best when other concerns come into play, such as dependency management.

RequireJS is compatible with the AMD (Asynchronous Module Definition) format, a format which was born from a desire to write something better than the 'write lots of script tags with implicit dependencies and manage them manually' approach to development. In addition to allowing you to clearly declare dependencies, AMD works well in the browser, supports string IDs for dependencies, declaring multiple modules in the same file and gives you easy-to-use tools to avoid polluting the global namespace.

2) Let's discuss dependency management a little more as it can actually be quite challenging to get right if you're doing it by hand. When we write modules in JavaScript, we ideally want to be able to handle the reuse of code units intelligently and sometimes this will mean pulling in other modules at run-time whilst at other times you may want to do this dynamically to avoid a large pay-load when the user first hits your application.

Think about the GMail web-client for a moment. When users initially load up the page on their first visit, Google can simply hide widgets such as the chat module until a user has indicated (by clicking 'expand') that they wish to use it. Through dynamic dependency loading, Google could load up the chat module only then, rather than forcing all users to load it when the page first initializes. This can improve performance and load times and can definitely prove useful when building larger applications.

I've previously written a detailed article covering both AMD and other module formats and script loaders here (http://addyosmani.com/writing-modular-js) in case you'd like to explore this topic further. The takeaway is that although it's perfectly fine to develop applications without a script loader or clean module format in place, it can be of significant benefit to consider using these tools in your application development.

###<a name="writing-amd-modules">Writing AMD modules with RequireJS</a>

As discussed above, the overall goal for the AMD format is to provide a solution for modular JavaScript that developers can use today. The two key concepts you need to be aware of when using it with a script-loader are a <code>define()</code> method for facilitating module definition and a <code>require()</code> method for handling dependency loading. <em>define()</em> is used to define named or unnamed modules based on the proposal using the following signature:</p>

```javascript
define(
    module_id /*optional*/, 
    [dependencies] /*optional*/, 
    definition function /*function for instantiating the module or object*/
);
```

As you can tell by the inline comments, the <code>module_id</code> is an optional argument which is typically only required when non-AMD concatenation tools are being used (there may be some other edge cases where it's useful too). When this argument is left out, we call the module 'anonymous'. When working with anonymous modules, the idea of a module's identity is DRY, making it trivial to avoid duplication of filenames and code. 

Back to the define signature, the dependencies argument represents an array of dependencies which are required by the module you are defining and the third argument ('definition function') is a function that's executed to instantiate your module. A barebone module (compatible with RequireJS) could be defined using <code>define()</code> as follows: </p>

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


<code>require()</code> on the other hand is typically used to load code in a top-level JavaScript file or within a module should you wish to dynamically fetch dependencies. An example of its usage is:</p>

```javascript
// Consider 'foo' and 'bar' are two external modules
// In this example, the 'exports' from the two modules loaded are passed as
// function arguments to the callback (foo and bar)
// so that they can similarly be accessed

require(['foo', 'bar'], function ( foo, bar ) {
        // rest of your code here
        foo.doSomething();
});
```


<strong>Wrapping modules, views and other components with AMD</strong>

Now that we've taken a look at how to define AMD modules, let's review how to go about wrapping components like views and collections so that they can also be easily loaded as dependencies for any parts of your application that require them. At it's simplest, a Backbone model may just require Backbone and Underscore.js. These are considered it's dependencies and so, to write an AMD model module, we would simply do this:

```javascript
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
```

Note how we alias Underscore.js's instance to "_" and Backbone to just 'Backbone', making it very trivial to convert non-AMD code over to using this module format. For a view which might require other dependencies such as jQuery, this can similarly be done as follows:

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

Aliasing to the dollar-sign ($), once again makes it very easy to encapsulate any part of an application you wish using AMD.


###<a name="external-templates">External [Underscore/Handlebars/Mustache] templates using RequireJS</a>

Moving your [Underscore/Mustache/Handlebars] templates to external files is actually quite straight-forward. As this application makes use of RequireJS, I'll discuss how to implement external templates using this specific script loader.

RequireJS has a special plugin called text.js which is used to load in text file dependencies. To use the text plugin, simply follow these simple steps:

1. Download the plugin from http://requirejs.org/docs/download.html#text and place it in either the same directory as your application's main JS file or a suitable sub-directory.

2. Next, include the text.js plugin in your initial RequireJS configuration options. In the code snippet below, we assume that RequireJS is being included in our page prior to this code snippet being executed. Any of the other scripts being loaded are just there for the sake of example.
 
```javascript
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
} );
```

3. When the <code>text!</code> prefix is used for a dependency, RequireJS will automatically load the text plugin and treat the dependency as a text resource. A typical example of this in action may look like..

```javascript
require(['js/app', 'text!templates/mainView.html'],
    function(app, mainView){
        // the contents of the mainView file will be
        // loaded into mainView for usage.
    }
);
```

4. Finally we can use the text resource that's been loaded for templating purposes. You're probably used to storing your HTML templates inline using a script with a specific identifier. 

With Underscore.js's micro-templating (and jQuery) this would typically be:

HTML:
```javascript
&lt;script type=&quot;text/template&quot; id=&quot;mainViewTemplate&quot;&gt;
    &lt;% _.each( person, function( person_item ){ %&gt;
                &lt;li&gt;&lt;%= person_item.get(&quot;name&quot;) %&gt;&lt;/li&gt;  
     &lt;% }); %&gt;
&lt;/script&gt;
```

JS:
```javascript
var compiled_template = _.template( $('#mainViewTemplate').html() );
```

With RequireJS and the text plugin however, it's as simple as saving your template into an external text file (say, mainView.html) and doing the following:

```javascript
require(['js/app', 'text!templates/mainView.html'],
    function(app, mainView){
        
        var compiled_template = _.template( mainView );
    }
);
```

That's it!. You can then go applying your template to a view in Backbone doing something like:

```javascript
collection.someview.el.html( compiled_template( { results: collection.models } ) );
```


All templating solutions will have their own custom methods for handling template compilation, but if you understand the above, substituting Underscore's micro-templating for any other solution should be fairly trivial.

<strong>Note:</strong> You may also be interested in looking at https://github.com/ZeeAgency/requirejs-tpl. It's an AMD-compatible version of the Underscore templating system that also includes support for optimization (pre-compiled templates) which can lead to better performance and no evals. I have yet to use it myself, but it comes as a recommended resource.




###<a name="optimizing-backbone-with-requirejs">Optimizing Backbone apps for production with the RequireJS Optimizer</a>


As experienced developers may know, an essential final step when writing both small and large JavaScript web applications is the build process.  The majority of non-trivial apps are likely to consist of more than one or two scripts and so optimizing, minimizing and concatenating your scripts prior to pushing them to production will require your users to download a reduced number (if not just one) script file. 

Note: If you haven't looked at build processes before and this is your first time hearing about them, you might find my post and screencast on this topic useful http://addyosmani.com/blog/client-side-build-process/.

With some other structural JavaScript frameworks, my recommendation would normally be to implicitly use YUI Compressor or Google's closure compiler tools, but we have a slightly more elegant when it comes to Backbone if you're using RequireJS. RequireJS has a command line optimization tool called r.js which has a number of capabilities, including:

* Concatenating specific scripts and minifiying them using external tools such as UglifyJS (which is used by default) or Google's Closure Compiler for optimal browser delivery, whilst preserving the ability to dynamically load modules
* Optimizing CSS and stylesheets by inlining CSS files imported using @import, stripping out comments etc.
* The ability to run AMD projects in both Node and Rhino (more on this later)

You'll notice that I mentioned the word 'specific' in the first bullet point. The RequireJS optimizer only concatenates module scripts that have been specified in arrays of string literals passed to top-level (i.e non-local) require and define calls. As clarified by the optimizer docs here (http://requirejs.org/docs/optimization.html) this means that Backbone modules defined like this..

```javascript
define(['jquery','backbone','underscore', collections/sample','views/test'], 
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

To get started with r.js, grab it from the RequireJS download page (http://requirejs.org/docs/download.html#rjs) or through NPM as specified here (http://requirejs.org/docs/optimization.html#download). Now, the RequireJS optimizer works absolutely fine for single script and CSS files, but for most cases you'll want to actually optimize an entire Backbone project. You *could* do this completely from the command-line, but a cleaner option is using build profiles.

Below is an example of a build file taken from the modular jQuery Mobile app referenced later in this book. A <strong>build profile</strong> (commonly named app.build.js) informs RequireJS to copy all of the content of <code>appDir</code> to a directory defined by <code>dir</code> (in this case '../release'). This will apply all of the necessary optimizations inside the release folder. The <code>baseUrl</code> is used to resolve the paths for your modules. It should ideally be relative to <code>appDir</code>.

Near the bottom of this sample file, you'll see an array called <code>modules</code>. This is where you specify the module names you wish to have optimized. In this case we're optimizing the main application called 'app', which maps to <code>appDir/app.js</code>. If we had set the <code>baseUrl</code> to 'scripts', it would be mapped to <code>appDir/scripts/app.js</code>.

```javascript
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
```

The way the build system in r.js works is that it traverses app.js (whatever modules you've passed) and resolved dependencies, concatenating them into the final <code>release</code>(dir) folder. CSS is treated the same way. 

The build profile is usually placed inside the 'scripts' or 'js' directory of your project. As per the docs, this file can however exist anywhere you wish, but you'll need to edit the contents of your build profile accordingly. 

Finally, to run the build, execute the following command once insice your <code>appDir</code> or <code>appDir/scripts</code> directory:

```javascript
node ../../r.js -o app.build.js
```


That's it. As long as you have UglifyJS/Closure tools setup correctly, r.js should be able to easily optimize your entire Backbone project in just a few key-strokes. If you would like to learn more about build profiles, James Burke has a heavily commented sample file with all the possible options available here: https://github.com/jrburke/r.js/blob/master/build/example.build.js


###<a name="backbone-and-jquery-mobile">Backbone and jQuery Mobile<a>

####<a name="bajm-resolving-routing-conflicts">Resolving the routing conflicts</a>

The first major hurdle developers typically run into when building Backbone applications with jQuery Mobile is that both frameworks have their own opinions about how to handle application navigation. 

Backbone's routers offer an explicit way to define custom navigation routes through Backbone.Router, whilst jQuery Mobile encourages the use of URL hash fragments to reference separate 'pages' or views in the same document. jQuery Mobile also supports automatically pulling in external content for links through XHR calls meaning that there can be quite a lot of inter-framework confusion about what a link pointing at '#/photo/id' should actually be doing. 

Some of the solutions that have been previously proposed to work-around this problem included manually patching Backbone or jQuery Mobile. I discourage opting for these techniques as it becomes necessary to manually patch your framework builds when new releases get made upstream. 

There's also https://github.com/azicchetti/jquerymobile-router, which tries to solve this problem differently, however I think my proposed solution is both simpler and allows both frameworks to cohabit quite peacefully without the need to extend either. What we're after is a way to prevent one framework from listening to hash changes so that we can fully rely on the other (e.g. Backbone.Router) to handle this for us exclusively. 

Using jQuery Mobile this can be done by setting: 

```javascript
$.mobile.hashListeningEnabled = false;
```

prior to initializing any of your other code. 

I discovered this method looking through some jQuery Mobile commits that didn't make their way into the official docs, but am happy to see that they are now covered here http://jquerymobile.com/test/docs/api/globalconfig.html in more detail.

The next question that arises is, if we're preventing jQuery Mobile from listening to URL hash changes, how can we still get the benefit of being able to navigate to other sections in a document using the built-in transitions and effects supported?. Good question. This can now be solve by simply calling <code>$.mobile.changePage()</code> as follows:

```javascript
var url = '#about',
    effect = 'slideup',
    reverse = false,
    changeHash = false;

$.mobile.changePage( url , { transition: effect}, reverse, changeHash );
```

In the above sample, <code>url</code> can refer to a URL or a hash identifier to navigate to, <code>effect</code> is simply the transition effect to animate the page in with and the final two parameters decide the direction for the transition (<code>reverse</code>) and whether or not the hash in the address bar should be updated (<code>changeHash</code>). With respect to the latter, I typically set this to false to avoid managing two sources for hash updates, but feel free to set this to true if you're comfortable doing so. 

<strong>Note:</strong> For some parallel work being done to explore how well the jQuery Mobile Router plugin works with Backbone, you may be interested in checking out https://github.com/Filirom1/jquery-mobile-backbone-requirejs.

##<a name="practical">Practical</a>
###<a name="sample-module-app">Practical: A Backbone + RequireJS/AMD app</a>

//This section needs to be heavily expanded.

My side-project TodoMVC now includes a modular Backbone example using AMD and RequireJS (thanks to Thomas Davis) for anyone wishing to look at a sample without any of the jQuery Mobile code included:

https://github.com/addyosmani/todomvc/tree/master/todo-example/backbone+require

This covers how to wrap your views, models, modules etc. using AMD and also cleanly demonstrates handling dependency management as well as Flickly does.

###<a name="sample-mobile-app">Practical: A Backbone, RequireJS/AMD app with jQuery Mobile</a>

####<a name="mobile-app-getting-started">Getting started</a>

Once you feel comfortable with the Backbone fundamentals (http://msdn.microsoft.com/en-us/scriptjunkie/hh377172.aspx) and you've put together a rough wireframe of the app you may wish to build, start to think about your application architecture. Ideally, you'll want to logically separate concerns so that it's as easy as possible to maintain the app in the future.

<strong>Namespacing</strong>

For this application, I opted for the nested namespacing pattern. Implemented correctly, this enables you to clearly identify if items being referenced in your app are views, other modules and so on. This initial structure is a sane place to also include application defaults (unless you prefer maintaining those in a separate file).

```javascript
window.mobileSearch = window.mobileSearch || {
    views: {
        appview: new AppView
    },
    routers:{
        workspace:new Workspace()
    },
    utils: utils,
    defaults:{
        resultsPerPage: 16,
        safeSearch: 2,
        maxDate:'',
        minDate:'01/01/1970'
    }
}
```

<strong>Models</strong>

In the Flickly application, there are at least two unique types of data that need to be modelled - search results and individual photos, both of which contain additional meta-data like photo titles. If you simplify this down, search results are actually groups of photos in their own right, so the application only requires:

* A single model (a photo or 'result' entry)
* A result collection (containing a group of result entries) for search results
* A photo collection (containing one or more result entries) for individual photos or photos with more than one image

<strong>Views</strong>

The views we'll need include an application view, a search results view and a photo view. Static views or pages of the single-page application which do not require a dynamic element to them (e.g an 'about' page) can be easily coded up in your document's markup, independant of Backbone. 

<strong>Routers</strong>

A number of possible routes need to be taken into consideration:

* Basic search queries <code>#search/kiwis</code>
* Search queries with additional parameters (e.g sort, pagination) <code>#search/kiwis/srelevance/p7</code>
* Queries for specific photos <code>#photo/93839</code>
* A default route (no parameters passed)


####<a name="jquery-mobile">jQuery Mobile: Going beyond mobile application development</a>

The majority of jQM apps I've seen in production have been developed for the purpose of providing an optimal experience to users on mobile devices. Given that the framework was developed for this purpose, there's nothing fundamentally wrong with this, but many developers forget that jQM is a UI framework not dissimilar to jQuery UI. It's using the widget factory and is capable of being used for a lot more than we give it credit for.

If you open up Flickly in a desktop browser, you'll get an image search UI that's modelled on Google.com, however, review the components (buttons, text inputs, tabs) on the page for a moment. The desktop UI doesn't look anything like a mobile application yet I'm still using jQM for theming mobile components; the tabs, date-picker, sliders - everything in the desktop UI is re-using what jQM would be providing users on mobile devices. Thanks to some media queries, the desktop UI can make optimal use of whitespace, expanding component blocks out and providing alternative layouts whilst still making use of jQM as a component framework.

The benefit of this is that I don't need to go pulling in jQuery UI separately to be able to take advantage of these features. Thanks to the recent ThemeRoller my components can look pretty much exactly how I would like them to and users of the app can get a jQM UI for lower-resolutions and a jQM-ish UI for everything else. 

The takeaway here is just to remember that if you're not (already) going through the hassle of conditional script/style loading based on screen-resolution (using matchMedia.js etc), there are simpler approaches that can be taken to cross-device component theming. 

                                  




