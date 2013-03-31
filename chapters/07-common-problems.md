# Common Problems & Solutions

In this section, we will review a number of common problems developers often experience once they've started to work on relatively non-trivial projects using Backbone.js, as well as present potential solutions.

Perhaps the most frequent of these questions surround how to do more with Views. If you are interested in discovering how to work with nested Views, learn about view disposal and inheritance, this section will hopefully have you covered.


#### Working With Nested Views

**Problem**

What is the best approach for rendering and appending nested Views (or Subviews) in Backbone.js?

**Solution 1**

Since pages are composed of nested elements and Backbone views correspond to elements within the page, nesting views is an intuitive approach to managing a hierarchy of elements.

The best way to combine views is simply using:

```
this.$('.someContainer').append(innerView.el);
```

which just relies on jQuery. We could use this in a real example as follows:

```javascript
...
initialize : function () { 
    //...
},

render : function () {

    this.$el.empty();

    this.innerView1 = new Subview({options});
    this.innerView2 = new Subview({options});

    this.$('.inner-view-container')
        .append(this.innerView1.el)
        .append(this.innerView2.el);
}
```

**Solution 2**

Beginners sometimes also try using `setElement` to solve this problem, however keep in mind that using this method is an easy way to shoot yourself in the foot. Avoid using this approach when the first solution is a viable option:


```javascript

// Where we have previously defined a View, SubView
// in a parent View we could do:

...
initialize : function () {

    this.innerView1 = new Subview({options});
    this.innerView2 = new Subview({options});
},

render : function () {

    this.$el.html(this.template());

    this.innerView1.setElement('.some-element1').render();
    this.innerView2.setElement('.some-element2').render();
}
```

Here we are creating subviews in the parent view's `initialize()` method and rendering the subviews in the parent's `render()` method. The elements managed by the subviews exist in the parent's template and the `View.setElement()` method is used to re-assign the element associated with each subview. 

`setElement()` changes a view's element, including re-delegating event handlers by removing them from the old element and binding them to the new element. Note that `setElement()` returns the view, allowing us to chain the call to `render()`.

This works and has some positive qualities: you don't need to worry about maintaining the order of your DOM elements when appending, views are initialized early, and the render() method doesn't need to take on too many responsibilities at once. 

Unfortunately, downsides are that you can't set the `tagName` property of subviews and events need to be re-delegated. The first solution doesn't suffer from this problem.


**Solution 3**

One more possible solution to this problem could be written:

```javascript

var OuterView = Backbone.View.extend({
    initialize: function() {
        this.inner = new InnerView();
    },

    render: function() {
        this.$el.html(template); // or this.$el.empty() if you have no template
        this.$el.append(this.inner.$el);
        this.inner.render();
    }
});

var InnerView = Backbone.View.extend({
    render: function() {
        this.$el.html(template);
        this.delegateEvents();
    }
});

```

This tackles a few specific design decisions:

* The order in which you append the sub-elements matters
* The OuterView doesn't contain the HTML elements to be set in the InnerView(s), meaning that we can still specify tagName in the InnerView
* render() is called after the InnerView element has been placed into the DOM. This is useful if your InnerView's render() method is sizing itself on the page based on the dimensions of another element. This is a common use case.

Note that InnerView needs to call `View.delegateEvents()` to bind its event handlers to its new DOM since it is replacing the content of its element.

**Solution 4**

A better solution, which is more clean but has the potential to affect performance is:

```javascript

var OuterView = Backbone.View.extend({
    initialize: function() {
        this.render();
    },

    render: function() {
        this.$el.html(template); // or this.$el.empty() if you have no template
        this.inner = new InnerView();
        this.$el.append(this.inner.$el);
    }
});

var InnerView = Backbone.View.extend({
    initialize: function() {
        this.render();
    },

    render: function() {
        this.$el.html(template);
    }
});
```

If multiple views need to be nested at particular locations in a template, a hash of child views indexed by child view cids' should be created. In the template, use a custom HTML attribute named `data-view-cid` to create placeholder elements for each view to embed. Once the template has been rendered and its output appended to the parent view's `$el`, each placeholder can be queried for and replaced with the child view's `el`.

A sample implementation containing a single child view could be written:

```javascript

var OuterView = Backbone.View.extend({
    initialize: function() {
        this.children = {};
        this.child = new Backbone.View();
        this.children[this.child.cid] = this.child;
    },

    render: function() {
        this.$el.html('<div data-view-cid="' + this.child.cid + '"></div>');        
        _.each(this.children, function(view, cid) {
            this.$('[data-view-cid="' + cid + '"]').replaceWith(view.el);
        }, this);
    }
};

```

The use of `cid`s (client ids) here is useful because it illustrates separating a model and its views by having views referenced by their instances and not their attributes. It's quite common to ask for all views that satisfy an attribute on their models, but if you have recursive subviews or repeated views (a common occurrance), you can't simply ask for views by attributes. That is, unless you specify additional attributes that separate duplicates. Using `cid`s solves this problem as it allows for direct references to views.


Generally speaking, more developers opt for Solution 1 or 5 as:

* The majority of their views may already rely on being in the DOM in their render() method
* When the OuterView is re-rendered, views don't have to be re-initialized where re-initialization has the potential to cause memory leaks and issues with existing bindings

The Backbone extensions [Marionette](#marionette) and [Thorax](#thorax) provide logic for nesting views, and rendering collections where each item has an associated view. Marionette provides APIs in JavaScript while Thorax provides APIs via Handlebars template helpers. We will examine both of these in an upcoming chapter.

(Thanks to [Lukas](http://stackoverflow.com/users/299189/lukas)  and [Ian Taylor](http://stackoverflow.com/users/154765/ian-storm-taylor) for these tips).

#### Managing Models In Nested Views

**Problem**

What is the best way to manage models in nested views?

**Solution**

In order to reach attributes on related models in a nested setup, models require some prior knowledge of each other, something which Backbone doesn't implicitly handle out of the box.

One approach is to make sure each child model has a 'parent' attribute. This way you can traverse the nesting first up to the parent and then down to any siblings that you know of. So, assuming we have models modelA, modelB and modelC:

```javascript

// When initializing modelA, I would suggest setting a link to the parent
// model when doing this, like this:

ModelA = Backbone.Model.extend({

    initialize: function(){
        this.modelB = new modelB();
        this.modelB.parent = this;
        this.modelC = new modelC();
        this.modelC.parent = this;
    }
}
```

This allows you to reach the parent model in any child model function through `this.parent`. 

Now, we have already discussed a few options for how to construct nested Views using Backbone. For the sake of simplicity, let us imagine that we are creating a new child view `ViewB` from within the `initialize()` method of `ViewA` below. `ViewB` can reach out over the `ViewA` model and listen out for changes on any of its nested models. 

See inline for comments on exactly what each step is enabling:

```javascript

// Define View A
ViewA = Backbone.View.extend({

    initialize: function(){
       // Create an instance of View B
       this.viewB = new ViewB();

       // Create a reference back to this (parent) view
       this.viewB.parentView = this;

       // Append ViewB to ViewA
       $(this.el).append(this.viewB.el);
    }
});

// Define View B
ViewB = Backbone.View.extend({

    //...,

    initialize: function(){
        // Listen for changes to the nested models in our parent ViewA
        this.listenTo(this.model.parent.modelB, "change", this.render);
        this.listenTo(this.model.parent.modelC, "change", this.render);

        // We can also call any method on our parent view if it is defined
        // $(this.parentView.el).shake();
    }

});

// Create an instance of ViewA with ModelA
// viewA will create its own instance of ViewB
// from inside the initialize() method
var viewA = new ViewA({ model: ModelA });
```


#### Rendering A Parent View From A Child View

**Problem**

How would one render a Parent View from one of its Children?

**Solution**

In a scenario where you have a view containing another view, such as a photo gallery containing a larger view modal, you may find that you need to render or re-render the parent view from the child. The good news is that solving this problem is quite straight-forward.

The simplest solution is to just use `this.parentView.render();`.

If however inversion of control is desired, events may be used to provide an equally valid solution. 

Say we wish to begin rendering when a particular event has occurred. For the sake of example, let us call this event 'somethingHappened'. The parent view can bind notifications on the child view to know when the event has occurred. It can then render itself.

In the parent view:

```javascript
// Parent initialize
this.listenTo(this.childView, 'somethingHappened', this.render);

// Parent removal
this.stopListening(this.childView, 'somethingHappened');
```

In the child view:

```javascript

// After the event has occurred
this.trigger('somethingHappened');

```

The child will trigger a "somethingHappened" event and the parent's render function will be called.

(Thanks to Tal [Bereznitskey](http://stackoverflow.com/users/269666/tal-bereznitskey) for this tip)


#### Disposing View Hierarchies

**Problem**

Where your application is setup with multiple Parent and Child Views, it is also common to desire removing any DOM elements associated with such views as well as unbinding any event handlers tied to child elements when you no longer require them.

**Solution**

The solution in the last question should be enough to handle this use case, but if you require a more explicit example that handles children, we can see one below:

```javascript
Backbone.View.prototype.close = function() {
    if (this.onClose) {
        this.onClose();
    }
    this.remove();
};

NewView = Backbone.View.extend({
    initialize: function() {
       this.childViews = [];
    },
    renderChildren: function(item) {
        var itemView = new NewChildView({ model: item });
        $(this.el).prepend(itemView.render());
        this.childViews.push(itemView);
    },
    onClose: function() {
      _(this.childViews).each(function(view) {
        view.close();
      });
    }
});

NewChildView = Backbone.View.extend({
    tagName: 'li',
    render: function() {
    }
});
```

Here, a close() method for views is implemented which disposes of a view when it is no longer needed or needs to be reset.

In most cases, the view removal should not affect any associated models. For example, if you are working on a blogging application and you remove a view with comments, perhaps another view in your app shows a selection of comments and resetting the collection would affect those views as well.

(Thanks to [dira](http://stackoverflow.com/users/906136/dira) for this tip)

Note: You may also be interested in reading the about Marionette Composite Views in the Extensions part of the book.

#### Rendering View Hierarchies

**Problem**

Let us say you have a Collection, where each item in the Collection could itself be a Collection. You can render each item in the Collection, and indeed can render any items which themselves are Collections. The problem you might have is how to render HTML that reflects the hierarchical nature of the data structure.

**Solution**

The most straight-forward way to approach this problem is to use a framework like Derick Bailey's [Backbone.Marionette](https://github.com/marionettejs/backbone.marionette). In this framework is a type of view called a CompositeView.

The basic idea of a CompositeView is that it can render a model and a collection within the same view.

It can render a single model with a template. It can also take a collection from that model and for each model in that collection, render a view. By default it uses the same composite view type that you've defined to render each of the models in the collection. All you have to do is tell the view instance where the collection is, via the initialize method, and you'll get a recursive hierarchy rendered.


There is a working demo of this in action available [online](http://jsfiddle.net/derickbailey/AdWjU/).

And you can get the source code and documentation for [Marionette](https://github.com/marionettejs/backbone.marionette) too.


#### Working With Nested Models Or Collections

**Problem**

Backbone doesn't include support for nested models or collections out of the box, favoring the use of good patterns for modeling your structured data on the client side. How do I work around this?

**Solution**

As we've seen, it's common to create collections representing groups of models using Backbone. It's also however common to wish to nest collections within models, depending on the type of application you are working on. 

Take for example a Building model that contains many Room models which could sit in a Rooms collection.

You could expose a `this.rooms` collection for each building, allowing you to lazy-load rooms once a building has been opened. 

```javascript
var Building = Backbone.Model.extend({

    initialize: function(){
        this.rooms = new Rooms;
        this.rooms.url = '/building/' + this.id + '/rooms';
        this.rooms.on("reset", this.updateCounts);
    },

    // ...

});

// Create a new building model
var townHall = new Building;

// once opened, lazy-load the rooms
townHall.rooms.fetch({reset: true});
```

There are also a number of Backbone plugins which can help with nested data structures, such as [Backbone Relational](https://github.com/PaulUithol/Backbone-relational). This plugin handles one-to-one, one-to-many and many-to-one relations between models for Backbone and has some excellent [documentation](http://backbonerelational.org/).


#### Better Model Property Validation

**Problem**

As we learned earlier in the book, the `validate` method on a Model is called by `set` (when the validate option is set) and `save`, and is passed the model attributes updated with the values passed to these methods.

By default, when we define a custom `validate` method, Backbone passes all of a Model's attributes through this validation each time, regardless of which model attributes are being set.

This means that it can be a challenge to determine which specific fields are being set or validated without being concerned about the others that aren't being set at the same time.

**Solution**

The most optimal solution to this problem probably isn't to stick validation in your model attributes. Instead, have a function specifically designed for validating that particular form. There are many good JavaScript form validation libraries out there. If you want to stick it on your model, just make it a class function:

```javascript
User.validate = function(formElement) {
  //...
};
```

To illustrate this problem better, let us look at a typical registration form use case that:

* Validates form fields using the blur event
* Validates each field regardless of whether other model attributes (i.e., other form data) are valid or not.

Here is one example of a desired use case:

We have a form where a user focuses and blurs first name, last name, and email HTML input boxes without entering any data. A "this field is required" message should be presented next to each form field.

HTML:

```
<!doctype html>
<html>
<head>
  <meta charset=utf-8>
  <title>Form Validation - Model#validate</title>
  <script src='http://code.jquery.com/jquery.js'></script>
  <script src='http://underscorejs.org/underscore.js'></script>
  <script src='http://backbonejs.org/backbone.js'></script>
</head>
<body>
  <form>
    <label>First Name</label>
    <input name='firstname'>
    <span data-msg='firstname'></span>
    <br>
    <label>Last Name</label>
    <input name='lastname'>
    <span data-msg='lastname'></span>
    <br>
    <label>Email</label>
    <input name='email'>
    <span data-msg='email'></span>
  </form>
</body>
</html>
```

Some simple validation that could be written using the current Backbone `validate` method to work with this form could be implemented using something like:


```javascript
validate: function(attrs) {

    if(!attrs.firstname) return 'first name is empty';
    if(!attrs.lastname) return 'last name is empty';
    if(!attrs.email) return 'email is empty';

}
```

Unfortunately, this method would trigger a first name error each time any of the fields were blurred and only an error message next to the first name field would be presented.

One potential solution to the problem is to validate all fields and return all of the errors:

```javascript
validate: function(attrs) {
  var errors = {};

  if (!attrs.firstname) errors.firstname = 'first name is empty';
  if (!attrs.lastname) errors.lastname = 'last name is empty';
  if (!attrs.email) errors.email = 'email is empty';

  if (!_.isEmpty(errors)) return errors;
}
```

This can be adapted into a complete solution that defines a Field model for each input in our form and works within the parameters of our use case as follows:

```javascript

$(function($) {

  var User = Backbone.Model.extend({
    validate: function(attrs) {
      var errors = this.errors = {};

      if (!attrs.firstname) errors.firstname = 'firstname is required';
      if (!attrs.lastname) errors.lastname = 'lastname is required';
      if (!attrs.email) errors.email = 'email is required';

      if (!_.isEmpty(errors)) return errors;
    }
  });

  var Field = Backbone.View.extend({
    events: {blur: 'validate'},
    initialize: function() {
      this.name = this.$el.attr('name');
      this.$msg = $('[data-msg=' + this.name + ']');
    },
    validate: function() {
      this.model.set(this.name, this.$el.val());
      this.$msg.text(this.model.errors[this.name] || '');
    }
  });

  var user = new User;

  $('input').each(function() {
    new Field({el: this, model: user});
  });

});

```


This works great as the solution checks the validation for each attribute individually and sets the message for the correct blurred field. A [demo](http://jsbin.com/afetez/2/edit) of the above by [@braddunbar](http://github.com/braddunbar) is also available.

Unfortunately, this solution does perform validation on all fields every time, even though we are only displaying errors for the field that has changed. If we have multiple client-side validation methods, we may not want to have to call each validation method on every attribute every time, so this solution might not be ideal for everyone.

A potentially better alternative to the above is to use [@gfranko](http://github.com/@franko)'s [Backbone.validateAll](https://github.com/gfranko/Backbone.validateAll) plugin, specifically created to validate specific Model properties (or form fields) without worrying about the validation of any other Model properties (or form fields).

Here is how we would setup a partial User Model and validate method using this plugin for our use case:


```javascript

// Create a new User Model
var User = Backbone.Model.extend({

      // RegEx Patterns
      patterns: {

          specialCharacters: '[^a-zA-Z 0-9]+',

          digits: '[0-9]',

          email: '^[a-zA-Z0-9._-]+@[a-zA-Z0-9][a-zA-Z0-9.-]*[.]{1}[a-zA-Z]{2,6}$'
      },

    // Validators
      validators: {

          minLength: function(value, minLength) {
            return value.length >= minLength;

          },

          maxLength: function(value, maxLength) {
            return value.length <= maxLength;

          },

          isEmail: function(value) {
            return User.prototype.validators.pattern(value, User.prototype.patterns.email);

          },

          hasSpecialCharacter: function(value) {
            return User.prototype.validators.pattern(value, User.prototype.patterns.specialCharacters);

          },
         ...

    // We can determine which properties are getting validated by
    // checking to see if properties are equal to null

        validate: function(attrs) {

          var errors = this.errors = {};

          if(attrs.firstname != null) {
              if (!attrs.firstname) {
                  errors.firstname = 'firstname is required';
                  console.log('first name isEmpty validation called');
              }

              else if(!this.validators.minLength(attrs.firstname, 2))
                errors.firstname = 'firstname is too short';
              else if(!this.validators.maxLength(attrs.firstname, 15))
                errors.firstname = 'firstname is too large';
              else if(this.validators.hasSpecialCharacter(attrs.firstname)) errors.firstname = 'firstname cannot contain special characters';
          }

          if(attrs.lastname != null) {

              if (!attrs.lastname) {
                  errors.lastname = 'lastname is required';
                  console.log('last name isEmpty validation called');
              }

              else if(!this.validators.minLength(attrs.lastname, 2))
                errors.lastname = 'lastname is too short';
              else if(!this.validators.maxLength(attrs.lastname, 15))
                errors.lastname = 'lastname is too large';
              else if(this.validators.hasSpecialCharacter(attrs.lastname)) errors.lastname = 'lastname cannot contain special characters';

          }
```


This allows the logic inside of our validate methods to determine which form fields are currently being set/validated, and ignore the model properties that are not being set.

It's fairly straight-forward to use as well. We can simply define a new Model instance and then set the data on our model using the `validateAll` option to use the behavior defined by the plugin:

```javascript
var user = new User();
user.set({ 'firstname': 'Greg' }, {validate: true, validateAll: false});

```

That's it!

The Backbone.validateAll logic doesn't override the default Backbone logic by default and so it's perfectly capable of being used for scenarios where you might care more about field-validation [performance](http://jsperf.com/backbone-validateall) as well as those where you don't. Both solutions presented in this section should work fine however.


#### Avoiding Conflicts With Multiple Backbone Versions

**Problem**

In instances out of your control, you may have to work around having more than one version of Backbone in the same page. How do you work around this without causing conflicts?

**Solution**

Like most client-side projects, Backbone's code is wrapped in an immediately-invoked function expression:

```javascript
(function(){
  // Backbone.js
}).call(this);
```

Several things happen during this configuration stage. A Backbone `namespace` is created, and multiple versions of Backbone on the same page are supported through the noConflict mode:

```javascript
var root = this;
var previousBackbone = root.Backbone;

Backbone.noConflict = function() {
  root.Backbone = previousBackbone;
  return this;
};
```

Multiple versions of Backbone can be used on the same page by calling `noConflict` like this:

```javascript
var Backbone19 = Backbone.noConflict();
// Backbone19 refers to the most recently loaded version,
// and `window.Backbone` will be restored to the previously
// loaded version
```


#### Building Model And View Hierarchies

**Problem**

How does inheritence work with Backbone? How can I share code between similar models and views? How can I call methods that have been overridden?

**Solution**

For its inheritance, Backbone internally uses an `inherits` function inspired by `goog.inherits`, Google's implementation from the Closure Library. It's basically a function to correctly setup the prototype chain.

```javascript
 var inherits = function(parent, protoProps, staticProps) {
      ...
```

The only major difference here is that Backbone's API accepts two objects containing `instance` and `static` methods.

Following on from this, for inheritance purposes all of Backbone's objects contain an `extend` method as follows:

```javascript
Model.extend = Collection.extend = Router.extend = View.extend = extend;
```

Most development with Backbone is based around inheriting from these objects, and they're designed to mimic a classical object-oriented implementation.

The above isn't quite the same as ECMAScript 5's `Object.create`, as it's actually copying properties (methods and values) from one object to another. As this isn't enough to support Backbone's inheritance and class model, the following steps are performed:

* The instance methods are checked to see if there's a constructor property. If so, the class's constructor is used, otherwise the parent's constructor is used (i.e., Backbone.Model)
* Underscore's extend method is called to add the parent class's methods to the new child class
* The `prototype` property of a blank constructor function is assigned with the parent's prototype, and a new instance of this is set to the child's `prototype` property
* Underscore's extend method is called twice to add the static and instance methods to the child class
* The child's prototype's constructor and a `__super__` property are assigned
* This pattern is also used for classes in CoffeeScript, so Backbone classes are compatible with CoffeeScript classes.

`extend` can be used for a great deal more and developers who are fans of mixins will like that it can be used for this too. You can define functionality on any custom object, and then quite literally copy & paste all of the methods and attributes from that object to a Backbone one:

For example:

```javascript
var MyMixin = {
  foo: 'bar',
  sayFoo: function(){alert(this.foo);}
};

var MyView = Backbone.View.extend({
 // ...
});

_.extend(MyView.prototype, MyMixin);

var myView = new MyView();
myView.sayFoo(); //=> 'bar'
```

We can take this further and also apply it to View inheritance. The following is an example of how to extend one View using another:

```javascript
var Panel = Backbone.View.extend({
});

var PanelAdvanced = Panel.extend({
});
```

**Calling Overridden Methods**

However, if you have an `initialize()` method in Panel, then it won't be called if you also have an `initialize()` method in PanelAdvanced, so you would have to call Panel's initialize method explicitly:

```javascript
var Panel = Backbone.View.extend({
  initialize: function(options){
    console.log('Panel initialized');
    this.foo = 'bar';
  }
});

var PanelAdvanced = Panel.extend({
  initialize: function(options){
    Panel.prototype.initialize.call(this, [options]);
    console.log('PanelAdvanced initialized');
    console.log(this.foo); // Log: bar
  }
});

// We can also inherit PanelAdvaned if needed
var PanelAdvancedExtra = PanelAdvanced.extend({
  initialize: function(options){
    PanelAdvanced.prototype.initialize.call(this, [options]);
    console.log('PanelAdvancedExtra initialized');
  }
});

new Panel();
new PanelAdvanced();
new PanelAdvancedExtra();
```

This isn't the most elegant of solutions because if you have a lot of Views that inherit from Panel, then you'll have to remember to call Panel's initialize from all of them.

It's worth noting that if Panel doesn't have an initialize method now but you choose to add it in the future, then you'll need to go to all of the inherited classes in the future and make sure they call Panel's initialize.

So here's an alternative way to define Panel so that your inherited views don't need to call Panel's initialize method:

```javascript
var Panel = function (options) {
  // put all of Panel's initialization code here
  console.log('Panel initialized');
  this.foo = 'bar';

  Backbone.View.apply(this, [options]);
};

_.extend(Panel.prototype, Backbone.View.prototype, {
  // put all of Panel's methods here. For example:
  sayHi: function () {
    console.log('hello from Panel');
  }
});

Panel.extend = Backbone.View.extend;

// other classes then inherit from Panel like this:
var PanelAdvanced = Panel.extend({
  initialize: function (options) {
    console.log('PanelAdvanced initialized');
    console.log(this.foo);
  }
});

var panelAdvanced = new PanelAdvanced(); //Logs: Panel initialized, PanelAdvanced initialized, bar
panelAdvanced.sayHi(); // Logs: hello from Panel
```

When used appropriately, Underscore's `extend` method can save a great deal of time and effort writing redundant code.

(Thanks to [Alex Young](http://dailyjs.com), [Derick Bailey](http://stackoverflow.com/users/93448/derick-bailey) and [JohnnyO](http://stackoverflow.com/users/188740/johnnyo) for the heads up about these tips).

**Backbone-Super**

[Backbone-Super](https://github.com/lukasolson/Backbone-Super) by Lukas Olson adds a *_super* method to *Backbone.Model* using [John Resig's Inheritance script](http://ejohn.org/blog/simple-javascript-inheritance/).
Rather than using Backbone.Model.prototype.set.call as per the Backbone.js documentation, _super can be called instead:

```javascript
// This is how we normally do it
var OldFashionedNote = Backbone.Model.extend({
  set: function(attributes, options) {
    // Call parent's method
    Backbone.Model.prototype.set.call(this, attributes, options);
    // some custom code here
    // ...
  }
});
```

After including this plugin, you can do the same thing with the following syntax:

```javascript
// This is how we can do it after using the Backbone-super plugin
var Note = Backbone.Model.extend({
  set: function(attributes, options) {
    // Call parent's method
    this._super(attributes, options);
    // some custom code here
    // ...
  }
});
```

#### Event Aggregators And Mediators

**Problem** 

How do I channel multiple event sources through a single object?

**Solution**

Using an Event Aggregator. It's common for developers to think of Mediators when faced with this problem, so let's explore what an Event Aggregator is, what the Mediator pattern is and how they differ.

Design patterns often differ only in semantics and intent. That is, the language used to describe the pattern is what sets it apart, more than an implementation of that specific pattern. It often comes down to squares vs rectangles vs polygons. You can create the same end result with all three, given the constraints of a square are still met – or you can use polygons to create an infinitely larger and more complex set of things.

When it comes to the Mediator and Event Aggregator patterns, there are some times where it may look like the patterns are interchangeable due to implementation similarities. However, the semantics and intent of these patterns are very different. And even if the implementations both use some of the same core constructs, I believe there is a distinct difference between them. I also believe they should not be interchanged or confused in communication because of the differences.

#####Event Aggregator

The core idea of the Event Aggregator, according to Martin Fowler, is to channel multiple event sources through a single object so that other objects needing to subscribe to the events don’t need to know about every event source.

######Backbone’s Event Aggregator

The easiest event aggregator to show is that of Backbone.js – it’s built in to the Backbone object directly.

```javascript
var View1 = Backbone.View.extend({
  // ...

  events: {
    "click .foo": "doIt"
  },

  doIt: function(){
    // trigger an event through the event aggregator
    Backbone.trigger("some:event");
  }
});

var View2 = Backbone.View.extend({
  // ...

  initialize: function(){
    // subscribe to the event aggregator's event
    Backbone.on("some:event", this.doStuff, this);
  },

  doStuff: function(){
    // ...
  }
})
```

In this example, the first view is triggering an event when a DOM element is clicked. The event is triggered through Backbone’s built-in event aggregator – the Backbone object. Of course, it’s trivial to create your own event aggregator in Backbone, and there are some key things that we need to keep in mind when using an event aggregator, to keep our code simple.

#######jQuery’s Event Aggregator

Did you know that jQuery has a built-in event aggregator? They don’t call it this, but it’s in there and it’s scoped to DOM events. It also happens to look like Backbone’s event aggregator:

```
$("#mainArticle").on("click", function(e){

  // handle the event that any element underneath of our #mainArticle element

});
```

This code sets up an event handler function that waits for an unknown number of event sources to trigger a “click” event, and it allows any number of listeners to attach to the events of those event publishers. jQuery just happens to scope this event aggregator to the DOM.

#####Mediator

A Mediator is an object that coordinates interactions (logic and behavior) between multiple objects. It makes decisions on when to call which objects, based on the actions (or in-action) of other objects and input.

**A Mediator For Backbone**

Backbone doesn’t have the idea of a mediator built in to it like a lot of other MV* frameworks do. But that doesn’t mean you can’t write one in 1 line of code:

```javascript
var mediator = {};
```

Yes, of course this is just an object literal in JavaScript. Once again, we’re talking about semantics here. The purpose of the mediator is to control the workflow between objects and we really don’t need anything more than an object literal to do this.

```javascript
var orgChart = {

  addNewEmployee: function(){

    // getEmployeeDetail provides a view that users interact with
    var employeeDetail = this.getEmployeeDetail();

    // when the employee detail is complete, the mediator (the 'orgchart' object)
    // decides what should happen next
    employeeDetail.on("complete", function(employee){

      // set up additional objects that have additional events, which are used
      // by the mediator to do additional things
      var managerSelector = this.selectManager(employee);
      managerSelector.on("save", function(employee){
        employee.save();
      });

    });
  },

  // ...
}
```

This example shows a very basic implementation of a mediator object with Backbone based objects that can trigger and subscribe to events. I’ve often referred to this type of object as a “workflow” object in the past, but the truth is that it is a mediator. It is an object that handles the workflow between many other objects, aggregating the responsibility of that workflow knowledge in to a single object. The result is workflow that is easier to understand and maintain.

#####Similarities And Differences

There are, without a doubt, similarities between the event aggregator and mediator examples that I’ve shown here. The similarities boil down to two primary items: events and third-party objects. These differences are superficial at best, though. When we dig in to the intent of the pattern and see that the implementations can be dramatically different, the nature of the patterns become more apparent.

######Events

Both the event aggregator and mediator use events, in the above examples. An event aggregator obviously deals with events – it’s in the name after all. The mediator only uses events because it makes life easy when dealing with Backbone, though. There is nothing that says a mediator must be built with events. You can build a mediator with callback methods, by handing the mediator reference to the child object, or by any of a number of other means.

The difference, then, is why these two patterns are both using events. The event aggregator, as a pattern, is designed to deal with events. The mediator, though, only uses them because it’s convenient.

######Third-Party Objects

Both the event aggregator and mediator, by design, use a third-party object to facilitate things. The event aggregator itself is a third-party to the event publisher and the event subscriber. It acts as a central hub for events to pass through. The mediator is also a third party to other objects, though. So where is the difference? Why don’t we call an event aggregator a mediator? The answer largely comes down to where the application logic and workflow is coded.

In the case of an event aggregator, the third party object is there only to facilitate the pass-through of events from an unknown number of sources to an unknown number of handlers. All workflow and business logic that needs to be kicked off is put directly in to the the object that triggers the events and the objects that handle the events.

In the case of the mediator, though, the business logic and workflow is aggregated in to the mediator itself. The mediator decides when an object should have it’s methods called and attributes updated based on factors that the mediator knows about. It encapsulates the workflow and process, coordinating multiple objects to produce the desired system behaviour. The individual objects involved in this workflow each know how to perform their own task. But it’s the mediator that tells the objects when to perform the tasks by making decisions at a higher level than the individual objects.

An event aggregator facilitates a “fire and forget” model of communication. The object triggering the event doesn’t care if there are any subscribers. It just fires the event and moves on. A mediator, though, might use events to make decisions, but it is definitely not “fire and forget”. A mediator pays attention to a known set of input or activities so that it can facilitate and coordinate additional behavior with a known set of actors (objects).

#####Relationships: When To Use Which

Understanding the similarities and differences between an event aggregator and mediator is important for semantic reasons. It’s equally as important to understand when to use which pattern, though. The basic semantics and intent of the patterns does inform the question of when, but actual experience in using the patterns will help you understand the more subtle points and nuanced decisions that have to be made.

######Event Aggregator Use

In general, an event aggregator is uses when you either have too many objects to listen to directly, or you have objects that are unrelated entirely.

When two objects have a direct relationship already – say, a parent view and child view – then there might be little benefit in using an event aggregator. Have the child view trigger an event and the parent view can handle the event. This is most commonly seen in Backbone’s Collection and Model, where all Model events are bubbled up to and through it’s parent Collection. A Collection often uses model events to modify the state of itself or other models. Handling “selected” items in a collection is a good example of this.

jQuery’s on method as an event aggregator is a great example of too many objects to listen to. If you have 10, 20 or 200 DOM elements that can trigger a “click” event, it might be a bad idea to set up a listener on all of them individually. This could quickly deteriorate performance of the application and user experience. Instead, using jQuery’s on method allows us to aggregate all of the events and reduce the overhead of 10, 20, or 200 event handlers down to 1.

Indirect relationships are also a great time to use event aggregators. In Backbone applications, it is very common to have multiple view objects that need to communicate, but have no direct relationship. For example, a menu system might have a view that handles the menu item clicks. But we don’t want the menu to be directly tied to the content views that show all of the details and information when a menu item is clicked. Having the content and menu coupled together would make the code very difficult to maintain, in the long run. Instead, we can use an event aggregator to trigger “menu:click:foo” events, and have a “foo” object handle the click event to show it’s content on the screen.

######Mediator Use

A mediator is best applied when two or more objects have an indirect working relationship, and business logic or workflow needs to dictate the interactions and coordination of these objects.

A wizard interface is a good example of this, as shown with the “orgChart” example, above. There are multiple views that facilitate the entire workflow of the wizard. Rather than tightly coupling the view together by having them reference each other directly, we can decouple them and more explicitly model the workflow between them by introducing a mediator.

The mediator extracts the workflow from the implementation details and creates a more natural abstraction at a higher level, showing us at a much faster glance what that workflow is. We no longer have to dig in to the details of each view in the workflow, to see what the workflow actually is.

#####Event Aggregator And Mediator Together

The crux of the difference between an event aggregator and a mediator, and why these pattern names should not be interchanged with each other, is illustrated best by showing how they can be used together. The menu example for an event aggregator is the perfect place to introduce a mediator as well.

Clicking a menu item may trigger a series of changes throughout an application. Some of these changes will be independent of others, and using an event aggregator for this makes sense. Some of these changes may be internally related to each other, though, and may use a mediator to enact those changes. A mediator, then, could be set up to listen to the event aggregator. It could run it’s logic and process to facilitate and coordinate many objects that are related to each other, but unrelated to the original event source.

```javascript
var MenuItem = Backbone.View.extend({

  events: {
    "click .thatThing": "clickedIt"
  },

  clickedIt: function(e){
    e.preventDefault();

    // assume this triggers "menu:click:foo"
    Backbone.trigger("menu:click:" + this.model.get("name"));
  }

});

// ... somewhere else in the app

var MyWorkflow = function(){
  Backbone.on("menu:click:foo", this.doStuff, this);
};

MyWorkflow.prototype.doStuff = function(){
  // instantiate multiple objects here.
  // set up event handlers for those objects.
  // coordinate all of the objects in to a meaningful workflow.
};
```

In this example, when the MenuItem with the right model is clicked, the `“menu:click:foo”` event will be triggered. An instance of the “MyWorkflow” object, assuming one is already instantiated, will handle this specific event and will coordinate all of the objects that it knows about, to create the desired user experience and workflow.

An event aggregator and a mediator have been combined to create a much more meaningful experience in both the code and the application itself. We now have a clean separation between the menu and the workflow through an event aggregator. And we are still keeping the workflow itself clean and maintainable through the use of a mediator.

#####Pattern Language: Semantics

There is one overriding point to make in all of this discussion: semantics. Communicating intent and semantics through the use of named patterns is only viable and only valid when all parties in a communication medium understand the language in the same way.

If I say “apple”, what am I talking about? Am I talking about a fruit? Or am I talking about a technology and consumer products company? As Sharon Cichelli says: “semantics will continue to be important, until we learn how to communicate in something other than language”.

