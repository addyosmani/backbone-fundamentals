
# Paginating Backbone.js Requests & Collections

## Introduction

Pagination is a ubiquitous problem we often find ourselves needing to solve on the web - perhaps most predominantly when working with service APIs and JavaScript-heavy clients which consume them. It's also a problem that is often under-refined as most of us consider pagination relatively easy to get right. This isn't however always the case as pagination tends to get more tricky than it initially seems. 

Before we dive into solutions for paginating data for your Backbone applications, let's define exactly what we consider pagination to be:

Pagination is a control system allowing users to browse through pages of search results (or any type of content) which is continued. Search results are the canonical example, but pagination today is found on news sites, blogs, and discussion boards, often in the form of Previous and Next links. More complete pagination systems offer granular control of the specific pages you can navigate to, giving the user more power to find what they are looking for. 

It isn't a problem limited to pages requiring some visual controls for pagination either - sites like Facebook, Pinterest, and Twitter have demonstrated that there are many contexts where infinite paging is also useful. Infinite paging is, of course, when we pre-fetch (or appear to pre-fetch) content from a subsequent page and add it directly to the user’s current page, making the experience feel "infinite".

Pagination is very context-specific and depends on the content being displayed. In the Google search results, pagination is important as they want to offer you the most relevant set of results in the first 1-2 pages. After that, you might be a little more selective (or random) with the page you choose to navigate to. This differs from cases where you'll want to cycle through consecutive pages for (e.g., for a news article or blog post). 

Pagination is almost certainly content and context-specific, but as Faruk Ates has [previously](https://gist.github.com/mislav/622561) pointed out the principles of good pagination apply no matter what the content or context is. As with everything extensible when it comes to Backbone, you can write your own pagination to address many of these content-specific types of pagination problems. That said, you'll probably spend quite a bit of time on this and sometimes you just want to use a tried and tested solution that just works.

On this topic, we're going to go through a set of pagination components I (and a group of [contributors](https://github.com/addyosmani/backbone.paginator/contributors)) wrote for Backbone.js, which should hopefully come in useful if you're working on applications which need to page Backbone Collections. They're part of an extension called [Backbone.Paginator](http://github.com/addyosmani/backbone.paginator).

### Backbone.Paginator

When working with data on the client-side, the three types of pagination we are most likely to run into are:

**Requests to a service layer (API)** - For example, query for results containing the term 'Paul' - if 5,000 results are available only display 20 results per page (leaving us with 250 possible result pages that can be navigated to).

This problem actually has quite a great deal more to it, such as maintaining persistence of other URL parameters (e.g sort, query, order) which can change based on a user's search configuration in a UI. One also has to think of a clean way of hooking views up to this pagination so you can easily navigate between pages (e.g., First, Last, Next, Previous, 1,2,3), manage the number of results displayed per page and so on.

**Further client-side pagination of data returned -** e.g we've been returned a JSON response containing 100 results. Rather than displaying all 100 to the user, we only display 20 of these results within a navigable UI in the browser.

Similar to the request problem, client-pagination has its own challenges like navigation once again (Next, Previous, 1,2,3), sorting, order, switching the number of results to display per page and so on.

**Infinite results** - with services such as Facebook, the concept of numeric pagination is instead replaced with a 'Load More' or 'View More' button. Triggering this normally fetches the next 'page' of N results but rather than replacing the previous set of results loaded entirely, we simply append to them instead.

A request pager which simply appends results in a view rather than replacing on each new fetch is effectively an 'infinite' pager.

**Let's now take a look at exactly what we're getting out of the box:**

Backbone.Paginator is a set of opinionated components for paginating collections of data using Backbone.js. It aims to provide both solutions for assisting with pagination of requests to a server (e.g an API) as well as pagination of single-loads of data, where we may wish to further paginate a collection of N results into M pages within a view.

![](img/paginator-ui.png)

Backbone.Paginator supports two main pagination components:

* **Backbone.Paginator.requestPager**: For pagination of requests between a client and a server-side API
* **Backbone.Paginator.clientPager**: For pagination of data returned from a server which you would like to further paginate within the UI (e.g 60 results are returned, paginate into 3 pages of 20)

### Live Examples

If you would like to look at examples built using the components included in the project, links to official demos are included below and use the Netflix API so that you can see them working with an actual data source.

* [Backbone.Paginator.requestPager()](http://addyosmani.github.com/backbone.paginator/examples/netflix-request-paging/index.html)
* [Backbone.Paginator.clientPager()](http://addyosmani.github.com/backbone.paginator/examples/netflix-client-paging/index.html)
* [Infinite Pagination (Backbone.Paginator.requestPager())](http://addyosmani.github.com/backbone.paginator/examples/netflix-infinite-paging/index.html)
* [Diacritic Plugin](http://addyosmani.github.com/backbone.paginator/examples/google-diacritic/index.html)

##Paginator.requestPager

In this section we're going to walk through using the requestPager. You would use this component when working with a service API which itself supports pagination. This component allows users to control the pagination settings for requests to this API (i.e navigate to the next, previous, N pages) via the client-side. 

The idea is that pagination, searching, and filtering of data can all be done from your Backbone application without the need for a page reload. 

![](img/paginator-request.png)

####1. Create a new Paginated collection

First, we define a new Paginated collection using `Backbone.Paginator.requestPager()` as follows:

```javascript

var PaginatedCollection = Backbone.Paginator.requestPager.extend({

```
####2: Set the model for the collection as normal

Within our collection, we then (as normal) specify the model to be used with this collection followed by the URL (or base URL) for the service providing our data (e.g the Netflix API).

```javascript

        model: model,

```
####3. Configure the base URL and the type of the request

We need to set a base URL. The `type` of the request is `GET` by default, and the `dataType` is `jsonp` in order to enable cross-domain requests.

```javascript
    paginator_core: {
      // the type of the request (GET by default)
      type: 'GET',

      // the type of reply (jsonp by default)
      dataType: 'jsonp',

      // the URL (or base URL) for the service
      // if you want to have a more dynamic URL, you can make this a function
      // that returns a string
      url: 'http://odata.netflix.com/Catalog/People(49446)/TitlesActedIn?'
    },
```

## Gotchas!

If you use `dataType` **NOT** jsonp, please remove the callback custom parameter inside the `server_api` configuration.

####4. Configure how the library will show the results

We need to tell the library how many items per page we would like to see, etc...

```javascript
    paginator_ui: {
      // the lowest page index your API allows to be accessed
      firstPage: 0,

      // which page should the paginator start from
      // (also, the actual page the paginator is on)
      currentPage: 0,

      // how many items per page should be shown
      perPage: 3,

      // a default number of total pages to query in case the API or
      // service you are using does not support providing the total
      // number of pages for us.
      // 10 as a default in case your service doesn't return the total
      totalPages: 10
    },
```

####5. Configure the parameters we want to send to the server

Only the base URL won't be enough for most cases, so you can pass more parameters to the server.
Note how you can use functions instead of hardcoded values, and you can also refer to the values you specified in `paginator_ui`.

```javascript
    server_api: {
      // the query field in the request
      '$filter': '',

      // number of items to return per request/page
      '$top': function() { return this.perPage },

      // how many results the request should skip ahead to
      // customize as needed. For the Netflix API, skipping ahead based on
      // page * number of results per page was necessary.
      '$skip': function() { return this.currentPage * this.perPage },

      // field to sort by
      '$orderby': 'ReleaseYear',

      // what format would you like to request results in?
      '$format': 'json',

      // custom parameters
      '$inlinecount': 'allpages',
      '$callback': 'callback'
    },
```

## Gotchas!

If you use `$callback`, please ensure that you did use the jsonp as a `dataType` inside your `paginator_core` configuration.

####6. Finally, configure Collection.parse() and we're done

The last thing we need to do is configure our collection's `parse()` method. We want to ensure we're returning the correct part of our JSON response containing the data our collection will be populated with, which below is `response.d.results` (for the Netflix API).

You might also notice that we're setting `this.totalPages` to the total page count returned by the API. This allows us to define the maximum number of (result) pages available for the current/last request so that we can clearly display this in the UI. It also allows us to infuence whether clicking say, a 'next' button should proceed with a request or not.

```javascript
        parse: function (response) {
            // Be sure to change this based on how your results
            // are structured (e.g d.results is Netflix specific)
            var tags = response.d.results;
            //Normally this.totalPages would equal response.d.__count
            //but as this particular NetFlix request only returns a
            //total count of items for the search, we divide.
            this.totalPages = Math.ceil(response.d.__count / this.perPage);
            return tags;
        }
    });

});
```

####Convenience methods:

For your convenience, the following methods are made available for use in your views to interact with the `requestPager`:

* **Collection.goTo( n, options )** - go to a specific page
* **Collection.nextPage( options )** - go to the next page
* **Collection.prevPage( options )** - go to the previous page
* **Collection.howManyPer( n )** - set the number of items to display per page

**requestPager** collection's methods `.goTo()`, `.nextPage()` and `.prevPage()` are all extensions of the original [Backbone Collection.fetch() methods](http://documentcloud.github.com/backbone/#Collection-fetch). As so, they all can take the same option object as a parameter.

This option object can use `success` and `error` parameters to pass a function to be executed after server answer.

```javascript
Collection.goTo(n, {
  success: function( collection, response ) {
    // called is server request success
  },
  error: function( collection, response ) {
    // called if server request fail
  }
});
```

To manage callback, you could also use the [jqXHR](http://api.jquery.com/jQuery.ajax/#jqXHR) returned by these methods to manage callback.

```javascript
Collection
  .requestNextPage()
  .done(function( data, textStatus, jqXHR ) {
    // called is server request success
  })
  .fail(function( data, textStatus, jqXHR ) {
    // called if server request fail
  })
  .always(function( data, textStatus, jqXHR ) {
    // do something after server request is complete
  });
});
```

If you'd like to add the incoming models to the current collection, instead of replacing the collection's contents, pass `{update: true, remove: false}` as options to these methods.

```javascript
Collection.prevPage({ update: true, remove: false });
```

##Paginator.clientPager


The clientPager is used to further paginate data that has already been returned by the service API. Say you've requested 100 results from the service and wish to split this into 5 pages of paginated results, each containing 20 results at a client level - the clientPager makes it trivial to do this.

![](img/paginator-client.png)

Use the clientPager when you prefer to get results in a single "load" and thus avoid making additional network requests each time your users want to fetch the next "page" of items. As the results have all already been requested, it's just a case of switching between the ranges of data actually presented to the user.

####1. Create a new paginated collection with a model and URL

As with `requestPager`, let's first create a new Paginated `Backbone.Paginator.clientPager` collection, with a model:

```javascript
    var PaginatedCollection = Backbone.Paginator.clientPager.extend({

        model: model,
```

####2. Configure the base URL and the type of the request

We need to set a base URL. The `type` of the request is `GET` by default, and the `dataType` is `jsonp` in order to enable cross-domain requests.

```javascript
    paginator_core: {
      // the type of the request (GET by default)
      type: 'GET',

      // the type of reply (jsonp by default)
      dataType: 'jsonp',

      // the URL (or base URL) for the service
      url: 'http://odata.netflix.com/v2/Catalog/Titles?&'
    },
```

####3. Configure how the library will show the results

We need to tell the library how many items per page we would like to see, etc...

```javascript
    paginator_ui: {
      // the lowest page index your API allows to be accessed
      firstPage: 1,

      // which page should the paginator start from
      // (also, the actual page the paginator is on)
      currentPage: 1,

      // how many items per page should be shown
      perPage: 3,

      // a default number of total pages to query in case the API or
      // service you are using does not support providing the total
      // number of pages for us.
      // 10 as a default in case your service doesn't return the total
      totalPages: 10,

      // The total number of pages to be shown as a pagination
      // list is calculated by (pagesInRange * 2) + 1.
      pagesInRange: 4
    },
```

####4. Configure the parameters we want to send to the server

Only the base URL won't be enough for most cases, so you can pass more parameters to the server.
Note how you can use functions instead of hardcoded values, and you can also refer to the values you specified in `paginator_ui`.

```javascript
    server_api: {
      // the query field in the request
      '$filter': 'substringof(\'america\',Name)',

      // number of items to return per request/page
      '$top': function() { return this.perPage },

      // how many results the request should skip ahead to
      // customize as needed. For the Netflix API, skipping ahead based on
      // page * number of results per page was necessary.
      '$skip': function() { return this.currentPage * this.perPage },

      // field to sort by
      '$orderby': 'ReleaseYear',

      // what format would you like to request results in?
      '$format': 'json',

      // custom parameters
      '$inlinecount': 'allpages',
      '$callback': 'callback'
    },
```

####5. Finally, configure Collection.parse() and we're done

And finally we have our `parse()` method, which in this case isn't concerned with the total number of result pages available on the server as we have our own total count of pages for the paginated data in the UI.

```javascript
    parse: function (response) {
            var tags = response.d.results;
            return tags;
        }

    });
```

####Convenience methods:

As mentioned, your views can hook into a number of convenience methods to navigate around UI-paginated data. For `clientPager` these include:

* **Collection.goTo(n, options)** - go to a specific page
* **Collection.prevPage(options)** - go to the previous page
* **Collection.nextPage(options)** - go to the next page
* **Collection.howManyPer(n)** - set how many items to display per page
* **Collection.setSort(sortBy, sortDirection)** - update sort on the current view. Sorting will automatically detect if you're trying to sort numbers (even if they're strored as strings) and will do the right thing.
* **Collection.setFilter(filterFields, filterWords)** - filter the current view. Filtering supports multiple words without any specific order, so you'll basically get a full-text search ability. Also, you can pass it only one field from the model, or you can pass an array with fields and all of them will get filtered. Last option is to pass it an object containing a comparison method and rules. Currently, only ```levenshtein``` method is available.

The `goTo()`, `prevPage()`, and `nextPage()` functions do not require the `options` param since they will be executed synchronously. However, when specified, the success callback will be invoked before the function returns. For example:

```javascript
nextPage(); // this works just fine!
nextPage({success: function() { }}); // this will call the success function
```

The options param exists to preserve (some) interface unification between the requestPaginator and clientPaginator so that they may be used interchangeably in your Backbone.Views.

```javascript
  this.collection.setFilter(
    {'Name': {cmp_method: 'levenshtein', max_distance: 7}}
    , "Amreican P" // Note the switched 'r' and 'e', and the 'P' from 'Pie'
  );
```

Also note that the Levenshtein plugin should be loaded and enabled using the ```useLevenshteinPlugin``` variable.
Last but not less important: performing Levenshtein comparison returns the ```distance``` between two strings. It won't let you *search* lengthy text.
The distance between two strings means the number of characters that should be added, removed or moved to the left or to the right so the strings get equal.
That means that comparing "Something" in "This is a test that could show something" will return 32, which is bigger than comparing "Something" and "ABCDEFG" (9).
Use Levenshtein only for short texts (titles, names, etc).

* **Collection.doFakeFilter(filterFields, filterWords)** - returns the models count after fake-applying a call to ```Collection.setFilter```.

* **Collection.setFieldFilter(rules)** - filter each value of each model according to `rules` that you pass as argument. Example: You have a collection of books with 'release year' and 'author'. You can filter only the books that were released between 1999 and 2003. And then you can add another `rule` that will filter those books only to authors who's name start with 'A'. Possible rules: function, required, min, max, range, minLength, maxLength, rangeLength, oneOf, equalTo, containsAllOf, pattern.  Passing this an empty rules set will remove any FieldFilter rules applied.


```javascript

  my_collection.setFieldFilter([
    {field: 'release_year', type: 'range', value: {min: '1999', max: '2003'}},
    {field: 'author', type: 'pattern', value: new RegExp('A*', 'igm')}
  ]);

  //Rules:
  //
  //var my_var = 'green';
  //
  //{field: 'color', type: 'equalTo', value: my_var}
  //{field: 'color', type: 'function', value: function(field_value){ return field_value == my_var; } }
  //{field: 'color', type: 'required'}
  //{field: 'number_of_colors', type: 'min', value: '2'}
  //{field: 'number_of_colors', type: 'max', value: '4'}
  //{field: 'number_of_colors', type: 'range', value: {min: '2', max: '4'} }
  //{field: 'color_name', type: 'minLength', value: '4'}
  //{field: 'color_name', type: 'maxLength', value: '6'}
  //{field: 'color_name', type: 'rangeLength', value: {min: '4', max: '6'}}
  //{field: 'color_name', type: 'oneOf', value: ['green', 'yellow']}
  //{field: 'color_name', type: 'pattern', value: new RegExp('gre*', 'ig')}
  //{field: 'color_name', type: 'containsAllOf', value: ['green', 'yellow', 'blue']}
```

* **Collection.doFakeFieldFilter(rules)** - returns the models count after fake-applying a call to ```Collection.setFieldFilter```.

####Implementation notes:

You can use some variables in your ```View``` to represent the actual state of the paginator.

* ```totalUnfilteredRecords``` - Contains the number of records, including all records filtered in any way. (Only available in ```clientPager```)
* ```totalRecords``` - Contains the number of records
* ```currentPage``` - The actual page were the paginator is at.
* ```perPage``` - The number of records the paginator will show per page.
* ```totalPages``` - The number of total pages.
* ```startRecord``` - The position of the first record shown in the current page (eg 41 to 50 from 2000 records) (Only available in ```clientPager```)
* ```endRecord``` - The position of the last record shown in the current page (eg 41 to 50 from 2000 records) (Only available in ```clientPager```)
* ```pagesInRange``` - The number of pages to be drawn on each side of the current page. So if ```pagesInRange``` is 3 and ```currentPage``` is 13 you will get the numbers 10, 11, 12, 13(selected), 14, 15, 16.

```html
<!-- sample template for pagination UI -->
<script type="text/html" id="tmpServerPagination">

  <div class="row-fluid">

    <div class="pagination span8">
      <ul>
        <% _.each (pageSet, function (p) { %>
        <% if (currentPage == p) { %>
          <li class="active"><span><%= p %></span></li>
        <% } else { %>
          <li><a href="#" class="page"><%= p %></a></li>
        <% } %>
        <% }); %>
      </ul>
    </div>

    <div class="pagination span4">
      <ul>
        <% if (currentPage > firstPage) { %>
          <li><a href="#" class="serverprevious">Previous</a></li>
        <% }else{ %>
          <li><span>Previous</span></li>
        <% }%>
        <% if (currentPage < totalPages) { %>
          <li><a href="#" class="servernext">Next</a></li>
        <% } else { %>
          <li><span>Next</span></li>
        <% } %>
        <% if (firstPage != currentPage) { %>
          <li><a href="#" class="serverfirst">First</a></li>
        <% } else { %>
          <li><span>First</span></li>
        <% } %>
        <% if (totalPages != currentPage) { %>
          <li><a href="#" class="serverlast">Last</a></li>
        <% } else { %>
          <li><span>Last</span></li>
        <% } %>
      </ul>
    </div>

  </div>

  <span class="cell serverhowmany"> Show <a href="#"
    class="selected">18</a> | <a href="#" class="">9</a> | <a href="#" class="">12</a> per page
  </span>

  <span class="divider">/</span>

  <span class="cell first records">
    Page: <span class="label"><%= currentPage %></span> of <span class="label"><%= totalPages %></span> shown
  </span>

</script>
```

### Plugins

**Diacritic.js**

A plugin for Backbone.Paginator that replaces diacritic characters (`´`, `˝`, `̏`, `˚`,`~` etc.) with characters that match them most closely. This is particularly useful for filtering.

![](img/paginator-dia.png)

To enable the plugin, set `this.useDiacriticsPlugin` to true, as can be seen in the example below:

```javascript
Paginator.clientPager = Backbone.Collection.extend({

    // Default values used when sorting and/or filtering.
    initialize: function(){
      this.useDiacriticsPlugin = true; // use diacritics plugin if available
    ...
```

### Bootstrapping

By default, both the clientPager and requestPager will make an initial request to the server in order to populate their internal paging data. In order to avoid this additional request, it may be beneficial to bootstrap your Backbone.Paginator instance from data that already exists in the dom.

**Backbone.Paginator.clientPager:**

```javascript

// Extend the Backbone.Paginator.clientPager with your own configuration options
var MyClientPager =  Backbone.Paginator.clientPager.extend({paginator_ui: {}});
// Create an instance of your class and populate with the models of your entire collection
var aClientPager = new MyClientPager([{id: 1, title: 'foo'}, {id: 2, title: 'bar'}]);
// Invoke the bootstrap function
aClientPager.bootstrap();
```

Note: If you intend to bootstrap a clientPager, there is no need to specify a 'paginator_core' object in your configuration (since you should have already populated the clientPager with the entirety of it's necessary data)

**Backbone.Paginator.requestPager:**

```javascript

// Extend the Backbone.Paginator.requestPager with your own configuration options
var MyRequestPager =  Backbone.Paginator.requestPager.extend({paginator_ui: {}});
// Create an instance of your class with the first page of data
var aRequestPager = new MyRequestPager([{id: 1, title: 'foo'}, {id: 2, title: 'bar'}]);
// Invoke the bootstrap function and configure requestPager with 'totalRecords'
aRequestPager.bootstrap({totalRecords: 50});
```

Note: Both the clientPager and requestPager ```bootstrap``` function will accept an options param that will be extended by your Backbone.Paginator instance. However the 'totalRecords' property will be set implicitly by the clientPager.

[More on Backbone bootstrapping](http://ricostacruz.com/backbone-patterns/#bootstrapping_data)

### Styling

You're of course free to customize the overall look and feel of the paginators as much as you wish. By default, all sample applications make use of the [Twitter Bootstrap](http://twitter.github.com/bootstrap) for styling links, buttons and drop-downs. 

CSS classes are available to style record counts, filters, sorting and more:

![](img/paginator-styling2.png)

Classes are also available for styling more granular elements like page counts within `breadcrumb > pages`  e.g `.page`, `.page selected`:

![](img/paginator-classes.png)

There's a tremendous amount of flexibility available for styling and as you're in control of templating too, your paginators can be made to look as visually simple or complex as needed.

### Conclusions

Although it's certainly possible to write your own custom pagination classes to work with Backbone Collections, Backbone.Paginator tries to take care of much of this for you. 

It's highly configurable, avoiding the need to write your own paging when working with Collections of data sourced from your database or API. Use the plugin to help tame large lists of data into more manageable, easily navigatable, paginated lists. 

Additionally, if you have any questions about Backbone.Paginator (or would like to help improve it), feel free to post to the project [issues](https://github.com/addyosmani/backbone.paginator) list.