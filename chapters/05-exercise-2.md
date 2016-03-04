# Exercise 2: Book Library - Your First RESTful Backbone.js App

While our first application gave us a good taste of how Backbone.js applications are made, most real-world applications will want to communicate with a back-end of some sort. Let's reinforce what we have already learned with another example, but this time we will also create a RESTful API for our application to talk to.

In this exercise, we will build a library application for managing digital books using Backbone. For each book, we will store the title, author, date of release, and some keywords. We'll also show a picture of the cover.


##Setting up

First, we need to create a folder structure for our project. To keep the front-end and back-end separate, we will create a folder called *site* for our client in the project root. Within it, we will create css, img, and js directories.

As with the last example, we will split our JavaScript files by their function, so under the js directory create folders named lib, models, collections, and views. Your directory hierarchy should look like this:

```
site/
    css/
    img/
    js/
        collections/
        lib/
        models/
        views/
```

Download the Backbone, Underscore, and jQuery libraries and copy them to your js/lib folder. We need a placeholder image for the book covers. Save this image to your site/img folder:

![](img/placeholder.png)

Just like before we need to load all of our dependencies in the site/index.html file:

```html
<!DOCTYPE html>
<html lang="en">
	<head>
		<meta charset="UTF-8"/>
		<title>Backbone.js Library</title>
		<link rel="stylesheet" href="css/screen.css">
	</head>
	<body>
		<script src="js/lib/jquery.min.js"></script>
		<script src="js/lib/underscore-min.js"></script>
		<script src="js/lib/backbone-min.js"></script>
		<script src="js/models/book.js"></script>
		<script src="js/collections/library.js"></script>
		<script src="js/views/book.js"></script>
		<script src="js/views/library.js"></script>
		<script src="js/app.js"></script>
	</body>
</html>

```

We should also add in the HTML for the user interface. We'll want a form for adding a new book, so add the following immediately inside the `body` element:

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

and we'll need a template for displaying each book which should be placed before the `script` tags:

```html
<script id="bookTemplate" type="text/template">
	<img src="<%= coverImage %>"/>
	<ul>
		<li><%= title %></li>
		<li><%= author %></li>
		<li><%= releaseDate %></li>
		<li><%= keywords %></li>
	</ul>

	<button class="delete">Delete</button>
</script>
```

To see what this will look like with some data in it, go ahead and add a manually filled-in book to the *books* div.

```html
<div class="bookContainer">
	<img src="img/placeholder.png"/>
	<ul>
		<li>Title</li>
		<li>Author</li>
		<li>Release Date</li>
		<li>Keywords</li>
	</ul>

	<button class="delete">Delete</button>
</div>
```

Open this file in a browser and it should look something like this:

![](img/chapter5-1.png)

Not so great. This is not a CSS tutorial, but we still need to do some formatting. Create a file named screen.css in your site/css folder:

```css
body {
	background-color: #eee;
}

.bookContainer {
	outline: 1px solid #aaa;
	width: 350px;
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
    margin-bottom: 0;
}

.bookContainer button {
    float: right;
    margin: 10px;
}

#addBook label {
	width: 100px;
	margin-right: 10px;
	text-align: right;
	line-height: 25px;
}

#addBook label, #addBook input {
	display: block;
	margin-bottom: 10px;
	float: left;
}

#addBook label[for="title"], #addBook label[for="releaseDate"] {
	clear: both;
}

#addBook button {
	display: block;
	margin: 5px 20px 10px 10px;
	float: right;
	clear: both;
}

#addBook div {
	width: 550px;
}

#addBook div:after {
	content: "";
	display: block;
	height: 0;
	visibility: hidden;
	clear: both;
	font-size: 0;
	line-height: 0;
}
```

Now it looks a bit better:

![](img/chapter5-2.png)

So this is what we want the final result to look like, but with more books. Go ahead and copy the bookContainer div a few more times if you would like to see what it looks like. Now, we are ready to start developing the actual application.

#### Creating the Model, Collection, Views, and App

First, we'll need a model of a book and a collection to hold the list. These are both very simple, with the model only declaring some defaults:

```javascript
// site/js/models/book.js

var app = app || {};

app.Book = Backbone.Model.extend({
	defaults: {
		coverImage: 'img/placeholder.png',
		title: 'No title',
		author: 'Unknown',
		releaseDate: 'Unknown',
		keywords: 'None'
	}
});
```

```javascript
// site/js/collections/library.js

var app = app || {};

app.Library = Backbone.Collection.extend({
	model: app.Book
});
```

Next, in order to display books we'll need a view:

```javascript
// site/js/views/book.js

var app = app || {};

app.BookView = Backbone.View.extend({
	tagName: 'div',
	className: 'bookContainer',
	template: _.template( $( '#bookTemplate' ).html() ),

	render: function() {
		//this.el is what we defined in tagName. use $el to get access to jQuery html() function
		this.$el.html( this.template( this.model.attributes ) );

		return this;
	}
});
```

We'll also need a view for the list itself:

```javascript
// site/js/views/library.js

var app = app || {};

app.LibraryView = Backbone.View.extend({
	el: '#books',

	initialize: function( initialBooks ) {
		this.collection = new app.Library( initialBooks );
		this.render();
	},

	// render library by rendering each book in its collection
	render: function() {
		this.collection.each(function( item ) {
			this.renderBook( item );
		}, this );
	},

	// render a book by creating a BookView and appending the
	// element it renders to the library's element
	renderBook: function( item ) {
		var bookView = new app.BookView({
			model: item
		});
		this.$el.append( bookView.render().el );
	}
});
```

Note that in the initialize function, we accept an array of data that we pass to the app.Library constructor. We'll use this to populate our collection with some sample data so that we can see everything is working correctly. Finally, we have the entry point for our code, along with the sample data:

```javascript
// site/js/app.js

var app = app || {};

$(function() {
	var books = [
		{ title: 'JavaScript: The Good Parts', author: 'Douglas Crockford', releaseDate: '2008', keywords: 'JavaScript Programming' },
		{ title: 'The Little Book on CoffeeScript', author: 'Alex MacCaw', releaseDate: '2012', keywords: 'CoffeeScript Programming' },
		{ title: 'Scala for the Impatient', author: 'Cay S. Horstmann', releaseDate: '2012', keywords: 'Scala Programming' },
		{ title: 'American Psycho', author: 'Bret Easton Ellis', releaseDate: '1991', keywords: 'Novel Splatter' },
		{ title: 'Eloquent JavaScript', author: 'Marijn Haverbeke', releaseDate: '2011', keywords: 'JavaScript Programming' }
	];

	new app.LibraryView( books );
});
```

Our app just passes the sample data to a new instance of app.LibraryView that it creates. Since the `initialize()` constructor in LibraryView invokes the view's `render()` method, all the books in the library will be displayed. Since we are passing our entry point as a callback to jQuery (in the form of its $ alias), the function will execute when the DOM is ready.

If you view index.html in a browser, you should see something like this:

![](img/chapter5-3.png)

This is a complete Backbone application, though it doesn't yet do anything interesting.

##Wiring in the interface

Now, we'll add some functionality to the useless form at the top and the delete buttons on each book.

###Adding models

When the user clicks the add button, we want to take the data in the form and use it to create a new model. In the LibraryView, we need to add an event handler for the click event:

```javascript
events:{
    'click #add':'addBook'
},

addBook: function( e ) {
	e.preventDefault();

	var formData = {};

	$( '#addBook div' ).children( 'input' ).each( function( i, el ) {
		if( $( el ).val() !== '' )
		{
			formData[ el.id ] = $( el ).val();
		}
	});

	this.collection.add( new app.Book( formData ) );
},
```

We select all the input elements of the form that have a value and iterate over them using jQuery's each. Since we used the same names for ids in our form as the keys on our Book model, we can simply store them directly in the formData object. We then create a new Book from the data and add it to the collection. We skip fields without a value so that the defaults will be applied.

Backbone passes an event object as a parameter to the event-handling function. This is useful for us in this case since we don't want the form to actually submit and reload the page. Adding a call to `preventDefault` on the event in the `addBook` function takes care of this for us.

Now, we just need to make the view render again when a new model is added. To do this, we put

```javascript
this.listenTo( this.collection, 'add', this.renderBook );
```

in the initialize function of LibraryView.

Now, you should be ready to take the application for a spin.

![](img/chapter5-4.png)

You may notice that the file input for the cover image isn’t working, but that is left as an exercise to the reader.

###Removing models

Next, we need to wire up the delete button. Set up the event handler in the BookView:

```javascript
	events: {
		'click .delete': 'deleteBook'
	},

	deleteBook: function() {
		//Delete model
		this.model.destroy();

		//Delete view
		this.remove();
	},
```

You should now be able to add and remove books from the library.

##Creating the back-end

Now, we need to make a small detour and set up a server with a REST api. Since this is a JavaScript book, we will use JavaScript to create the server using node.js. If you are more comfortable in setting up a REST server in another language, this is the API you need to conform to:

```
url             HTTP Method  Operation
/api/books      GET          Get an array of all books
/api/books/:id  GET          Get the book with id of :id
/api/books      POST         Add a new book and return the book with an id attribute added
/api/books/:id  PUT          Update the book with id of :id
/api/books/:id  DELETE       Delete the book with id of :id
```

The outline for this section looks like this:

* Install node.js, npm, and MongoDB
* Install node modules
* Create a simple web server
* Connect to the database
* Create the REST API

###Install node.js, npm, and MongoDB

Download and install node.js from nodejs.org. The node package manager (npm) will be installed as well.

Download, install, and run MongoDB from mongodb.org (you need Mongo to be running to store data in a Mongo database). There are detailed installation guides [on the website](http://docs.mongodb.org/manual/installation/).

###Install node modules

Create a file called `package.json` in the root of your project. It should look like

```javascript
{
	"name": "backbone-library",
	"version": "0.0.1",
	"description": "A simple library application using Backbone",
	"dependencies": {
		"express": "~3.1.0",
		"path": "~0.4.9",
		"mongoose": "~3.5.5",
		"body-parser": "~1.9.1"
	}
}

```

Amongst other things, this file tells npm what the dependencies are for our project. On the command line, from the root of your project, type:

```sh
npm install
```

You should see npm fetch the dependencies that we listed in our package.json and save them within a folder called node_modules.

Your folder structure should look something like this:

```
node_modules/
  .bin/
  body-parser/
  express/
  mongoose/
  path/
site/
  css/
  img/
  js/
  index.html
package.json
```

###Create a simple web server

Create a file named server.js in the project root containing the following code:

```javascript
// Module dependencies.
var application_root = __dirname,
    express = require( 'express' ), //Web framework
    bodyParser = require('body-parser'), //Parser for reading request body
    path = require( 'path' ), //Utilities for dealing with file paths
    mongoose = require( 'mongoose' ); //MongoDB integration

//Create server
var app = express();

//Where to serve static content
app.use( express.static( path.join( application_root,'../', 'site') ) );
app.use(bodyParser());

//Start server
var port = 4711;

app.listen( port, function() {
    console.log( 'Express server listening on port %d in %s mode', port, app.settings.env );
});
```

We start off by loading the modules required for this project: Express for creating the HTTP server, Path for dealing with file paths, and mongoose for connecting with the database. We then create an Express server and configure it using an anonymous function. This is a pretty standard configuration and for our application we don’t actually need the methodOverride part. It is used for issuing PUT and DELETE HTTP requests directly from a form, since forms normally only support GET and POST. Finally, we start the server by running the listen function. The port number used, in this case 4711, could be any free port on your system. I simply used 4711 since it is unlikely to have been used by anything else. We are now ready to run our first server:

```javascript
node server.js
```

If you open a browser on http://localhost:4711 you should see something like this:

![](img/chapter5-5.png)

This is where we left off in Part 2, but we are now running on a server instead of directly from the files. Great job! We can now start defining routes (URLs) that the server should react to. This will be our REST API. Routes are defined by using app followed by one of the HTTP verbs get, put, post, and delete, which corresponds to Create, Read, Update and Delete. Let us go back to server.js and define a simple route:

```javascript
// Routes
app.get( '/api', function( request, response ) {
    response.send( 'Library API is running' );
});
```

The get function takes a URL as the first parameter and a function as the second. The function will be called with request and response objects. Now, you can restart node and go to our specified URL:

![](img/chapter5-6.png)

###Connect to the database

Fantastic. Now, since we want to store our data in MongoDB, we need to define a schema. Add this to server.js:

```javascript
//Connect to database
mongoose.connect( 'mongodb://localhost/library_database' );

//Schemas
var Book = new mongoose.Schema({
	title: String,
	author: String,
	releaseDate: Date
});

//Models
var BookModel = mongoose.model( 'Book', Book );

// Configure server
app.configure( function() {
	//parses request body and populates request.body
	app.use( express.bodyParser() );

	//checks request.body for HTTP method overrides
	app.use( express.methodOverride() );

	//perform route lookup based on url and HTTP method
	app.use( app.router );

	//Where to serve static content
	app.use( express.static( path.join( application_root, '../', site') ) );

	//Show all errors in development
	app.use( express.errorHandler({ dumpExceptions: true, showStack: true }));
});
```

As you can see, schema definitions are quite straight forward. They can be more advanced, but this will do for us. I also extracted a model (BookModel) from Mongo. This is what we will be working with. Next up, we define a GET operation for the REST API that will return all books:

```javascript
//Get a list of all books
app.get( '/api/books', function( request, response ) {
	return BookModel.find( function( err, books ) {
		if( !err ) {
			return response.send( books );
		} else {
			return console.log( err );
		}
	});
});
```

The find function of Model is defined like this: `function find (conditions, fields, options, callback)` – but since we want a function that returns all books we only need the callback parameter. The callback will be called with an error object and an array of found objects. If there was no error, we return the array of objects to the client using the `send` function of the response object. Otherwise, we log the error to the console.

To test our API, we need to do a little typing in a JavaScript console. Restart node and go to localhost:4711 in your browser. Open up the JavaScript console. If you are using Google Chrome, go to View->Developer->JavaScript Console. If you are using Firefox, install Firebug and go to View->Firebug. Most other browsers will have a similar console. In the console type the following:

```javascript
jQuery.get( '/api/books/', function( data, textStatus, jqXHR ) {
	console.log( 'Get response:' );
	console.dir( data );
	console.log( textStatus );
	console.dir( jqXHR );
});
```

…and press enter and you should get something like this:

![](img/chapter5-7.png)

Here I used jQuery to make the call to our REST API, since it was already loaded on the page. The returned array is obviously empty, since we have not put anything into the database yet. Let's go and create a POST route that enables adding new items in server.js:

```javascript
//Insert a new book
app.post( '/api/books', function( request, response ) {
	var book = new BookModel({
		title: request.body.title,
		author: request.body.author,
		releaseDate: request.body.releaseDate
	});

	return book.save( function( err ) {
		if( !err ) {
			console.log( 'created' );
			return response.send( book );
		} else {
  			console.log( err );
		}
	});
});
```

We start by creating a new BookModel, passing an object with title, author, and releaseDate attributes. The data are collected from request.body. This means that anyone calling this operation in the API needs to supply a JSON object containing the title, author, and releaseDate attributes. Actually, the caller can omit any or all attributes since we have not made any of them mandatory.

We then call the save function on the BookModel passing in a callback in the same way as with the previous get route. Finally, we return the saved BookModel. The reason we return the BookModel and not just “success” or similar string is that when the BookModel is saved it will get an _id attribute from MongoDB, which the client needs when updating or deleting a specific book. Let's try it out again. Restart node and go back to the console and type:

```javascript
jQuery.post( '/api/books', {
	'title': 'JavaScript the good parts',
	'author': 'Douglas Crockford',
	'releaseDate': new Date( 2008, 4, 1 ).getTime()
}, function(data, textStatus, jqXHR) {
	console.log( 'Post response:' );
	console.dir( data );
	console.log( textStatus );
	console.dir( jqXHR );
});
```

..and then

```javascript
jQuery.get( '/api/books/', function( data, textStatus, jqXHR ) {
    console.log( 'Get response:' );
    console.dir( data );
    console.log( textStatus );
    console.dir( jqXHR );
});
```

You should now get a one-element array back from our server. You may wonder about this line:

```javascript
'releaseDate': new Date(2008, 4, 1).getTime()
```

MongoDB expects dates in UNIX time format (milliseconds from the start of Jan 1st 1970 UTC), so we have to convert dates before posting. The object we get back, however, contains a JavaScript Date object. Also note the _id attribute of the returned object.

![](img/chapter5-8.png)


Let's move on to creating a GET request that retrieves a single book in server.js:

```javascript
//Get a single book by id
app.get( '/api/books/:id', function( request, response ) {
	return BookModel.findById( request.params.id, function( err, book ) {
		if( !err ) {
			return response.send( book );
		} else {
			return console.log( err );
		}
	});
});
```

Here, we use colon notation (:id) to tell Express that this part of the route is dynamic. We also use the `findById` function on BookModel to get a single result. If you restart node, you can get a single book by adding the id previously returned to the URL like this:

```javascript
jQuery.get( '/api/books/4f95a8cb1baa9b8a1b000006', function( data, textStatus, jqXHR ) {
    console.log( 'Get response:' );
    console.dir( data );
    console.log( textStatus );
    console.dir( jqXHR );
});
```

Let's create the PUT (update) function next:

```javascript
//Update a book
app.put( '/api/books/:id', function( request, response ) {
	console.log( 'Updating book ' + request.body.title );
	return BookModel.findById( request.params.id, function( err, book ) {
		book.title = request.body.title;
		book.author = request.body.author;
		book.releaseDate = request.body.releaseDate;

		return book.save( function( err ) {
			if( !err ) {
				console.log( 'book updated' );
	    		return response.send( book );
	    	} else {
	    		console.log( err );
	    	}
		});
	});
});
```

This is a little larger than previous ones, but is also pretty straight forward – we find a book by id, update its properties, save it, and send it back to the client.

To test this, we need to use the more general jQuery ajax function. Again, in these examples, you will need to replace the id property with one that matches an item in your own database:

```javascript
jQuery.ajax({
	url: '/api/books/4f95a8cb1baa9b8a1b000006',
	type: 'PUT',
	data: {
		'title': 'JavaScript The good parts',
		'author': 'The Legendary Douglas Crockford',
		'releaseDate': new Date( 2008, 4, 1 ).getTime()
	},
	success: function( data, textStatus, jqXHR ) {
		console.log( 'Put response:' );
		console.dir( data );
		console.log( textStatus );
		console.dir( jqXHR );
	}
});
```

Finally, we create the delete route:

```javascript
//Delete a book
app.delete( '/api/books/:id', function( request, response ) {
	console.log( 'Deleting book with id: ' + request.params.id );
	return BookModel.findById( request.params.id, function( err, book ) {
		return book.remove( function( err ) {
			if( !err ) {
				console.log( 'Book removed' );
				return response.send( '' );
			} else {
				console.log( err );
			}
		});
	});
});
```

…and try it out:

```javascript
jQuery.ajax({
	url: '/api/books/4f95a5251baa9b8a1b000001',
	type: 'DELETE',
	success: function( data, textStatus, jqXHR ) {
		console.log( 'Delete response:' );
		console.dir( data );
		console.log( textStatus );
		console.dir( jqXHR );
	}
});
```

So now our REST API is complete – we have support for all four HTTP verbs. What's next? Well, until now I have left out the keywords part of our books. This is a bit more complicated since a book could have several keywords and we don’t want to represent them as a string, but rather an array of strings. To do that, we need another schema. Add a Keywords schema right above our Book schema:

```javascript
//Schemas
var Keywords = new mongoose.Schema({
    keyword: String
});
```

To add a sub schema to an existing schema, we use brackets notation like so:

```javascript
var Book = new mongoose.Schema({
	title: String,
	author: String,
	releaseDate: Date,
	keywords: [ Keywords ]                       // NEW
});
```

Also update POST and PUT:

```javascript
//Insert a new book
app.post( '/api/books', function( request, response ) {
	var book = new BookModel({
		title: request.body.title,
		author: request.body.author,
		releaseDate: request.body.releaseDate,
		keywords: request.body.keywords       // NEW
	});
	book.save( function( err ) {
		if( !err ) {
			console.log( 'created' );
            return response.send( book );
		} else {
			return console.log( err );
		}
	});
});

//Update a book
app.put( '/api/books/:id', function( request, response ) {
	console.log( 'Updating book ' + request.body.title );
	return BookModel.findById( request.params.id, function( err, book ) {
		book.title = request.body.title;
		book.author = request.body.author;
		book.releaseDate = request.body.releaseDate;
		book.keywords = request.body.keywords; // NEW

		return book.save( function( err ) {
			if( !err ) {
				console.log( 'book updated' );
			} else {
				console.log( err );
			}
			return response.send( book );
		});
	});
});
```

There we are, that should be all we need, now we can try it out in the console:

```javascript
jQuery.post( '/api/books', {
	'title': 'Secrets of the JavaScript Ninja',
	'author': 'John Resig',
	'releaseDate': new Date( 2008, 3, 12 ).getTime(),
	'keywords':[
		{ 'keyword': 'JavaScript' },
		{ 'keyword': 'Reference' }
	]
}, function( data, textStatus, jqXHR ) {
	console.log( 'Post response:' );
	console.dir( data );
	console.log( textStatus );
	console.dir( jqXHR );
});
```

You now have a fully functional REST server that we can hook into from our front-end.

##Talking to the server

In this part, we will cover connecting our Backbone application to the server through the REST API.

As we mentioned in chapter 3 *Backbone Basics*, we can retrieve models from a server using `collection.fetch()` by setting `collection.url` to be the URL of the API endpoint. Let's update the Library collection to do that now:

```javascript
var app = app || {};

app.Library = Backbone.Collection.extend({
	model: app.Book,
	url: '/api/books'     // NEW
});
```

This results in the default implementation of Backbone.sync assuming that the API looks like this:

```
url             HTTP Method  Operation
/api/books      GET          Get an array of all books
/api/books/:id  GET          Get the book with id of :id
/api/books      POST         Add a new book and return the book with an id attribute added
/api/books/:id  PUT          Update the book with id of :id
/api/books/:id  DELETE       Delete the book with id of :id
```

To have our application retrieve the Book models from the server on page load, we need to update the LibraryView. The Backbone documentation recommends inserting all models when the page is generated on the server side, rather than fetching them from the client side once the page is loaded. Since this chapter is trying to give you a more complete picture of how to communicate with a server, we will go ahead and ignore that recommendation. Go to the LibraryView declaration and update the initialize function as follows:

```javascript
initialize: function() {					// UPDATED
	this.collection = new app.Library();	// UPDATED
	this.collection.fetch({reset: true}); 	// NEW
	this.render();

	this.listenTo( this.collection, 'add', this.renderBook );
	this.listenTo( this.collection, 'reset', this.render ); // NEW
},
```

Now that we are populating our Library from the database using `this.collection.fetch()`, the `initialize()` function no longer takes a set of sample data as an argument and doesn't pass anything to the app.Library constructor. You can now remove the sample data from site/js/app.js, which should reduce it to a single statement which creates the LibraryView:

```javascript
// site/js/app.js

var app = app || {};

$(function() {
	new app.LibraryView();	// UPDATED
});
```

We have also added a listener on the reset event. We need to do this since the models are fetched asynchronously after the page is rendered. When the fetch completes, Backbone fires the reset event, as requested by the `reset: true` option, and our listener re-renders the view. If you reload the page now, you should see all books that are stored on the server:

![](img/chapter5-9.png)

As you can see, the date and keywords look a bit weird. The date delivered from the server is converted into a JavaScript Date object and when applied to the underscore template, it will use the toString() function to display it. There isn’t very good support for formatting dates in JavaScript so we will use the dateFormat jQuery plugin to fix this. Go ahead and download it from [here](http://github.com/phstc/jquery-dateFormat) and put it in your site/js/lib folder. Update the book template so that the date is displayed with:

```html
<li><%= $.format.date( new Date( releaseDate ), 'MMMM yyyy' ) %></li>
```

and add a script element for the plugin

```html
<script src="js/lib/jquery-dateFormat-1.0.js"></script>
```

Now, the date on the page should look a bit better. How about the keywords? Since we are receiving the keywords in an array we need to execute some code that generates a string of separated keywords. To do that, we can omit the equals character in the template tag which will let us execute code that doesn’t display anything:

```html
<li><% _.each( keywords, function( keyobj ) {%> <%= keyobj.keyword %><% } ); %></li>
```

Here I iterate over the keywords array using the Underscore `each` function and print out every single keyword. Note that I display the keyword using the <%= tag. This will display the keywords with spaces between them.

Reloading the page again should look quite decent:

![](img/chapter5-10.png)

Now go ahead and delete a book and then reload the page: Tadaa! the deleted book is back! Not cool, why is this? This happens because when we get the BookModels from the server they have an _id attribute (notice the underscore), but Backbone expects an id attribute (no underscore). Since no id attribute is present, Backbone sees this model as new and deleting a new model doesn’t need any synchronization.

To fix this, we can use the parse function of Backbone.Model. The parse function lets you edit the server response before it is passed to the Model constructor. Add a parse method to the Book model:

```javascript
parse: function( response ) {
	response.id = response._id;
	return response;
}
```

Simply copy the value of _id to the needed id attribute. If you reload the page, you will see that models are actually deleted on the server when you press the delete button.

Another, simpler way of making Backbone recognize _id as its unique identifier is to set the idAttribute of the model to _id.

If you now try to add a new book using the form, you’ll notice that it is a similar story to delete – models won't get persisted on the server. This is because Backbone.Collection.add doesn’t automatically sync, but it is easy to fix. In the LibraryView, we find in `views/library.js` change the line reading:

```javascript
this.collection.add( new Book( formData ) );
```

…to:

```javascript
this.collection.create( formData );
```

Now, newly created books will get persisted. Actually, they probably won't if you enter a date. The server expects a date in UNIX timestamp format (milliseconds since Jan 1, 1970). Also, any keywords you enter won't be stored since the server expects an array of objects with the attribute ‘keyword’.

We'll start by fixing the date issue. We don’t really want the users to manually enter a date in a specific format, so we’ll use the standard datepicker from jQuery UI. Go ahead and create a custom jQuery UI download containing datepicker from [here](http://jqueryui.com/download/). Add the css theme to site/css/ and the JavaScript to site/js/lib. Link to them in index.html:

```html
<link rel="stylesheet" href="css/cupertino/jquery-ui-1.10.0.custom.css">
```

"cupertino" is the name of the style I chose when downloading jQuery UI.

The JavaScript file must be loaded after jQuery.

```html
<script src="js/lib/jquery.min.js"></script>
<script src="js/lib/jquery-ui-1.10.0.custom.min.js"></script>
```

Now in app.js, bind a datepicker to our releaseDate field:

```javascript
var app = app || {};

$(function() {
	$( '#releaseDate' ).datepicker();
	new app.LibraryView();
});
```

You should now be able to pick a date when clicking in the releaseDate field:

![](img/chapter5-11.png)

Finally, we have to make sure that the form input is properly transformed into our storage format. Change the addBook function in LibraryView to:

```javascript
addBook: function( e ) {
	e.preventDefault();

	var formData = {};

	$( '#addBook div' ).children( 'input' ).each( function( i, el ) {
		if( $( el ).val() != '' )
		{
			if( el.id === 'keywords' ) {
				formData[ el.id ] = [];
				_.each( $( el ).val().split( ' ' ), function( keyword ) {
					formData[ el.id ].push({ 'keyword': keyword });
				});
			} else if( el.id === 'releaseDate' ) {
				formData[ el.id ] = $( '#releaseDate' ).datepicker( 'getDate' ).getTime();
			} else {
				formData[ el.id ] = $( el ).val();
			}
		}
		// Clear input field value
		$( el ).val('');
	});

	this.collection.create( formData );
},

```

Our change adds two checks to the form input fields. First, we're checking if the current element is the keywords input field, in which case we're splitting the string on each space and creating an array of keyword objects.

Then we're checking if the current element is the releaseDate input field, in which case we're calling `datePicker(“getDate”)` which returns a Date object. We then use the `getTime` function on that to get the time in milliseconds.

Now, you should be able to add new books with both a release date and keywords!

![](img/chapter5-12.png)

### Summary

In this chapter, we made our application persistent by binding it to a server using a REST API. We also looked at some problems that might occur when serializing and deserializing data and their solutions. We looked at the dateFormat and the datepicker jQuery plugins and how to do some more advanced things in our Underscore templates. The code is available [here](https://github.com/addyosmani/backbone-fundamentals/tree/gh-pages/practicals/exercise-2).
