
##New Beginnings

###What is Backbone?

Backbone.js is one of a number of JavaScript frameworks for creating MVC-like web applications. On the front-end, it's my architectural framework of choice as it's both mature, relatively lightweight and can be easily tested using third-party toolkits such as Jasmine or QUnit. Other MVC options you may be familiar with include Ember.js (SproutCore 2.0), Spine, YUILibrary and JavaScriptMVC.

Backbone is maintained by a number of contributors, most notably: Jeremy Ashkenas, creator of CoffeeScript, Docco and Underscore.js. As Jeremy is a believer in detailed documentation, there's a level of comfort in knowing you're unlikely to run into issues which are either not explained in the official docs or which can't be nailed down with some assistance from the #documentcloud IRC channel. I strongly recommend using the latter if you find yourself getting stuck.

###Why should you consider using it?

Backbone's main benefits, regardless of your target platform or device, include helping:

* Organize the structure to your application
* Simplify server-side persistence
* Decouple the DOM from your page's data
* Model data, views and routers in a succinct manner
* Provide DOM, model and collection synchronization

In ways, the real question is why you should consider applying the MVC-pattern to your JavaScript projects and the one word answer is simply structure.

If opting to use jQuery, Zepto or another querySelectorAll-based selection library to produce a non-trivial application it can become very easy to produce an unwieldy amount of code; that is - unless you have a plan for how you're going to structure and organize your application. Separating concerns into models, views and controllers (or routers) is one way of solving this.

Remember that if you have experience with structuring applications using the MVVM (Model-View ViewModel) pattern, modules or otherwise, these are also equally as valid but do require you to know what you're doing. For most single-page applications, I find that the MVC pattern works well so Backbone is a perfect fit for our needs.

##The Basics

In this section, you'll learn the essentials about Backbone's models, views, collections and routers. Whilst this isn't meant as a replacement for the official documentation, it will help you understand many of the core concepts behind Backbone before we build mobile applications with it. I will also be covering some tips on effective namespacing.

* Models
* Collections
* Routers
* Views
* Namespacing

###Models

Backbone models contain interactive data for an application as well as the logic around this data. For example, we can use a model to represent the concept of a photo object including its attributes like tags, titles and a location.

Models are quite straight-forward to create and can be constructed by extending `Backbone.Model` as follows:

    var Photo = Backbone.Model.extend({
        defaults: {
            src: 'placeholder.jpg',
            title: 'an image placeholder',
            coordinates: [0,0]
        },
        initialize: function(){
            this.bind("change:src", function(){
                var src = this.get("src"); 
                console.log('Image source updated to ' + src);
            });
        },
        changeSrc: function( source ){
            this.set({ src: source });
        }
    });
     
    var somePhoto = new Photo({ src: "test.jpg", title:"testing"});
    somePhoto.changeSrc("magic.jpg"); // which triggers "change:src" and logs an update message to the console.


####Initialization

The `initialize()` method is called when creating a new instance of a model. It's use is optional, however we'll be reviewing some reasons you may want to use it shortly.

<pre>
var Photo = Backbone.Model.extend({
    initialize: function(){
        console.log('this model has been initialized');
    }
});
 
/*We can then create our own instance of a photo as follows:*/
var myPhoto = new Photo();
</pre>

####Getters & Setters

**Model.get()**

`Model.get()` provides easy access to a model's attributes. Attributes which are passed through to the model on instantiation are instantly available for retrieval.

<pre>
var myPhoto = new Photo({ title: "My awesome photo", 
                          src:"boston.jpg", 
                          location: "Boston", 
                          tags:['the big game', 'vacation']}),
                          
    title = myPhoto.get("title"), //my awesome photo
    location = myPhoto.get("location"), //Boston
    tags = myPhoto.get("tags"), // ['the big game','vacation']
    photoSrc = myPhoto.get("src"); //boston.jpg
</pre>

Alternatively, if you wish to directly access all of the attributes in an model's instance directly, you can achieve this as follows:

<pre>
var myAttributes = myPhoto.attributes;
console.log(myAttributes);
</pre>

It is best practice to use `Model.set()` or direct instantiation to set the values of a model's attributes.

Accessing `Model.attributes` directly is generally discouraged. Instead, should you need to read or clone data, `Model.toJSON()` is recommended for this purpose. If you would like to access or copy a model's attributes for purposes such as JSON stringification (e.g. for serialization prior to being passed to a view), this can be achieved using `Model.toJSON()`:

<pre>
var myAttributes = myPhoto.toJSON();
console.log(myAttributes);
/* this returns { title: "My awesome photo", 
             src:"boston.jpg", 
             location: "Boston", 
             tags:['the big game', 'vacation']}*/
</pre>

**Model.set()**

`Model.set()` allows us to pass attributes into an instance of our model. Attributes can either be set during initialization or later on.

<pre>
var Photo = Backbone.Model.extend({
    initialize: function(){
        console.log('this model has been initialized');
    }
});
 
/*Setting the value of attributes via instantiation*/
var myPhoto = new Photo({ title: 'My awesome photo', location: 'Boston' });
 
var myPhoto2 = new Photo();

/*Setting the value of attributes through Model.set()*/
myPhoto2.set({ title:'Vacation in Florida', location: 'Florida' });
</pre>

**Default values**

There are times when you want your model to have a set of default values (e.g. in a scenario where a complete set of data isn't provided by the user). This can be set using a property called `defaults` in your model.

<pre>
var Photo = Backbone.Model.extend({
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
</pre>

**Listening for changes to your model**

Any and all of the attributes in a Backbone model can have listeners bound to them which detect when their values change. This can be easily added to the `initialize()` function as follows:

<pre>
this.bind('change', function(){
    console.log('values for this model have changed');
});
</pre>

In the following example, we can also log a message whenever a specific attribute (the title of our Photo model) is altered.

<pre>
var Photo = Backbone.Model.extend({
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
</pre>

**Validation**

Backbone supports model validation through `Model.validate()`, which allows checking the attribute values for a model prior to them being set.

It supports including as complex or terse validation rules against attributes and is quite straight-forward to use. If the attributes provided are valid, nothing should be returned from `.validate()`, however if they are invalid a custom error can be returned instead.

A basic example for validation can be seen below:

<pre>
var Photo = Backbone.Model.extend({
    validate: function(attribs){
        if(attribs.src === undefined){
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
</pre>



###Views

Views in Backbone don't contain the markup for your application, but rather they are there to support models by defining how they should be visually represented to the user. This is usually achieved using JavaScript templating (e.g. Mustache, jQuery tmpl etc). When a model updates, rather than the entire page needing to be refreshed, we can simply bind a view's `render()` function to a model's `change()` event, allowing the view to always be up to date.

####Creating new views

Similar to the previous sections, creating a new view is relatively straight-forward. We simply extend `Backbone.View`. Here's an example of a possible implementation of this, which I'll explain shortly:

<pre>
var PhotoSearch = Backbone.View.extend({
    el: $('#results'),
    render: function( event ){
        var compiled_template = _.template( $("#results-template").html() );
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
</pre>

####What is el?

`el` is basically a reference to a DOM element and all views must have one, however Backbone allows you to specify this in four different ways. You can either directly use an `id`, a `tagName`, `className` or if you don't state anything `el` will simply default to a plain div element without any id or class. Here are some quick examples of how these may be used:

<pre>
el: $('#results')  //select based on an ID or other valid jQuery selector.
tagName: 'li' //select based on a specific tag. Here el itself will be derived from the tagName
className: 'items' //similar to the above, this will also result in el being derived from it
el: '' //defaults to a div without an id, name or class.
</pre>

**Note:** A combination of these methods can also be used to define `el`. For example:

<pre>
tagName: "li",
className: "container"
</pre>
will use specific tags with a particular `className`.


**Understanding `render()`**

`render()` is an optional function to define how you would like a template to be rendered. Backbone allows you to use any JavaScript templating solution of your choice for this but for the purposes of this book, we'll opt for Underscore's micro-templating.

The `_.template` method in underscore compiles JavaScript templates into functions which can be evaluated for rendering. In the above view, I'm passing the markup from a template with id `results-template` to be compiled. Next, I set the html for `el` (our DOM element for this view) the output of processing a JSON version of the model associated with the view through the compiled template.

Presto! This populates the template, giving you a data-complete set of markup in just a few short lines of code.

**The `events` attribute**

The Backbone `events` attribute allows us to attach event listeners to either custom selectors, or `el` if no selector is provided. An event takes the form `{"eventName selector": "callbackFunction"}` and a number of event-types are supported, including 'click', 'submit', 'mouseover', 'dblclick' and more.

What isn't instantly obvious is that under the bonnet, Backbone uses jQuery's `.delegate()` to provide instant support for event delegation but goes a little further, extending it so that `this` always refers to the current view object. The only thing to really keep in mind is that any string callback supplied to the events attribute must have a corresponding function with the same name within the scope of your view otherwise you may incur exceptions.

###Collections

Collections are basically sets of models and can be easily created by extending `Backbone.Collection`.

Normally, when creating a collection you'll also want to pass through a property specifying the model that your collection will contain as well as any instance properties required.

In the following example, we're creating a PhotoCollection containing the Photo models we previously defined.

<pre>
var PhotoCollection = Backbone.Collection.extend({
    model: Photo
});
</pre>

**Getters and Setters**

There are a few different options for retrieving a model from a collection. The most straight-forward is using `Collection.get()` which accepts a single id as follows:

<pre>
var skiingEpicness = PhotoCollection.get(2);
</pre>

Sometimes you may also want to get a model based on something called the client id. This is an id that is internally assigned automatically when creating models that have not yet been saved, should you need to reference them. You can find out what a model's client id is by accessing its `.cid` property.

<pre>
var mySkiingCrash = PhotoCollection.getByCid(456);
</pre>

Backbone Collections don't have setters as such, but do support adding new models via `.add()` and removing models via `.remove()`.

<pre>
var a = new Backbone.Model({ title: 'my vacation'}),
    b = new Backbone.Model({ title: 'my holiday'});

var photoCollection = new PhotoCollection([a,b]);
photoCollection.remove([a,b]);
</pre>

**Listening for events**

As collections represent a group of items, we're also able to listen for `add` and `remove` events for when new models are added or removed from the collection. Here's an example:

<pre>
var PhotoCollection = new Backbone.Collection();
PhotoCollection.bind("add", function(photo) {
  console.log("I liked " + photo.get("title") + ' its this one, right? '  + photo.get("src"));
});
 
PhotoCollection.add([
  {title: "My trip to Bali", src: "bali-trip.jpg"},
  {title: "The flight home", src: "long-flight-oofta.jpg"},
  {title: "Uploading pix", src: "too-many-pics.jpg"}
]);
</pre>

In addition, we're able to bind a `change` event to listen for changes to models in the collection.

<pre>
PhotoCollection.bind("change:title", function(){
    console.log('there have been updates made to this collections titles');    
});
</pre>

**Fetching models from the server**

`Collections.fetch()` provides you with a simple way to fetch a default set of models from the server in the form of a JSON array. When this data returns, the current collection will refresh.

<pre>
var PhotoCollection = new Backbone.Collection;
PhotoCollection.url = '/photos';
PhotoCollection.fetch();
</pre>

Under the covers, `Backbone.sync` is the function called every time Backbone tries to read (or save) models to the server. It uses jQuery or Zepto's ajax implementations to make these RESTful requests, however this can be overridden as per your needs.

In the above fetch example if we wish to log an event when `.sync()` gets called, we can simply achieve this as follows:

<pre>
Backbone.sync = function(method, model) {
  console.log("I've been passed " + method + " with " + JSON.stringify(model));
};
</pre>

**Resetting/Refreshing Collections**

Rather than adding or removing models individually, you occasionally wish to update an entire collection at once. `Collection.reset()` allows us to replace an entire collection with new models as follows:

<pre>
PhotoCollection.reset([
  {title: "My trip to Scotland", src: "scotland-trip.jpg"},
  {title: "The flight from Scotland", src: "long-flight.jpg"},
  {title: "Latest snap of lock-ness", src: "lockness.jpg"}]);
</pre>

###Underscore utility functions

As Backbone requires Underscore as a hard dependency, we're able to use many of the utilities it has to offer to aid with our application development. Here's an example of how Underscore's `sortBy()` method can be used to sort a collection of photos based on a particular attribute.

<pre>
var sortedByAlphabet = PhotoCollection.sortBy(function(photo)){
    return photo.get("title").toLowerCase();
});
</pre>
The complete list of what it can do is beyond the scope of this guide, but can be found in the official docs.

###Routers

In Backbone, routers are used to handle routing for your application. This is achieved using hash-tags with URL fragments which you can read more about if you wish. Some examples of valid routes may be seen below:

<pre>
http://unicorns.com/#/whatsup
http://unicorns.com/#/search/seasonal-horns/page2
</pre>

Note: A router will usually have at least one URL route defined as well as a function that maps what happens when you reach that particular route. This type of key/value pair may resemble:

<pre>
"/route" : "mappedFunction"
</pre>

Let us now define our first controller by extending `Backbone.Router`. For the purposes of this guide, we're going to continue pretending we're creating a photo gallery application that requires a GalleryRouter.

Note the inline comments in the code example below as they continue the rest of the lesson on routers.

<pre>
var GalleryRouter = Backbone.Router.extend({
    /* define the route and function maps for this router */
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
 
/* Now that we have a router setup, remember to instantiate it*/
 
var myGalleryRouter = new GalleryRouter();
</pre>

Note: In Backbone 0.5+, it's possible to opt-in for HTML5 pushState support via `window.history.pushState`. This effectively permits non-hashtag routes such as http://www.scriptjunkie.com/just/an/example to be supported with automatic degradation should your browser not support it. For the purposes of this tutorial, we won't be relying on this newer functionality as there have been reports about issues with it under iOS/Mobile Safari. Backbone's hash-based routes should however suffice for our needs.

####Backbone.history

Next, we need to initialize `Backbone.history` as it handles `hashchange` events in our application. This will automatically handle routes that have been defined and trigger callbacks when they've been accessed.

The `Backbone.history.start()` method will simply tell Backbone that it's OK to begin monitoring all `hashchange` events as follows:

<pre>
Backbone.history.start();
Router.navigate();
</pre>

As an aside, if you would like to save application state to the URL at a particular point you can use the `.navigate()` method to achieve this. It simply updates your URL fragment without the need to trigger the `hashchange` event.

<pre>
/*Lets imagine we would like a specific fragment for when a user zooms into a photo*/
zoomPhoto: function(factor){
    this.zoom(factor); //imagine this zooms into the image
    this.navigate("zoom/" + factor); //updates the fragment for us, but doesn't trigger the route
}
</pre>

It is also possible for `Router.navigate()` to trigger the route aswell as updating the URL fragment.

<pre>
zoomPhoto: function(factor){
    this.zoom(factor); //imagine this zooms into the image
    this.navigate("zoom/" + factor, true); //updates the fragment for us and triggers the route
}
</pre>

###Namespacing

When learning how to use Backbone, an important and commonly overlooked area by tutorials is namespacing. If you already have experience with namespacing in JavaScript, the following section will provide some advice on how to specifically apply concepts you know to Backbone, however I will also be covering explanations for beginners to ensure everyone is on the same page.

####What is namespacing?

The basic idea around namespacing is to avoid collisions with other objects or variables in the global namespace. They're important as it's best to safeguard your code from breaking in the event of another script on the page using the same variable names as you are. As a good 'citizen' of the global namespace, it's also imperative that you do your best to similarly not prevent other developer's scripts executing due to the same issues.

JavaScript doesn't really have built-in support for namespaces like other languages, however it does have closures which can be used to achieve a similar effect.

In this section we'll be taking a look shortly at some examples of how you can namespace your models, views, routers and other components specifically. The patterns we'll be examining are:

* Single global variables
* Object Literals
* Nested namespacing

**Single global variables**

One popular pattern for namespacing in JavaScript is opting for a single global variable as your primary object of reference. A skeleton implementation of this where we return an object with functions and properties can be found below:

<pre>
var myApplication = (function(){
    function(){
      // ...
    },
    return {
      // ...
    }
})();
</pre>

which you're likely to have seen before. A Backbone-specific example which may be more useful is:

<pre>
var myViews = (function(){
    return {
        PhotoView: Backbone.View.extend({ .. }),
        GalleryView: Backbone.View.extend({ .. }),
        AboutView: Backbone.View.extend({ .. });
        //etc.
    };
})();
</pre>

Here we can return a set of views or even an entire collection of models, views and routers depending on how you decide to structure your application. Although this works for certain situations, the biggest challenge with the single global variable pattern is ensuring that no one else has used the same global variable name as you have in the page.

One solution to this problem, as mentioned by Peter Michaux, is to use prefix namespacing. It's a simple concept at heart, but the idea is you select a basic prefix namespace you wish to use (in this example, `myApplication_`) and then define any methods, variables or other objects after the prefix.

<pre>
var myApplication_photoView = Backbone.View.extend({}),
myApplication_galleryView = Backbone.View.extend({});
</pre>

This is effective from the perspective of trying to lower the chances of a particular variable existing in the global scope, but remember that a uniquely named object can have the same effect. This aside, the biggest issue with the pattern is that it can result in a large number of global objects once your application starts to grow.

For more on Peter's views about the single global variable pattern, read his [excellent post on them](http://michaux.ca/articles/javascript-namespacing).

Note: There are several other variations on the single global variable pattern out in the wild, however having reviewed quite a few, I felt these applied best to Backbone.

**Object Literals**

Object Literals have the advantage of not polluting the global namespace but assist in organizing code and parameters logically. They're beneficial if you wish to create easily-readable structures that can be expanded to support deep nesting. Unlike simple global variables, Object Literals often also take into account tests for the existence of a variable by the same name so the chances of collision occurring are significantly reduced.

The code at the very top of the next sample demonstrates the different ways in which you can check to see if a namespace already exists before defining it. I commonly use Option 3.

<pre>
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
</pre>

One can also opt for adding properties directly to the namespace (such as your views, in the following example):

<pre>
var myGalleryViews = myGalleryViews || {};
myGalleryViews.photoView = Backbone.View.extend({});
myGalleryViews.galleryView = Backbone.View.extend({});
</pre>

The benefit of this pattern is that you're able to easily encapsulate all of your models, views, routers etc. in a way that clearly separates them and provides a solid foundation for extending your code.

This pattern has a number of useful applications. It's often of benefit to decouple the default configuration for your application into a single area that can be easily modified without the need to search through your entire codebase just to alter them - Object Literals work great for this purpose. Here's an example of a hypothetical Object Literal for configuration:

<pre>
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
</pre>

Note that there are really only minor syntactical differences between the Object Literal pattern and a standard JSON data set. If for any reason you wish to use JSON for storing your configurations instead (e.g. for simpler storage when sending to the back-end), feel free to.

For more on the Object Literal pattern, I recommend reading Rebecca Murphey's [excellent article on the topic](http://blog.rebeccamurphey.com/2009/10/15/using-objects-to-organize-your-code).

**Nested namespacing**

An extension of the Object Literal pattern is nested namespacing. It's another common pattern used that offers a lower risk of collision due to the fact that even if a namespace already exists, it's unlikely the same nested children do.

Does this look familiar?

<pre>
YAHOO.util.Dom.getElementsByClassName('test');
</pre>

Yahoo's YUI uses the nested object namespacing pattern regularly and even DocumentCloud (the creators of Backbone) use the nested namespacing pattern in their main applications. A sample implementation of nested namespacing with Backbone may look like this:

<pre>
var galleryApp =  galleryApp || {};
 
/*perform similar check for nested children*/
galleryApp.routers = galleryApp.routers || {};
galleryApp.model = galleryApp.model || {};
galleryApp.model.special = galleryApp.model.special || {};
 
/*routers*/
galleryApp.routers.Workspace   = Backbone.Router.extend({}); 
galleryApp.routers.PhotoSearch = Backbone.Router.extend({}); 
 
/*models*/
galleryApp.model.Photo   = Backbone.Model.extend({});
galleryApp.model.Comment = Backbone.Model.extend({}); 
 
/*special models*/
galleryApp.model.special.Admin = Backbone.Model.extend({});
</pre>

This is both readable, organized and is a relatively safe way of namespacing your Backbone application in a similar fashion to what you may be used to in other languages.

The only real caveat however is that it requires your browser's JavaScript engine first locating the galleryApp object and then digging down until it gets to the function you actually wish to use.

This can mean an increased amount of work to perform lookups, however developers such as Juriy Zaytsev (kangax) have previously tested and found the performance differences between single object namespacing vs the 'nested' approach to be quite negligible.

**Recommendation**

Reviewing the namespace patterns above, the option that I would personally use with Backbone is nested object namespacing with the object literal pattern.

Single global variables may work fine for applications that are relatively trivial, however, larger codebases requiring both namespaces and deep sub-namespaces require a succinct solution that promotes readability and scales. I feel this pattern achieves all of these objectives well and is a perfect companion for Backbone development.

###Additional Tips

####Automated Backbone Scaffolding

Scaffolding can assist in expediting how quickly you can begin a new application by creating the basic files required for a project automatically. If you enjoy the idea of automated MVC scaffolding using Backbone, I'm happy to recommend checking out a tool called Brunch.

It works very well with Backbone, Underscore, jQuery and CoffeeScript and is even used by companies such as Red Bull and Jim Bean. You may have to update any third party dependencies (e.g. latest jQuery or Zepto) when using it, but other than that it should be fairly stable to use right out of the box.

Brunch can easily be installed via the nodejs package manager and takes just little to no time to get started with. If you happen to use Vim or Textmate as your editor of choice, you may be happy to know that there are also Brunch bundles available for both.

####Clarifications on Backbone's MVC

As Thomas Davis has previously noted, Backbone.js's MVC is a loose interpretation of traditional MVC, something common to many client-side MVC solutions. Backbone's views are what could be considered a wrapper for templating solutions such as the Mustache.js and `Backbone.View` is the equivalent of a controller in traditional MVC. `Backbone.Model` is however the same as a classical 'model'.

Whilst Backbone is not the only client-side MVC solution that could use some improvements in it's naming conventions, `Backbone.Controller` was probably the most central source of some confusion but has been renamed a router in more recent versions. This won't prevent you from using Backbone effectively, however this is being pointed out just to help avoid any confusion if for any reason you opt to use an older version of the framework.

The official Backbone docs do attempt to clarify that their routers aren't really the C in MVC, but it's important to understand where these fit rather than considering client-side MVC a 1:1 equivalent to the pattern you've probably seen in server-side development.

####Is there a limit to the number of routers I should be using?

Andrew de Andrade has pointed out that DocumentCloud themselves usually only use a single controller in most of their applications. You're very likely to not require more than one or two routers in your own projects as the majority of your application routing can be kept organized in a single controller without it getting unwieldy.

####Is Backbone too small for my application's needs?

If you find yourself unsure of whether or not your application is too large to use Backbone, I recommend reading my post on building large-scale jQuery & JavaScript applications or reviewing my slides on client-side MVC architecture options. In both, I cover alternative solutions and my thoughts on the suitability of current MVC solutions for scaled application development.

Backbone can be used for building both trivial and complex applications as demonstrated by the many examples Ashkenas has been referencing in the Backbone documentation. As with any MVC framework however, it's important to dedicate time towards planning out what models and views your application really needs. Diving straight into development without doing this can result in either spaghetti code or a large refactor later on and it's best to avoid this where possible.

At the end of the day, the key to building large applications is not to build large applications in the first place. If you however find Backbone doesn't cut it for your requirements I strongly recommend checking out JavaScriptMVC or SproutCore as these both offer a little more than Backbone out of the box. Dojo and Dojo Mobile may also be of interest as these have also been used to build significantly complex apps by other developers.

