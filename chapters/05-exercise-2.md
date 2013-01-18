# Exercise 2: Book Library - Your first RESTful Backbone.js app


*Credits: Björn Ekengren, Addy Osmani*

##Part 1

In this exercise we will build a library application for managing digital books using Backbone.


###Setting up

First we need a simple folder structure for our project. Create the directories css, img and js in your project root folder.

Download the Backbone, Underscore and jQuery libraries and copy them to your js folder. Save this image to your img folder:

![](http://codebyexample.info/wp-content/uploads/2012/03/placeholder.png)

Create a new file index.html in the root of your project folder and enter this:

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8"/>
    <title>Backbone.js Library</title>
    <link rel="stylesheet" href="css/screen.css">
</head>
<body>
<div id="books">
    <div class="bookContainer">
        <img src="img/placeholder.png"/>
        <ul>
            <li>Title</li>
            <li>Author</li>
            <li>Release date</li>
            <li>Keywords</li>
        </ul>
    </div>
</div>
<script src="js/jquery-1.7.1.js"></script>
<script src="js/underscore.js"></script>
<script src="js/backbone.js"></script>
<script src="js/app.js"></script>
</body>
</html>
```

open this file in a browser and it should look something like this:

![](http://codebyexample.info/wp-content/uploads/2012/03/Screen-Shot-2012-03-05-at-12.57.08-AM1.png)

Not so awesome. This is not a CSS tutorial, but we need to do some formatting. Create a file screen.css in your css folder and type this:

```css
body {
    background-color: #eee;
}

.bookContainer {
    border: 1px solid #aaa;
    width: 300px;
    height: 130px;
    background-color: #fff;
    float: left;
    margin: 5px;
}
.bookContainer img {
    float: left;
    margin: 10px;
}
.bookContainer ul {
    list-style-type: none;
}
```

Now it looks a bit better:

![](http://codebyexample.info/wp-content/uploads/2012/03/Screen-Shot-2012-03-05-at-1.06.15-AM.png)


So this is what we want the final result to look like, but with more books. Go ahead and copy the bookContainer div a couple of times if you would like to see what it looks like. Now we are ready to start developing the actual application. Open up app.js and enter this:

```javascript
(function ($) {

    var Book = Backbone.Model.extend({
        defaults:{
            coverImage:"img/placeholder.png",
            title:"Some title",
            author:"John Doe",
            releaseDate:"2012",
            keywords:"JavaScript Programming"
        }
    });

})(jQuery);
```

This is our model of a book. It contains a defaults property that gives us default values for all the values we want our model to contain. Note that I have wrapped everything in a self executing/invoking function with the argument “jQuery”. This is standard procedure to prevent polluting the global scope and to avoid any collisions with the $ variable. The model itself isn’t very useful so we need to pair it with a View. For the view we need a template so open up index.html and create a template from our bookContainer we created earlier:

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8"/>
    <title>Backbone.js Library</title>
    <link rel="stylesheet" href="css/screen.css">
</head>
<body>
<div id="books">
    <div class="bookContainer">
        <img src="img/placeholder.png"/>
        <ul>
            <li>Title</li>
            <li>Author</li>
            <li>Release date</li>
            <li>Keywords</li>
        </ul>
    </div>
    <script id="bookTemplate" type="text/template">
        <img src="<%= coverImage %>"/>
        <ul>
            <li><%= title %></li>
            <li><%= author %></li>
            <li><%= releaseDate %></li>
            <li><%= keywords %></li>
        </ul>
    </script>
</div>
<script src="js/jquery-1.7.1.js"></script>
<script src="js/underscore.js"></script>
<script src="js/backbone.js"></script>
<script src="js/app.js"></script>
</body>
</html>
```

So what’s going on here? Well, I have wrapped the template in a script tag with the type “text/template”. By doing this the browser will not recognize the type and not render it, but we can still access all the elements in JavaScript. I have removed the surrounding div tag since the templating engine (Underscore) will create that for us. In the various fields I have inserted <%= field %> that Underscore will find and replace with real data. Now that we have our template in place we can go and create the view, so switch over to app.js:

```javascript
(function ($) {

    var Book = Backbone.Model.extend({
        defaults:{
            coverImage:"img/placeholder.png",
            title:"Some title",
            author:"John Doe",
            releaseDate:"2012",
            keywords:"JavaScript Programming"
        }
    });

    var BookView = Backbone.View.extend({
        tagName:"div",
        className:"bookContainer",
        template:$("#bookTemplate").html(),

        render:function () {
            var tmpl = _.template(this.template); //tmpl is a function that takes a JSON object and returns html

            this.$el.html(tmpl(this.model.toJSON())); //this.el is what we defined in tagName. use $el to get access to jQuery html() function
            return this;
        }
    });

})(jQuery);
```

So the view works like the model in that we use the extend function and pass it properties. The tagName property defines the container of the view, the className defines the class of the container and template is, well, the template for the view. The render function creates the tmpl function by calling underscores template function with one argument (the template from our index.html). It then calls the `tmpl` function with our model… wait a minute! What model? Well, this is something that we need to provide as an argument when we call our view constructor. As it looks now our view isn’t connected to any element in our html page either, so let’s take care of that now:

```javascript
(function ($) {

    var Book = Backbone.Model.extend({
        defaults:{
            coverImage:"img/placeholder.png",
            title:"Some title",
            author:"John Doe",
            releaseDate:"2012",
            keywords:"JavaScript Programming"
        }
    });

    var BookView = Backbone.View.extend({
        tagName:"div",
        className:"bookContainer",
        template:$("#bookTemplate").html(),

        render:function () {
            var tmpl = _.template(this.template); //tmpl is a function that takes a JSON object and returns html

            this.$el.html(tmpl(this.model.toJSON())); //this.el is what we defined in tagName. use $el to get access to jQuery html() function
            return this;
        }
    });

    var book = new Book({
        title:"Some title",
        author:"John Doe",
        releaseDate:"2012",
        keywords:"JavaScript Programming"
    });

    bookView = new BookView({
        model:book
    });

    $("#books").html(bookView.render().el);

})(jQuery);
```

Here we have created a new Book model by calling the Book constructor and passing an object with our desired properties. The model is then used when creating a BookView. Finally the bookView is rendered and inserted into our page. It should look something like this:

![](http://codebyexample.info/wp-content/uploads/2012/03/Screen-Shot-2012-03-05-at-10.53.34-PM.png)


Looking good, we now have a working Backbone application. Since a library cannot contain just one book, we need to push further. Lets have a look at Collections. Collections contain collections of models. The models must be of the same kind, i.e. you cannot mix apples and oranges in the same collection. Other than that, collections are quite simple, you just tell them what kind of models they contain like this:

```javascript
var Library = Backbone.Collection.extend({
    model:Book
});
```

Go ahead and insert the code in app.js. Next stop is a new view for our Library collection. This view is a bit more complicated than the earlier BookView:

```javascript
var LibraryView = Backbone.View.extend({
    el:$("#books"),

    initialize:function(){
        this.collection = new Library(books);
        this.render();
    },

    render:function(){
        var that = this;
        _.each(this.collection.models, function(item){
            that.renderBook(item);
        }, this);
    },

    renderBook:function(item){
        var bookView = new BookView({
            model:item
        });
        this.$el.append(bookView.render().el);
    }
});
```

I added some line numbers to help me explain what is going on. On line 02 I specified the property “el”. A view can take either a tagName, as we saw in our `BookView`, or an el. When we use tagName, the view will create the element for us, but we are responsible for inserting it into the page. When we use el we specify an existing element in the page and the view will write directly into the page, into the specified element. In this case we select the div with id=”books”.

Next on line 04 is the initialize function. This function will be called by Backbone when the view constructor is called. On line 05 in we create a new Library (collection of Book models) and store it in a local property called “collection”. On line 06 it calls its own render function, which means that as soon as we call the `LibraryView` constructor it will get rendered, so this is a self rendering view. We don’t have to make it self rendered but it is common practice.

On line 10 in the render function we store a reference to the current object in a variable “that” and then (on line 11) use the each function of underscore to iterate over all the models (Books) in our collection. The first argument to “each” is the array that will be iterated over. The second argument is the function that will be applied to each member of the array. The function in our case calls the renderBook function with the current model as argument. We need to use “that” to get this right since if we would have used “this” it would have referenced the function itself. The third argument (line 13) is the context that each is executing in.

On line 16 we define a function `renderBook` that takes a model (a Book) as argument and uses it to create a `BookView`. The `bookView` is then rendered and appended to the view container as specified in our el property (on line 02).

You may have noticed that on line 05 we called the Library constructor with the argument “books”. Library is a Backbone collection that expects an array of  objects that it can use to create Book models. We haven’t defined the books variable yet so lets go ahead and do that:

```javascript
var books = [{title:"JS the good parts", author:"John Doe", releaseDate:"2012", keywords:"JavaScript Programming"},
    {title:"CS the better parts", author:"John Doe", releaseDate:"2012", keywords:"CoffeeScript Programming"},
    {title:"Scala for the impatient", author:"John Doe", releaseDate:"2012", keywords:"Scala Programming"},
    {title:"American Psyco", author:"Bret Easton Ellis", releaseDate:"2012", keywords:"Novel Splatter"},
    {title:"Eloquent JavaScript", author:"John Doe", releaseDate:"2012", keywords:"JavaScript Programming"}];
```

Now we are almost ready to try out our first version of the Backbone library. Replace this code:

```javascript
var book = new Book({
    title:"Some title",
    author:"John Doe",
    releaseDate:"2012",
    keywords:"JavaScript Programming"
});

bookView = new BookView({
    model:book
});

$("#books").html(bookView.render().el);
```

with this:

```javascript
var libraryView = new LibraryView();
```

If you view index.html in a browser you should see something like this:

![](http://codebyexample.info/wp-content/uploads/2012/03/Screen-Shot-2012-03-06-at-9.58.03-AM.png)

Here is the final app.js:

```javascript
(function ($) {
    var books = [{title:"JS the good parts", author:"John Doe", releaseDate:"2012", keywords:"JavaScript Programming"},
        {title:"CS the better parts", author:"John Doe", releaseDate:"2012", keywords:"CoffeeScript Programming"},
        {title:"Scala for the impatient", author:"John Doe", releaseDate:"2012", keywords:"Scala Programming"},
        {title:"American Psyco", author:"Bret Easton Ellis", releaseDate:"2012", keywords:"Novel Splatter"},
        {title:"Eloquent JavaScript", author:"John Doe", releaseDate:"2012", keywords:"JavaScript Programming"}];

    var Book = Backbone.Model.extend({
        defaults:{
            coverImage:"img/placeholder.png",
            title:"Some title",
            author:"John Doe",
            releaseDate:"2012",
            keywords:"JavaScript Programming"
        }
    });

    var Library = Backbone.Collection.extend({
       model:Book
    });

    var BookView = Backbone.View.extend({
        tagName:"div",
        className:"bookContainer",
        template:$("#bookTemplate").html(),

        render:function () {
            var tmpl = _.template(this.template); //tmpl is a function that takes a JSON object and returns html

            this.$el.html(tmpl(this.model.toJSON())); //this.el is what we defined in tagName. use $el to get access to jQuery html() function
            return this;
        }
    });

 var LibraryView = Backbone.View.extend({
        el:$("#books"),

        initialize:function(){
            this.collection = new Library(books);
            this.render();
        },

        render:function(){
            var that = this;
            _.each(this.collection.models, function(item){
                that.renderBook(item);
            }, this);
        },

        renderBook:function(item){
            var bookView = new BookView({
                model:item
            });
            this.$el.append(bookView.render().el);
        }
    });

    var libraryView = new LibraryView();

})(jQuery);
```

So lets recap what happens here:

* Line 58: the LibraryView constructor is called
* Line 36: the LibraryView is tied to the books div in our html
* Line 39: the Library constructor is called with the array of book properties from line 02, creating a collection of Book models
* Line 43: The render function iterates over the Book collection and calls renderBook (line 46) for each Book model
* Line 51: renderBook function creates a new BookView from the given Book model
* Line 54: The render function of BookView is called and the result is appended to the books div in our html

You have now been introduced to the basic components of Backbone Model, View and Collection. In the next part I will continue to develop our Library into something more interactive.

The code for this tutorial is available [here](https://dl.dropbox.com/u/70775642/workshop-practical/code.zip).

##Part 2

In the first part, I covered some Backbone basics of models, views and collections. I recommend reading that before continuing since this tutorial builds on the first part. In this tutorial we are going to look at how we can add and remove models from a collection.

###Adding models

To add a new book we need to have some sort of input form. Go ahead and add one in index.html

```html
<div id="books">
    <form id="addBook" action="#">
        <div>
            <label for="coverImage">CoverImage: </label><input id="coverImage" type="file" />
            <label for="title">Title: </label><input id="title" type="text" />
            <label for="author">Author: </label><input id="author" type="text" />
            <label for="releaseDate">Release date: </label><input id="releaseDate" type="text" />
            <label for="keywords">Keywords: </label><input id="keywords" type="text" />
            <button id="add">Add</button>
        </div>
    </form>
</div>
```

I made the ids of the inputs match the attributes in our Book model so that we don’t have to remap those later. Add this to screen.css for looks

```css
#addBook label {
    width:100px;
    margin-right:10px;
    text-align:right;
    line-height:25px;
}
 
#addBook label, #addBook input {
    display:block;
    margin-bottom:10px;
    float:left;
}
 
#addBook label[for="title"], #addBook label[for="releaseDate"] {
    clear:both;
}
 
#addBook button {
    display:block;
    margin:5px 20px 10px 10px;
    float:right;
    clear:both;
}
 
#addBook div {
    width:550px;
}
 
#addBook div:after {
    content:"";
    display:block;
    height:0;
    visibility:hidden;
    clear:both;
    font-size:0;
    line-height:0;
}
```

Now lets create a function that lets us add a book. We put it in our master view (LibraryView) after the render function

```javascript
addBook:function(){
    var formData = {};
 
    $("#addBook div").children("input").each(function(i, el){
        formData[el.id] = $(el).val();
    });
 
    books.push(formData);
 
    this.collection.add(new Book(formData));
},
```

Here I select all the input elements of the form and iterate over them using jQuery's each. Since we used the same names for ids in our form as the keys on our Book model we can simply store them directly in the formData object and add it to the books array. We then create a new Book model and add it to our collection. Now lets wire this function to the Add button in our form. We do this by adding an event listener in the LibraryView

```javascript
events:{
    "click #add":"addBook"
},
```

By default, Backbone will send an event object as parameter to the function. This is useful for us in this case since we want to prevent the form from actually submit and reloading the page. Add a preventDefault to the addBook function

```javascript
addBook:function(e){
    e.preventDefault();
 
    var formData = {};
 
    $("#addBook div").children("input").each(function(i, el){
        formData[el.id] = $(el).val();
    });
 
    books.push(formData);
 
    this.collection.add(new Book(formData));
},
```

Ok, so this will add a book to our books array and our collection. It will not however be visible, because we have not called the render function of our BookView. When a model is added to a collection, the collection will fire an add event. If we listen to this event we will be able to call our renderBook function. This binding is done in initialize.

```javascript
initialize:function () {
    this.collection = new Library(books);
    this.render();
 
    this.collection.on("add", this.renderBook, this);
},
```

Now you should be ready to take the application for a spin.

![](http://codebyexample.info/wp-content/uploads/2012/04/Screen-Shot-2012-04-05-at-1.24.51-AM.png)

As you may notice, if you leave a field blank, it will be blank in the created view as well. This is not what we want, we would like the default values to kick in. To do that we need to add a bit of logic. Also note that the file input for the cover image isn’t working, but that is left as an exercise to the reader.

```javascript
addBook:function (e) {
    e.preventDefault();
 
    var formData = {};
 
    $("#addBook div").children("input").each(function (i, el) {
        if ($(el).val() !== "") {
            formData[el.id] = $(el).val();
        }
    });
 
    books.push(formData);
 
    this.collection.add(new Book(formData));
},
```

Here I added (line 7) a check to see if the field value is empty, in which case we do not add it to the model data. While we are at it lets add default values for the other properties of Book

```javascript
var Book = Backbone.Model.extend({
    defaults:{
        coverImage:"img/placeholder.png",
        title:"No title",
        author:"Unknown",
        releaseDate:"Unknown",
        keywords:"None"
    }
});
```

Now it has better default behaviour

![](http://codebyexample.info/wp-content/uploads/2012/04/Screen-Shot-2012-04-05-at-11.41.59-AM1.png)

###Removing models

Now lets see how to remove Books. We start by adding a delete button to the template in index.html

```html
<script id="bookTemplate" type="text/template">
    <img src="<%= coverImage %>" />
    <ul>
        <li><%= title %></li>
        <li><%= author %></li>
        <li><%= releaseDate %></li>
        <li><%= keywords %></li>
    </ul>

    <button class="delete">Delete</button>
</script>
```

We add some css to it for good looks. Note that I removed the margin of the existing ul rule above to tighten things up a bit.

```css
.bookContainer ul {
    list-style-type:none;
    margin-bottom:0;
}
 
.bookContainer button {
    float:right;
    margin:10px;
}
```

Looks okay!

![](http://codebyexample.info/wp-content/uploads/2012/04/Screen-Shot-2012-04-05-at-11.59.54-AM.png)

Now we need to wire up the button to the logic. This works in the same way as with add. We start by creating a deleteBook function in the BookView

```javascript
var BookView = Backbone.View.extend({
    tagName:"div",
    className:"bookContainer",
    template:$("#bookTemplate").html(),
 
    render:function () {
        var tmpl = _.template(this.template); //tmpl is a function that takes a JSON and returns html
 
        this.$el.html(tmpl(this.model.toJSON())); //this.el is what we defined in tagName. use $el to get access to jQuery html() function
        return this;
    },
 
    deleteBook:function () {
        //Delete model
        this.model.destroy();
 
        //Delete view
        this.remove();
    }
});
```

Then we add an event listener to the delete button

```javascript
var BookView = Backbone.View.extend({
    tagName:"div",
    className:"bookContainer",
    template:$("#bookTemplate").html(),
 
    render:function () {
        var tmpl = _.template(this.template); //tmpl is a function that takes a JSON and returns html
 
        this.$el.html(tmpl(this.model.toJSON())); //this.el is what we defined in tagName. use $el to get access to jQuery html() function
        return this;
    },
 
    events: {
        "click .delete":"deleteBook"
    },
 
    deleteBook:function () {
        //Delete model
        this.model.destroy();
 
        //Delete view
        this.remove();
    }
});
```

If you try it out now you will see that it seems to work. However we are not yet finished. When we click the delete button the model and view will be deleted, but not the data in our books array. This means that if we were to re-render the LibraryView the deleted books would reappear. The Backbone collection is smart enough to notice when one of its models is deleted and will fire a “remove” event. This is something we can listen to in our LibraryView and take action. Add a listener in initialize of LibraryView

```javascript
initialize:function () {
    this.collection = new Library(books);
    this.render();
 
    this.collection.on("add", this.renderBook, this);
    this.collection.on("remove", this.removeBook, this);
},
```

Here I specified that the removeBook function should be called when the remove event from our collection fires, so lets create this function. Note that the collection provides the removed model as a parameter to the event.

```javascript
removeBook:function(removedBook){
    var removedBookData = removedBook.attributes;
 
    _.each(removedBookData, function(val, key){
        if(removedBookData[key] === removedBook.defaults[key]){
            delete removedBookData[key];
        }
    });
 
    _.each(books, function(book){
        if(_.isEqual(book, removedBookData)){
            books.splice(_.indexOf(books, book), 1);
        }
    });
},
```

Since the default values were not saved in the books array we need to remove them in order to find a match. We use underscores each function to iterate over the properties of the removed Book model and delete any property that is equal to the default value. Since the underscore each function is also capable of iterating over objects in an array we use it again to iterate over the objects in our books array to find the data of the removed Book. To get a match we use the isEqual function of underscore that performs a deep comparison of objects. Similarly the indexOf can find complex objects, which we use when we remove the book data using splice.

###Summary

In this tutorial we have looked at how to add and remove models from a collection. We also looked at how to wire view, model and collection together using event handlers. We also saw the useful underscore functions each, indexOf and isEqual.

The code for this part is available [here](https://dl.dropbox.com/u/70775642/workshop-practical/code.zip).

##Part 2.5

In the first two parts of this tutorial series we looked at the basic structure of a Backbone application and how to add and remove models. In the third part we will look at how to synchronize the models with the back end, but in order to do that we need to make a small detour and set up a server with a REST api. That is what we are going to do in this part. Since this is a JavaScript tutorial I will use JavaScript to create the server using node.js. If you are more comfortable in setting up a REST server in another language, this is the API you need to conform to:

```
url HTTP Method Operation
/api/books  GET Get an array of all books
/api/books/:id  GET Get the book with id of :id
/api/books  POST  Add a new book and return the book with an id attribute added
/api/books/:id  PUT Update the book with id of :id
/api/books/:id  DELETE  Delete the book with id of :id
```

The outline for this tutorial looks like this:

* Install node.js, npm and MongoDB
* Install node modules
* Create directory structure
* Create a simple web server
* Connect to the database
* Create the REST API

Download and install node.js from nodejs.org. The node package manager (npm) will be installed as well.

Download and install MongoDB from mongodb.org. When installing MongoDB you get instructions on copying a config file in order for MongoDB to run. On OSX it can look something like this:

```
sudo cp /usr/local/Cellar/mongodb/2.0.4-x86_64/mongod.conf /usr/local/etc/mongod.conf
```

Once it is installed we can go ahead and start it. On my machine I did this:

```
mongod run --config /usr/local/Cellar/mongodb/2.0.4-x86_64/mongod.conf
```

###Installing node modules

Start by creating a new folder for this project. Using a terminal, go to your project folder and run the following commands:

```
npm install express@2.5.9
npm install path
npm install mongoose
```

Note the @2.5.9 on express. The current stable is 3.0.3 and has a different API, but we will be using 2.5.9 for the time being.

###Create directory structure

In your project folder root, create a file server.js – this is where our server code will go. Then create a folder public. Anything within the public folder will be served by the express web server as static content to the client. Now go ahead and copy everything from Part 2 into the public folder. When you are done your folder structure should look something like this:

```
node_modules/
  .bin/
  express/
  mongoose/
  path/
public/
  css/
  img/
  js/
  index.html
server.js
package.json
```

###Create a simple web server

Open server.js and enter the following:

```javascript
// Module dependencies.
var application_root = __dirname,
    express = require("express"), //Web framework
    path = require("path"), //Utilities for dealing with file paths
    mongoose = require('mongoose'); //MongoDB integration
 
//Create server
var app = express.createServer();
 
// Configure server
app.configure(function () {
    app.use(express.bodyParser()); //parses request body and populates req.body
    app.use(express.methodOverride()); //checks req.body for HTTP method overrides
    app.use(app.router); //perform route lookup based on url and HTTP method
    app.use(express.static(path.join(application_root, "public"))); //Where to serve static content
    app.use(express.errorHandler({ dumpExceptions:true, showStack:true })); //Show all errors in development
});
 
//Start server
app.listen(4711, function () {
    console.log("Express server listening on port %d in %s mode", app.address().port, app.settings.env);
});
```

I start off by loading the modules required for this project: Express for creating the HTTP server, Path for dealing with file paths and mongoose for connecting with the database. We then create an express server and configure it using an anonymous function. This is a pretty standard configuration and for our application we don’t actually need the methodOverride part. It is used for issuing PUT and DELETE HTTP requests directly from a form, since forms normally only support GET and POST. Finally I start the server by running the listen function. The port number used, in this case 4711, could be any free port on your system. I simply used 4711 since it is the most random number. We are now ready to run our first server:

```javascript
node server.js
```

If you open a browser on localhost:4711 you should see something like this:

![](http://codebyexample.info/wp-content/uploads/2012/04/Screen-Shot-2012-04-05-at-11.59.54-AM1.png)

This is where we left off in Part 2, but we are now running on a server instead of directly from the files. Great job! We can now start defining routes (URLs) that the server should react to. This will be our REST API. Routes are defined by using app followed by one of the HTTP verbs get, put, post and delete, which corresponds to Create, Read, Update and Delete. Let us go back to server.js and define a simple route:

```javascript
// Routes
app.get('/api', function(req, res){
    res.send('Library API is running');
});
```

The get function will take the URL as first parameter and a function as second. The function will be called with request and response objects. Now you can restart node and go to our specified URL:

![](http://codebyexample.info/wp-content/uploads/2012/04/Screen-Shot-2012-04-23-at-10.09.35-AM.png)


###Connect to database

Fantastic. Now since we want to store our data in MongoDB we need to define a schema. Add this to server.js:

```javascript
//Connect to database
mongoose.connect('mongodb://localhost/library_database');

//Schemas
var Book = new mongoose.Schema({
    title:String,
    author:String,
    releaseDate:Date
});

//Models
var BookModel = mongoose.model('Book', Book);
```

As you can see, schema definitions are quite straight forward. They can be more advanced, but this will do for us. I also extracted a model (BookModel) from Mongo. This is what we will be working with. Next up we define a get operation for the REST API that will return all books:

```javascript
//Get a list of all books
app.get('/api/books', function (req, res) {
    return BookModel.find(function (err, books) {
        if (!err) {
            return res.send(books);
        } else {
            return console.log(err);
        }
    });
});
```

The find function of Model is defined like this: function find (conditions, fields, options, callback) – but since we want a function that return all books we only need the callback parameter. The callback will be called with an error object and an array of found objects. If there was no error we return the array of objects to the client using the send function of the result object, otherwise we log the error to the console.

To test our API we need to do a little typing in a JavaScript console. Restart node and go to localhost:4711 in your browser. Open up the JavaScript console. If you are using Google Chrome, go to View->Developer->JavaScript Console. If you are using Firefox, install Firebug and go to View->Firebug. If you are on any other browser I’m sure you will find a console somewhere. In the console type the following:

```javascript
jQuery.get("/api/books/", function (data, textStatus, jqXHR) {
    console.log("Get response:");
    console.dir(data);
    console.log(textStatus);
    console.dir(jqXHR);
});
```

…and press enter and you should get something like this:

![](http://codebyexample.info/wp-content/uploads/2012/04/Screen-Shot-2012-04-29-at-12.50.32-AM.png)


Here I used jQuery to make the call to our REST API, since it was already loaded on the page. The returned array is obviously empty, since we have not put anything into the database yet. Lets go and create a POST route that enables this in server.js:

```javascript
//Insert a new book
app.post('/api/books', function (req, res) {
    var book = new BookModel({
        title:req.body.title,
        author:req.body.author,
        releaseDate:req.body.releaseDate
    });
    book.save(function (err) {
        if (!err) {
            return console.log('created');
        } else {
            return console.log(err);
        }
    });
    return res.send(book);
});
```

We start by creating a new BookModel passing an object with title, author and releaseDate attributes. The data are collected from req.body. This means that anyone calling this operation in the API needs to supply a JSON object containing the title, author and releaseDate attributes. Actually, the caller can omit any or all attributes since we have not made any one mandatory. 

We then call the save function on the BookModel passing in an anonymous function for handling errors in the same way as with the previous get route. Finally we return the saved BookModel. The reason we return the BookModel and not just “success” or similar string is that when the BookModel is saved it will get an _id attribute from MongoDB, which the client needs when updating or deleting a specific book. Lets try it out again, restart node and go back to the console and type:

```javascript
jQuery.post("/api/books", {
  "title":"JavaScript the good parts",
  "author":"Douglas Crockford",
  "releaseDate":new Date(2008, 4, 1).getTime()
}, function(data, textStatus, jqXHR) {
    console.log("Post response:"); 
    console.dir(data); 
    console.log(textStatus); 
    console.dir(jqXHR);
});
```

..and then

```javascript
jQuery.get("/api/books/", function (data, textStatus, jqXHR) {
    console.log("Get response:");
    console.dir(data);
    console.log(textStatus);
    console.dir(jqXHR);
});
```

You should now get an array of size 1 back from our server. You may wonder about this line:

```javascript
"releaseDate":new Date(2008, 4, 1).getTime()
```

MongoDB expects dates in UNIX time format (milliseconds from Jan 1st 1970), so we have to convert dates before posting. The object we get back however, contains a JavaScript Date object. Also note the _id attribute of the returned object.

![](http://codebyexample.info/wp-content/uploads/2012/04/Screen-Shot-2012-04-29-at-1.16.58-AM.png)


Lets move on to creating a get that retrieves a single book in server.js:

```javascript
app.get('/api/books/:id', function(req, res){
    return BookModel.findById(req.params.id, function(err, book){
        if(!err){
            return res.send(book);
        } else {
            return console.log(err);
        }
    });
});
```

Here we use colon notation (:id) to tell express that this part of the route is dynamic. We also use the findById function on BookModel to get a single result. Now you can get a single book by adding its id to the URL like this:

```javascript
jQuery.get("/api/books/4f95a8cb1baa9b8a1b000006", function (data, textStatus, jqXHR){
    console.log("Get response:");
    console.dir(data);
    console.log(textStatus);
    console.dir(jqXHR);
});
```

Lets create the PUT (update) function next:

```javascript
app.put('/api/books/:id', function(req, res){
    console.log('Updating book ' + req.body.title);
    return BookModel.findById(req.params.id, function(err, book){
        book.title = req.body.title;
        book.author = req.body.author;
        book.releaseDate = req.body.releaseDate;
        return book.save(function(err){
            if(!err){
                console.log('book updated');
            } else {
                console.log(err);
            }
            return res.send(book);
        });
    });
});
```

This is a little larger than previous ones, but should be pretty straight forward – we find a book by id, update its properties, save it and send it back to the client.

To test this we need to use the more general jQuery ajax function (replace the id with what you got from a GET request):

```javascript
jQuery.ajax({
  url:"/api/books/4f95a8cb1baa9b8a1b000006",
  type:"PUT",
  data:{
    "title":"JavaScript The good parts",
    "author":"The Legendary Douglas Crockford",
    "releaseDate":new Date(2008, 4, 1).getTime()
  },
  success: function(data, textStatus, jqXHR) {
    console.log("Post response:"); 
    console.dir(data); 
    console.log(textStatus); 
    console.dir(jqXHR);
  }
});
```

Finally we create the delete route:

```javascript
app.delete('/api/books/:id', function(req, res){
    console.log('Deleting book with id: ' + req.params.id);
    return BookModel.findById(req.params.id, function(err, book){
        return book.remove(function(err){
            if(!err){
                console.log('Book removed');
                return res.send('');
            } else {
                console.log(err);
            }
        });
    });
});
```

…and try it out:

```javascript
jQuery.ajax({
  url:'/api/books/4f95a5251baa9b8a1b000001',
  type:'DELETE',
  success:function(data, textStatus, jqXHR){
    console.log("Post response:");
    console.dir(data);
    console.log(textStatus);
    console.dir(jqXHR);
  }
});
```

So now our REST API is complete – we have support for all HTTP verbs. What next? Well, until now I have left out the keywords part of our books. This is a bit more complicated since a book could have several keywords and we don’t want to represent them as a string, but rather an array of strings. To do that we need another schema. Add a Keywords schema right above our Book schema:

```javascript
//Schemas
var Keywords = new mongoose.Schema({
    keyword:String
});
```

To add a sub schema to an existing schema we use brackets notation like so:

```javascript
var Book = new mongoose.Schema({
    title:String,
    author:String,
    releaseDate:Date,
    keywords:[Keywords]
});
```

Also update POST and PUT

```javascript
app.post('/api/books', function (req, res) {
    var book = new BookModel({
        title:req.body.title,
        author:req.body.author,
        releaseDate:req.body.releaseDate,
        keywords:req.body.keywords
    });
    book.save(function (err) {
        if (!err) {
            return console.log('created');
        } else {
            return console.log(err);
        }
    });
    return res.send(book);
});
 
app.put('/api/books/:id', function(req, res){
    return BookModel.findById(req.params.id, function(err, book){
        book.title = req.body.title;
        book.author = req.body.author;
        book.releaseDate = req.body.releaseDate;
        book.keywords = req.body.keywords;
        return book.save(function(err){
            if(!err){
                console.log('book updated');
            } else {
                console.log(err);
            }
            return res.send(book);
        });
    });
});
```

There we are, that should be all we need, now we can try it out in the console:

```javascript
jQuery.post("/api/books", {
  "title":"Secrets of the JavaScript Ninja",
  "author":"John Resig",
  "releaseDate":new Date(2008, 3, 12).getTime(),
  "keywords":[{
     "keyword":"JavaScript"
   },{
     "keyword":"Reference"
   }]
}, function(data, textStatus, jqXHR) {
    console.log("Post response:"); 
    console.dir(data); 
    console.log(textStatus); 
    console.dir(jqXHR);
});
```

You should now have a fully functional REST server. We will use this in the next part of the tutorial to make our library application persistent.

##Part 3

In this part we will cover connecting our Backbone application to a server through a REST API. This tutorial builds on the first two parts, so I strongly recommend reading those to fully benefit from this part.

To synchronize a Backbone application to a server we need a server with a REST API that communicates in JSON format. The server I created in part 2.5 will fit our needs, but if you are creating your own server in another language you might want to read through that tutorial to get a grip of the API. Backbone makes use of the sync function in order to persist models with the server. You usually do not need to use this function directly, but instead set a url attribute to a collection (or a Model if the Models are not in a collection) informing Backbone where to sync. So lets go ahead and add a url attribute to our Library collection:

```javascript
var Library = Backbone.Collection.extend({
    model:Book,
    url:'/api/books'
});
```

Hint: If you, for some reason, don’t have access to a server you could use the Backbone.Localstorage plugin (you’ll need a browser that supports localstorage though). To do this, download the plugin [here](http://github.com/jeromegn/Backbone.localStorage), add it in a script tag in the HTML page after Backbone and initialize it in the collection like this:

```javascript
var Library = Backbone.Collection.extend({
    localStorage:new Backbone.LocalStorage("LibraryStorage"),
    model:Book,
    url:'/api/books'
});
```

By this Backbone will assume that the API looks like this:

```
url HTTP Method Operation
/api/books  GET Get an array of all books
/api/books/:id  GET Get the book with id of :id
/api/books  POST  Add a new book and return the book with an id attribute added
/api/books/:id  PUT Update the book with id of :id
/api/books/:id  DELETE  Delete the book with id of :id

```

To make our application get the Book models from the server on page load we need to update the LibraryView. It is recommended in the Backbone documentation to insert all models when the page is generated on the server side, rather than fetching them from the client side once the page is loaded. Since this tutorial will try to give you a more complete picture of how to communicate with a server, we will go ahead and ignore that recommendation. Go to the LibraryView declaration in app.js and update the initialize function as follows:

```javascript
initialize:function () {
    //this.collection = new Library(books);
    this.collection = new Library();
    this.collection.fetch();
    this.render();

    this.collection.on("add", this.renderBook, this);
    this.collection.on("remove", this.removeBook, this);
    this.collection.on("reset", this.render, this);
},
```

Here I have replaced the row where we create a collection from the internal array with `this.collection.fetch()`. I have also added a listener on the reset event. We need to do this since the fetching of models is asynchronous and happens after the page is rendered. When the fetching is finished, Backbone will fire the reset event, which we listen to and re-render the view. If you reload the page now you should see all books that are stored on the server:

![](http://codebyexample.info/wp-content/uploads/2012/04/Screen-Shot-2012-04-30-at-12.27.32-AM.png)


As you can see the date and keywords look a bit weird. The date delivered from the server is converted into a JavaScript Date object and when applied to the underscore template it will use the toString() function to display it. There isn’t very good support for formatting dates in JavaScript so we will use the dateFormat jQuery plugin to fix this. Go ahead and download it from [here](http://github.com/phstc/jquery-dateFormat) and put it in your js folder. Then go ahead and change  index.html like this:

```html
<body>
<div id="books">
    <form id="addBook" action="#">
        <div>
            <label for="coverImage">CoverImage: </label><input id="coverImage" type="file">
            <label for="title">Title: </label><input id="title">
            <label for="author">Author: </label><input id="author">
            <label for="releaseDate">Release date: </label><input id="releaseDate">
            <label for="keywords">Keywords: </label><input id="keywords">
            <button id="add">Add</button>
        </div>
    </form>
</div>
<script id="bookTemplate" type="text/template">
    <img src="<%= coverImage %>"/>
    <ul>
        <li><%= title%></li>
        <li><%= author%></li>
        <li><%= $.format.date(new Date(releaseDate), 'dd/MM/yyyy') %></li>
        <li><%= keywords %></li>
    </ul>
    <button class="delete">Delete</button>
</script>
<script src="js/jquery-1.7.1.js"></script>
<script src="js/jquery.dateFormat-1.0.js"></script>
<script src="js/underscore.js"></script>
<script src="js/backbone.js"></script>
<script src="js/app.js"></script>
</body>
```

So I added the jquery.dateFormat-1.0.js file on row 25 and used dateFormat to print the date in dd/MM/yyyy format on line 19. Now the date on the page should look a bit better. How about the keywords? Since we are receiving the keywords in an array we need to execute some code that generates a string of separated keywords.  To do that we can omit the equals letter in the template tag which will let us execute code that doesn’t display anything:

```html
<script id="bookTemplate" type="text/template">
    <img src="<%= coverImage %>"/>
    <ul>
        <li><%= title%></li>
        <li><%= author%></li>
        <li><%= $.format.date(new Date(releaseDate), 'dd/MM/yyyy') %></li>
        <li><% _.each(keywords, function(keyobj){%> <%= keyobj.keyword %><% }); %></li>
    </ul>
    <button class="delete">Delete</button>
</script>
```

Here I iterate over the keywords array using the each function and print out every single keyword. Note that I display the keyword using the <%= tag. This will display the keywords with a space between. If you would like to support space characters within a keyword, like for example “new age” then you could separate them by comma (no sane person would put a comma in a keyword right?). That would lead us to write some more code to remove the comma after the last keyword like this:

```html
<li>
    <% _.each(keywords, function(keyobj, index, list){ %>
        <%= keyobj.keyword %>
        <% if(index < list.length - 1){ %>
            <%= ', ' %>
        <% } %>
    <% }); %>
</li>
```

Reloading the page again should look quite decent:

![](http://codebyexample.info/wp-content/uploads/2012/04/Screen-Shot-2012-04-30-at-1.02.14-AM.png)

Now go ahead and delete a book and then reload the page: Tadaa! the deleted book is back! Not cool, why is this? This happens because when we get the BookModels from the server they have an _id attribute (notice the underscore), but Backbone expects an id attribute (no underscore). Since no id attribute is present, Backbone sees this model as new and deleting a new model don’t need any synchronization. To fix this we could change the server response, but we are instead going to look at the parse function of Backbone.Model. The parse function lets you edit the server response before it is passed to the Model constructor. Update the Book model like this:

```javascript
var Book = Backbone.Model.extend({
    defaults:{
        coverImage:"img/placeholder.png",
        title:"No title",
        author:"Unknown",
        releaseDate:"Unknown",
        keywords:"None"
    },
    parse:function (response) {
        console.log(response);
        response.id = response._id;
        return response;
    }
});
```

I simply copy the value of _id to the needed id attribute. If you reload the page you will see that models are actually deleted on the server when you press the delete button.

Note: A simpler way of making Backbone recognize _id as its unique identifier is to set the idAttribute of the model like this:

```javascript
var Book = Backbone.Model.extend({
    defaults:{
        coverImage:"img/placeholder.png",
        title:"No title",
        author:"Unknown",
        releaseDate:"Unknown",
        keywords:"None"
    },
    idAttribute:"_id"
});
```

If you now try to add a new book using the form you’ll notice that it is the same story as with delete – models wont get persisted on the server. This is because Backbone.Collection.add doesn’t automatically sync, but it is easy to fix. In LibraryView in app.js, change the line reading:

```javascript
this.collection.add(new Book(formData));
```

…to:

```javascript
this.collection.create(formData);
```

Now newly created books will get persisted. Actually, they probably wont if you enter a date. The server expects a date in UNIX timestamp format (milliseconds since Jan 1, 1970). Also any keywords you enter wont be stored since the server expects an array of objects with the attribute ‘keyword’. 

We start with fixing the date issue. We don’t really want the users to enter a date into a specific format manually so we’ll use the standard datepicker from jQuery UI. Go ahead and create a custom jQuery UI download containing datepicker from [here](http://jqueryui.com/download/). Add the css and js files to index.html:

```html
<head>
    <meta charset="UTF-8"/>
    <title>Backbone.js Web App</title>
    <link rel="stylesheet" href="css/screen.css">
    <link rel="stylesheet" href="css/cupertino/jquery-ui-1.8.19.custom.css">
</head>
```

and the js file after jQuery:

```html
<script src="js/jquery-1.7.1.js"></script>
<script src="js/jquery-ui-1.8.19.custom.min.js"></script>
<script src="js/jquery.dateFormat-1.0.js"></script>
<script src="js/underscore.js"></script>
<script src="js/backbone.js"></script>
<script src="js/app.js"></script>
```

Now in the beginning of app.js bind datepicker to our releaseDate field:

```javascript
(function ($) {
    $("#releaseDate").datepicker();
 
    var books = [
        {title:"JS the good parts", author:"John Doe", releaseDate:"2012", keywords:"JavaScript Programming"},

```
        
You should now be able to pick a date when clicking in the releaseDate field:

![](http://codebyexample.info/wp-content/uploads/2012/04/Screen-Shot-2012-04-30-at-1.34.19-AM.png)

Now go to the addBook function in LibraryView and update it like this:

```javascript
addBook:function (e) {
    e.preventDefault();
 
    var formData = {};
 
    $("#addBook div").children("input").each(function (i, el) {
        if ($(el).val() !== "") {
            if (el.id === 'releaseDate'){
                formData[el.id] = $('#releaseDate').datepicker("getDate").getTime();
            } else {
                formData[el.id] = $(el).val();
            }
        }
    });
 
    books.push(formData);
 
    //this.collection.add(new Book(formData));
    this.collection.create(formData);
},
```

Here I check if the current element is the releaseDate input field, in which case I use datePicker(“getDate”) which will give me a Date object and then use the getTime function on that to get the time in milliseconds. While we are at it lets fix the keywords issue as well:

```javascript
addBook:function (e) {
    e.preventDefault();
 
    var formData = {};
 
    $("#addBook div").children("input").each(function (i, el) {
        if ($(el).val() !== "") {
            if (el.id === 'keywords') {
                var keywordArray = $(el).val().split(',');
                var keywordObjects = [];
                for (var j = 0; j < keywordArray.length; j++) {
                    keywordObjects[j] = {"keyword":keywordArray[j]};
                }
                formData[el.id] = keywordObjects;
            } else if (el.id === 'releaseDate'){
                formData[el.id] = $('#releaseDate').datepicker("getDate").getTime();
            } else {
                formData[el.id] = $(el).val();
            }
        }
    });
 
    books.push(formData);
 
    //this.collection.add(new Book(formData));
    this.collection.create(formData);
},
```

Here I check if the current element is the keywords input field, in which case I split the string on each comma and then create a new array with keyword objects. In other words I assume that keywords are separated by commas, so I better write a comment on that in the form. Now you should be able to add new books with both release date and keywords!

![](http://codebyexample.info/wp-content/uploads/2012/04/Screen-Shot-2012-05-01-at-8.57.51-PM.png)

### Summary

In this tutorial we made our application persistent by binding it to a server using a REST API. We also looked at some problems that might occur when serializing and deserializing data and their solutions. We looked at the dateFormat and the datepicker jQuery plugins and how to do some more advanced things in our Underscore templates.  The code is available [here](https://dl.dropbox.com/u/70775642/workshop-practical/code.zip).
