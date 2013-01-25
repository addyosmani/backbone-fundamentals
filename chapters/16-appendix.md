# Appendix



## MVP

Model-View-Presenter (MVP) is a derivative of the MVC design pattern which focuses on improving presentation logic. It originated at a company named [Taligent](http://en.wikipedia.org/wiki/Taligent) in the early 1990s while they were working on a model for a C++ CommonPoint environment. Whilst both MVC and MVP target the separation of concerns across multiple components, there are some fundamental differences between them.

For the purposes of this summary we will focus on the version of MVP most suitable for web-based architectures.

### Models, Views & Presenters

The P in MVP stands for presenter. It's a component which contains the user-interface business logic for the view. Unlike MVC, invocations from the view are delegated to the presenter, which are decoupled from the view and instead talk to it through an interface. This allows for all kinds of useful things such as being able to mock views in unit tests.

The most common implementation of MVP is one which uses a Passive View (a view which is for all intents and purposes "dumb"), containing little to no logic. MVP models are almost identical to MVC models and handle application data. The presenter acts as a mediator which talks to both the view and model, however both of these are isolated from each other. They effectively bind models to views, a responsibility held by Controllers in MVC. Presenters are at the heart of the MVP pattern and as you can guess, incorporate the presentation logic behind views.

Solicited by a view, presenters perform any work to do with user requests and pass data back to them. In this respect, they retrieve data, manipulate it and determine how the data should be displayed in the view. In some implementations, the presenter also interacts with a service layer to persist data (models). Models may trigger events but it's the presenter's role to subscribe to them so that it can update the view. In this passive architecture, we have no concept of direct data binding. Views expose setters which presenters can use to set data.

The benefit of this change from MVC is that it increases the testability of your application and provides a more clean separation between the view and the model. This isn't however without its costs as the lack of data binding support in the pattern can often mean having to take care of this task separately.

Although a common implementation of a [Passive View](http://martinfowler.com/eaaDev/PassiveScreen.html) is for the view to implement an interface, there are variations on it, including the use of events which can decouple the View from the Presenter a little more. As we don't have the interface construct in JavaScript, we're using it more and more as a protocol than an explicit interface here. It's technically still an API and it's probably fair for us to refer to it as an interface from that perspective.


There is also a [Supervising Controller](http://martinfowler.com/eaaDev/SupervisingPresenter.html) variation of MVP, which is closer to the MVC and [MVVM - Model-View-ViewModel](http://en.wikipedia.org/wiki/Model_View_ViewModel) patterns as it provides data-binding from the Model directly from the View. Key-value observing (KVO) plugins (such as Derick Bailey's Backbone.ModelBinding plugin) introduce this idea of a Supervising Controller to Backbone.


## MVP or MVC?

MVP is generally used most often in enterprise-level applications where it's necessary to reuse as much presentation logic as possible. Applications with very complex views and a great deal of user interaction may find that MVC doesn't quite fit the bill here as solving this problem may mean heavily relying on multiple controllers. In MVP, all of this complex logic can be encapsulated in a presenter, which can simplify maintenance greatly.

As MVP views are defined through an interface and the interface is technically the only point of contact between the system and the view (other than a presenter), this pattern also allows developers to write presentation logic without needing to wait for designers to produce layouts and graphics for the application.

Depending on the implementation, MVP may be more easy to automatically unit test than MVC. The reason often cited for this is that the presenter can be used as a complete mock of the user-interface and so it can be unit tested independent of other components. In my experience this really depends on the languages you are implementing MVP in (there's quite a difference between opting for MVP for a JavaScript project over one for say, ASP.NET).

At the end of the day, the underlying concerns you may have with MVC will likely hold true for MVP given that the differences between them are mainly semantic. As long as you are cleanly separating concerns into models, views and controllers (or presenters) you should be achieving most of the same benefits regardless of the pattern you opt for.

## MVC, MVP and Backbone.js

There are very few, if any architectural JavaScript frameworks that claim to implement the MVC or MVP patterns in their classical form as many JavaScript developers don't view MVC and MVP as being mutually exclusive (we are actually more likely to see MVP strictly implemented when looking at web frameworks such as ASP.NET or GWT). This is because it's possible to have additional presenter/view logic in your application and yet still consider it a flavor of MVC.

Backbone contributor [Irene Ros](http://ireneros.com/) subscribes to this way of thinking as when she separates Backbone views out into their own distinct components, she needs something to actually assemble them for her. This could either be a controller route (such as a ```Backbone.Router```, covered later in the book) or a callback in response to data being fetched.

That said, some developers do however feel that Backbone.js better fits the description of MVP than it does MVC
. Their view is that:

* The presenter in MVP better describes the ```Backbone.View``` (the layer between View templates and the data bound to it) than a controller does
* The model fits ```Backbone.Model``` (it isn't that different from the classical MVC "Model")
* The views best represent templates (e.g Handlebars/Mustache markup templates)

A response to this could be that the view can also just be a View (as per MVC) because Backbone is flexible enough to let it be used for multiple purposes. The V in MVC and the P in MVP can both be accomplished by ```Backbone.View``` because they're able to achieve two purposes: both rendering atomic components and assembling those components rendered by other views.

We've also seen that in Backbone the responsibility of a controller is shared with both the Backbone.View and Backbone.Router and in the following example we can actually see that aspects of that are certainly true.

Here, our Backbone ```TodoView``` uses the Observer pattern to 'subscribe' to changes to a View's model in the line ```this.model.on('change',...)```. It also handles templating in the ```render()``` method, but unlike some other implementations, user interaction is also handled in the View (see ```events```).


```javascript
// The DOM element for a todo item...
app.TodoView = Backbone.View.extend({

  //... is a list tag.
  tagName:  'li',

  // Pass the contents of the todo template through a templating
  // function, cache it for a single todo
  template: _.template( $('#item-template').html() ),

  // The DOM events specific to an item.
  events: {
    'click .toggle':  'togglecompleted'
  },

  // The TodoView listens for changes to its model, re-rendering. Since there's
  // a one-to-one correspondence between a **Todo** and a **TodoView** in this
  // app, we set a direct reference on the model for convenience.
  initialize: function() {
    this.model.on( 'change', this.render, this );
    this.model.on( 'destroy', this.remove, this );
  },

  // Re-render the titles of the todo item.
  render: function() {
    this.$el.html( this.template( this.model.toJSON() ) );
    return this;
  },

  // Toggle the `"completed"` state of the model.
  togglecompleted: function() {
    this.model.toggle();
  },
});
```

Another (quite different) opinion is that Backbone more closely resembles [Smalltalk-80 MVC](http://martinfowler.com/eaaDev/uiArchs.html#ModelViewController), which we went through earlier.

As MarionetteJS author Derick Bailey has [written](http://lostechies.com/derickbailey/2011/12/23/backbone-js-is-not-an-mvc-framework/), it's ultimately best not to force Backbone to fit any specific design patterns. Design patterns should be considered flexible guides to how applications may be structured and in this respect, Backbone doesn't fit either MVC nor MVP perfectly. Instead, it borrows some of the best concepts from multiple architectural patterns and creates a flexible framework that just works well. Call it **the Backbone way**, MV* or whatever helps reference its flavor of application architecture.

It *is* however worth understanding where and why these concepts originated, so I hope that my explanations of MVC and MVP have been of help. Most structural JavaScript frameworks will adopt their own take on classical patterns, either intentionally or by accident, but the important thing is that they help us develop applications which are organized, clean and can be easily maintained.


## Upgrading to Backbone 0.9.10

For developers transitioning from earlier versions of Backbone.js, the following is a guide of [changes](http://backbonejs.org/#changelog) between 0.9.2 and 0.9.10 grouped by classes, where applicable.

### Model
* Model validation is now only enforced by default in `Model#save` and no longer enforced by default upon construction or in `Model#set`, unless the `{validate:true}` option is passed.
* Passing `{silent:true}` on change will no longer delay individual `"change:attr"` events, instead they are silenced entirely.
* The `Model#change` method has been removed, as delayed attribute changes as no longer available.
* Calling `destroy` on a Model will now return `false` if the model `isNew`.
* After fetching a model or a collection, all defined parse functions will now be run. So fetching a collection and getting back new models could cause both the collection to parse the list, and then each model to be parsed in turn, if you have both functions defined.
* While listening to a [reset](http://backbonejs.org/#Collection-reset) event, the list of previous models is now available in `options.previousModels`, for convenience.


### Collection

* When using `add` on a collection, passing `{merge: true}` will now cause duplicate models to have their attributes merged in to the existing models, instead of being ignored.
* `Collection#sort` now triggers a `sort` event, instead of a `reset` event.
* Removed `getByCid` from Collections. `collection.get` now supports lookup by both id and cid.
* Collections now also proxy Underscore method name aliases (`collect`, `inject`, `foldl`, `foldr`, `head`, `tail`, `take`, and so on...)
* Added `update` (which is also available as an option to fetch) for "smart" updating of sets of models.
* `collection.indexOf(model)` can be used to retrieve the index of a model as necessary.


### View
* `View#make` has been removed. You'll need to use `$` directly to construct DOM elements now.
* When declaring a View, `options`, `el`, `tagName`, `id` and `className` may now be defined as functions, if you want their values to be determined at runtime.

### Events
* Added [listenTo](http://backbonejs.org/#Events-listenTo) and [stopListening](http://backbonejs.org/#Events-stopListening) to Events. They can be used as inversion-of-control flavors of [on](http://backbonejs.org/#Events-on) and [off](http://backbonejs.org/#Events-off), for convenient unbinding of all events an object is currently listening to. `view.remove()` automatically calls `view.stopListening()`.
* The Backbone object now extends Events so that you can use it as a global event bus, if you like.
* Backbone events now support jQuery-style event maps `obj.on({click: action})`.
* Backbone events now supports [once](http://backbonejs.org/#Events-once), similar to Node's [once](http://nodejs.org/api/events.html#events_emitter_once_event_listener), or jQuery's [one](http://api.jquery.com/one/).

### Routers
* A "route" event is triggered on the router in addition to being fired on Backbone.history.
* For semantic and cross browser reasons, routes will now ignore search parameters. Routes like `search?query=…&page=3` should become `search/…/3`.
* Bugfix for normalizing leading and trailing slashes in the Router definitions. Their presence (or absence) should not affect behavior.
* Router URLs now support optional parts via parentheses, without having to use a regex. 


### Sync
* For mixed-mode APIs, `Backbone.sync` now accepts [emulateHTTP](http://backbonejs.org/#Sync-emulateHTTP) and [emulateJSON](http://backbonejs.org/#Sync-emulateJSON) as inline options.
* Consolidated `"sync"` and `"error"` events within `Backbone.sync`. They are now triggered regardless of the existence of success or error callbacks.
* Added a `"request"` event to `Backbone.sync`, which triggers whenever a request begins to be made to the server. The natural complement to the `"sync"` event. 


### Other
* Bug fix on change where attribute comparison uses `!==` instead of `_.isEqual`.
* Bug fix where an empty response from the server on save would not call the success function.
* To improve the performance of add, `options.index` will no longer be set in the `add` event callback. 
* Removed the `Backbone.wrapError` helper method. Overriding sync should work better for those particular use cases.
* To set what library Backbone uses for DOM manipulation and Ajax calls, use `Backbone.$ =` ... instead of `setDomLibrary`.
* Added a `Backbone.ajax` hook for more convenient overriding of the default use of `$.ajax`. If AJAX is too passé, set it to your preferred method for server communication.
* Validation now occurs even during `"silent"` changes. This change means that the `isValid` method has been removed. Failed validations also trigger an error, even if an error callback is specified in the options.
* HTTP PATCH support in save by passing `{patch: true}`.


---
Where relevant, copyright Addy Osmani, 2012-2013.
