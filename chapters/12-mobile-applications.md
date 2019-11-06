# Backbone & jQuery Mobile

### Mobile app development with jQuery Mobile

The mobile web is huge and it is continuing to grow at an impressive rate. Along with the massive growth of the mobile internet comes a striking diversity of devices and browsers. As a result, making your applications cross-platform and mobile-ready is both important and challenging.
Creating native apps is expensive.
It is very costly in terms of time and it usually requires varied experiences in programming languages like Objective C , C#, Java and JavaScript to support multiple runtime environments.

HTML, CSS, and JavaScript enable you to build a single application targeting a common runtime environment: the browser. This approach supports a broad range of mobile devices such as tablets, smartphones, and notebooks along with traditional PCs.

The challenging task is not only to adapt contents like text and pictures properly to various screen resolutions but also to have same user experience across native apps under different operating systems.
Like jQueryUI, jQuery Mobile is a user interface framework based on jQuery that works across all popular phone, tablet, e-Reader, and desktop platforms. It is built with accessibility and universal access in mind. 


The main idea of the framework is to enable anyone to create a mobile app using only HTML. Knowledge of a programming language is not required and there is no need to write complex, device specific CSS. For this reason jQMobile follows two main principles we first need to understand in order to integrate the framework to Backbone: *progressive enhancement* and *responsive web design*.


#### The Principle of progressive widget enhancement by jQMobile

JQuery Mobile follows progressive enhancement and responsive web design principles using HTML-5 markup-driven definitions and configurations.

A page in jQuery Mobile consists of an element with a `data-role="page"` attribute.
Within the `page` container, any valid HTML markup can be used, but for typical pages in jQM, the immediate children are divs with `data-role="header"`, `data-role="content"`, and `data-role="footer"`.
The baseline requirement for a page is only a page wrapper to support the navigation system, the rest is optional.

An initial HTML page looks like this:

```html
<!DOCTYPE html>
<html>
<head>
	<title>Page Title</title>

	<meta name="viewport" content="width=device-width, initial-scale=1">

	<link rel="stylesheet" href="http://code.jquery.com/mobile/1.3.0/jquery.mobile-1.3.0.min.css" />
	<script src="http://code.jquery.com/jquery-1.9.1.min.js"></script>
	<script src="http://code.jquery.com/mobile/1.3.0/jquery.mobile-1.3.0.min.js"></script>
</head>
<body>

<div data-role="page">
  <div data-role="header">
    <h1>Page Title</h1>
  </div>
  <div data-role="content">
     <p>Page content goes here.</p>
     <form>
       <label for="slider-1">Slider with tooltip:</label>
       <input type="range" name="slider-1" id="slider-1" min="0" max="100" value="50" 
        data-popup-enabled="true">
     </form>
  </div>
  <div data-role="footer">
     <h4>Page Footer</h4>
  </div>
</div>
</body>
</html>
```

*Example HTML setup of a basic jQuery Mobile page*

JQuery Mobile will transform the written HTML definition to the rendered HTML and CSS using its Progressive Widget Enhancement API. It also executes JavaScript which is conditioned by configurations, attribute properties, and runtime specific settings.

This implies: Whenever HTML content is added or changed, it needs to be handled by the progressive widget enhancement of jQuery Mobile.

![](img/chapter10-1-1-1.png)

*Comparison of the user interface of the default HTML to the jQuery Mobile enhanced version*

#### Understanding jQuery Mobile Navigation

The jQuery Mobile navigation system controls its application's lifecycle by automatically "hijacking" standard links and form submissions and turning them into AJAX requests. Whenever a link is clicked or a form is submitted, that event is automatically intercepted and used to issue an AJAX request based on the href or form action instead of reloading the page.

When the page document is requested, jQuery Mobile searches the document for all elements with the `data-role="page"` attribute, parses its contents, and inserts that code into the DOM of the original page.
Once the new page is prepared, jQuery Mobile's JavaScript triggers a transition that shows the new page and hides the HTML of the previous page in the DOM.

Next, any widgets in the incoming page are enhanced to apply all the styles and behavior. The rest of the incoming page is discarded so any scripts, stylesheets, or other information will not be included.

Via the *multi-page templating feature*, you can add as many pages as you want to the same HTML file within the body tag by defining divs with `data-role="page"` or `data-role="dialog"` attributes along with an `id` which can be used in links (preceded by a hashbang):


```html
<html>
  <head>...</head>
  <body>
  ...
  <div data-role="page" id="firstpage">
    ...
   <div data-role="content"> 
     <a href="#secondpage">go to secondpage</a>
   </div>
  </div>
  <div data-role="page" id="secondpage">
    ...
    <div data-role="content" >
       <a href="#firstdialog" data-rel="dialog" >open a page as a dialog</a>
    </div>
  </div>
  <div data-role="dialog" id="firstdialog">
    ...
     <div data-role="content">
       <a href="#firstpage">leave dialog and go to first page</a>
     </div>
  </div>
</body>
</html>
```

*jQuery Mobile multi-page templating example*

To, for example, navigate to *secondpage* and have it appear in a modal dialog using a fade-transition, you would just add the `data-rel="dialog"`, `data-transition="fade"`, and `href="index.html#secondpage"` attributes to an anchor tag.

Roughly speaking, having its own event cycle, jQuery Mobile is a tiny MVC framework which includes features like
progressive widget enhancement, pre-fetching, caching, and multi-page templating by HTML configurations innately. In general, a Backbone.js developer does not need to know about its internal event workflow, but will need to know how to apply HTML-based configurations which will take action within the event phase. The *Intercepting jQuery Mobile Events* section goes into detail regarding how to handle special scenarios when fine-grained JavaScript adaptions need to be applied.

For further introduction and explanations about jQuery Mobile visit:

* [http://view.jquerymobile.com/1.3.0/docs/intro/](http://view.jquerymobile.com/1.3.0/docs/intro/)
* [http://view.jquerymobile.com/1.3.0/docs/widgets/pages/](http://view.jquerymobile.com/1.3.0/docs/widgets/pages/)
* [http://view.jquerymobile.com/1.3.0/docs/intro/rwd.php](http://view.jquerymobile.com/1.3.0/docs/intro/rwd.php)

### Basic Backbone app setup for jQuery Mobile

The first major hurdle developers typically run into when building applications with jQuery Mobile and an MV* framework is that both frameworks want to handle application navigation.

To combine Backbone and jQuery Mobile, we first need to disable jQuery Mobile's navigation system and progressive enhancement. The second step will then be to make use of jQM's custom API to apply configurations and enhance components during Backbone's application lifecycle instead.

The mobile app example presented here is based on the existing codebase of the TodoMVC Backbone-Require.js example, which was discussed in an earlier chapter, and is enhanced to support jQuery Mobile.

![](img/chapter10-1-1.png)

*Screenshot of the TodoMVC app with jQuery Mobile*

This implementation makes use of Grunt-BBB as well as Handlebars.js. Additional utilities useful for mobile applications will be provided, which can be easily combined and extended.
(see the *Backbone Boilerplate & Grunt-BBB* and *Backbone Extensions* chapters) 

![](img/chapter10-1-2.png)

*Workspace of the TodoMVC app with jQueryMobile and Backbone*

The order of the files loaded by Require.js is as follows:

1. jQuery
2. Underscore/Lodash
3. handlebars.compiled
4. TodoRouter (instantiates specific views)
5. jQueryMobile
6. JqueryMobileCustomInitConfig
7. Instantiation of the Backbone Router

By opening the console in the project directory and then running the Grunt-Backbone command `grunt handlebars` or `grunt watch` in the console, it will combine and compile all template files to `dist/debug/handlebars_packaged`. To start the application, run `grunt server`.

Files instantiated, when redirected from the Backbone-Router are:

a) *BasicView.js* and *basic_page_simple.template*

   The BasicView is responsible for the Handlebars multipage-template processing. Its implementation of `render` calls the jQuery Mobile API `$.mobile.changePage` to handle page navigation and progressive widget enhancement.
   
b) Concrete view with its template partial

   E.g., `EditTodoPage.js` and `editTodoView.template_partial`


The head section of `index.html` needs to load the `jquerymobile.css` as well as the `base.css`,
which is used by all Todo-MVC apps, and the `index.css` for some project-specific custom CSS.

```html
<html>
<head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
    <meta name="viewport" content="width=device-width,initial-scale=1">

    <title>TodoMVC Jquery Mobile</title>

<!-- widget and responsive design styles -->
    <link rel="stylesheet" href="/assets/css/jquerymobile.css">
<!-- used by all TodoMVC apps -->
    <link rel="stylesheet" href="/assets/css/base.css">
<!-- custom css -->
    <link rel="stylesheet" href="/assets/css/index.css">
</head>

<body>
    <script data-main="/app/config" src="/assets/js/libs/require.js"></script>
</body>
</html>
```

*index.html*

### Workflow with Backbone and jQueryMobile

By delegating the routing and navigation functions of the jQuery Mobile Framework to Backbone, we can profit from its clear separation of application structure to later on easily share application logic between a desktop webpage, tablets, and mobile apps.

We now need to contend with the different ways in which Backbone and jQuery Mobile handle requests. `Backbone.Router` offers an explicit way to define custom navigation routes, while jQuery Mobile uses URL hash fragments to reference separate pages or views in the same document.

Some of the ideas that have been previously proposed to work-around this problem included manually patching Backbone and jQuery Mobile. The solution demonstrated below will not only simplify the handling of the jQuery Mobile component initialization event-cycle, but also enables use of existing Backbone Router handlers.

To adapt the navigation control from jQuery Mobile to Backbone, we first need to apply some specific settings to the `mobileinit` event which occurs after the framework has loaded in order to let the Backbone Router decide which page to load.

A configuration which will get jQM to delegate navigation to Backbone and which will also enable manual widget creation triggering is given below:

```javascript
$(document).bind("mobileinit", function(){

// Disable jQM routing and component creation events   
   // disable hash-routing
   $.mobile.hashListeningEnabled = false;
   // disable anchor-control
   $.mobile.linkBindingEnabled = false;
   // can cause calling object creation twice and back button issues are solved
   $.mobile.ajaxEnabled = false;
   // Otherwise after mobileinit, it tries to load a landing page
   $.mobile.autoInitializePage = false;
   // we want to handle caching and cleaning the DOM ourselves
   $.mobile.page.prototype.options.domCache = false;

// consider due to compatibility issues
   // not supported by all browsers
   $.mobile.pushStateEnabled = false;
   // Solves phonegap issues with the back-button
   $.mobile.phonegapNavigationEnabled = true;
   //no native datepicker will conflict with the jQM component
   $.mobile.page.prototype.options.degradeInputs.date = true;
});
```

*jquerymobile.config.js*

The behaviour and usage of the new workflow will be explained below, grouped by its functionalities:

a) Routing to a concrete View-page

b) Management of mobile page templates

c) DOM management

d) $.mobile.changePage


In the following discussion, the steps 1-11 in the text refer to the new workflow diagram of the mobile application below.

![](img/chapter10-2-1.png)

*Workflow of TodoMVC, with Backbone and jQueryMobile*

####Routing to a concrete View page, Inheriting from BasicView

When the hash URL changes, e.g., a link is clicked, the configuration above prevents jQM from triggering its events. Instead, the Backbone Router listens to the hash changes and decides which view to request.

Experience has shown that, for mobile pages, it is a good practice to create basic prototypes for jQM components such as basic pages, popups, and dialogs, as well as for using the jQuery Validation Plugin. This makes it much easier to exchange device-specific view logic at runtime and adopt general strategies. This will also help to add syntax and to support multi-chaining of prototype inheritance with JavaScript and Backbone.

By creating a `BasicView` superclass, we enable all inheriting view-pages to share a common way of handling jQM along with common usage of a template engine and specific view handling.

When building with Grunt/Yeoman, the semantic templates are compiled by Handlebar.js and the AMDs template files are combined into a single file. By merging all page definitions into a single-file-app, it becomes offline capable, which is important for mobile app.


#### Management of Mobile Page Templates

Within a concrete View page, you can override properties for static values and functions to return dynamic values of the super class `BasicView`. These values will be processed later by the BasicView to construct the HTML of a jQuery Mobile page with the help of Handlebars.

Additional dynamic template parameters, e.g., Backbone model information, will be taken from the specific View and merged with the ones from the BasicView.

A concrete View might look like:

```javascript
define([
    "backbone", "modules/view/abstract/BasicView"],
    function (Backbone, BasicView) {
        return BasicView.extend({
            id : "editTodoView", 
            getHeaderTitle : function () {
                return "Edit Todo";
            },
            getSpecificTemplateValues : function () {
                return _.clone(this.model.attributes);
            },
            events : function () {
                // merged events of BasicView, to add an older fix for back button functionality
                return _.extend({
                    'click #saveDescription' : 'saveDescription'
                }, this.constructor.__super__.events);
            },
            saveDescription : function (clickEvent) {
                this.model.save({
                    title : $("#todoDescription", this.el).val()
                });
                return true;
            }
        });
    });
```

*A concrete View (EditTodoPage.js)*


By default, the BasicView uses `basic_page_simple.template` as the Handlebars template. If you need to use a custom template or want to introduce a new Super abstract View with an alternate template, override the `getTemplateID` function:

```javascript
getTemplateID : function(){
  return "custom_page_template";
}
```

By convention, the `id` attribute will be taken as the id of the jQM page as well as the filename of the corresponding template file to be inserted as a partial in the `basic_page_simple` template. In the case of the `EditTodoPage` view, the name of the file will be `editTodoPage.template_partial`.

Every concrete page is meant to be a partial, which will be inserted in the `data-role="content"` element,
where the parameter `templatePartialPageID` is located.

Later on, the result of the `getHeaderTitle` function from `EditTodoPage` will replace the *headerTitle* in the abstract template.


```javascript
<div data-role="header">
        {{whatis "Specific loaded Handlebars parameters:"}}
        {{whatis this}}
        <h2>{{headerTitle}}</h2>
        <a id="backButton" href="href="javascript:history.go(-1);" data-icon="star" data-rel="back" >back</a>
    </div>
    <div data-role="content">
        {{whatis "Template page trying to load:"}}
        {{whatis templatePartialPageID}}
        {{> templatePartialPageID}}
    </div>
    <div data-role="footer">
        {{footerContent}}
</div>
```

*basic_page_simple.template*


*Note: The `whatis` Handlebars View helper does simple logging of parameters.*


All the additional parameters being returned by `getSpecificTemplateValues` will be inserted into the concrete template `editTodoPage.template_partial`.

Because `footerContent` is expected to be used rarely, its content is returned by `getSpecificTemplateValues`.

In the case of the EditTodoPage view, all the model information is being returned and `title` is used in the concrete partial page:


```html
<div data-role="fieldcontain">
    <label for="todoDescription">Todo Description</label>
    <input type="text" name="todoDescription" id="todoDescription" value="{{title}}" />
</div>
    <a id="saveDescription" href="#" data-role="button" data-mini="true">Save</a>
```

*editTodoView.template_partial*


When `render` is triggered, the `basic_page_simple.template` and `editTodoView.template_partial` templates will be loaded and the parameters from `EditTodoPage` and `BasicView` will be combined and generated by Handlebars to generate:

```html
    <div data-role="header">
        <h2>Edit Todo</h2>
        <a id="backButton" href="href="javascript:history.go(-1);" data-icon="star" data-rel="back" >back</a>
    </div>
    <div data-role="content">
      <div data-role="fieldcontain">
       <label for="todoDescription">Todo Description</label>
       <input type="text" name="todoDescription" id="todoDescription" value="Cooking" />
      </div>
      <a id="saveDescription" href="#" data-role="button" data-mini="true">Save</a>
    </div>
    <div data-role="footer">
        Footer
    </div>
```

*Final HTML definition resulting from basic_page_simple_template and editTodoView.template_partial*

The next section explains how the template parameters are collected by the `BasicView` and the HTML definition is loaded.


#### DOM management and $.mobile.changePage

When `render` is executed (line 29 is the source code listing below), `BasicView` first cleans up the DOM by removing the previous page (line 70). To delete the elements from the DOM, `$.remove` cannot be used, but `$previousEl.detach()` can be since `detach` does not remove the element's attached events and data.

This is important, because jQuery Mobile still needs information (e.g., to trigger transition effects when switching to another page). Keep in mind that the DOM data and events should be cleared later on as well to avoid possible performance issues.

Other strategies than the one used in the function `cleanupPossiblePageDuplicationInDOM` to cleanup the DOM are viable. To only remove the old page having the same id as the current from the DOM, when it was already requested before, would also be a working strategy of preventing DOM duplication. Depending on what fits best to your application needs, it is also possibly a one-banana problem to exchange it using a caching mechanism.

Next, `BasicView` collects all template parameters from the concrete View implementation and inserts the HTML of the requested page into the body. This is done in steps 4, 5, 6, and 7 in the diagram above (between lines 23 and 51 in the source listing).

Additionally, the `data-role` will be set on the jQuery Mobile page. Commonly used attribute values are
page, dialog, or popup.

As you can see, (starting at line 74), the `goBackInHistory` function contains a manual implementation to handle the back button's action. In certain scenarios, the back button navigation functionality of jQuery Mobile was not working with older versions and disabled jQMobile's navigation system.


```javascript
 1 define([
 2     "lodash",
 3     "backbone",
 4     "handlebars",
 5     "handlebars_helpers"
 6 ],
 7 
 8 function (_, Backbone, Handlebars) {
 9     var BasicView = Backbone.View.extend({
10         initialize: function () {
11             _.bindAll();
12             this.render();
13         },
14         events: {
15             "click #backButton": "goBackInHistory"
16         },
17         role: "page",
18         attributes: function () {
19             return {
20                 "data-role": this.role
21             };
22         },
23         getHeaderTitle: function () {
24             return this.getSpecificTemplateValues().headerTitle;
25         },
26         getTemplateID: function () {
27             return "basic_page_simple";
28         },
29         render: function () {
30             this.cleanupPossiblePageDuplicationInDOM();
31             $(this.el).html(this.getBasicPageTemplateResult());
32             this.addPageToDOMAndRenderJQM();
33             this.enhanceJQMComponentsAPI();
34         },
35 // Generate HTML using the Handlebars templates
36         getTemplateResult: function (templateDefinitionID, templateValues) {
37             return window.JST[templateDefinitionID](templateValues);
38         },
39 // Collect all template paramters and merge them
40         getBasicPageTemplateResult: function () {
41             var templateValues = {
42                 templatePartialPageID: this.id,
43                 headerTitle: this.getHeaderTitle()
44             };
45             var specific = this.getSpecificTemplateValues();
46             $.extend(templateValues, this.getSpecificTemplateValues());
47             return this.getTemplateResult(this.getTemplateID(), templateValues);
48         },
49         getRequestedPageTemplateResult: function () {
50             this.getBasicPageTemplateResult();
51         },
52         enhanceJQMComponentsAPI: function () {
53 // changePage
54             $.mobile.changePage("#" + this.id, {
55                 changeHash: false,
56                 role: this.role
57             });
58         },
59 // Add page to DOM
60         addPageToDOMAndRenderJQM: function () {
61             $("body").append($(this.el));
62             $("#" + this.id).page();
63         },
64 // Cleanup DOM strategy
65         cleanupPossiblePageDuplicationInDOM: function () {
66         // Can also be moved to the event "pagehide": or "onPageHide"
67             var $previousEl = $("#" + this.id);
68             var alreadyInDom = $previousEl.length >= 0;
69             if (alreadyInDom) {
70                 $previousEl.detach();
71             }
72         },
73 // Strategy to always support back button with disabled navigation
74         goBackInHistory: function (clickEvent) {
75             history.go(-1);
76             return false;
77         }
78     });
79 
80     return BasicView;
81 });
```

*BasicView.js*

After the dynamic HTML is added to the DOM, `$.mobile.changePage` has to be applied at step 8 (code line 54).

This is the most important API call, because it triggers the jQuery Mobile component creation for the current page.

Next, the page will be displayed to the user at step 9.


```javascript
<a data-mini="true" data-role="button" href="#" id="saveDescription" data-corners="true" 
data-shadow="true" data-iconshadow="true" data-wrapperels="span" data-theme="c" 
class="ui-btn ui-shadow ui-btn-corner-all ui-mini ui-btn-up-c">
    <span class="ui-btn-inner">
         <span class="ui-btn-text">Save</span>
     </span>
</a>
```

![](img/chapter10-2-2.png)

*Look and feel of the written HTML code and the jQuery Mobile enhanced Todo description page*

UI enhancement is done in the `enhanceJQMComponentsAPI` function in line 52:

```javascript
$.mobile.changePage("#" + this.id, {
                      changeHash: false,
                      role: this.role
                    });
```

To retain control of hash routing, `changeHash` has to be set to false and the proper `role` parameter provided to guarantee proper page appearance. Finally, `changePage` will show the new page with its defined transition to the user.

For the basic use cases, it is advised to have one View per page, and always render the complete page again by calling `$.mobile.changePage` when widget enhancement needs to be done.

To progress component enrichment of a newly added HTML-fragment into the DOM, advanced techniques need to be applied to guarantee correct appearance of the mobile components. You need to be very careful when creating partial HTML code and updating values on UI elements. The next section will explain how to handle these situations.


### Applying advanced jQM techniques to Backbone

#### Dynamic DOM Scripting

The solution described above solves the issues of handling routing with Backbone by calling `$.mobile.changePage('pageID')`.  Additionally, it guarantees that the HTML page will be completely enhanced by the markup for jQuery Mobile.

The second tricky part with jQuery Mobile is to dynamically manipulate specific DOM contents (e.g. after loading in content with Ajax). We suggest you use this technique only if there is evidence for an appreciable performance gain.

With the current version (1.3), jQM provides three ways, documented and explained below in the official API, on forums, and blogs.

* **$(“pageId”).trigger(“pagecreate”)**

    *Creates markup of header, content as well as footer*

* **$(“anyElement”).trigger(“create”)**

    *Creates markup of the element as well as all children*

* **$(“myListElement”).listview(“refresh”)**
* **$('[type="radio"]').checkboxradio()**
* **$('[type="text"]').textinput()**
* **$('[type="button"]').button()**
* **$('[data-role="navbar"]').navbar()**
* **$('[type="range"]').slider()**
* **$('select').selectmenu()**

    *Every component of jQM offers plugins methods they can invoke to update the state of specific UI elements.*


Sometimes, when creating a component from scratch, the following error can occur: 'cannot call methods on listview prior to initialization'. This can be avoided, with component initialization prior to markup enhancement, by calling it in the following way:

```javascript
 $('#mylist').listview().listview('refresh')
```

To see more details and enhancements for further scripting pages of JQM read their API and follow the release notes frequently.

* [jQuery Mobile: Page Scripting](http://jquerymobile.com/test/docs/pages/page-scripting.html)
* [jQuery Mobile: Document Ready vs. Page Events](http://stackoverflow.com/questions/14468659/jquery-mobile-document-ready-vs-page-events/ )
* [StackOverflow: Markup Enhancement of Dynamically Added Content](http://stackoverflow.com/questions/14550396/jquery-mobile-markup-enhancement-of-dynamically-added-content)


If you consider using a `Model-Binding Plugin`, you will need to come up with an automated mechanism to enrich single components.

After having a look at the previous section about Dynamic DOM Scripting, it might not be acceptable to completely re-create a component (e.g. a Listview) which takes a longer time to load and to reduce the complexity of event-delegation. Instead, the component-specific plugins, which will only update the needed parts of the HTML and CSS, should be used.

In the case of a Listview, you would need to call the following function to update the list of added, edited, or removed entries:

```javascript
$('#mylist').listview()
```

You need to come up with a means of detecting the component type to in order to decide which plugin method needs to be called. The jQuery Mobile Angular.js Adapter provides such a strategy and solution as well.

[Example of Model Binding with jQuery Mobile](https://github.com/tigbro/jquery-mobile-angular-adapter/blob/master/src/main/webapp/integration/jqmWidgetPatches.js)


#### Intercepting jQuery Mobile Events

In special situations you will need to take action on a triggered jQuery Mobile event, which can be done as follows:

```javascript
$('#myPage').live('pagebeforecreate', function(event){
         console.log('page was inserted into the DOM');
   	//run your own enhancement scripting here...
          // prevent the page plugin from making its manipulations
  	return false;
});

$('#myPage').live('pagecreate', function(event){
          console.log(‘page was enhanced by jQM');
});
```

In such scenarios, it is important to know when the jQuery Mobile events occur. The following diagram  depicts the event cycle (page A is the outgoing page and page B is the incoming page).


![](img/chapter10-3-1.png)

*jQuery Mobile Event Cycle*

An alternative is the jQuery Mobile Router project, which you might use to replace the Backbone Router. With the help of the jQM Router project, you could achieve a powerful way to intercept and route one of the various jQM events. It is an extension to jQuery Mobile, which can be used independently.

Be aware that jQM-Router misses some features of Backbone.Router and is tightly coupled with the jQuery Mobile framework. For these reasons, we did not use it for the TodoMVC app. If you intend to use it, consider using a Backbone.js custom build to exclude the router code. This might save around 25% relative to the max compressed size of 17,1 KB.

[Backbone's Custom Builder](http://gregfranko.com/backbone/customBuild/)


#### Performance

Performance is an important topic on mobile devices. jQuery Mobile provides various tools that create performance logs which can give you a good overview of the actual time spent in routing logic, component enhancement, and visual effects.

Depending on the device, the time spent on transitions can take up to 90% of the load time. To disable all transitions, you can either pass the transition `none` to `$.mobile.changePage()`, in the configuration code block:

```javascript
$(document).bind("mobileinit", function(){
…
// Otherwise, depending on takes up to 90% of loadtime
  $.mobile.defaultPageTransition = "none";
  $.mobile.defaultDialogTransition = "none";
    });
  })
```

or consider adding device-specific settings, for example: 

```javascript
$(document).bind("mobileinit", function(){

  var iosDevice =((navigator.userAgent.match(/iPhone/i))
  || (navigator.userAgent.match(/iPod/i))) ? true : false;

  $.extend(  $.mobile , {
    slideText :  (iosDevice) ? "slide" : "none",
    slideUpText :  (iosDevice) ? "slideup" : "none",
    defaultPageTransition:(iosDevice) ? "slide" : "none",
    defaultDialogTransition:(iosDevice) ? "slide" : "none"
  });
```

Also, consider doing your own pre-caching of enhanced jQuery Mobile pages.

The jQuery Mobile API is frequently enhanced with regards to this topic in each new release. We suggest you take a look at the latest updated API to determine an optimal caching strategy with dynamic scripting that best fits your needs.

For further information on performance, see the following:

* [jQuery Mobile Profiling Tools](https://github.com/jquery/jquery-mobile/tree/master/tools)
* [Device specific jQuery Mobile configuations](http://backbonefu.com/2012/01/jquery-mobile-and-backbone-js-the-ugly/)
* [jQuery Mobile Debugging tools](http://www.objectpartners.com/2012/11/02/use-jquery-mobile%E2%80%99s-tools-suite-to-help-you-debug-and-improve-your-jquery-mobile-application/)
* [jQuery Mobile precaching functionalities](http://jquerymobile.com/demos/1.2.0/docs/pages/page-cache.html)


#### Clever Multi-Platform Support Management

Nowadays, a company typically has an existing webpage and management decides to provide an additional mobile app to customers. The code of the web page and the mobile app become independent of each other and the time required for content or feature changes becomes much higher than for the webpage alone.

As the trend is towards an increasing number of mobile platforms and dimensions, the effort required to support them is only increasing as well. Ultimately, creating per-device experiences is not always viable. However, it is essential that content is available to all users, regardless of their browser and platform. This principle must be kept in mind during the design phase.

*[Responsive Design](http://www.lukew.com/ff/entry.asp?933)* and *[Mobile First](http://www.abookapart.com/products/mobile-first)* approaches address these challenges.

The mobile app architecture presented in this chapter takes care of a lot of the actual heavy lifting required, as it supports responsive layouts out of the box and even supports browsers which cannot handle media queries. It might not be obvious that jQM is a UI framework not dissimilar to jQuery UI. jQuery Mobile is using the widget factory and is capable of being used for more than just mobile environments.

To support multi-platform browsers using jQuery Mobile and Backbone,
you can, in order of increasing time  and effort:

1. Ideally, have one code project, where only CSS differs for different devices.
2. Same code project, and at runtime different HTML templates
   and super-classes are exchanged per device type.
3. Same code project, and the Responsive Design API and most widgets of jQuery Mobile will be reused.
   For the desktop browser, some components will be added by another widget framework
   (e.g. *jQueryUI* or *Twitter Boostrap*), e.g. controlled by the HTML templating.
4. Same code project, and at runtime, jQuery Mobile will be completely replaced 
   by another widget framework (e.g. *jQueryUI* or *Twitter Boostrap*).
   Super-classes and configurations, as well as concrete Backbone.View code snippets need to be replaced.
5. Different code projects, but common modules are reused.
6. For the desktop app, there is a completely separate code project.
   Reasons might be the usage of complete different programming languagages
   and/or frameworks, lack of Responsive Design knowledge or legacy of pollution.

The ideal solution, to build a nice-looking desktop application with only one mobile framework, sounds crazy, but is feasible.

If you have a look at the jQuery Mobile API page in a desktop browser, it does not look anything like a mobile application.

![](img/chapter10-3-3.png)

*Desktop view of the jQuery Mobile API and Docs application (http://view.jquerymobile.com/1.3.0/)*

The same goes for the jQuery Mobile design examples, where jQuery Mobile intends to add further user interface experiences.

![](img/chapter10-3-4.png)

*Design examples of jQuery Mobile for desktop environments, http://jquerymobile.com/designs/#desktop*

The accordions, date-pickers, sliders - everything in the desktop UI is re-using what jQM would be providing users on mobile devices. By way of example, adding the attribute `data-mini="true"` on components will lose the clumsiness of the mobile widgets on a desktop browser.

See http://jquerymobile.com/demos/1.2.0/docs/forms/forms-all-mini.html, Mini-widgets for desktop applications by jQuery Mobile.

Thanks to some media queries, the desktop UI can make optimal use of whitespace, expanding component blocks out and providing alternative layouts while still making use of jQM as the component framework.

The benefit of this is that you don’t need to pull in another widget framework (e.g., jQuery UI) separately to be able to take advantage of these features. Thanks to the ThemeRoller, the components can look pretty much exactly how you would like them to and users of the app can get a jQM UI for lower-resolutions and a jQM-ish UI for everything else.

The take away here is just to remember that if you are not already going through the hassle of conditional script/style loading based on screen-resolution (using matchMedia.js, etc.), there are simpler approaches that can be taken to cross-device component theming. At least the Responsive Design API of jQuery Mobile, which was added since version 1.3.0, is always reasonable because it will work for mobile as well as for desktop. In summary, you can manage jQuery Mobile components to give users a typical desktop appearance and they will not realize a difference.

[Responsive Design with jQuery Mobile](http://view.jquerymobile.com/1.3.0/docs/intro/rwd.php)

Also, if you hit your limits of CSS-styling and configurations of your jQuery Mobile application for desktop browsers, the additional effort to use jQuery Mobile and Twitter Bootstrap together can be quite simple.
In the case that a desktop browser requests the page and Twitter Bootstrap has been loaded, the mobile TodoMVC app would need conditional code to not trigger the  jQM Widget processive enhancement plugins API (demonstrated in the  *Dynamic DOM Scripting* section) in the Backbone.View implementation. Therefore, as explained in the previous sections, we recommend triggering widget enhancements by `$.mobile.changePage` only once to load the complete page.

An example of such a widget hybrid usage can be seen here:

![](img/chapter10-3-2.png)

*[Appengine boilerplate, desktop and mobile appearance](http://appengine.beecoss.com)*

Although it is using server-side technologies for templating using the programming language Python,
the principle of triggering progressive enhancement at page load is the same as `$mobile.changePage`.

As you can see, the JavaScript and even the CSS stays the same. The only device-specific conditions and differences in implementations are for selecting the appropriate framework imports, which are located in the HTML template:

```html
...
 {% if is_mobile %}
    <link rel="stylesheet" href="/mobile/jquery.mobile-1.1.0.min.css" />
    {% else %}
      <link rel="apple-touch-icon" href="/apple-touch-icon.png" />
      <link rel="stylesheet" href="/css/style.css" />
      <link rel="stylesheet" href="/css/bootstrap.min.css">
      <link rel="stylesheet" href="/css/bootstrap-responsive.min.css">
    {% endif %}
      <link rel="stylesheet" href="/css/main.css" />

    {% block mediaCSS %}{% endblock %}
...
 {% if is_mobile %}
      <script src="/mobile/jquery.mobile-1.1.0.min.js"></script>
    {% else %}
      <script src="/js/libs/bootstrap.min.js"></script>
    {% endif %}
...
```
