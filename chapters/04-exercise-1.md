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

First, we'll set up the header and the basic application dependencies: [jQuery](http://jquery.com), [Underscore](http://underscorejs.org), Backbone.js and the [Backbone LocalStorage adapter](https://github.com/jeromegn/Backbone.localStorage).

```html
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
  <title>Backbone.js • TodoMVC</title>
  <link rel="stylesheet" href="assets/base.css">
</head>
<body>
  <script type="text/template" id="item-template"></script>
  <script type="text/template" id="stats-template"></script>
  <script src="js/lib/jquery.min.js"></script>
  <script src="js/lib/underscore-min.js"></script>
  <script src="js/lib/backbone-min.js"></script>
  <script src="js/lib/backbone.localStorage.js"></script>
  <script src="js/models/todo.js"></script>
  <script src="js/collections/todos.js"></script>
  <script src="js/views/todos.js"></script>
  <script src="js/views/app.js"></script>
  <script src="js/routers/router.js"></script>
  <script src="js/app.js"></script>
</body>
</html>
```

In addition to the aforementioned dependencies, note that a few other application-specific files are also loaded. These are organized into folders representing their application responsibilities: models, views, collections, and routers. An app.js file is present to house central initialization code.

Note: If you want to follow along, create a directory structure as demonstrated in index.html:

1. Place the index.html in a top-level directory.
2. Download jQuery, Underscore, Backbone, and Backbone LocalStorage from their respective web sites and place them under js/lib
3. Create the directories js/models, js/collections, js/views, and js/routers

You will also need [base.css](https://raw.github.com/addyosmani/todomvc/gh-pages/assets/base.css) and [bg.png](https://raw.github.com/addyosmani/todomvc/gh-pages/assets/bg.png), which should live in an assets directory. And remember that you can see a demo of the final application at [TodoMVC.com](http://todomvc.com).

We will be creating the application JavaScript files during the tutorial. Don't worry about the two 'text/template' script elements - we will replace those soon!

#### Application HTML

Now let's populate the body of index.html. We'll need an `<input>` for creating new todos, a `<ul id="todo-list" />` for listing the actual todos, and a footer where we can later insert statistics and links for performing operations such as clearing completed todos. We'll add the following markup immediately inside our body tag before the script elements:

```html
  <section id="todoapp">
    <header id="header">
      <h1>todos</h1>
      <input id="new-todo" placeholder="What needs to be done?" autofocus>
    </header>
    <section id="main">
      <input id="toggle-all" type="checkbox">
      <label for="toggle-all">Mark all as complete</label>
      <ul id="todo-list"></ul>
    </section>
    <footer id="footer"></footer>
  </section>
  <div id="info">
    <p>Double-click to edit a todo</p>
    <p>Written by <a href="https://github.com/addyosmani">Addy Osmani</a></p>
    <p>Part of <a href="http://todomvc.com">TodoMVC</a></p>
  </div>
```

#### Templates

To complete index.html, we need to add the templates which we will use to dynamically create HTML by injecting model data into their placeholders. One way of including templates in the page is by using custom script tags. These don't get evaluated by the browser, which just interprets them as plain text. Underscore micro-templating can then access the templates, rendering fragments of HTML.

We'll start by filling in the #item-template which will be used to display individual todo items.

```html
  <!-- index.html -->

  <script type="text/template" id="item-template">
    <div class="view">
      <input class="toggle" type="checkbox" <%= completed ? 'checked' : '' %>>
      <label><%- title %></label>
      <button class="destroy"></button>
    </div>
    <input class="edit" value="<%- title %>">
  </script>
```

The template tags in the above markup, such as `<%=` and `<%-`, are specific to Underscore.js and are documented on the Underscore site. In your own applications, you have a choice of template libraries, such as Mustache or Handlebars. Use whichever you prefer, Backbone doesn't mind.

We also need to define the #stats-template template which we will use to populate the footer.

```html
  <!-- index.html -->

  <script type="text/template" id="stats-template">
    <span id="todo-count"><strong><%= remaining %></strong> <%= remaining === 1 ? 'item' : 'items' %> left</span>
    <ul id="filters">
      <li>
        <a class="selected" href="#/">All</a>
      </li>
      <li>
        <a href="#/active">Active</a>
      </li>
      <li>
        <a href="#/completed">Completed</a>
      </li>
    </ul>
    <% if (completed) { %>
    <button id="clear-completed">Clear completed (<%= completed %>)</button>
    <% } %>
  </script>
```

The #stats-template displays the number of remaining incomplete items and contains a list of hyperlinks which will be used to perform actions when we implement our router. It also contains a button which can be used to clear all of the completed items.

Now that we have all the HTML that we will need, we'll start implementing our application by returning to the fundamentals: a Todo model.

## Todo model

The `Todo` model is remarkably straightforward. First, a todo has two attributes: a `title` stores a todo item's title and a `completed` status indicates if it's complete. These attributes are passed as defaults, as shown below:

```javascript

  // js/models/todo.js

  var app = app || {};

  // Todo Model
  // ----------
  // Our basic **Todo** model has `title`, `order`, and `completed` attributes.

  app.Todo = Backbone.Model.extend({

    // Default attributes ensure that each todo created has `title` and `completed` keys.
    defaults: {
      title: '',
      completed: false
    },

    // Toggle the `completed` state of this todo item.
    toggle: function() {
      this.save({
        completed: !this.get('completed')
      });
    }

  });

```

Second, the Todo model has a `toggle()` method through which a Todo item's completion status can be set and simultaneously persisted.

## Todo collection

Next, a `TodoList` collection is used to group our models. The collection uses the LocalStorage adapter to override Backbone's default `sync()` operation with one that will persist our Todo records to HTML5 Local Storage. Through local storage, they're saved between page requests.

```javascript

  // js/collections/todos.js

  var app = app || {};

  // Todo Collection
  // ---------------

  // The collection of todos is backed by *localStorage* instead of a remote
  // server.
  var TodoList = Backbone.Collection.extend({

    // Reference to this collection's model.
    model: app.Todo,

    // Save all of the todo items under the `"todos-backbone"` namespace.
    localStorage: new Backbone.LocalStorage('todos-backbone'),

    // Filter down the list of all todo items that are finished.
    completed: function() {
      return this.filter(function( todo ) {
        return todo.get('completed');
      });
    },

    // Filter down the list to only todo items that are still not finished.
    remaining: function() {
      return this.without.apply( this, this.completed() );
    },

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
  });

  // Create our global collection of **Todos**.
  app.Todos = new TodoList();

```

The collection's `completed()` and `remaining()` methods return an array of finished and unfinished todos, respectively.

A `nextOrder()` method implements a sequence generator while a `comparator()` sorts items by their insertion order.

*Note*: `this.filter`, `this.without` and `this.last` are Underscore methods that are mixed in to Backbone.Collection so that the reader knows how to find out more about them.

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

    // Our template for the line of statistics at the bottom of the app.
    statsTemplate: _.template( $('#stats-template').html() ),

    // At initialization we bind to the relevant events on the `Todos`
    // collection, when items are added or changed.
    initialize: function() {
      this.allCheckbox = this.$('#toggle-all')[0];
      this.$input = this.$('#new-todo');
      this.$footer = this.$('#footer');
      this.$main = this.$('#main');

      this.listenTo(app.Todos, 'add', this.addOne);
      this.listenTo(app.Todos, 'reset', this.addAll);
    },

    // Add a single todo item to the list by creating a view for it, and
    // appending its element to the `<ul>`.
    addOne: function( todo ) {
      var view = new app.TodoView({ model: todo });
      $('#todo-list').append( view.render().el );
    },

    // Add all items in the **Todos** collection at once.
    addAll: function() {
      this.$('#todo-list').html('');
      app.Todos.each(this.addOne, this);
    }

  });

```

A few notable features are present in our initial version of AppView, including a `statsTemplate`, an `initialize` method that's implicitly called on instantiation, and several view-specific methods.

An `el` (element) property stores a selector targeting the DOM element with an ID of `todoapp`. In the case of our application, `el` refers to the matching `<section id="todoapp" />` element in index.html.

The call to _.template uses Underscore's micro-templating to construct a statsTemplate object from our #stats-template. We will use this template later when we render our view.

Now let's take a look at the `initialize` function. First, it's using jQuery to cache the elements it will be using into local properties (recall that `this.$()` finds elements relative to `this.$el`). Then it's binding to two events on the Todos collection: `add` and `reset`. Since we're delegating handling of updates and deletes to the `TodoView` view, we don't need to worry about those here. The two pieces of logic are:

* When an `add` event is fired the `addOne()` method is called and passed the new model. `addOne()` creates an instance of TodoView view, renders it, and appends the resulting element to our Todo list.

* When a `reset` event occurs (i.e., we update the collection in bulk as happens when the Todos are loaded from Local Storage), `addAll()` is called, which iterates over all of the Todos currently in our collection and fires `addOne()` for each item.

Note that we were able to use `this` within `addAll()` to refer to the view because `listenTo()` implicitly set the callback's context to the view when it created the binding.

Now, let's add some more logic to complete our AppView!

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

    // Our template for the line of statistics at the bottom of the app.
    statsTemplate: _.template( $('#stats-template').html() ),

    // Delegated events for creating new items, and clearing completed ones.
    events: {
      'keypress #new-todo': 'createOnEnter',
      'click #clear-completed': 'clearCompleted',
      'click #toggle-all': 'toggleAllComplete'
    },

    // At initialization we bind to the relevant events on the `Todos`
    // collection, when items are added or changed. Kick things off by
    // loading any preexisting todos that might be saved in *localStorage*.
    initialize: function() {
      this.allCheckbox = this.$('#toggle-all')[0];
      this.$input = this.$('#new-todo');
      this.$footer = this.$('#footer');
      this.$main = this.$('#main');

      this.listenTo(app.Todos, 'add', this.addOne);
      this.listenTo(app.Todos, 'reset', this.addAll);
      this.listenTo(app.Todos, 'change:completed', this.filterOne);
      this.listenTo(app.Todos,'filter', this.filterAll);
      this.listenTo(app.Todos, 'all', this.render);

      app.Todos.fetch();
    },

    // Re-rendering the App just means refreshing the statistics -- the rest
    // of the app doesn't change.
    render: function() {
      var completed = app.Todos.completed().length;
      var remaining = app.Todos.remaining().length;

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

      this.allCheckbox.checked = !remaining;
    },

    // Add a single todo item to the list by creating a view for it, and
    // appending its element to the `<ul>`.
    addOne: function( todo ) {
      var view = new app.TodoView({ model: todo });
      $('#todo-list').append( view.render().el );
    },

    // Add all items in the **Todos** collection at once.
    addAll: function() {
      this.$('#todo-list').html('');
      app.Todos.each(this.addOne, this);
    },

    filterOne : function (todo) {
      todo.trigger('visible');
    },

    filterAll : function () {
      app.Todos.each(this.filterOne, this);
    },

    // Generate the attributes for a new Todo item.
    newAttributes: function() {
      return {
        title: this.$input.val().trim(),
        order: app.Todos.nextOrder(),
        completed: false
      };
    },

    // If you hit return in the main input field, create new Todo model,
    // persisting it to localStorage.
    createOnEnter: function( event ) {
      if ( event.which !== ENTER_KEY || !this.$input.val().trim() ) {
        return;
      }

      app.Todos.create( this.newAttributes() );
      this.$input.val('');
    },

    // Clear all completed todo items, destroying their models.
    clearCompleted: function() {
      _.invoke(app.Todos.completed(), 'destroy');
      return false;
    },

    toggleAllComplete: function() {
      var completed = this.allCheckbox.checked;

      app.Todos.each(function( todo ) {
        todo.save({
          'completed': completed
        });
      });
    }
  });

```

We have added the logic for creating new todos, editing them, and filtering them based on their completed status.

* events: We've defined an `events` hash containing declarative callbacks for our DOM events. It binds those events to the following methods:
  * `createOnEnter()`: Creates a new Todo model and persists it in localStorage when a user hits enter inside the `<input/>` field. Also resets the main `<input/>` field value to prepare it for the next entry. The model is populated by newAttributes(), which returns an object literal composed of the title, order, and completed state of the new item. Note that `this` is referring to the view and not the DOM element since the callback was bound using the `events` hash.
  * `clearCompleted()`: Removes the items in the todo list that have been marked as completed when the user clicks the clear-completed checkbox (this checkbox will be in the footer populated by the #stats-template).
  * `toggleAllComplete()`: Allows a user to mark all of the items in the todo list as completed by clicking the toggle-all checkbox.

* `initialize()`: We've bound callbacks to several additional events:
  * We've bound a `filterOne()` callback on the Todos collection for a `change:completed` event. This listens for changes to the completed flag for any model in the collection. The affected todo is passed to the callback which triggers a custom `visible` event on the model.
  * We've bound a `filterAll()` callback for a `filter` event, which works a little similar to addOne() and addAll(). It’s responsibility is to toggle which todo items are visible based on the filter currently selected in the UI (all, completed or remaining) via calls to filterOne().
  * We've used the special `all` event to bind any event triggered on the Todos collection to the view's render method (discussed below).

The `initialize()` method completes by fetching the previously saved todos from localStorage.

* `render()`: Several things are happening in our `render()` method:
  * The #main and #footer sections are displayed or hidden depending on whether there are any todos in the collection.
  * The footer is populated with the HTML produced by instantiating the statsTemplate with the number of completed and remaining todo items.
  * The HTML produced by the preceding step contains a list of filter links. The value of `app.TodoFilter`, which will be set by our router, is being used to apply the class 'selected' to the link corresponding to the currently selected filter. This will result in conditional CSS styling being applied to that filter.
  * The allCheckbox is updated based on whether there are remaining todos.

## Individual Todo View

Now let’s look at the `TodoView` view. This will be in charge of individual Todo records, making sure the view updates when the todo does. To enable this functionality, we will add event listeners to the view that listen for events on an individual todo's HTML representation.

```javascript

  // js/views/todos.js

  var app = app || {};

  // Todo Item View
  // --------------

  // The DOM element for a todo item...
  app.TodoView = Backbone.View.extend({

    //... is a list tag.
    tagName: 'li',

    // Cache the template function for a single item.
    template: _.template( $('#item-template').html() ),

    // The DOM events specific to an item.
    events: {
      'dblclick label': 'edit',
      'keypress .edit': 'updateOnEnter',
      'blur .edit': 'close'
    },

    // The TodoView listens for changes to its model, re-rendering. Since there's
    // a one-to-one correspondence between a **Todo** and a **TodoView** in this
    // app, we set a direct reference on the model for convenience.
    initialize: function() {
      this.listenTo(this.model, 'change', this.render);
    },

    // Re-renders the titles of the todo item.
    render: function() {
      this.$el.html( this.template( this.model.toJSON() ) );
      this.$input = this.$('.edit');
      return this;
    },

    // Switch this view into `"editing"` mode, displaying the input field.
    edit: function() {
      this.$el.addClass('editing');
      this.$input.focus();
    },

    // Close the `"editing"` mode, saving changes to the todo.
    close: function() {
      var value = this.$input.val().trim();

      if ( value ) {
        this.model.save({ title: value });
      }

      this.$el.removeClass('editing');
    },

    // If you hit `enter`, we're through editing the item.
    updateOnEnter: function( e ) {
      if ( e.which === ENTER_KEY ) {
        this.close();
      }
    }
  });

```

In the `initialize()` constructor, we set up a listener that monitors a todo model’s `change` event. As a result, when the todo gets updated, the application will re-render the view and visually reflect its changes. Note that the model passed in the arguments hash by our AppView is automatically available to us as `this.model`.

In the `render()` method, we render our Underscore.js `#item-template`, which was previously compiled into this.template using Underscore’s `_.template()` method.  This returns an HTML fragment that replaces the content of the view's element (an li element was implicitly created for us based on the `tagName` property). In other words, the rendered template is now present under `this.el` and can be appended to the todo list in the user interface. `render()` finishes by caching the input element within the instantiated template into `this.input`.

Our events hash includes three callbacks:

* `edit()`: changes the current view into editing mode when a user double-clicks on an existing item in the todo list. This allows them to change the existing value of the item’s title attribute.
* `updateOnEnter()`: checks that the user has hit the return/enter key and executes the close() function.
* `close()`: trims the value of the current text in our `<input/>` field, ensuring that we don’t process it further if it does not contain any text (e.g ‘’). If a valid value has been provided, we save the changes to the current todo model and close editing mode by removing the corresponding CSS class.

## Startup

So now we have two views: `AppView` and `TodoView`. The former needs to be instantiated on page load so its code gets executed. This can be accomplished through jQuery's `ready()` utility, which will execute a function when the DOM is loaded.

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

If you are following along, open `file://*path*/index.html` in your web browser and monitor its console. If all is well, you shouldn't see any JavaScript errors other than regarding the router.js file that we haven't created yet. The todo list should be blank as we haven't yet created any todos. Plus, there is some additional work we'll need to do before the user interface fully functions.

However, a few things can be tested through the JavaScript console.

In the console, add a new todo item: `window.app.Todos.create({ title: 'My first Todo item'});` and hit return.

![](img/todos_d.png)

If all is functioning properly, this should log the new todo we've just added to the todos collection. The newly created todo is also saved to Local Storage and will be available on page refresh.

`window.app.Todos.create()` executes a collection method (`Collection.create(attributes, [options])`) which instantiates a new model item of the type passed into the collection definition, in our case `app.Todo`:

```javascript

  // from our js/collections/todos.js

  var TodoList = Backbone.Collection.extend({

      model: app.Todo // the model type used by collection.create() to instantiate new model in the collection
      ...
  )};

```

Run the following in the console to check it out:

```javascript
var secondTodo = window.app.Todos.create({ title: 'My second Todo item'});
secondTodo instanceof app.Todo // returns true
```

Now refresh the page and we should be able to see the fruits of our labour.

The todos added through the console should still appear in the list since they are populated from the Local Storage. Also, we should be able to create a new todo by typing a title and pressing enter.

![](img/todos_b.png)

Excellent, we're making great progress, but what about completing and deleting todos?

## Completing & deleting todos


The next part of our tutorial is going to cover completing and deleting todos. These two actions are specific to each Todo item, so we need to add this functionality to the TodoView view. We will do so by adding `togglecompleted()` and `clear()` methods along with corresponding entries in the `events` hash.

```javascript

  // js/view/todos.js

  var app = app || {};

  // Todo Item View
  // --------------

  // The DOM element for a todo item...
  app.TodoView = Backbone.View.extend({

    //... is a list tag.
    tagName: 'li',

    // Cache the template function for a single item.
    template: _.template( $('#item-template').html() ),

    // The DOM events specific to an item.
    events: {
      'click .toggle': 'togglecompleted', // NEW
      'dblclick label': 'edit',
      'click .destroy': 'clear',           // NEW
      'keypress .edit': 'updateOnEnter',
      'blur .edit': 'close'
    },

    // The TodoView listens for changes to its model, re-rendering. Since there's
    // a one-to-one correspondence between a **Todo** and a **TodoView** in this
    // app, we set a direct reference on the model for convenience.
    initialize: function() {
      this.listenTo(this.model, 'change', this.render);
      this.listenTo(this.model, 'destroy', this.remove);        // NEW
      this.listenTo(this.model, 'visible', this.toggleVisible); // NEW
    },

    // Re-render the titles of the todo item.
    render: function() {
      this.$el.html( this.template( this.model.toJSON() ) );

      this.$el.toggleClass( 'completed', this.model.get('completed') ); // NEW
      this.toggleVisible();                                             // NEW

      this.$input = this.$('.edit');
      return this;
    },

    // NEW - Toggles visibility of item
    toggleVisible : function () {
      this.$el.toggleClass( 'hidden',  this.isHidden());
    },

    // NEW - Determines if item should be hidden
    isHidden : function () {
      var isCompleted = this.model.get('completed');
      return ( // hidden cases only
        (!isCompleted && app.TodoFilter === 'completed')
        || (isCompleted && app.TodoFilter === 'active')
      );
    },

    // NEW - Toggle the `"completed"` state of the model.
    togglecompleted: function() {
      this.model.toggle();
    },

    // Switch this view into `"editing"` mode, displaying the input field.
    edit: function() {
      this.$el.addClass('editing');
      this.$input.focus();
    },

    // Close the `"editing"` mode, saving changes to the todo.
    close: function() {
      var value = this.$input.val().trim();

      if ( value ) {
        this.model.save({ title: value });
      } else {
        this.clear(); // NEW
      }

      this.$el.removeClass('editing');
    },

    // If you hit `enter`, we're through editing the item.
    updateOnEnter: function( e ) {
      if ( e.which === ENTER_KEY ) {
        this.close();
      }
    },

    // NEW - Remove the item, destroy the model from *localStorage* and delete its view.
    clear: function() {
      this.model.destroy();
    }
  });

```

The key part of this is the two event handlers we've added, a togglecompleted event on the todo's checkbox, and a click event on the todo's `<button class="destroy" />` button.

Let's look at the events that occur when we click the checkbox for a todo item:

1. The `togglecompleted()` function is invoked which calls `toggle()` on the todo model.
2. `toggle()` toggles the completed status of the todo and calls `save()` on the model.
3. The save generates a `change` event on the model which is bound to our TodoView's `render()` method. We've added a statement in `render()` which toggles the completed class on the element depending
on the model's completed state. The associated CSS changes the color of the title text and strikes a line through it when the todo is completed.
4. The save also results in a `change:completed` event on the model which is handled by the AppView's `filterOne()` method. If we look back at the AppView, we see that filterOne() will trigger a `visible` event on the model. This is used in conjunction with the filtering in our routes and collections so that we only display an item if its completed state falls in line with the current filter. In our update to the TodoView, we bound the model's visible event to the `toggleVisible()` method. This method uses the new `isHidden()` method to determine if the todo should be visible and updates it accordingly.

Now let's look at what happens when we click on a todo's destroy button:

1. The `clear()` method is invoked which calls `destroy()` on the todo model.
2. The todo is deleted from local storage and a `destroy` event is triggered.
3. In our update to the TodoView, we bound the model's `destroy` event to the view's inherited `remove()` method. This method deletes the view and automatically removes the associated element from the DOM. Since we used `listenTo()` to bind the view's listeners to its model, `remove()` also unbinds the listening callbacks from the model ensuring that a memory leak does not occur.
4. `destroy()` also removes the model from the Todos collection, which triggers a `remove` event on the collection.
5. Since the AppView has its `render()` method bound to `all` events on the Todos collection, that view is rendered and the stats in the footer are updated.

That's all there is to it!

If you want to see an example of those, see the [complete source](https://github.com/addyosmani/todomvc/tree/gh-pages/architecture-examples/backbone).

## Todo routing

Finally, we move on to routing, which will allow us to easily filter the list of items that are active as well as those which have been completed. We’ll be supporting the following routes:

```
#/ (all - default)
#/active
#/completed
```

![](img/todos_e.png)

When the route changes, the todo list will be filtered on a model level and the selected class on the filter links in the footer will be toggled as described above. When an item is updated while a filter is active it will be updated accordingly (e.g., if the filter is active and the item is checked, it will be hidden). The active filter is persisted on reload.

```javascript

  // js/routers/router.js

  // Todo Router
  // ----------

  var Workspace = Backbone.Router.extend({
    routes:{
      '*filter': 'setFilter'
    },

    setFilter: function( param ) {
      // Set the current filter to be used
      app.TodoFilter = param.trim() || '';

      // Trigger a collection filter event, causing hiding/unhiding
      // of Todo view items
      app.Todos.trigger('filter');
    }
  });

  app.TodoRouter = new Workspace();
  Backbone.history.start();

```

Our router uses a *splat to set up a default route which passes the string after '#/' in the URL to `setFilter()` which sets `window.app.TodoFilter` to that string.

As we can see in the line `window.app.Todos.trigger('filter')`, once the filter has been set, we simply trigger 'filter' on our Todos collection to toggle which items are visible and which are hidden. Recall that our AppView's `filterAll()` method is bound to the collection's filter event and that any event on the collection will cause the AppView to re-render.

Finally, we create an instance of our router and call `Backbone.history.start()` to route the initial URL during page load.

## Summary

We’ve now built our first complete Backbone.js application. The latest version of the full app can be viewed online at any time and the sources are readily available via [TodoMVC](http://www.todomvc.com).

Later on in the book, we’ll learn how to further modularize this application using RequireJS, swap out our persistence layer to a database back-end, and finally unit test the application with a few different testing frameworks.
