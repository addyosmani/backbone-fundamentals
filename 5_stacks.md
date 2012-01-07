
#Advanced

##Building RESTful applications with Backbone

In this section of the book, we're going to take a look at developing RESTful applications using Backbone.js and modern technology stacks. When the data for your back-end is exposed through a purely RESTful API, tasks such as retrieving (GET), creating (POST), updating (PUT) and deleting (DELETE) models are made easy through Backbone's Model API. This API is so intuitive in fact that switching from storing records in a local data-store (e.g localStorage) to a database/noSQL data-store is a lot simpler than you may think.


##Stack 1: Using Node.js, Express, Mongoose and MongoDB

The first stack we'll be looking at is:

* [Node.js](nodejs.org)
* [Express](http://expressjs.com/)
* [Mongoose](http://mongoosejs.com/)
* and [MongoDB](http://www.mongodb.org/)

with [Jade](http://jade-lang.com/) used optionally as a view/templating engine.

###Reviewing the stack

As you may know, node.js is an event-driven platform (built on the [V8](http://code.google.com/apis/v8/design.html) runtime), designed for writing fast, scalable network applications. It's reasonably lightweight, efficient and great for real-time applications that are data-intensive. 

Express is a small web-development framework written with node.js, based on [Sinatra](http://www.sinatrarb.com/). It supports a number of useful features such as intuitive views, robust routing and a focus on high performance.

Next on the list are MongoDB and Mongoose. MongoDB is an open-source, document-oriented database store designed with scalability and agility in mind. As a [noSQL](http://en.wikipedia.org/wiki/NoSQL) database, rather than storing data in tables and rows (something we're very used to doing with relational databases), with MongoDB we instead store JSON-like documents using dynamic schemas. One of the goals of Mongo is to try bridging the gap between key-value stores (speed, scalability) and [relational](http://en.wikipedia.org/wiki/Relational_database) databases (rich functionality).

Mongoose is a JavaScript library that simplifies how we interact with Mongo. Like Express, it's designed to work within the node.js environment and tries to solve some of the complexities with asynchronous data storage by offering a more user-friendly API. It also adds chaining features into the mix, allowing for a slightly more expressive way of dealing with our data.

Jade is a template engine influenced by Haml (which we'll be looking at later). It's implemented with JavaScript (and also runs under node). In addition to supporting Express out of the box, it boasts a number of useful features including support for mixins, includes, caching, template inheritance and much more. Whilst abstractions like Jade certainly aren't for everyone, our practical will cover working both with and without it.


###Practical

For this practical, we're going to once again look at extending the popular Backbone Todo application. Rather than relying on localStorage for data persistance, we're going to switch to storing Todos in a MongoDB document-store instead. The code for this practical can be found at: https://github.com/addyosmani/backbone-boilerplates/tree/master/option2


**app.js**

(See [here](https://github.com/addyosmani/backbone-boilerplates/blob/master/option2/app.js) for the source)

We must first include the node dependencies required by our application. These are Express, Mongoose and Path (a module containing utilities for dealing with file paths.

<pre>
var application_root = __dirname,
  express = require("express"),
  path = require("path"),
  mongoose = require('mongoose');
</pre>

Next, create a new Express server. `express.createServer()` is a simple way of creating an instance of express.HTTPServer, which we'll be using to pass in our routes.

<pre>
var app = express.createServer();
</pre>

After this, connect Mongoose up to a database (in our case, localhost should suffice). Should you require the ability to pass in authentication information, here's a sample containing all of the supported URL parameters: `mongodb://[username:password@]host1[:port1][,host2[:port2],...[,hostN[:portN]]][/[database][?options]]`

<pre>
mongoose.connect('mongodb://localhost/my_database');
</pre>

A Mongoose model for any Todo item can now be easily defined by passing a schema instance to `mongoose.model`. In our case the schema covers a Todo item's `text` content, its `done` state and `order` position in the overall Todo list.

<pre>
var Todo = mongoose.model('Todo', new mongoose.Schema({
  text: String,
  done: Boolean,
  order: Number
}));
</pre>

The `configure()` methods allows us to setup what we need for the current environment with our Express server. Note that lower down in the configuration are two view/view related lines. The last one explicitly sets the viewing/templating engine to be used as Jade `app.set('view engine', 'jade')`. We can avoid these if we wish to use plain HTML/JS for our templates instead.

<pre>
app.configure(function(){
  // the bodyParser middleware parses JSON request bodies
  app.use(express.bodyParser());
  app.use(express.methodOverride());
  app.use(app.router);
  app.use(express.static(path.join(application_root, "public")));
  app.use(express.errorHandler({ dumpExceptions: true, showStack: true }));
  app.set('views', path.join(application_root, "views"));
  app.set('view engine', 'jade')
});

</pre>

Should you prefer to switch out Jade for an alternative view engine, this can be done fairly trivially. See the section under 'Templating' here:
https://github.com/joyent/node/wiki/modules. For example, to switch to EJS, you would simply write `app.set('view engine', 'ejs')`


Express makes use of commong HTTP verbs (get, put, post etc.) to provide easy to use, expressive routing API based on CRUD (Create, Read, Update and Delete). Below for example, we can define what happens when the browser requests the root '/'. As a trivial route in this application, it doesn't do anything particularly exciting, however getters typically read or retrieve data.

<pre>
app.get('/', function(req, res){
  res.send('Hello World');
});
</pre>

Onto something a little more useful and in our next route, navigating to '/todo' will actually render our Jade view 'todo.jade', as seen in the callback. Additional configuration values can be passed as the second parameter, such as the custom title specified below.

<pre>
app.get('/todo', function(req, res){
  res.render('todo', {title: "Our sample application"});
});
</pre>

Next, we can see the first of our '/api/' routes. 

<pre>
app.get('/api/todos', function(req, res){
  return Todo.find(function(err, todos) {
    return res.send(todos);
  });
});
</pre>

The calback to our next route supports querying for todos based on a specific ID. The route string itself (once compiled) will be converted from '/api/todos/:id' to a regular expression. As you might have guessed, this is a hint that routes can also be regular expression literals if we wished to do something more complex.

<pre>
app.get('/api/todos/:id', function(req, res){
  return Todo.findById(req.params.id, function(err, todo) {
    if (!err) {
      return res.send(todo);
    }
  });
});
</pre>

Simplarly, we want to support updating todos based on a specific ID as well. The following allows us to query a todo by ID and then update the values of it's three attributes (text, done, order) easily. 

<pre>
app.put('/api/todos/:id', function(req, res){
  return Todo.findById(req.params.id, function(err, todo) {
    todo.text = req.body.text;
    todo.done = req.body.done;
    todo.order = req.body.order;
    return todo.save(function(err) {
      if (!err) {
        console.log("updated");
      }
      return res.send(todo);
    });
  });
});
</pre>

We've so far covered requesting todos and updating them, but a core part of the application requires us to insert (or add) new todos to our data-store. Below we can create new <code>Todo</code> models and simply save them.

<pre>
app.post('/api/todos', function(req, res){
  var todo;
  todo = new Todo({
    text: req.body.text,
    done: req.body.done,
    order: req.body.order
  });
  todo.save(function(err) {
    if (!err) {
      return console.log("created");
    }
  });
  return res.send(todo);
});
</pre>

We of course also want to support deleting todos (e.g if a todo has been 'cleared', it should be deleted). This also works based on a specific todo ID.

<pre>
app.delete('/api/todos/:id', function(req, res){
  return Todo.findById(req.params.id, function(err, todo) {
    return todo.remove(function(err) {
      if (!err) {
        console.log("removed");
        return res.send('')
      }
    });
  });
});
</pre>

Finally, this last line is to ensure we're only listening on the port app.js is running.

<pre>
app.listen(3000);
</pre>

**script.js - updating our Backbone.js app**

In the `/public/js` folder of options 1 (HTML templates) and 2 (Jade) for the practical, you'll find a version of the Backbone Todo app originally by Jerome Gravel-Niquet. Let's pay attention to [script.js](https://github.com/addyosmani/backbone-boilerplates/blob/master/option2/public/js/script.js). In order to change the application to work with our new back-end, we'll need to make some very minor changes to this.

Reviewing `window.TodoList` (a Backbone Collection), you'll notice that it has a property called `localStorage`, which uses the Backbone [localStorage](https://github.com/jeromegn/Backbone.localStorage) adapter in order to facilitate storing data using the browser's localStorage features.

<pre>
window.TodoList = Backbone.Collection.extend({

    // Reference to this collection's model.
    model: Todo,

    // Save all of the todo items under the `"todos"` namespace.
    // Typically, this should be a unique name within your application
    localStorage: new Store("todos"),
</pre>

In order to switch it over to our RESTful backend, we're going to make use of the `url` property or function on a collection to reference its location on the server. Models inside of a collection then use `url` to construct URLs of their own. As all of the CRUD for our RESTful API works on the base route '/api/todos', this is the value we set `url` to.

<pre>
    // localStorage: new Store("todos"),
    url: '/api/todos',
</pre>

This is the only change necessary to our existing Backbone application in order to get things working. Pretty easy, right?



**todo.jade**

The Jade templates for our application cover declarative markup for both the index (layout.jade) of the application and the main Todo container (todo.jade). It also covers the script-tag templates used for rendering each new Todo item that's added.

<pre>
// Todo App Interface

#todoapp
  .title
    h1 Todos
  .content
    #create-todo
      input#new-todo(placeholder=&quot;What needs to be done?&quot;, type=&quot;text&quot;)
      span.ui-tooltip-top(style=&quot;display:none;&quot;) Press Enter to save this task
    #todos
      ul#todo-list
    #todo-stats


// Templates
script#item-template(type=&quot;text/template&quot;)
  &lt;div class=&quot;todo &lt;%= done ? &#39;done&#39; : &#39;&#39; %&gt;&quot;&gt;
  .display
    &lt;input class=&quot;check&quot; type=&quot;checkbox&quot; &lt;%= done ? &#39;checked=&quot;checked&quot;&#39; : &#39;&#39; %&gt; /&gt;
    .todo-text 
    span#todo-destroy
  .edit
    input.todo-input(type=&quot;text&quot;, &quot;value&quot;=&quot;&quot;)
  &lt;/div&gt;

script#stats-template(type=&quot;text/template&quot;)
  &lt;% if (total) { %&gt;
  span.todo-count
    span.number &lt;%= remaining %&gt; 
    span.word &lt;%= remaining == 1 ? &#39;item&#39; : &#39;items&#39; %&gt;
    |  left.
  &lt;% } %&gt;
  &lt;% if (done) { %&gt;
  span.todo-clear
    a(href=&quot;#&quot;)
      |  Clear
      span.number-done &lt;%= done %&gt;
      |  completed
      span.word-done &lt;%= done == 1 ? &#39;item&#39; : &#39;items&#39; %&gt;
  &lt;% } %&gt;
</pre>

**layout.jade**

<pre>
!!! 5
//if lt IE 8
  &lt;html class=&quot;no-js ie6 oldie&quot; lang=&quot;en&quot;&gt; 
//if IE 7
  &lt;html class=&quot;no-js ie7 oldie&quot; lang=&quot;en&quot;&gt;
//if IE 8
  &lt;html class=&quot;no-js ie8 oldie&quot; lang=&quot;en&quot;&gt;
//if gt IE 8
  &lt;!--&gt; &lt;html class=&quot;no-js&quot; lang=&quot;en&quot;&gt; &lt;!--
head
  meta(charset=&quot;utf-8&quot;)
  meta(http-equiv=&quot;X-UA-Compatible&quot;, content=&quot;IE=edge,chrome=1&quot;)

  title=title
  meta(name=&quot;description&quot;, content=&quot;&quot;)
  meta(name=&quot;author&quot;, content=&quot;&quot;)
  meta(name=&quot;viewport&quot;, content=&quot;width=device-width,initial-scale=1&quot;)
  
  // CSS concatenated and minified via ant build script
  link(rel=&quot;stylesheet&quot;, href=&quot;css/style.css&quot;)
  // end CSS

  script(src=&quot;js/libs/modernizr-2.0.6.min.js&quot;)
body

  #container
    header
    #main(role=&quot;main&quot;)!=body
    footer
  //! end of #container

  script(src=&quot;//ajax.googleapis.com/ajax/libs/jquery/1.6.2/jquery.min.js&quot;)
  script
    window.jQuery || document.write(&#39;&lt;script src=&quot;js/libs/jquery-1.6.2.min.js&quot;&gt;&lt;\\/script&gt;&#39;)

  // scripts concatenated and minified via ant build script
  script(src=&quot;js/mylibs/underscore.js&quot;)
  script(src=&quot;js/mylibs/backbone.js&quot;)
  script(defer, src=&quot;js/plugins.js&quot;)
  script(defer, src=&quot;js/script.js&quot;)
  // end scripts

  // Change UA-XXXXX-X to be your site&#39;s ID
  script
    window._gaq = [[&#39;_setAccount&#39;,&#39;UAXXXXXXXX1&#39;],[&#39;_trackPageview&#39;],[&#39;_trackPageLoadTime&#39;]];
    Modernizr.load({load: (&#39;https:&#39; == location.protocol ? &#39;//ssl&#39; : &#39;//www&#39;) + &#39;.google-analytics.com/ga.js&#39;});

  //if lt IE 7
    script(src=&quot;//ajax.googleapis.com/ajax/libs/chrome-frame/1.0.3/CFInstall.min.js&quot;)
    script
      window.attachEvent(&#39;onload&#39;,function(){CFInstall.check({mode:&#39;overlay&#39;})})
&lt;/html&gt;

</pre>


**static.html**

Alternatively, a static version of our index which doesn't rely on Jade can be put together as follows. See [here](https://github.com/addyosmani/backbone-boilerplates/blob/master/option1/public/static.html) for the complete file or below for a sample.

<pre>
 &lt;div id=&quot;container&quot;&gt;
    &lt;div id=&quot;main&quot; role=&quot;main&quot;&gt;

      &lt;!-- Todo App Interface--&gt;

      &lt;div id=&quot;todoapp&quot;&gt;
        &lt;div class=&quot;title&quot;&gt;
          &lt;h1&gt;Todos&lt;/h1&gt;
        &lt;/div&gt;

        &lt;div class=&quot;content&quot;&gt;
          &lt;div id=&quot;create-todo&quot;&gt;
            &lt;input id=&quot;new-todo&quot; placeholder=&quot;What needs to be done?&quot; type=
            &quot;text&quot; /&gt;&lt;span style=&quot;display:none;&quot; class=&quot;ui-tooltip-top&quot;&gt;Press Enter to
            save this task&lt;/span&gt;
          &lt;/div&gt;

          &lt;div id=&quot;todos&quot;&gt;
            &lt;ul id=&quot;todo-list&quot;&gt;&lt;/ul&gt;
          &lt;/div&gt;

          &lt;div id=&quot;todo-stats&quot;&gt;&lt;/div&gt;
        &lt;/div&gt;
      &lt;/div&gt;


    &lt;!-- Templates--&gt;

      &lt;script id=&quot;item-template&quot; type=&quot;text/template&quot;&gt;
      &lt;div class=&quot;todo &lt;%= done ? &#39;done&#39; : &#39;&#39; %&gt;&quot;&gt;
      &lt;div class=&quot;display&quot;&gt;&lt;input class=&quot;check&quot; type=&quot;checkbox&quot; &lt;%= done ? &#39;checked=&quot;checked&quot;&#39; : &#39;&#39; %&gt; /&gt;
      &lt;div class=&quot;todo-text&quot;&gt;&lt;/div&gt;&lt;span id=&quot;todo-destroy&quot;&gt;&lt;/span&gt;&lt;/div&gt;&lt;div class=&quot;edit&quot;&gt;&lt;input type=&quot;text&quot; value=&quot;&quot; class=&quot;todo-input&quot;/&gt;&lt;/div&gt;&lt;/div&gt;
      &lt;/script&gt;

      &lt;script id=&quot;stats-template&quot; type=&quot;text/template&quot;&gt;
      &lt;% if (total) { %&gt;
      &lt;span class=&quot;todo-count&quot;&gt;&lt;span class=&quot;number&quot;&gt;&lt;%= remaining %&gt; &lt;/span&gt;&lt;span class=&quot;word&quot;&gt;&lt;%= remaining == 1 ? &#39;item&#39; : &#39;items&#39; %&gt;&lt;/span&gt; left.
      &lt;/span&gt;&lt;% } %&gt;
      &lt;% if (done) { %&gt;
      &lt;span class=&quot;todo-clear&quot;&gt;&lt;a href=&quot;#&quot;&gt; Clear
      &lt;span class=&quot;number-done&quot;&gt;&lt;%= done %&gt;&lt;/span&gt; completed
      &lt;span class=&quot;word-done&quot;&gt;&lt;%= done == 1 ? &#39;item&#39; : &#39;items&#39; %&gt;&lt;/span&gt;&lt;/a&gt;&lt;/span&gt;&lt;% } %&gt;
      &lt;/script&gt;

    &lt;/div&gt;
  &lt;/div&gt;

  &lt;!--! end of #container--&gt;
</pre>


### Practical Setup

We've now gone through the major points of developing a RESTful backend using Node.js, Express and Mongoose. Next, let's make sure you can get your environment setup to run the updated Todo app.

####MongoDB

Once you've downloaded [MongoDB](http://www.mongodb.org/downloads), you'll need to complete two steps to get it up and running.

**Data directories**

MongoDB stores data in the bin/data/db folder but won't actually create this directory for you. Navigate to where you've downloaded and extracted MongoDB and run the following from terminal:

<pre>
sudo mkdir -p /data/db/
sudo chown `id -u` /data/db
</pre>

**Running and connecting to your server**

Once this is done, open up two terminal windows. 

In the first, `cd` to your MongoDB bin directory or type in the complete path to it. You'll need to start <cpde>mongod`.

<pre>
$ ./bin/mongod
</pre>

Next, in the second terminal, start the `mongo</code shell which will connect up to localhost by default.

<pre>
$ ./bin/mongo
</pre>

That's it!.

####Express and Mongoose

Option 1 (HTML) and Option 2 (Jade) of the practical download both come with an install.sh bash script. This allows you to easily install Express, Mongoose, Jade (and optionally MongoDB if you prefer to) through npm (the node package manager). 

* Make sure you have Node.js installed. If not, you can grab it [here](http://nodejs.org/#download)
* Next run `$ ./install.sh` at the terminal to install the rest of our dependencies. To see the exact contents of the install.sh file, see below:

**install.sh**

<pre>
#!/bin/bash
npm install express
npm install mongodb --mongodb:native
npm install mongoose
npm install jade
</pre>


* After you've installed all of the dependencies for the stack, we can get to cloning the repo containing our practicals and running them. Start by running the below lines:
   
<pre>
git clone git://github.com/addyosmani/backbone-boilerplates.git
cd option2
node app.js
</pre>

For option1 (without Jade), simply cd into option1 and run `node app.js` from there.

Finally, either of the example apps can now be accessed by navigating to:

* Option 1: `http://localhost:3000/static.html`
* Option 2: `http://localhost:3000/todo`

That's it! Whilst there's a lot more than can be done to expand on the concepts covered so far, the base we're reviewed should be enough to get you up and running with this stack if you wish to use them with Backbone. 

In the next addition to this section (coming soon), we'll take a look at a stack consisting of Ruby, Sinatra, Haml and more.




