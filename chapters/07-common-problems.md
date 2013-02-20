# Common Problems & Solutions

In this section, we will review a number of common problems developers often experience once they've started to work on relatively non-trivial projects using Backbone.js, as well as present potential solutions.

Perhaps the most frequent of these questions surround how to do more with Views. If you are interested in discovering how to work with nested Views, learn about view disposal and inheritance, this section will hopefully have you covered.


#### View Nesting

**Problem**

What is the best approach for rendering and appending Subviews in Backbone.js?

**Solution**

Since web pages are composed of nested elements and Backbone views correspond to elements within the page, nesting views is an intuitive approach to managing a hierarchy of elements. As a beginner, one might try writing a very simple setup with subviews (e.g inner views) as follows:

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

Here we are creating subviews in the parent view's `initialize()` method and rendering the subviews in the parent's `render()` method. The elements managed by the subviews exist in the parent's template and the `View.setElement()` method is used to re-assign the element associated with each subview. `setElement()` changes a view's element, including re-delegating event handlers by removing them from the old element and binding them to the new element. Note that `setElement()` returns the view, allowing us to chain the call to `render()`.

This works and has some positive qualities: you don't need to worry about maintaining the order of your DOM elements when appending, views are initialized early, and the render() method doesn't need to take on too many responsibilities at once. Unfortunately, downsides are that you can't set the `tagName` property of subviews and events need to be re-delegated.

An alternative approach which doesn't suffer from the re-delegation problem can be written as follows:

```javascript

initialize : function () {

},

render : function () {

    this.$el.empty();

    this.innerView1 = new Subview({options});
    this.innerView2 = new Subview({options});


    this.$el.append(this.innerView1.render().el, this.innerView2.render().el);
}

```

In this version, we are creating new instances of the subviews, rendering them, and appending their elements to an empty parent element each time the parent is rendered. Each subview is responsible for creating and rendering its own element, so we don't need a template containing empty placeholders and the issue with `tagName`s is solved as they are defined by the subviews once again.

Yet another variation which moves logic into an `onRender` event, could be written with only a few subtle changes:

```javascript

initialize : function () {
    this.on('render', this.onRender);
},

render : function () {

    this.$el.html(this.template);

    // more logic

    return this.trigger('render');
},

onRender : function () {
    this.innerView1 = new Subview();
    this.innerView2 = new Subview();
    this.innerView1.setElement('.some-element').render();
    this.innerView2.setElement('.some-element').render();
}
```

If you find yourself nesting views in your application, there are more optimal approaches possible for initializing, rendering, and appending your subviews. One such solution could be written:

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

A second potential solution is this, which may appear cleaner but in reality has a tendency to affect performance:

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

A sample implementation containing a single child view:

```javascript

var OuterView = Backbone.View.extend({
    initialize: function() {
        this.children = {};
        var child = new Backbone.View();
        this.children[child.cid] = child;
    },

    render: function() {
        this.$el.html('<div data-view-cid="' + this.child.cid + '"></div>');        
        _.each(this.children, function(view, cid) {
            this.$('[data-view-cid="' + cid + '"]').replaceWith(view.el);
        }, this);
    }
};

```

Generally speaking, more developers opt for the first solution as:

* The majority of their views may already rely on being in the DOM in their render() method
* When the OuterView is re-rendered, views don't have to be re-initialized where re-initialization has the potential to cause memory leaks and issues with existing bindings

The Backbone extensions [Marionette](#marionette) and [Thorax](#thorax) provide logic for nesting views, and rendering collections where each item has an associated view. Marionette provides APIs in JavaScript while Thorax provides APIs via Handlebars template helpers. We will examine both of these in an upcoming chapter.

(Thanks to [Lukas](http://stackoverflow.com/users/299189/lukas)  and [Ian Taylor](http://stackoverflow.com/users/154765/ian-storm-taylor) for these tips).

#### What is the best way to manage models in nested Views?

In order to reach attributes on related models in a nested setup, the models involved need to have some prior knowledge about which models this refers to. Backbone.js doesn't implicitly handle relations or nesting, meaning it's up to us to ensure models have a knowledge of each other.

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

This allows you to reach the parent model in any child model function by calling this.parent.

When you have a need to nest Backbone.js views, you might find it easier to let each view represent a single HTML tag using the tagName option of the View. This may be written as:

```javascript
ViewA = Backbone.View.extend({

    tagName: 'div',
    id: 'new',

    initialize: function(){
       this.viewB = new ViewB();
       this.viewB.parentView = this;
       $(this.el).append(this.viewB.el);
    }
});

ViewB = Backbone.View.extend({

    tagName: 'h1',

    render: function(){
        $(this.el).html('Header text'); // or use this.options.headerText or equivalent
    },

    funcB1: function(){
        this.model.parent.doSomethingOnParent();
        this.model.parent.modelC.doSomethingOnSibling();
        $(this.parentView.el).shakeViolently();
    }

});
```

Then in your application initialization code , you would initiate ViewA and place its element inside the body element.

An alternative approach is to use an extension called [Backbone-Forms](https://github.com/powmedia/backbone-forms). Using a similar schema to what we wrote earlier, nesting could be achieved as follows:

```javascript
var ModelB = Backbone.Model.extend({
    schema: {
        attributeB1: 'Text',
        attributeB2: 'Text'
    }
});

var ModelC = Backbone.Model.extend({
    schema: {
        attributeC: 'Text',
    }
});

var ModelA = Backbone.Model.extend({
    schema: {
        attributeA1: 'Text',
        attributeA2: 'Text',
        refToModelB: { type: 'NestedModel', model: ModelB, template: 'templateB' },
        refToModelC: { type: 'NestedModel', model: ModelC, template: 'templateC' }
    }
});
```

There is more information about this technique available on [GitHub](https://github.com/powmedia/backbone-forms#customising-templates).

(Thanks to [Jens Alm](http://stackoverflow.com/users/100952/jens-alm) and [Artem Oboturov](http://stackoverflow.com/users/801466/artem-oboturov) for these tips)

#### Rendering Parent View from Child

**Problem**

How would one render a Parent View from one of its Children?

**Solution**

If you say, have a view which contains another view (e.g., a main view containing a modal view) and  would like to render or re-render the parent view from the child, this is extremely straight-forward.

In such a scenario, you would most likely want to execute the rendering when a particular event has occurred. For the sake of example, let us call this event 'somethingHappened'. The parent view can bind notifications on the child view to know when the event has occurred. It can then render itself.

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


#### Triggering updates in other Views

**Problem**

How can a View trigger an update in an unrelated View?

**Solution**

The Mediator pattern is an excellent option for implementing a solution to this problem.

Without going into too much detail about the pattern, a mediator can effectively be used as an event manager that lets you to subscribe to and publish events. So an ApplicationViewA could subscribe to an event, (e.g., 'selected') and then the ApplicationViewB would publish the 'selected' event.

The reason I like this is it allows you to send events between views, without the views being directly bound together.

For Example:

```javascript

// See http://addyosmani.com/largescalejavascript/#mediatorpattern
// for an implementation or alternatively for a more thorough one
// http://thejacklawson.com/Mediator.js/

var mediator = new Mediator();

var ApplicationViewB = Backbone.View.extend({
    toggle_select: function() {
        ...
        mediator.publish('selected', any, data, you, want);
        return this;
    }
});

var ApplicationViewA = Backbone.View.extend({
    initialize: function() {
        mediator.subscribe('selected', this.delete_selected)
    },

    delete_selected: function(any, data, you, want) {
        ... do something ...
    },
});
```

This way your ApplicationViewA doesn't care if it is an ApplicationViewB or FooView that publishes the 'selected' event, only that the event occurred. As a result, you may find it a maintainable way to manage events between parts of your application, not just views.

(Thanks to [John McKim](http://stackoverflow.com/users/937577/john-mckim) for this tip and for referencing my Large Scale JavaScript Patterns article).


#### Disposing Views cleanly

**Problem**

How do you cleanly dispose Views to avoid memory leaks?

**Solution**

As your application grows, keeping live views around which aren't being used can quickly become difficult to maintain. Instead, you may find it more optimal to destroy views that are no longer required and simply create new ones as the need arises.

A solution to help with this is to create a BaseView from which the rest of your views inherit from. The idea here is that your view will maintain a reference to all of the events to which its subscribed to so that when it is time to dispose of a view, all of those bindings will be automatically unbound.

Here is a sample implementation of this:

```javascript
var BaseView = function (options) {

    this.bindings = [];
    Backbone.View.apply(this, [options]);
};

_.extend(BaseView.prototype, Backbone.View.prototype, {

    bindTo: function (model, ev, callback) {

        model.bind(ev, callback, this);
        this.bindings.push({ model: model, ev: ev, callback: callback });
    },

    unbindFromAll: function () {
        _.each(this.bindings, function (binding) {
            binding.model.unbind(binding.ev, binding.callback);
        });
        this.bindings = [];
    },

    dispose: function () {
        this.unbindFromAll(); // this will unbind all events that this view has bound to
        this.unbind(); // this will unbind all listeners to events from this view. This is probably not necessary because this view will be garbage collected.
        this.remove(); // uses the default Backbone.View.remove() method which removes this.el from the DOM and removes DOM events.
    }

});
```

```javascript
BaseView.extend = Backbone.View.extend;
```

Then, whenever a view has the need to bind to an event on a model or a collection, you would use the bindTo method. e.g:

```
var SampleView = BaseView.extend({

    initialize: function(){
        this.bindTo(this.model, 'change', this.render);
        this.bindTo(this.collection, 'reset', this.doSomething);
    }
});
```

When you remove a view, simply call the dispose() method which will clean everything up for you automatically:

```javascript
var sampleView = new SampleView({model: some_model, collection: some_collection});
sampleView.dispose();
```

(Thanks to [JohnnyO](http://stackoverflow.com/users/188740/johnnyo) for this tip).

#### Disposing View hierarchies

**Problem**

In the last question, we looked at how to effectively dispose views to decrease memory usage.

Where your application is setup with multiple Parent and Child Views, it is also common to desire removing any DOM elements associated with such views as well as unbinding any event handlers tied to child elements when you no longer require them.

**Solution**

The solution in the last question should be enough to handle this use case, but if you require a more explicit example that handles children, we can see one below:

```javascript
Backbone.View.prototype.close = function() {
    if (this.onClose) {
        this.onClose();
    }
    this.remove();
    this.unbind();
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

#### Rendering View hierarchies

**Problem**

Let us say you have a Collection, where each item in the Collection could itself be a Collection. You can render each item in the Collection, and indeed can render any items which themselves are Collections. The problem you might have is how to render this structure where the HTML reflects the hierarchical nature of the data structure.

**Solution**

The most straight-forward way to approach this problem is to use a framework like Derick Baileys [Backbone.Marionette](https://github.com/marionettejs/backbone.marionette). In this framework is a type of view called a CompositeView.

The basic idea of a CompositeView is that it can render a model and a collection within the same view.

It can render a single model with a template. It can also take a collection from that model and for each model in that collection, render a view. By default it uses the same composite view type that you've defined, to render each of the models in the collection. All you have to do is tell the view instance where the collection is, via the initialize method, and you'll get a recursive hierarchy rendered.

There is a working demo of this in action available [online](http://jsfiddle.net/derickbailey/AdWjU/).

And you can get the source code and documentation for [Marionette](https://github.com/marionettejs/backbone.marionette) too.

#### Better Model Property Validation

**Problem**

As we learned earlier in the book, the `validate` method on a Model is called by `set` (when the validate option is set) and `save`, and is passed the model attributes updated with the values passed to these methods.

By default, when we define a custom `validate` method, Backbone passes all of a Model's attributes through this validation each time, regardless of which model attributes are being set.

This means that it can be a challenge to determine which specific fields are being set or validated without being concerned about the others that aren't being set at the same time.

**Solution**

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


#### Multiple Backbone versions

**Problem**

In some instances it may be necessary to use multiple versions of Backbone in the same project.

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

Multiple versions of Backbone can be used on the same page by calling noConflict like this:

```javascript
var Backbone19 = Backbone.noConflict();
// Backbone19 refers to the most recently loaded version,
// and `window.Backbone` will be restored to the previously
// loaded version
```

This initial configuration code also supports CommonJS modules so Backbone can be used in Node projects:

```javascript
var Backbone;
if (typeof exports !== 'undefined') {
  Backbone = exports;
} else {
  Backbone = root.Backbone = {};
}
```

#### Building Model and View hierarchies

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

If this sounds familiar, it's because `extend` is an Underscore.js utility, although Backbone itself does a lot more with this. See below for Underscore's `extend`:

```javascript
each(slice.call(arguments, 1), function(source) {
  for (var prop in source) {
    obj[prop] = source[prop];
  }
});
return obj;
```

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
