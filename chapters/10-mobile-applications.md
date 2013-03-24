## Backbone & jQuery Mobile

### Mobile app development with jQuery Mobile

The mobile web is huge and it is continuing to grow at an impressive rate. Along with the massive growth of the mobile internet comes a striking diversity of devices and browsers. As a result, making your applications cross-platform and mobile-ready is both important and challenging.
Creating native apps is expensive.
It is very costly in terms of time and it usually requires varied experiences in programming languages like Objective C , C#, Java and JavaScript to support multiple runtime environments.

HTML, CSS, and JavaScript enables you to build an app only once. Because of the common runtime environment, the browser, it supports a broad band of mobile devices such as tablets, smartphones, notebooks or PCs.

The challenging task is not only to adopt contents like text and pictures properly to various screen resolutions but also to have same user experience across native apps by different operating systems.
Like jQueryUI, jQuery Mobile is a user interface framework, based on jQuery that works across all popular phones, tablet, e-reader, and desktop platforms. Built with accessibility and universal access in mind. 


The main idea of the framework is to enable anybody creating mobile apps, even without the knownledge of a programming language and no need to write complex, device specific CSS, but HTML only.
For this reason jQMobile follows two main principles, we first need to understand to be able to integrate the framework to Backbone.




#### 1. The Principle of progressive widget enhancement by jQMobile

JQuery Mobile follows a "progressive enhancement" and responsive web design principle by HTML-5 markup-driven definitions and configurations.

A page in jQuery Mobile consists of an element with a data-role="page" attribute.
Within the "page" container, any valid HTML markup can be used, but for typical pages in jQM, the immediate children of a "page" are divs with data-roles of "header", "content", and "footer".
The baseline requirement for a page is only the page wrapper to support the navigation system, the rest is optional.

A initial HTML page would look like:

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


JQuery Mobiles will transform the written HTML definition by the progressive widget enhancement API. See the results below:

![](../img/chapter10-1-1-1.png)

<i>Default HTML view in comparison to

#### 2. Understand the navigation of jQueryMobile


The navigation system controls its application lifecycle by automatically 'hijacking' standard links and form submissions and turning them into an AJAX request.
Whenever a link is clicked or a form is submitted, that event is automatically intercepted and is used to issue an AJAX request based on the href or form action instead of reloading the page. 

When the requested page loads, jQuery Mobile parses the document for an element with the data-role="page" attribute and inserts that code into the DOM of the original page.
Next, any widgets in the incoming page are enhanced to apply all the styles and behavior. The rest of the incoming page is discarded so any scripts, stylesheets or other information will not be included.

Roughly speaking, having its own event cycle, it is a tiny MVC framework which enables features like progressive widget enhancement, prefetching, cacheing and multi-page templating by HTML configurations, innately.
In general, the developer does not need to know about its event-workflow, but only about how to apply HTML based configurations, which will take action within the event phase.
The chapter "Intercept jQuery Mobile events" explains, goes into detail, how to handle special scenarios, when fine grained JavaScript adaptiona need to be applied.


To e.g. navigate to the "secondpage" with the appearance of a modal dialog with fade-transition, the data-rel="dialog", data-transition="fade" and href="index.html#secondpage" attributes can be added to the anchor tag.
Via the multi-page templating feature, you can add as many pages as you want to the same HTML file within the body-tag, by defining a div with a data-role "page" or "dialog" and the id, being the value of the link after the hashbang like 


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



For further introduction and explanations about jQuery Mobile visit:

http://view.jquerymobile.com/1.3.0/docs/intro/
http://view.jquerymobile.com/1.3.0/docs/widgets/pages/
http://view.jquerymobile.com/1.3.0/docs/intro/rwd.php

### Basic Backbone app setup for jQuery Mobile

Throughout "Developing Backbone.js Applications" content, we learned how to use and benefit from the Backbone-Router, -Views, -Events and -Model, such as advanced plugins and customized JavaScript programming.

The first major hurdle developers typically run into when building applications with jQuery Mobile and a M* framework is that they have their own opinions about how to handle application navigation.
The same is true for Backbone.

To combine Backbone and jQuery Mobile, we first need to disable the navigation system and the progressive enhancement of jQuery Mobile.
The second step will be to make usage of the custom API of jQM to apply configurations and enhance components by different techniques in the application lifecycle of Backbone.


The mobile app example presented in the following is build upon the existing codebase of the "TodoMVC Backbone-Require.js” of the chapter "Modular Development" and is enhanced to support jQuery Mobile.

![](../img/chapter10-1-1.png)

<i>Screenshot of the TodoMVC app with jQuery Mobile</i>

Apart from that it makes use of Grunt-BBB ( see chapter "Backbone Boilerplate & Grunt-BBB") as well as Handlebars.js (see the "Throrax"-chapter). Additional utilities useful for mobile applications will be provided, which can be easily combined and extended.


![](../img/chapter10-1-2.png)

<i>Workspace of the TodoMVC app with jQueryMobile and Backbone</i>


The initial load of the files to load by Require.js is as follows:

1. <b>jQuery</b>
2. <b>Underscore/Lodash</b>
3. <b>handlebars.compiled</b>
   By opening the console in the project directory and running the Grunt-Backbone command <b>bbb handlebars</b> or <b>bbb watch</b> in the console, 
   it will combine and compile all template files to <b>dist/debug/handlebars_packaged</b>.
4. <b>TodoRouter</b> (instantiates specific views)
5. <b>jQueryMobile</b>
6. <b>JqueryMobileCustomInitConfig</b>
7. Instantiation of the Backbone Router


Files instantiated, when redirected from the Backbone-Router are:

a) <b>BasicView.js and basic_page_simple.template</b>
   
   The BasicView is responsible for the Handlebars multipage-template processing.
   Last, it calls the jQuery Mobile API "$.mobile.changePage", when Backbone.View.render is applied 
   to handle page navigation and progressive widget enhancement.
   
b) <b>Concrete view with its template partial</b>

    E.g. <b>EditTodoPage.js</b> and <b>editTodoView.template_partial</b>
    
    

The head of index.html needs to load the <b>jquerymobile.css</b> as well as the <b>base.css</b>,
which is being used by all Todo-MVC apps, and the "index.css" for some project specific custom CSS.

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


### The workflow with Backbone and jQueryMobile



By delegating routing and navigation functionalities of the jQuery Mobile Framework to Backbone, we can profit from its clear separation of application structure to later on easily share application logic between a desktop webpage, tablets and mobile apps.

Backbone.Router offers an explicit way to define custom navigation routes. Whilst jQuery Mobile encourages the use of URL hash fragments to reference separate pages or views in the same document.
Some of the ideas that have been previously proposed to work-around this problem included manually patching Backbone and jQuery Mobile.

The solution demonstrated below, will not only simplify the handling of the jQuery Mobile component initialization event-cycle, but also enables the usage of the existing Backbone’s Router functionalities.

To adopt the navigation control from jQuery Mobile to Backbone, we first need to apply some specific setting to the mobileinit event, after the framework was loaded to e.g. let the Backbonerouter decide, which page to load.

A suggested configuration with its descriptions to delegate jQM navigation and enable manual widget creation triggering to Backbone is listed below:

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

The behaviour and usage of the new workflow will be explained, grouped by its functionalities:

<b>Routing to a concrete View-page</b>, <b>Management of mobile page templates</b> and <b>DOM management and $.mobile.changePage</b>.

For an overview and understanding the chronological order, have a look on the diagram below.
In the following, the <b>steps 1-11</b> are attached to the text and refer to the new workflow diagram of the mobile application.

![](../img/chapter10-2-1.png)

<i>Workflow of Backbone and jQueryMobile</i>

####Routing to a concrete View-page, inherited by BasicView

Whenever the hash URL has changed, e.g. by clicking the link, the configuration prevented jQM to trigger its events. Instead, the Backbone Router listens to the hash changes and decides which view to request.

However, experience has shown that, it is a good practice for mobile pages, to create basic anatomies by prototyping(inheritange) for various situations like a basic page, the usage of the jQuery Validation Plugin, popups or dialogs with jQM.


Then it becomes much easier to exchange device specific view logics, at runtime, and adopt general strategies- This would also help to add syntax and to support multi chaining of prototype inheritance with JavaScript and Backbone.
By creating a “BasicView” of superclass, we enable all inherited-view-pages to share a common way to handle jQM the common usage of a template engine and specific view handling.

In the build progress of Grunt/Yeoman, the semantic templates will be compiled by Handlebar.js and the AMDs template files are combined into a single file.
By merging all page definitions into a single-file-app, it becomes offline capable, which is important for mobile app.


#### Management of mobile page templates

A concrete View-page, you can override properties for static values and functions to return dynamic values of the super class <b>BasicView</b>.
These values will be later on processed by the BasicView to construct the HTML of a jQuery Mobile page with the help of Handlebars.

Additional dynamic template parameters e.g. Backbone Model information, will be taken from the specific View and merged with the ones from the BasicView.

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
                return this.model.toJSON();
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

<i>Sample of a concrete View of <b>EditTodoPage.js</b></i>


By default, the BasicView uses the <b>basic_page_simple.template</b> as the Handlebars template.
if you need a custom template, being used only once, 
or in the case you want to create a new Super-abstract-View, like the basic_page_simple, override the <b>getTemplateID</b> function.

```javascript
getTemplateID : function(){
  return "custom_page_template";
}
```

By convention, the overridden <b>id</b>-attribute will be taken as the id of the jQM page
as well as the filename of the corresponding template-file to be inserted as a partial in the <b>basic_page_simple</b> template.
In the case of the <b>EditTodoPage</b>-View, the name of the file will be <b>editTodoPage.template_partial</b>.
Every concrete page is meant to be a partial, which will be inserted in the <b>data-role="content"</b> element,
where the parameter <b>templatePartialPageID</b> is located.

Later on, the <b>getHeaderTitle</b> function from the <b>EditTodoPage</b> will be replaced in the abstract template with <b>headerTitle</b>.


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

<i>Code of <b>basic_page_simple.template</b></i>


The <b>whatis</b> Handlebars View helper does simple logging of parameters.


All the additional parameters being returned by <b>getSpecificTemplateValues</b> 
will be replaced in the concrete template <b>editTodoPage.template_partial</b>.
Because <b>footerContent</b> is expected to be used rarely, it can be returned in <b>getSpecificTemplateValues</b>.

In the case of the EditTodoPage View, all the model information is being returned,
where <b>title</b> is used in the concrete partial page.


```html
<div data-role="fieldcontain">
    <label for="todoDescription">Todo Description</label>
    <input type="text" name="todoDescription" id="todoDescription" value="{{title}}" />
</div>
    <a id="saveDescription" href="#" data-role="button" data-mini="true">Save</a>
```

<i>editTodoView.template_partial</i>


When <b>render</b> is triggered, the <b>basic_page_simple.template</b> and <b>editTodoView.template_partial</b> templates with the collected parameters from <b>EditTodoPage.js</b> and <b>BasicView</b>,
will be combined and generated by Handlebars templates to:

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

<i>Final HTML definition result of the Handlebars templating</i>

How the template parameters are collected by the <b>BasicView</b> and the HTML definition above will be enhanced by jQuery Mobile will be explained in the next section.


#### DOM management and $.mobile.changePage

When <b>render</b> is triggered, BasicView will clean up the DOM, by removing the previous page(Line 70).
To delete the elements from the DOM, <b>$.remove</b> cannot be used, but <b>$previousEl.detach()</b>.
<b>Detach</b> does not remove the elements attached events and data.
This is important, because jQuery Mobile still needs information to e.g.
trigger transition effects when switching to another page.

Other strategies than the one used in the function <b>cleanupPossiblePageDuplicationInDOM</b> to cleanup the DOM are applicable.
To only remove the old page having the same id than the current from the DOM, when it was already requested before,
would also be a working strategy of preventing DOM duplication. Depending on what fits best to your application needs,
it is also possible a one-banana problem to exchange it with a caching mechanism.


Next, BasicView will collect all template parameters from the concrete View implementation
and inserts the HTML of the requested page into the body(<b>steps 4, 5, 6 and 7</b>).
This is done between line 23 and 51.

Additional, the <b>data-role</b> will be set on the jQuery Mobile page. Common used attribute values are
<b>page</b>, <b>dialog</p> or <b>popup</b>.

As you can see, in line 74, it also contains a manual implementation to handle the back buttons action.
In special scenarios, the back button navigation functionality of jQuery Mobile was not working with older version.


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
35 // Generate HTML by the Handlebars templates
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


After the dynamic HTML is added to the DOM, <b>$.mobile.changePage</b> has to be applied.

It is the most important API call, because it triggers the jQuery Mobile component creation(<b>step 8</b>) of the current page and displays it(<b>step 9</b>.
(see <b>Final HTML definition result of the Handlebars templating</b> for the HTML definition, before enhancement) 

```javascript
<a data-mini="true" data-role="button" href="#" id="saveDescription" data-corners="true" 
data-shadow="true" data-iconshadow="true" data-wrapperels="span" data-theme="c" 
class="ui-btn ui-shadow ui-btn-corner-all ui-mini ui-btn-up-c">
    <span class="ui-btn-inner">
         <span class="ui-btn-text">Save</span>
     </span>
</a>
```

![](../img/chapter10-2-2.png)


UI enhancement is done in the enhanceJQMComponentsAPI function in line 52:

```javascript
$.mobile.changePage("#" + this.id, {
                changeHash: false,
                role: this.role
            });
```

To retain control of hash routing, <b>changeHash</b> has to be set to false as well as the proper <b>role</b>-parameter to guarantee proper page appearance.
Finally, changePage will show the new page to the user.


For the basic use cases, it is advised, to have one View per page, and always render again the complete page by calling $.mobile.changePage when widget enhancement needs to be done.

To progress component enrichment of a newly added HTML-fragment into the DOM, advanced techniques need to be applied, to guarantee correct appearance of the mobile components.
You have to be very careful, when creating partial HTML code and updating the values on UI elements.
The next section will explain how to handle such situations.


### Applying advanced jQM techniques to Backbone

#### Dynamic DOM Scripting


The solution described above solves the issues of handling routing with Backbone by calling $.mobile.changePage('pageID').
Additionally, it is guaranteed, that the HTML page will be completely enhanced by the markup of jQuery Mobile.

The second tricky part with jQuery Mobile is to dynamically manipulate specific DOM contents (e.g. after loading in content with Ajax).
We suggest you use this technique, only if there is evidence for an appreciable performance gain.
With the current version 1.3, jQM provides three ways, documented and explained below in the official API, on forums and blogs.


* <b>$(“pageId”).trigger(“pagecreate”)</b>
  <p>Creates markup of header, content as well as footer</p>
* <b>$(“anyElement”).trigger(“create”)</b>
  <p>Creates markup of the element as well as all children</p>
* <b>$(“myListElement”).listview(“refresh”)</b>
  <p>Every component of jQM offers plugins methods to
    update the state of specific UI elements.
    Sometimes, when creating a component from scratch, the following error can occur 'cannot call methods on listview prior to initialization'.
    This can be avoided, with component initialization prior to markup enhancement, by calling it in the following way:
    $('#mylist').listview().listview('refresh')
    </p>

To see more details and enhancements for further scripting pages of JQM read their API and follow the release notes, frequently.

http://jquerymobile.com/test/docs/pages/page-scripting.html

http://stackoverflow.com/questions/14468659/jquery-mobile-document-ready-vs-page-events/ 

http://stackoverflow.com/questions/14550396/jquery-mobile-markup-enhancement-of-dynamically-added-content


#### Model-Binding with jQuery Mobile

If you consider using a Model-Binding Plugin, you will need to come up with an automated mechanism to enrich single components.

After having a look on the previous section about 'Dynamic DOM Scripting',
it might not be acceptable, to completely recreate a component (e.g a Listview), which takes longer time to load and to reduce the complexity of event-delegation.
Instead, the component specific plugins, which will only update the needed parts of the HTML and CSS, should be used.
In the case of a Listview, you would need to call the following function to update the list of added, edited, or removed entries:

$('#mylist').listview()

You need to come up with a detection of the component type to decide which plugin method needs to be called. The jQuery Mobile Angular.js Adapter provides such a strategy and solution as well.

( https://github.com/tigbro/jquery-mobile-angular-adapter/blob/master/src/main/webapp/integration/jqmWidgetPatches.js)


#### Intercept jQuery Mobile events

In special situations you will need to to take action to a triggered jQuery Mobile event, which can be done like this:

```javascript
$('#myPage').live('pagebeforecreate', function(event){
         console.log(page was inserted into the DOM');
   	//run your own enhancement scripting here...
          // prevent the page plugin from making its manipulations
  	return false;
});

$('#myPage').live('pagecreate', function(event){
          console.log(‘page was enhanced by jQM');
});
```
In such scenarios, it is important to know when the jQuery Mobile events occurs. The following diagram and the API depicts the event-cycle.


![](../img/chapter10-3-1.png)

An alternative is the jQuery Mobile Router project, which you might need to replace with the Backbone Router. With the help of jQM-Router project, you could achieve a powerful way to intercept and route one of the various jQM events. It is an extension to jQuery Mobile, which can be used independenly.
If you intend to use it, consider 'Backbone.js Custom Builds' to exclude the router code (see http://gregfranko.com/backbone/customBuild/). This might save ~ 25%, out of the max compressed size of 17,1 KB.


#### Performance

Performance is a sensible topic on mobile devices.
jQuery Mobile provides various tools, to create performance logs, which gives you a good overview about the actual time spend to routing logic, component enhancement, and visual effects.

Depending on the device, the time spend on transitions can take up to 90% load time. To disable transitions all over, you can eighter pass the transition ‘none’ to $.mobile.changePage(), in the configuration code block:

```javascript
$(document).bind("mobileinit", function(){
…
// Otherwise, depending on Takes up to 90% of loadtime
  $.mobile.defaultPageTransition = "none";
  $.mobile.defaultDialogTransition = "none";
    });
  })

consider to add device specific settings, which can be achieved like: 

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


See further information on performance at:

https://github.com/jquery/jquery-mobile/tree/master/tools

http://backbonefu.com/2012/01/jquery-mobile-and-backbone-js-the-ugly/ 

http://www.objectpartners.com/2012/11/02/use-jquery-mobile%E2%80%99s-tools-suite-to-help-you-debug-and-improve-your-jquery-mobile-application/)


#### Multi platform support management by responsive design
TODO

The reality is that we don't always have the scope to create per-device experiences, so today's application will attempt to optimize the devices or browsers most likely to access it.
It is essential, that the content the application exposes, is readily available to all users, regardless of what browser they are accessing it from, so this principle will be kept in mind during the design phase.
We will be aiming to create a solution that is responsive from the start and jQuery Mobile simplifies a lot of the actual heavy lifting here as it supports responsive layouts, out of the box (including browsers that do not support media queries).
Many developers forget that jQM is a UI framework not dissimilar to jQuery UI. It is using the widget factory and is capable for being used for a lot more than we accredited.

Nowadays, in the common case a webpage already exists, and the management decides to offer a mobile app on top of it.
The webpages and the apps code will be independant of each other. The time spend on changes, are doubled, and mobile applications become more and more important.

<b>Mobile first</b> is the answer to address this issue and explains more advantages, to start over with the mobile app first and later on with the desktop application.

Further resources about Mobile first development are located at:

http://www.lukew.com/ff/entry.asp?933

http://www.abookapart.com/products/responsive-web-design

http://www.abookapart.com/products/mobile-first

http://shop.oreilly.com/product/0636920018100.do



To support multi platforms by the usage of jQuery Mobile and Backbone,
you can, descending by more time effort. Mobile first can be applied to all solutions:

1. Ideally, one code project, where only CSS differs for different devices.
2. Same code project, and at runtime, different HTML template code
   and super-classes are exchanged per device type.
3. Same code project, and at runtime, jQuery Mobile will be replaced 
   by another widget framework e.g. <b>jQueryUI</b> or <b>Twitter Boostrap</b>.
   Super-classes and configurations have to be replaced as well.
   The device specific code needs to be downloaded by the browser.
4. Different code projects, but parts of the functions are reused.
5. For the desktop app, have a completely separate code project.
   Reasons are the usage of complete different programming languagages
   and/or frameworks.

Let us discuss, why we think it is a good idea, to follow the <b>Mobile First</b> approach and do not change the UI framework.

If you have a look on the jQuery Mobile showcase(http://jquerymobile.com/demos/1.2.0/docs/forms/forms-all-mini.html), in a desktop browser, it doesn’t look anything like a mobile application.
The accordeons, date-pickers, sliders - everything in the desktop UI is re-using what jQM would be providing users on mobile devices.
Especially if you enable the attribute “data-mini” on components, you’ll not realize any difference.
Thanks to some media queries, the desktop UI can make optimal use of whitespace,
expanding component blocks out and providing alternative layouts whilst still making use of jQM as a component framework.

The benefit of this is that you don’t need to go pulling in jQuery UI separately to be able to take advantage of these features.
Thanks to the recent ThemeRoller the components can look pretty much exactly how I would like them to and users of the app can get a jQM UI for lower-resolutions and a jQM-ish UI for everything else.

The takeaway here is just to remember that if you’re not (already) going through the hassle of conditional script/style loading based on screen-resolution (using matchMedia.js etc), there are simpler approaches that can be taken to cross-device component theming.
