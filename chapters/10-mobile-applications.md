## Backbone & jQuery Mobile

### Mobile app development with JavaScript

The mobile web is huge and it's continuing to grow at an impressive rate. Along with the massive growth of the mobile internet comes a striking diversity of devices and browsers. As a result, making your applications cross-platform and mobile-ready is both important and challenging.
Creating native apps is expensive.
It is very costly in terms of time and it usually requires varied experiences in programming languages like Objective C , C#, Java and JavaScript to support multiple runtime environments.

By using HTML, CSS, and JavaScript it enables you to build an app only once. Because of the common runtime environment, the browser, it supports a broad band of mobile devices such as tablets.

The greatest difficulty is to adopt content like text and pictures properly to the various screen resolutions (=Responsive Design) and to get the same user experience with the native apps by different operating system vendors.

The mobile app example is build upon the existing codebase of “TodoMVC Backbone-Require.js” ( see https://github.com/addyosmani/todomvc/tree/gh-pages/dependency-examples/backbone_require) and enhanced to support jQuery Mobile. On top it uses the  build tool Grunt.js#(BBB#), as well as Handlebars.js, and the performance improved and AMD enabled Underscore version, Lodash. Furthermore, we will also provide utilities, useful for mobile applications, which can be easily extended and combined.

The basic setup of the libraries to load by Require.js are in the following order:

1. jQuery
2. Underscore/Lodash
3. TodoRouter (instantiates specific views)
4. jQueryMobile
5. JqueryMobileCustomInitConfig
6. Instantiation of the Backbone Router

![](../img/chapter10-1-1.png)

### Delegating jQuery Mobile navigation to Backbone

The first major hurdle developers typically run into when building Backbone applications with jQuery Mobile is that both frameworks have their own opinions about how to handle application navigation.

By delegating routing and navigation functionalities of the jQuery Mobile Framework to Backbone, we can profit from its clear separation of application structure to later on easily share application logic between a desktop webpage, tablets and mobile apps.

Through Backbone.Router, Backbone offers an explicit way to define custom navigation routes, whilst jQuery Mobile encourages the use of URL hash fragments to reference separate pages or views in the same document. jQuery Mobile also supports automatically pulling in external content for links through XHR calls meaning that there can be quite a lot of inter-framework confusion about what a link pointing at #todo/id should actually be doing.
Some of the ideas that have been previously proposed to work-around this problem included manually patching Backbone and jQuery Mobile.

The solution demonstrated will simplify the handling of the jQuery Mobile component initialization event-cycle and enable the usage of the existing Backbone’s Router functionalities.

To take over the navigation control from jQuery Mobile to Backbone, we first have to apply specific settings to the mobileinit event(Flow of Component interactions, Step 1) , after the framework was loaded.

A preferable configuration with its descriptions is listed below:

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

![](../img/chapter10-2-1.png)

Every time the hash URL was changed, e.g. by clicking a link, the configuration prevents jQM to trigger its events. Instead, the Backbone Router will listen to hash changes and decide with view to request.

Experience has shown that it’s a good practice, especially for mobile pages, to create prototypes(superclasses) for various situations like the usage of the jQuery Validation Plugin or Popups with jQM.
It will become much easier to exchange device specific view logic, at runtime and adopt general strategies.
To add shorter syntax and to support multi chaining of prototype inheritance with JavaScript and Backbone, the “Backbone_Super”(see https://github.com/lukasolson/Backbone-Super) function is loaded by Require.js. A parent function can be called with this._super("save", arguments);

By creating a “BasicView” superclass, we enable all inherited-view-pages to share a common way to handle jQM specific view handling and the common usage of a template engine.

```javascript
define([
    "lodash",
    "backbone",
    "handlebars",
    "handlebars_helpers"
],

function (_, Backbone, Handlebars) {
    var BasicView = Backbone.View.extend({
        initialize: function () {
            _.bindAll();
            this.render();
        },
        events: {
            "click #backButton": "goBackInHistory"
        },
        role: "page",
        attributes: function () {
            return {
                "data-role": this.role
            };
        },
        getHeaderTitle: function () {
            return this.getSpecificTemplateValues().headerTitle;
        },
        getTemplateID: function () {
            return "basic_page_simple";
        },
        goBackInHistory: function (clickEvent) {
            history.go(-1);
            return false;
        },
        getTemplateResult: function (templateDefinitionID, templateValues) {
            return window.JST[templateDefinitionID](templateValues);
        },
        getBasicPageTemplateResult: function () {
            var templateValues = {
                templatePartialPageID: this.id,
                headerTitle: this.getHeaderTitle()
            };
            var specific = this.getSpecificTemplateValues();
            $.extend(templateValues, this.getSpecificTemplateValues());
            return this.getTemplateResult(this.getTemplateID(), templateValues);
        },
        getRequestedPageTemplateResult: function () {
            this.getBasicPageTemplateResult();
        },
        render: function () {
            this.cleanupPossiblePageDuplicationInDOM();
            $(this.el).html(this.getBasicPageTemplateResult());
            this.addPageToDOMAndRenderJQM();
            this.enhanceJQMComponentsAPI();
        },
        enhanceJQMComponentsAPI: function () {
            $.mobile.changePage("#" + this.id, {
                changeHash: false,
                role: this.role
            });
        },
        addPageToDOMAndRenderJQM: function () {
            $("body").append($(this.el));
            $("#" + this.id).page();
        },
        // Instead you could move it to the event "pagehide": or "onPageHide"
        cleanupPossiblePageDuplicationInDOM: function () {
            var $previousEl = $("#" + this.id);
            var alreadyInDom = $previousEl.length >= 0;
            if (alreadyInDom) {
                $previousEl.detach();
            }
        }
    });

    return BasicView;
});
```

When “render” is triggered, first you have to remove the previous jQuery Mobile page from the DOM, if it already exists to prevent duplication.

JQuery Mobile supports the concept of Multipage Templates( see http://view.jquerymobile.com/1.3.0/docs/widgets/pages/), where you organize your entire markup into a single HTML page body.
To follow the DRY(Don’t repeat yourself) principle, it is positively to register the basic anatomies of jQM pages, popups and dialogs once in a separate HTML template file.
In the build progress of Grunt/Yeoman the semantic templates will be compiled by Handlebar.js and the AMDs template files will be combined to a single file.

In the next step, the “basic_page_simple” template, consisting of a common jQM page anatomy, will be loaded. ( see line _ of code).
As you realize, it also contains a manual implementation to handle the back buttons action as well as some console outputs for development.

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

The overridden “id”-attribute in the concrete view will be taken as the id of the jQM page as well as the templatePartialPageID of the file.
In the case of the “EditTodoPage”-View( see Image: “Todo Editing View” ) the name of the templatePartialPageID file is by convention:
“editTodoPage.template_partial”.
This can be changed in the grunt handlebars plugin configurations in the “grunt.js” file.

Some common used parameters, such as headerTitle, footer description and the “data-role”-attribute (e.g. “page”, “dialog”, or  popup) will be placed in the template as well.

Additional dynamic template parameters e.g. Backbone Model information, will be taken from the specific View and merged with the ones from the BasicView.

As a result, a concrete View might look like:

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

After the dynamic HTML is added to the DOM( step _), “$.mobile.changePage” has to be applied. It is the most important functionality, because it manual triggers the jQuery Mobile component creation and displays the current page.

This is done in the enhanceJQMComponentsAPI function

```javascript
$.mobile.changePage("#" + this.id, {
                changeHash: false,
                role: this.role
            });
```

To retain control of hash routing, “changeHash” has to be set to false as well as the proper “role”-parameter to guarantee right page appearance.

Make sure to always progress component enrichment of new added HTML into the DOM, to guarantee correct appearance of the mobile components.

![](../img/chapter10-2-2.png)

If you consider the following HTML declaration of the editing Todo page the following input

```javascript
<a id="saveDescription" href="#" data-role="button" data-mini="true">Save</a>
```
will be translated to the HTML below with additional CSS.
```javascript
<a data-mini="true" data-role="button" href="#" id="saveDescription"   data-corners="true" data-shadow="true" data-iconshadow="true" data-wrapperels="span" data-theme="c" class="ui-btn ui-shadow ui-btn-corner-all ui-mini ui-btn-up-c">
    <span class="ui-btn-inner">
         <span class="ui-btn-text">Save</span>
     </span>
</a>
```

For basic use cases, it is very simple and straight forward, to always render the complete page by calling $.mobile.changePage on component enhancements.

If you need to create partial HTML code and update values on UI elements, you have to be careful. How to handle such situations will be explained in the next section.

### Applying advanced jQM techniques to Backbone

#### Dynamic DOM Scripting


The described solution above solves the issue to handle routing with Backbone by calling $.mobile.changePage(“pageID”). Additional, it is guaranteed, that the written HTML page will be completely enhanced by the markup of jQuery Mobile.

The second tricky part with jQuery Mobile is to dynamically manipulate specific DOM content, e.g. after loading in content with Ajax. We recommend you to only use this technique, if there is evidence for an appreciable performance gain provided.
With its current version 1.3, jQM provides three ways, documented and explained in the official API, on forums and blogs.
( for further information see http://stackoverflow.com/questions/14468659/jquery-mobile-document-ready-vs-page-events/ and http://jquerymobile.com/test/docs/pages/page-scripting.html)

* <b>$(“pageId”).trigger(“pagecreate”)</b>
  <p>Creates markup of header, content as well as footer</p>
* <b>$(“anyElement”).trigger(“create”)</b>
  <p>Creates markup of the element as well as all children</p>
* <b>$(“myListElement”).listview(“refresh”)</b>
  <p>Every component of jQM offers plugin methods to
    update the state of specific ui element types.
    Sometimes when creating a component from scratch the error “cannot call methods on listview prior to initialization” will occur.
    This can be prevented with component initialization prior to markup enhancement, by calling:
    $('#mylist').listview().listview('refresh')
    </p>

To check more details and enhancements for further scripting pages of JQM it’s worth to check their API and follow the release notes, frequently.
( e.g. http://stackoverflow.com/questions/14550396/jquery-mobile-markup-enhancement-of-dynamically-added-content

#### Model-Binding with jQuery Mobile

When you consider using a Model-Binding Plugin, you will need to come up with an automated mechanism to enrich single components.

By having a look on the previous section about “Dynamic DOM Scripting”,

It might not be acceptable, to completely recreate a component .e.g a Listview, due to longer loadtime and to reduce complexity of event-delegation.
Instead, the component specific plugins, which will only update the needed parts of the HTML and CSS, should be used.
In the case of a Listview you would need to call $('#mylist').listview(), to update the list of added, edited, or removed entries.

You need to come up with a detection of the component type to decide which plugin method to be called. The jQuery Mobile Angular.js Adapter provides such a strategy and solution as well.( see
https://github.com/tigbro/jquery-mobile-angular-adapter/blob/master/src/main/webapp/integration/jqmWidgetPatches.js)

#### Intercept jQuery Mobile events

In special situations you would probably need to to take action to a triggered jQuery Mobile event, which can be done like this:

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
In such scenarios, it is important to known when the jQuery Mobile events occur, so have a look on the diagram.

![](../img/chapter10-3-1.png)

An alternative is the jQuery Mobile Router project, which you would need to replace with the Backbone Router. With the jQM-Router project, you get a powerful way to intercept and route one of the various jQM events. It’s independent of another framework, but very tight bound to jQM.
If you decide to use it, consider Backbone.js Custom Builds to exclude the router code (see http://gregfranko.com/backbone/customBuild/). It will save you ~ 25% out of the max compressed size of 17,1 KB.

#### Performance

Today, performance is a sensible topic on mobile devices.
jQuery Mobile provide tools, to create performance logs, which give you a good overview about the actual time spend to routing logic, component enhancement, and visual effects.
(check https://github.com/jquery/jquery-mobile/tree/master/tools and http://www.objectpartners.com/2012/11/02/use-jquery-mobile%E2%80%99s-tools-suite-to-help-you-debug-and-improve-your-jquery-mobile-application/)


Depending on the device, the time spend on transitions can take up to 90% load time. To disable transitions all over, you can eighter pass the transition ‘none’ to $.mobile.changePage(), or add settings in the configuration code block:

```javascript
$(document).bind("mobileinit", function(){
…
// Otherwise, depending on Takes up to 90% of loadtime
        	$.mobile.defaultPageTransition = "none";
$.mobile.defaultDialogTransition = "none";
    });
  })
```

To have a more finegrained configuration consider platform dependant settings ( see http://backbonefu.com/2012/01/jquery-mobile-and-backbone-js-the-ugly/ for further information:

```javascript
var iosDevice =((navigator.userAgent.match(/iPhone/i))
|| (navigator.userAgent.match(/iPod/i))) ? true : false;

  $.extend(  $.mobile , {
    slideText :  (iosDevice) ? "slide" : "none",
    slideUpText :  (iosDevice) ? "slideup" : "none",
    defaultPageTransition:(iosDevice) ? "slide" : "none",
    defaultDialogTransition:(iosDevice) ? "slide" : "none"
  });
```

#### Multi platform support management
The reality is that we don't always have the scope to create per-device experiences, so today's application will attempt to optimize for the devices or browsers most likely to access it. It's essential that the content the application exposes is readily available to all users, regardless of what browser they are accessing it from so this principle will be kept in mind during the design phase. We'll be aiming to create a solution that's responsive from the start and jQuery Mobile simplifies a lot of the actual heavy lifting here as it supports responsive layouts out of the box (including browsers that don't support media queries).#
Many developers forget that jQM is a UI framework not dissimilar to jQuery UI. It’s using the widget factory and is capable of being used for a lot more than we give it credit for.

If you have a look on the jQuery Mobile showcase(http://jquerymobile.com/demos/1.2.0/docs/forms/forms-all-mini.html), in a desktop browser, it doesn’t look anything like a mobile application. The accordeons, date-pickers, sliders - everything in the desktop UI is re-using what jQM would be providing users on mobile devices. Especially if you enable the attribute “data-mini” on components, you’ll not realize any difference.
Thanks to some media queries, the desktop UI can make optimal use of whitespace, expanding component blocks out and providing alternative layouts whilst still making use of jQM as a component framework.

The benefit of this is that you don’t need to go pulling in jQuery UI separately to be able to take advantage of these features. Thanks to the recent ThemeRoller the components can look pretty much exactly how I would like them to and users of the app can get a jQM UI for lower-resolutions and a jQM-ish UI for everything else.

The takeaway here is just to remember that if you’re not (already) going through the hassle of conditional script/style loading based on screen-resolution (using matchMedia.js etc), there are simpler approaches that can be taken to cross-device component theming.
