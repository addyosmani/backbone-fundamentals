# Exercise 1: Todos - Your First Backbone.js App

Now that we've covered fundamentals, let's write our first Backbone.js application. We'll build the Backbone Todo List application exhibited on [TodoMVC.com](http://todomvc.com). Building a Todo List is a great way to learn Backbone’s conventions. It's a relatively simple application, yet technical challenges surrounding binding, persisting model data, routing, and template rendering provide opportunities to illustrate some core Backbone features.

![](img/todos_a.png)

Let's consider the application's architecture at a high level. We'll need:

* a `Todo` model to describe individual todo items
* a `TodoList` collection to store and persist todos
* a way of creating todos
* a way to display a listing of todos
* a way to edit existing todos
* a way to mark a todo as completed
* a way to delete todos
* a way to filter the items that have been completed or are remaining

Essentially, these features are classic [CRUD](http://en.wikipedia.org/wiki/Create,_read,_update_and_delete) methods. Let's get started!

## Static HTML

We'll place all of our HTML in a single file named index.html.

#### Header and Scripts

First, we'll set up the header and the basic application dependencies: [jQuery](http://jquery.com), [Underscore](http://underscorejs.org), and Backbone.js. Later, we'll add more functionality, which will mean coming back to this page and loading some more dependencies and files. 

```html
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
  <title>Backbone.js • TodoMVC</title>
  <link rel="stylesheet" href="assets/index.css">
</head>
<body>
  <script type="text/template" id="item-template"></script>
  <script src="js/lib/jquery.min.js"></script>
  <script src="js/lib/underscore-min.js"></script>
  <script src="js/lib/backbone-min.js"></script>
  <script src="js/models/todo.js"></script>
  <script src="js/collections/todos.js"></script>
  <script src="js/views/todos.js"></script>
  <script src="js/views/app.js"></script>
  <script src="js/app.js"></script>
</body>
</html>
```

In addition to the aforementioned dependencies, note that a few other application-specific files are also loaded. These are organized into folders representing their application responsibilities: models, views, collections, and routers. An app.js file is present to house central initialization code.

Note: If you want to follow along, create a directory structure as demonstrated in index.html:

1. Place the index.html in a top-level directory.
2. Download jQuery, Underscore, and Backbone from their respective web sites and place them under js/lib
3. Create the directories js/models, js/collections, and js/views

You will also need [index.css](https://raw.githubusercontent.com/tastejs/todomvc/gh-pages/examples/backbone/node_modules/todomvc-app-css/index.css), which should live in an assets directory. And remember that you can see a demo of the final application at [TodoMVC.com](http://todomvc.com).

We will be creating the application JavaScript files during the tutorial. Don't worry about the 'text/template' script element - we will replace it soon!

#### Application HTML

Now let's populate the body of index.html. We'll need an `<input>` for creating new todos, and a `<ul id="todo-list" />` for listing the actual todos. (Later, we'll add a footer where we can later insert statistics and links for performing operations such as clearing completed todos, as well as a checkbox to mark all items as complete, but for now let's keep things simple.) We'll add the following markup immediately inside our body tag before the script elements:

```html
  <section id="todoapp">
    <header id="header">
      <h1>todos</h1>
      <input id="new-todo" placeholder="What needs to be done?" autofocus>
    </header>
    <section id="main">
      <ul id="todo-list"></ul>
    </section>
  </section>
  <div id="info">
    <p>Double-click to edit a todo</p>
    <p>Written by <a href="https://github.com/addyosmani">Addy Osmani</a></p>
    <p>Part of <a href="http://todomvc.com">TodoMVC</a></p>
  </div>
```

#### Templates

To complete index.html, we need to add the templates which we will use to dynamically create HTML by injecting model data into their placeholders. One way of including templates in the page is by using custom script tags. These don't get evaluated by the browser, which just interprets them as plain text. Underscore micro-templating can then access the templates, rendering fragments of HTML.

We'll start by filling in the `#item-template` which will be used to display individual todo items.

```html
  <!-- index.html -->

  <script type="text/template" id="item-template">
    <div class="view">
      <input class="toggle" type="checkbox" <%= completed ? 'checked' : '' %>>
      <label><%= title %></label>
      <button class="destroy"></button>
    </div>
    <input class="edit" value="<%= title %>">
  </script>
```

The template tags in the above markup, such as `<%=` and `<%-`, are specific to Underscore.js and are documented on the Underscore site. In your own applications, you have a choice of template libraries, such as Mustache or Handlebars. Use whichever you prefer, Backbone doesn't mind.

Now that we have all the HTML that we will need, we'll start implementing our application by returning to the fundamentals: a Todo model.

## Todo model

Remember, a model represents a single data object. The `Todo` model is remarkably straightforward. First, a todo has two attributes: a `title` stores a todo item's title and a `completed` status indicates if it's complete. These attributes are passed as defaults, as shown below:

```javascript

  // js/models/todo.js

  var app = app || {};

  // Todo Model
  // ----------
  // Our basic **Todo** model has `title` and `completed` attributes.

  app.Todo = Backbone.Model.extend({

    // Default attributes ensure that each todo created has `title` and `completed` keys.
    defaults: {
      title: '',
      completed: false
    }

  });

```

This is pretty straightforward, but it might be worth pausing over the first line:

```javascript
  var app = app || {};
```

This defines the variable `app` to be an empty object (`{}`) if it is not already defined. It takes advantage of the way that Javascript interprets the `||` (OR) operator. If the left hand side is TRUE, that will be returned (i.e. if `app` is already defined, `app` will continue to refer to what `app` was referring to before...); otherwise (if `app` is not already defined, and therefore if the left hand side of the `||` operator is false), the right hand side will be returned, in which case `app` will be set to `{}`, or an empty object. Basically, this line checks to see if the namespace `app` already exists, and if it doesn't, creates it.

## Todo collection

Remember the Todo model represents a *single* todo. Any todo list needs to contain multiple todos, which we will store in a collection. A `TodoList` collection is used to group our models. The collection will eventually use the LocalStorage adapter to override Backbone's default `sync()` operation with one that will persist our Todo records to HTML5 Local Storage. Through local storage, they will be saved between page requests. But for now we'll just set up the collection:

```javascript

  // js/collections/todos.js

  var app = app || {};

  // Todo Collection
  // ---------------

  var TodoList = Backbone.Collection.extend({

    // Reference to this collection's model.
    model: app.Todo

  });

  // Create our global collection of **Todos**.
  app.Todos = new TodoList();

```

This could not be simpler: it specifies that the model collected by this collection is `Todo`, in the namespace `app`.

The last line creates a new collection and stores it in the variable `Todos`. Pay attention to this -- it will be referred to elsewhere in the code. If it helps, think of it as the main data structure for the entire app. When we make a `create` call later (as we shall see) it will be `app.Todos` that is being called.

## Application View

Let's examine the core application logic which resides in the views. Each view supports functionality such as edit-in-place, and therefore contains a fair amount of logic. To help organize this logic, we'll use the element controller pattern. The element controller pattern consists of two views: one controls a collection of items while the other deals with each individual item.

In our case, an `AppView` will handle the creation of new todos and rendering of the initial todo list. Instances of `TodoView` will be associated with each individual Todo record. Todo instances can handle editing, updating, and destroying their associated todo.

To keep things short and simple, we won't be implementing all of the application's features in this tutorial, we'll just cover enough to get you started. Even so, there is a lot for us to cover in `AppView`, so we'll split our discussion into two sections.

```javascript

  // js/views/app.js

  var app = app || {};

  // The Application
  // ---------------

  // Our overall **AppView** is the top-level piece of UI.
  app.AppView = Backbone.View.extend({

    // Instead of generating a new element, bind to the existing skeleton of
    // the App already present in the HTML.
    el: '#todoapp',

    // At initialization we bind to the relevant events on the `Todos`
    // collection, when items are added or changed.
    initialize: function() {
      this.$input = this.$('#new-todo');

      this.listenTo(app.Todos, 'add', this.addOne);
    },

    // Add a single todo item to the list by creating a view for it, and
    // appending its element to the `<ul>`.
    addOne: function( todo ) {
      var view = new app.TodoView({ model: todo });
      $('#todo-list').append( view.render().el );
    }

  });

```

Recall from the earlier section [What is `el`?](#what-is-el), that `el` refers to an element in the DOM, and is a necessary component of all views. It can be created and added to the DOM, or it can refer to an already existing element. In our case, we're referring to the `id` (`todoapp`) of the `section` element created in the HTML in our first step.

Next up is the `initialize()` method. For now we only set one jQuery variable, `this.$input`, which we set to the `id` of the `input` field we created above in the HTML: `<input id="new-todo" placeholder="What needs to be done?" autofocus>`. It's just a convenient way for referencing it later when we come to handling the adding of todos.

There is also a `listenTo()` binding. It can be a bit confusing here to see where the events are coming from. When an `add` event is fired the `addOne()` method is called and passed the new model. 

What is firing the `add` event? Here's our binding again:

```javascript
  this.listenTo(app.Todos, 'add', this.addOne);
```

As we put it in the [Events](#events) section, 

> While `on()` and `off()` add callbacks directly to an observed object, `listenTo()` tells an object to listen for events on another object

In our binding, `listenTo()` tells one object (`this`) to listen for events on another object (`app.Todos`). This, recall, is the Collection which we earlier said would be the main focus of the entire app. So we're listening for an `add` event on the Todos Collection. 

But wait! If we go back and look at our Collection, there's no `add` event there. Where is this coming from? Well, if we look at the Backbone.js [manual](http://backbonejs.org/#Collection-add), we'll see that the `add` event is baked in to Backbone.js Collections. So this listener is saying: when a model is added to the collection, call `this.addOne`. (We'll see later what causes the `add` event to be fired.)

Lastly, the `this.addOne()` method is pretty simple. First, it creates an instance of the TodoView (which we have not yet coded), then it renders this instance and adds it to the end of the todo list.

Now, let's add some more logic to complete our AppView! Recall that we should [distinguish between two types of events](#events-and-views) when we are dealing with Views (as we are in this section). One is a DOM event, the other is an event triggered by the Event API. It is this second kind we were dealing with in the previous section. In this section we're going to look at the first kind.

```javascript

  // js/views/app.js

  var app = app || {};

  // The Application
  // ---------------

  // Our overall **AppView** is the top-level piece of UI.
  app.AppView = Backbone.View.extend({

    // Instead of generating a new element, bind to the existing skeleton of
    // the App already present in the HTML.
    el: '#todoapp',

    // New
    // Delegated events for creating new items, and clearing completed ones.
    events: {
      'keypress #new-todo': 'createOnEnter'
    },

    // At initialization we bind to the relevant events on the `Todos`
    // collection, when items are added or changed. Kick things off by
    // loading any preexisting todos that might be saved in *localStorage*.
    initialize: function() {
      this.$input = this.$('#new-todo');

      this.listenTo(app.Todos, 'add', this.addOne);
    },

    // Add a single todo item to the list by creating a view for it, and
    // appending its element to the `<ul>`.
    addOne: function( todo ) {
      var view = new app.TodoView({ model: todo });
      $('#todo-list').append( view.render().el );
    },

    // New
    // Generate the attributes for a new Todo item.
    newAttributes: function() {
      return {
        title: this.$input.val().trim(),
        completed: false
      };
    },

    // New
    // If you hit return in the main input field, create new Todo model,
    // persisting it to localStorage.
    createOnEnter: function( event ) {
      if ( event.which !== ENTER_KEY || !this.$input.val().trim() ) {
        return;
      }

      app.Todos.create( this.newAttributes() );
      this.$input.val('');
    },
  });

```

Notice we only have one new piece of functionality: `createOnEnter()`. This is called via a DOM event rather than the Events API. As was said earlier in the section on [Events and Views](#events-and-views), there are two ways two bind DOM events: using a View's `events` property or using `jQuery.on()`. Here we're using the [`events`](http://backbonejs.org/#View-events) hash, which calls the method `createOnEnter()` whenever there's a `keypress` in the `<input>` field.

`createOnEnter()` itself is pretty simple. After checking that the `<input>` text field contains some valid data, it calls the Collection's `create()` method and clears the field. The object passed to `create()` is created by this View's `newAttributes()` method, which simply returns an object literal with two fields (matching our Model's fields): `title` and `completed`. 

Remember the `add` event in the previous section?

```javascript
  this.listenTo(app.Todos, 'add', this.addOne);
```

We asked what actually triggered this `add` event. Well, as the Backbone.js [manual](http://backbonejs.org/#Collection-create) tells us, it's the Collection's `create()` method that triggers the `add` event. So what the code in this View does is: first, add a listener to the input field; second, if the field has some text and the user hits Return, create a new Todo model and add it to the collection; third, create a new view with this model and append it to the list.

## Individual Todo View

Now let’s look at the `TodoView` view. This will be in charge of individual Todo records, making sure the view updates when the todo does. To enable this functionality, we will add event listeners to the view that listen for events on an individual todo's HTML representation. Let's break it down into manageable chunks:

```javascript

  // js/views/todos.js

  var app = app || {};

  // Todo Item View
  // --------------

  // The DOM element for a todo item...
  app.TodoView = Backbone.View.extend({

    //... is a list tag.
    tagName: 'li',
```

Notice here that instead of creating our `el` element by associating it with an already existing element in the DOM (as we did with our AppView), here it's being created by giving it a `tagName`, in this case the list item tag.

```javascript
    // Cache the template function for a single item.
    template: _.template( $('#item-template').html() ),
```

This is simply grabbing the item template we created back in the first section. We're using the Underscore.js `_.template()` method to convert it to a template.

```javascript
    // The DOM events specific to an item.
    events: {
      'dblclick label': 'edit',
      'keypress .edit': 'updateOnEnter',
      'blur .edit': 'close'
    },
```

As with the AppView, this code creates three DOM event bindings with methods in this View. Our events hash includes three callbacks:

* `edit()`: changes the current view into editing mode when a user double-clicks on an existing item in the todo list. This allows them to change the existing value of the item’s title attribute.
* `updateOnEnter()`: checks that the user has hit the return/enter key and executes the close() function.
* `close()`: trims the value of the current text in our `<input/>` field, ensuring that we don’t process it further if it does not contain any text (e.g ‘’). If a valid value has been provided, we save the changes to the current todo model and close editing mode by removing the corresponding CSS class.

```javascript
    // The TodoView listens for changes to its model, re-rendering. Since there's
    // a one-to-one correspondence between a **Todo** and a **TodoView** in this
    // app, we set a direct reference on the model for convenience.
    initialize: function() {
      this.listenTo(this.model, 'change', this.render);
    },
```

In the `initialize()` constructor, we set up a listener that monitors a todo model’s `change` event. As a result, when the todo gets updated, the application will re-render the view and visually reflect its changes. Note that the model passed in the arguments hash by our AppView is automatically available to us as `this.model`.

```javascript
    // Re-renders the titles of the todo item.
    render: function() {
      this.$el.html( this.template( this.model.attributes ) );
      this.$input = this.$('.edit');
      return this;
    },
```

In the `render()` method, we render our Underscore.js `#item-template`, which was previously compiled into this.template using Underscore’s `_.template()` method.  This returns an HTML fragment that replaces the content of the view's element (an li element was implicitly created for us based on the `tagName` property). In other words, the rendered template is now present under `this.el` and can be appended to the todo list in the user interface. `render()` finishes by caching the input element within the instantiated template into `this.$input`.

```javascript
    // Switch this view into `"editing"` mode, displaying the input field.
    edit: function() {
      this.$el.addClass('editing');
      this.$input.focus();
    },
```

Casting our eyes up for a second to our `events` property, we'll see that this `edit()` method is called when a `<label>` element is double-clicked. All this simple method does is add a CSS class to our template and set the focus to its `<input>` text field.

```javascript
    // Close the `"editing"` mode, saving changes to the todo.
    close: function() {
      var value = this.$input.val().trim();

      if ( value ) {
        this.model.save({ title: value });
      }

      this.$el.removeClass('editing');
    },
```

`close()` is called when the element with the class `edit` (which is our `<input>` text field) loses focus. Here we grab the value of the field, and save it using the model's `save()` method. Finally, we remove the `editing` CSS class.

```javascript
    // If you hit `enter`, we're through editing the item.
    updateOnEnter: function( e ) {
      if ( e.which === ENTER_KEY ) {
        this.close();
      }
    }
  });

```

`updateOnEnter()` just calls `close()` when the user hits Enter in the `<input>` text field.

## Startup

So now we have two views: `AppView` and `TodoView`. The former needs to be instantiated on page load so its code gets executed. This can be accomplished through jQuery's `ready()` utility, which will execute a function when the DOM is loaded. Note: The `ENTER_KEY` constant is used in the Todo View above (in the `updateOnEnter()` method).

```javascript

  // js/app.js

  var app = app || {};
  var ENTER_KEY = 13;

  $(function() {

    // Kick things off by creating the **App**.
    new app.AppView();

  });

```

## In action

Let's pause and ensure that the work we've done so far functions as intended.

If you are following along, open `file://*path*/index.html` in your browser and monitor its console. If all is well, you shouldn't see any JavaScript errors. The todo list should be blank as we haven't yet created any todos. 

However, a few things can be tested through the JavaScript console.

In the console, add a new todo item: `app.Todos.create({ title: 'My first Todo item'});` and hit return.

![](img/todos_d.png)

If all is functioning properly, this should log the new todo we've just added to the todos collection. One thing you will see if you look in the console is an error:

> Uncaught Error: A "url" property or function must be specified

This is because for the moment we've ignored anything to do with saving or persisting the todo list. You'll see this if you add a few items and then refresh the page: the list will disappear. The error appears because of the `save()` method in the Model:

```javascript
  this.model.save({ title: value });
```

and (indirectly) in the `createOnEnter()` method in the AppView:

```javascript
  app.Todos.create( this.newAttributes() );
```

This calls the Collection.create() method, which (if we look at the [annotated code](http://backbonejs.org/docs/backbone.html#section-134)), boths `add`s and `save`s a model.

Looking back at the code we entered in the console, `app.Todos.create()` executes a collection method (`Collection.create(attributes, [options])`) which instantiates a new model item of the type passed into the collection definition, in our case `app.Todo`:

```javascript

  // from our js/collections/todos.js

  var TodoList = Backbone.Collection.extend({

      model: app.Todo // the model type used by collection.create() to instantiate new model in the collection
      ...
  )};

```

Run the following in the console to check it out:

```javascript
  var secondTodo = app.Todos.create({ title: 'My second Todo item'});
  secondTodo instanceof app.Todo // returns true
```

Also, we should be able to create a new todo by typing a title and pressing enter.

![](img/todos_b.png)

Note that clicking the "completed" or "destroy" buttons next to the newly-added entry won't do anything yet. 

Excellent, we're making great progress, but what about completing and deleting todos, as well as persisting them? And how about displaying some statistics?

## LocalStorage

Ok, let's get rid of that nasty error:

> Uncaught Error: A "url" property or function must be specified

Remember that this was being triggered by a couple of `save()` methods, without our having specified any resource to save to. 

So, let's go back and add that in. First, in `index.html`, near the bottom, add in a reference to `backbone.localStorage.js` (you can [get it from here](https://github.com/jeromegn/Backbone.localStorage)).

```html
<script src="js/lib/backbone.localStorage.js"></script>
```

Try it again and you'll see we still get that error. This is because we still need to specify in our Collection that we're overriding the usual Backbone.js `sync()` method with local storage. This line goes in `collections\todo.js`:

```javascript
  var TodoList = Backbone.Collection.extend({

    model: app.Todo,

    localStorage: new Backbone.LocalStorage('todos-backbone'),
```

Now go back to the browser, refresh the page, add a todo, and you should see that the error is gone. It's still not working perfectly though. Refresh the page again and you'll see that the contents of the list are wiped. What we want is for them to be persisted through page refreshes.

To retrieve the stored todos on page refresh, we just need to open up `views/app.js` and add one line to the end of the `initialize()` method:

```javascript
  app.Todos.fetch();
```

This simply calls the `fetch()` method on the Collection. Try refreshing again and you'll see the todos you already entered appear as part of the list when it's initialized.

## Ordering

There's just a little bit of boilerplate left before we move on to handling todo completion and deletion. We need to have a means of ordering todos as they're entered. Since this is happening at the level of the collection, we need to add a couple of methods in `collections/todos.js`:

```javascript
  // We keep the Todos in sequential order, despite being saved by unordered
  // GUID in the database. This generates the next order number for new items.
  nextOrder: function() {
    if ( !this.length ) {
      return 1;
    }
    return this.last().get('order') + 1;
  },

  // Todos are sorted by their original insertion order.
  comparator: function( todo ) {
    return todo.get('order');
  } 
```

The `nextOrder()` method implements a sequence generator while the `comparator()` sorts items by their insertion order. But where are these methods called from? Notice that this makes use of an `order` attribute [.....]. We'll need to modify our `newAttributes()` method to take this into account, and this will call our `nextOrder()` method. Open up `views/app.js` and add the `order` attribute:

```javascript
  newAttributes: function() {
    return {
      title: this.$input.val().trim(),
      order: app.Todos.nextOrder(),
      completed: false
    };
  },
```

As for the `comparator()` method, if we turn to the [Backbone.js documentation](http://backbonejs.org/#Collection-comparator), we will see that the `comparator` is a built-in feature of Collections, which (in our case) returns the attribute to sort by - the todo's `order` attribute. 


## Completing & deleting todos

The next part of our tutorial is going to cover completing and deleting todos. These two actions are specific to each Todo item, so we need to add this functionality to the TodoView view (`views/todos.js`). We will do so by adding `togglecompleted()` and `clear()` methods along with corresponding entries in the `events` hash.
First, we want to add an extra event to the DOM events hash:

```javascript
  'click .toggle': 'togglecompleted',
```

This will simply listen for the `.toggle` checkbox to be clicked, and call the `togglecompleted()` method when triggered. So let's add that to the View now:

```javascript
  togglecompleted: function() {
    this.model.toggle();
  },
```

And as you can see, this calls the Model's `toggle()` method. So let's open up `models/todo.js` and add that method (don't forget to add a comma after the `defaults` object):

```javascript
  toggle: function() {
    this.save({
      completed: !this.get('completed')
    });
  }
```

You might have to look twice at:

```javascript
  completed: !this.get('completed')
```

To understand this, we need to consult the [manual entry for the `save` method](http://backbonejs.org/#Model-save): "The attributes hash (as in set) should contain the attributes you'd like to change — keys that aren't mentioned won't be altered". So for a start, we know here we're only saving the `completed` key: we're leaving the `title` key alone. And what are we setting `completed` to? Well, we're negating (with `!`) the current value of `completed` (fetched with `this.get()`). Which is what a `toggle` function should do.

And that's all we need for completing a todo. Try it now - click the checkbox next to a todo item, then refresh the page. It will stay ticked. There is one final cosmetic touch in the book's code, which toggles a `completed` CSS class on the list item. Add this line to the `render()` method in `views/todos.js`, before the `return` statement:

```javascript
  this.$el.toggleClass( 'completed', this.model.get('completed') );
```

Now refresh and tick a todo again, and you'll see a line-through is added to it.

Let's look at the events that occur when we click the checkbox for a todo item:

1. The `togglecompleted()` function is invoked which calls `toggle()` on the todo model.
2. `toggle()` toggles the completed status of the todo and calls `save()` on the model.
3. The save generates a `change` event on the model which is bound to our TodoView's `render()` method. We've added a statement in `render()` which toggles the completed class on the element depending
on the model's completed state. The associated CSS changes the color of the title text and strikes a line through it when the todo is completed.

Now let's build out the functionality for a todo's destroy button. First, add the new event to the DOM events hash in the View (`views/todos.js`):

```javascript
  'click .destroy': 'clear',
```

As you can see, this calls the `clear()` method in the View, so let's create that:

```javascript
  clear: function() {
    this.model.destroy();
  }
```

And this calls the `destroy()` method in the Model, but in contrast to the `toggle()` method we created above, we don't need to create a `destroy()` method, because [it's already built in to the Model](http://backbonejs.org/#Model-destroy). Since this is different from the `change` trigger, we need to add a new listener to the `initialize()` method of the View:

```javascript
  this.listenTo(this.model, 'destroy', this.remove);
```

This waits for a `destroy` event to come from the Model, and when it does, calls the View's `remove()` method, which [is also built in](http://backbonejs.org/#View-remove).

Now let's recap what happens when we click on a todo's destroy button:

1. The `clear()` method is invoked which calls `destroy()` on the todo model.
2. The todo is deleted from local storage and a `destroy` event is triggered.
3. In our update to the TodoView, we bound the model's `destroy` event to the view's inherited `remove()` method. This method deletes the view and automatically removes the associated element from the DOM. Since we used `listenTo()` to bind the view's listeners to its model, `remove()` also unbinds the listening callbacks from the model ensuring that a memory leak does not occur.
4. `destroy()` also removes the model from the Todos collection, which triggers a `remove` event on the collection.
5. Since the AppView has its `render()` method bound to `all` events on the Todos collection, that view is rendered and the stats in the footer are updated.

That's all there is to it!

If you want to see an example of those, see the [complete source](https://github.com/tastejs/todomvc/tree/gh-pages/examples/backbone).

## Mark all as complete

A little detail we've left to near the end: in the `index.html` file add this to the `<section>` with the ID of `main`:

```html
<input type="checkbox" id="toggle-all">
<label for="toggle-all">Mark all as complete</label>
```

Refresh the page and you'll see this adds an arrow to the left column at the top. We'll want to add a DOM handler to take care of that, and since it occurs at a level *above* individual todos, we'll need to put that in the root View, so open `views/app.js`, and add this to the `events` object:

```javascript
  'click #toggle-all': 'toggleAllComplete'
```

We'll then need to create the `toggleAllComplete()` method in the same View:

```javascript
  toggleAllComplete: function() {
    var completed = this.allCheckbox.checked;

    app.Todos.each(function( todo ) {
      todo.save({
        'completed': completed
      });
    });
  }
```

This is pretty straightforward: first it gets the boolean value of the "Mark All" checkbox (here rendered as an arrow by the CSS). Then it assigns this boolean to each todo in the `app.Todos` Collection. For this to work, we need to create the variable `this.allCheckbox`. So add this line to the `initialize()` method:

```javascript
  this.allCheckbox = this.$('#toggle-all')[0];
```

Try it out: click the "All Completed" arrow, and it should toggle the state of all the items in the list. The only problem is that when refreshing the page, although the todos stay in their correct state, the arrow doesn't. So we need to add a `render()` method to `views\app.js`. First, add the method declaration and these two variables:

```javascript
  render: function() {
        var completed = app.Todos.completed().length;
        var remaining = app.Todos.remaining().length;
  },
```

Then, set the state of the checkbox at the end of the `render()` method:

```javascript
  this.allCheckbox.checked = !remaining;
```

Next, we'll need to add an event listener to call this new `render()` method whenever anything changes. Add this to the `initialize()` method:

```javascript
  this.listenTo(app.Todos, 'all', this.render);
```

You'll have noticed that the two variables `completed` and `remaining` depend on methods in the Collection which we haven't yet created, so let's do that now. In `collections/todos.js`, add:

```javascript
  completed: function() {
    return this.filter(function(todo){
      return todo.get('completed');
    });
  },

  remaining: function() {
    return this.without.apply(this, this.completed());
  },
```

The collection’s `completed()` and `remaining()` methods return an array of finished and unfinished todos, respectively.

The `completed()` method simply calls Javascript's [filter](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/filter) method to build an array of todos which have the `completed` flag set. 

The `remaining()` method uses the Underscore.js [`without()`](http://underscorejs.org/#without) method to return an array of todos *without* the completed ones.

With these in place, when you refresh the page the Mark all as Complete checkbox retains its state.

## Statistics

Let's display some stats. First, add the `<footer>` tag right after the `<section>` with the `id` of `main`:

```html
<footer id="footer"></footer>
```

then add the stats template right after the item template:

```html
<script type="text/template" id="stats-template">
    <span id="todo-count"><strong><%= remaining %></strong> <%= remaining === 1 ? 'item' : 'items' %> left</span>
    <ul id="filters">
      <li><a href="#/" class="selected">All</a></li>
      <li><a href="#/active">Active</a></li>
      <li><a href="#/completed">Completed</a></li>
    </ul>
    <% if (completed) { %>
    <button id="clear-completed">Clear completed (<%= completed %>)</button>
    <% } %>
</script>
```

The #stats-template displays the number of remaining incomplete items and contains a list of hyperlinks which will be used to perform actions when we implement our router. It also contains a button which can be used to clear all of the completed items. 

This code is all pretty self-explanatory. But we need to add some functionality, so go back to `views/app.js`, and define the `template` near the top, right after the `el` definition:

```javascript
  statsTemplate: _.template( $('#stats-template').html() ),
```

This works exactly the same as the `_.template()` method in `views/todos.js`. In the `initialize()` method we also want to instantiate a `footer` variable to grab the footer element. We might as well grab the `#main` section while we're at it:

```javascript
  this.$footer = this.$('#footer');
  this.$main = this.$('#main');
```

Finally, we have to render the footer. Add this code in the `render()` method:

```javascript
  if ( app.Todos.length ) {
    this.$main.show();
    this.$footer.show();

    this.$footer.html(this.statsTemplate({
      completed: completed,
      remaining: remaining
    }));

    this.$('#filters li a')
      .removeClass('selected')
      .filter('[href="#/' + ( app.TodoFilter || '' ) + '"]')
      .addClass('selected');
  } else {
    this.$main.hide();
    this.$footer.hide();
  }
```

This logic shows or hides certain parts of the GUI depending on whether or not there are any todos in the Collection. If not, the footer and the Mark all as Complete button (in the `main` section) are hidden. If there *are* todos, it shows the main and footer sections, fills the footer template with the `completed` and `remaining` variables already created in this method, and performs a bit of jQuery magic to highlight the appropriate filter (All, Active or Completed) depending on what the URL is (we will come to this in the next section).

Now if you look in the stats template, you'll see a button with the `id` of `clear-completed`. So let's add that functionality. First, add the event to the DOM handler of `views/app.js`:

```javascript
  'click #clear-completed': 'clearCompleted',
```

then create the `clearCompleted()` method:

```javascript
  clearCompleted: function() {
    _.invoke(app.Todos.completed(), 'destroy');
    return false;
  },
```

This uses the Underscore.js [`invoke()`](http://underscorejs.org/#invoke) method to call `destroy` on all completed todos in the `app.Todos` Collection.

## Todo routing

Finally, we move on to routing, which will allow us to easily filter the list of items that are active as well as those which have been completed. We’ll be supporting the following routes:

```
#/ (all - default)
#/active
#/completed
```

![](img/todos_e.png)

When the route changes, the todo list will be filtered on a model level and the selected class on the filter links in the footer will be toggled as described above. When an item is updated while a filter is active it will be updated accordingly (e.g., if the filter is active and the item is checked, it will be hidden). The active filter is persisted on reload. First, add the reference to the router in `index.html`:

```html
<script src="js/routers/router.js"></script>
```

Then, create the file `routers/router.js`:

```javascript

  // js/routers/router.js

  // Todo Router
  // ----------

  var app = app || {};

  var Workspace = Backbone.Router.extend({
    routes:{
      '*filter': 'setFilter'
    },

    setFilter: function( param ) {
      // Set the current filter to be used
      if (param) {
        param = param.trim();
      }
      app.TodoFilter = param || '';

      // Trigger a collection filter event, causing hiding/unhiding
      // of Todo view items
      app.Todos.trigger('filter');
    }
  });

  app.TodoRouter = new Workspace();
  Backbone.history.start();

```

Let's take a quick tour of this file. Notice it sets up a `routes` object. If we look at the [manual](http://backbonejs.org/#Router-routes) we'll see that this maps a series of routes to a series of functions. In our case we're using a wildcard, or a `*splat` part, for the whole path; in other words, any path will be matched by this and passed to the `setFilter()` method. We want the URL to specify what kind of filter is being applied to the todo list - display all, active or completed. 

This event makes no difference to the collection - it only affects what we can see. So it makes sense to put the code for this in the main AppView. Open up `views/app.js` and add two new event listeners to the `initialize()` method:

```javascript
  this.listenTo(app.Todos, 'change:completed', this.filterOne);
  this.listenTo(app.Todos, 'filter', this.filterAll);
```

and add the two methods:

```javascript
  filterOne : function (todo) {
    todo.trigger('visible');
  },

  filterAll : function () {
    app.Todos.each(this.filterOne, this);
  },
```

When a todo is saved it results in a `change:completed` event on the model which is handled by the AppView's `filterOne()` method. Notice that `filterOne()` will trigger a `visible` event on the model. This is used in conjunction with the filtering in our routes and collections so that we only display an item if its completed state falls in line with the current filter. Now we bound the model's visible event to the `toggleVisible()` method. Open `views/todos.js`, and add the listener to its `initialize()` method:

```javascript
  this.listenTo(this.model, 'visible', this.toggleVisible);
```

We then create the `toggleVisible()` method, which determines if the todo should be visible and updates it accordingly.:

```javascript
  toggleVisible : function () {
    this.$el.toggleClass( 'hidden',  this.isHidden());
  },
```

This calls the jQuery `toggleClass()` method, passing it another new method, `isHidden()` as the boolean test:

```javascript
  isHidden : function () {
    var isCompleted = this.model.get('completed');
    return ( // hidden cases only
      (!isCompleted && app.TodoFilter === 'completed')
      || (isCompleted && app.TodoFilter === 'active')
    );
  },
```

This returns `TRUE` if the filter is set to `completed` (i.e. if the URL is `#/completed`) and this todo is not completed, or if the filter is `active` (the URL is `#/active`) and the todo *is* completed. In either of these cases, it returns `TRUE`, i.e. this todo is hidden. 

That's all you need for the filtering. Refresh the page and you'll see the filters at the bottom hiding todos depending on their state.

Finally, going back to `router.js`, we create an instance of our router and call `Backbone.history.start()` to route the initial URL during page load.

## Summary

We’ve now built our first complete Backbone.js application. The latest version of the full app can be viewed online at any time and the sources are readily available via [TodoMVC](http://www.todomvc.com).

Later on in the book, we’ll learn how to further modularize this application using RequireJS, swap out our persistence layer to a database back-end, and finally unit test the application with a few different testing frameworks.
