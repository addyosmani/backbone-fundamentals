# Exercise 1: Todos - Your First Backbone.js App

Now that we've covered fundamentals, let's write our first Backbone.js application. We'll build the Backbone Todo List application exhibited on [TodoMVC.com](http://todomvc.com). Building a Todo List is a great way to learn Backbone’s conventions. It's a relatively simple application, yet technical challenges surrounding binding, persisting model data, routing and template rendering provide the opportunity to illustrate some core Backbone features.

![](img/todoapp.png)

Let's consider the application's architecture at a high level. We'll need:

* a `Todo` model to describe individual todo items
* a `TodoList` collection to store and persist todos
* a way of creating todos
* a way to display a listing of todos
* a way to edit existing todos
* a way to deem a todo complete
* a way to deleting todos
* a way to bookmark the items that have been completed or are remaining

Essentially, these features are classic [CRUD](http://en.wikipedia.org/wiki/Create,_read,_update_and_delete) methods. Let's get started!

## Index

First, we'll set up the basic application dependencies: [jQuery](http://jquery.com), [Underscore](http://underscorejs.org), Backbone.js and the [Backbone LocalStorage adapter](https://github.com/jeromegn/Backbone.localStorage). These will be loaded in index.html, the application's sole HTML file:

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
  <script src="js/lib/jquery.min.js"></script>
  <script src="js/lib/underscore-min.js"></script>
  <script src="js/lib/backbone-min.js"></script>
  <script src="js/lib/backbone.localStorage.js"></script>
  <script src="js/models/todo.js"></script>
  <script type="text/template" id="item-template"></script>
  <script type="text/template" id="stats-template"></script>
  <script src="js/collections/todos.js"></script>
  <script src="js/views/todo.js"></script>
  <script src="js/views/app.js"></script>
  <script src="js/routers/router.js"></script>
  <script src="js/app.js"></script>
</body>
</html>

```

In addition to the aforementioned application dependencies, note that a few other, application-specific files are also loaded. These are organized into folders representing their application responsibilities: models, views, collections and routers. An app.js file is present to house central initialization code.

Note: If you want to follow along, create a directory structure as demonstrated in index.html. You will also need [base.css](https://raw.github.com/addyosmani/todomvc/gh-pages/assets/base.css) and [bg.png](https://raw.github.com/addyosmani/todomvc/gh-pages/assets/bg.png), which can both live in an assets directory. And again, the final application can be demo'd at [TodoMVC.com](http://todomvc.com).


## Application HTML

Now let's take a look at our application's static HTML. We'll need an `<input>` for creating new todos, a `<ul id="todo-list" />` for listing the actual todos and a section containing some operations, such as clearing completed todos.

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

We’ll populate our todo-list and add a statistics section containing details about what incomplete items later on.

In order to implement these features, we'll return to the fundamentals: a Todo model.

## Todo model

The `Todo` model is remarkably straightforward. First, a todo has two attributes: a `title` stores todo item's title and a `completed` status indicates if it's complete. These attributes are passed as defaults, as is exemplified below:

```javascript

  // js/models/todo.js

  var app = app || {};

  // Todo Model
  // ----------
  // Our basic **Todo** model has `title` and `completed` attributes.

  app.Todo = Backbone.Model.extend({

    // Default attributes for the todo
    // and ensure that each todo created has `title` and `completed` keys.
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

The Todo model also features a `toggle()` method through which a Todo item's completion status can be set.


## Todo collection


Next, a `TodoList` collection is used to group our models. The collection is extended by localStorage, which automatically persists Todo records to HTML5 Local Storage via the Backbone LocalStorage adapter. Through local storage, they're saved between page requests.

The collection's `completed()` and `remaining()` methods return an array of unfinished and finished todos, respectively.

A `nextOrder()` method keeps our Todo items in sequential order while a `comparator()` sorts items by their insertion order.

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

    // Save all of the todo items under the `"todos"` namespace.
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

## Application View

So let's look at the core of the application's logic, the views. Since each todo has a fair bit of logic associated with it, such as edit in place, we're going to use the element controller pattern - a pattern which consists of two views, one that controls a collection of items, and the other deals with each individual item.

In other words, we're going to have one view `AppView`, which will be in charge of creating new todos, and rendering the initial todo list. Then we'll have another view called TodoView instances of which will be associated with an individual Todo record. Todo instances will be in charge of editing, updating and destroying their associated todo.

To keep things simple, we'll keep things 'read-only' at the moment, and won't provide any functionality for creating, editing or deleting todos:

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
    // collection, when items are added or changed. Kick things off by
    // loading any preexisting todos that might be saved in *localStorage*.
    initialize: function() {
      this.input = this.$('#new-todo');
      this.allCheckbox = this.$('#toggle-all')[0];
      this.$footer = this.$('#footer');
      this.$main = this.$('#main');

      window.app.Todos.on( 'add', this.addOne, this );
      window.app.Todos.on( 'reset', this.addAll, this );
      window.app.Todos.on( 'all', this.render, this );

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
    }

  });

```


You can see we've got a couple of things going on, an el (element), a `statsTemplate`, a constructor function and several view specific methods. To the right of the `el:` key is a DOM element selector for the element with ID `todoapp`. The value of this is just a string and Backbone will create a reference pointing to the element matching the selector #todoapp, where here it will be the `<section id="todoapp" />` element, which we previously defined in our HTML.

In a nutshell this means we can now refer to this.el in our controller, which points to the `<section id="todoapp" />` element. As you can see, we're referring to el in the `addOne()` function, appending an element to the list.

Now let's take a look at the constructor function. It's binding to several events on the Todo model, such as add, reset and all. Since we're delegating handling of updates and deletes to the `TodoView` view, we don't need to worry about that here. The two pieces of logic are:

* When a new todo is created, the `add` event will be fired, calling `addOne()` which instantiates the TodoView view, rendering it and appending the resultant element to our Todo list.

* When a `reset` event is called (i.e. we wish to update the collection in bulk such as when the Todos have been loaded from Local Storage), `addAll()` is called, which iterates over all of the Todos currently in our collection and fires `addOne()` for each item.

We can then add in the logic for creating new todos, editing them and filtering them based on whether they are complete.

* events: We define an events hash containing declarative callbacks for our DOM events.
  * `createOnEnter()`: Creates a new Todo model which persists in localStorage when a user hits return inside the `<input/>` field and resets the main `<input/>` field value to prepare it for the next entry. This creates the model via newAttributes(), which is an object literal composed of the title, order and completed state of the new item being added.
  * `clearCompleted()`: Removes the items in the todo list that have been marked as completed.
  * `toggleAllComplete()`: Allows a user to set all of the items in the todo list to completed.
  * `initialize()`:
    We bind a callback for a change:completed event, letting us know a change has been made as well to an existing todo item.
    We also bind a callback for a filter event, which works a little similar to addOne() and addAll(). It’s responsibility is to toggle what todo items are visible based on the filter currently selected in the UI (all, completed or remaining) through filterOne() and filterAll().
  * `render()`:
    We add some conditional CSS styling based on the filter currently selected so that the route that has been selected is highlighted.

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
      this.input = this.$('#new-todo');
      this.allCheckbox = this.$('#toggle-all')[0];
      this.$footer = this.$('#footer');
      this.$main = this.$('#main');

      window.app.Todos.on( 'add', this.addOne, this );
      window.app.Todos.on( 'reset', this.addAll, this );
      window.app.Todos.on( 'change:completed', this.filterOne, this );
      window.app.Todos.on( 'filter', this.filterAll, this );

      window.app.Todos.on( 'all', this.render, this );

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
        title: this.input.val().trim(),
        order: app.Todos.nextOrder(),
        completed: false
      };
    },

    // If you hit return in the main input field, create new **Todo** model,
    // persisting it to *localStorage*.
    createOnEnter: function( e ) {
      if ( e.which !== ENTER_KEY || !this.input.val().trim() ) {
        return;
      }

      app.Todos.create( this.newAttributes() );
      this.input.val('');
    },

    // Clear all completed todo items, destroying their models.
    clearCompleted: function() {
      _.each( window.app.Todos.completed(), function( todo ) {
        todo.destroy();
      });

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

## Individual Todo View

Let’s look at the `TodoView` view, now. This will be in charge of individual Todo records, making sure the view updates when the todo does. To enable this interactive behavior we should add some event listeners to the view, that will listen to the events on individual todo represented in html.

```javascript

  // js/views/todo.js

  var app = app || {};

  // Todo Item View
  // --------------

  // The DOM element for a todo item...
  app.TodoView = Backbone.View.extend({

    //... is a list tag.
    tagName:  'li',

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
      this.model.on( 'change', this.render, this );
    },

    // Re-renders the todo item to the current state of the model and
    // updates the reference to the todo's edit input within the view.
    render: function() {
      this.$el.html( this.template( this.model.toJSON() ) );
      this.input = this.$('.edit');
      return this;
    },

    // Switch this view into `"editing"` mode, displaying the input field.
    edit: function() {
      this.$el.addClass('editing');
      this.input.focus();
    },

    // Close the `"editing"` mode, saving changes to the todo.
    close: function() {
      var value = this.input.val().trim();

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


In the `initialize()` constructor, we're setting up a listener to the todo model’s change event. In other words, when the todo updates, we want to re-render the view to reflect its changes.

In the `render()` method, we're rendering an Underscore.js JavaScript template, called `#item-template`, which we’ve previously compiled into this.template using Underscore’s `_.template()` method.  This returns a piece of HTML that we're using to replace the view’s current element. In other words, the rendered template is now present under `this.el`, and can be appended to the todo list.

Our events hash includes three callbacks:

* `edit()`: Changes the current view into editing mode when a user double-clicks on an existing item in the todo list. This allows them to change the existing value of the item’s title attribute
* `updateOnEnter()`: checks that the user has hit the return/enter key and executes the close() function.
* `close()`: This trims the value of the current text in our `<input/>` field, ensuring that we don’t process it further if it contains no text (e.g ‘’). If a valid value has been provided, we save the changes to the current todo model and close editing mode, by removing the corresponding CSS class.

## Setup

So now we have two views: `AppView` and `TodoView`. The former needs to get instantiated when the page loads, so some code actually gets run. You can do this simply enough, by using jQuery's `ready()` utility, which will execute a function when the DOM's loaded.

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

Now we've gone far enough without checking that things work as they should. 

If you are following along open up index.html and, if everything's going to plan, you shouldn't see any errors in the console. The todo list will be blank (we haven't created any todos yet), and the todo-list won't work through our slick interface, as we haven't yet hooked it up fully. However, we can create a Todo from the console.

Type in: `window.app.Todos.create({ title: 'My first Todo item'});` and hit return.

![](img/todoconsole.png)

Once you've run the above in the console, we should be looking at a brand new todo (logged in console) we've just added in the todos collection. Created todo is saved into Local Storage as well and will be available on page refresh.

`window.app.Todos.create()` used above is collection method (`collection.create(attributes, [options])`) which instantiate new model item of the type passed into the collection definition, in our case `app.Todo`:

```javascript

  var TodoList = Backbone.Collection.extend({

      model: app.Todo // the model type used by collection.create() to instantiate new model in the collection
      ...
  )};

```

Run this into console to check it out:

`var secondTodo = window.app.Todos.create({ title: 'My second Todo item'});`

`secondTodo instanceof app.Todo`

![](img/todoconsole2.png)

## Templates


The `#item-template` used in the `TodoView` view needs defining, so let's do that. One way of including templates in the page is by using custom script tags. These don't get evaluated by the browser, which just interprets them as plain text. Underscore micro-templating can then access the templates, rendering pieces of HTML.

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

The template tags demonstrated above, such as `<%=` , are specific to Underscore.js, and documented on the Underscore site. In your own applications, you have a choice of template libraries, such as Mustache or Handlebars. Use whichever you prefer, Backbone doesn't mind.

Now when `_.template( $('#item-template').html() )` is called in the `TodoView` view our template will render correctly.


We also need to define #stats-template template we use to display how many items have been completed, as well as allowing the user to clear these items.

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


## In action

Now refresh index.html and we should be able to see the fruits of our labour. 

The todos added through console earlier should appear in the list populated from the Local Storage. Also, we should be able to type a todo name, and press return to submit the form, creating a new todo.

![](img/todocompleted.png)


Excellent, we're making great progress, but how about completing and deleting todos?

## Completing & deleting todos


So the next part of our tutorial is going to cover completing and deleting todos. These two actions are specific to each Todo item, so we need to add this functionality to the TodoView view.

The key part of this is the two event handlers we've added, a togglecompleted event on the todo's checkbox, and a click event on the todo's `<button class="destroy" />` button.

The checkbox's togglecompleted event invokes the toggle() function, which toggles the todos's completed status, then resaving the todo - very straightforward!
The button's click event invokes `clear()`, which will simply destroy the todo.


That's all there is to it. Since we're binding to the change event, whenever the todo changes the view will automatically be re-rendered, checking or un-checking the checkbox as appropriate. Similarly, when the todo is destroyed, the model's `destroy()` function will be called, removing the todo from the view as we’re binding to the destroy event too.

One more piece to mention is that we’ve also binded to a visible event to handle the visibility state of the todo item. This is used in conjunction with the filtering in our routes and collections so that we only display an item if its completed state falls in line with the current filter.

This tutorial is long enough as is, so we won't go into in-place editing or updating. If you want an example of that, see the [complete source](https://github.com/addyosmani/todomvc/tree/gh-pages/architecture-examples/backbone).

```javascript

  // js/view/todo.js

  // Todo Item View
  // --------------

  // The DOM element for a todo item...
  app.TodoView = Backbone.View.extend({

    //... is a list tag.
    tagName:  'li',

    // Cache the template function for a single item.
    template: _.template( $('#item-template').html() ),

    // The DOM events specific to an item.
    events: {
      'click .toggle':  'togglecompleted',
      'dblclick label': 'edit',
      'click .destroy': 'clear',
      'keypress .edit': 'updateOnEnter',
      'blur .edit':   'close'
    },

    // The TodoView listens for changes to its model, re-rendering. Since there's
    // a one-to-one correspondence between a **Todo** and a **TodoView** in this
    // app, we set a direct reference on the model for convenience.
    initialize: function() {
      this.model.on( 'change', this.render, this );
      this.model.on( 'destroy', this.remove, this );
      this.model.on( 'visible', this.toggleVisible, this );
    },

    // Re-render the titles of the todo item.
    render: function() {
      this.$el.html( this.template( this.model.toJSON() ) );
      this.$el.toggleClass( 'completed', this.model.get('completed') );

      this.toggleVisible();
      this.input = this.$('.edit');
      return this;
    },

    toggleVisible : function () {
      this.$el.toggleClass( 'hidden',  this.isHidden());
    },

    isHidden : function () {
      var isCompleted = this.model.get('completed');
      return ( // hidden cases only
        (!isCompleted && app.TodoFilter === 'completed')
        || (isCompleted && app.TodoFilter === 'active')
      );
    },

    // Toggle the `"completed"` state of the model.
    togglecompleted: function() {
      this.model.toggle();
    },

    // Switch this view into `"editing"` mode, displaying the input field.
    edit: function() {
      this.$el.addClass('editing');
      this.input.focus();
    },

    // Close the `"editing"` mode, saving changes to the todo.
    close: function() {
      var value = this.input.val().trim();

      if ( value ) {
        this.model.save({ title: value });
      } else {
        this.clear();
      }

      this.$el.removeClass('editing');
    },

    // If you hit `enter`, we're through editing the item.
    updateOnEnter: function( e ) {
      if ( e.which === ENTER_KEY ) {
        this.close();
      }
    },

    // Remove the item, destroy the model from *localStorage* and delete its view.
    clear: function() {
      this.model.destroy();
    }
  });

```



## Todo routing

Finally, we move on to routing, which will allow us to easily bookmark the list of items that are active as well as those which have been completed. We’ll be supporting the following routes:

```
#/ (all - default)
#/active
#/completed
```

![](img/todorouting.png)

When the route changes the todo list will be filtered on a model level and the selected class on the filter links will be toggled. When an item is updated while in a filtered state, it will be updated accordingly. 
E.g. if the filter is active and the item is checked, it will be hidden. The active filter is persisted on reload.

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
      window.app.TodoFilter = param.trim() || '';

      // Trigger a collection filter event, causing hiding/unhiding
      // of Todo view items
      window.app.Todos.trigger('filter');
    }
  });

  app.TodoRouter = new Workspace();
  Backbone.history.start();

```

As we can see in the line `window.app.Todos.trigger('filter')`, once a string filter has been set, we simply trigger our filter at a collection level to toggle which items are displayed and which of those are hidden.

Finally, we call `Backbone.history.start()` to route the initial URL during page load.

## Conclusions

We’ve now learned how to build our first complete Backbone.js application. The full app can be viewed online at any time and the sources are readily available via [TodoMVC](http://www.todomvc.com).

Later on in the book, we’ll learn how to further modularize this application using RequireJS, swap out our persistence layer to a database back-end and finally unit test the application with a few different testing frameworks.
