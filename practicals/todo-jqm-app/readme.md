Backbone and jquerymobile JQM Boilerplate code
===============================================

This project provides a bulletproof template + build process to develop
an application by using jQuery mobile and backbone.js
The aim is to provide it in Addy Osmanis [Backbone Fundamentals](https://github.com/addyosmani/backbone-fundamentals)

It consists of:
- [Backbone.js](http://backbonejs.org/) 0.9.2
- extended [super](https://gist.github.com/2423599) call (supports multiple hierarchies of inerheritance)
- [Lodash](http://lodash.com/) ( performance improved version of underscore + AMD support) 0.5
- [jQuery Toolkit](http://jquery.com/) 1.8.0
- [jQuery Mobile](http://jquerymobile.com/) 1.1.1
- [Require.js](http://requirejs.org/) (as an AMD loader) 2.0.5
- [Handlebars.js](http://handlebarsjs.com/) ( instead, underscore js or any other template engine can be used) 1.0.beta.6
- [Grunt.js](https://github.com/cowboy/grunt) as build tool, like ant or maven, to precompile handlebars.js templates, and r.js optimizer tasks
- [Grunt bbb](https://github.com/backbone-boilerplate/grunt-bbb) a collection of tasks, containing [grunt-contrib](https://github.com/gruntjs/grunt-contrib) task collection

- code structure and super-classes for clean code and easy reuse

### Table of contents
* [General](#general)
* [Basic classes](#basicclasses) : [Basic View](#basicclasses_basicview) [Basic Dialog] (#basicclasses_basicdialog) [Validateable](#basisclasses_validateable)
* [Handlebars](#handlebars)
* [Grunt](#grunt)
* [Grunt-contrib](#grunt_contrib)
* [Settings](#settings)
* [Todo](#todo)
* [References](#references)


#### Quick start

Preparations:
* Checkout from git with your favourite tool or cmd. For those who are new, I would recommand usage of Eclipse Git, Webstorm or Tortoise Git
* Open your command line tool
* cd to the project folder "backbone.js-jquerymobile-boilerplate-template"
* Download and install [node.js](http://nodejs.org/) for your os system and run in your project folder:
* npm install -g grunt            ( installs grunt command line tool)
* npm install -g bbb              ( is a grunt addon, which offers backbone templates and task collections like grunt-contrib)
* npm install grunt-contrib       ( needed to install not global, because you'll need the newest version)

On development:
* bbb handlebars                  ( precompiles templates to javascript functions)
* bbb server                      ( runs server on your local machine in development mode)
* go to http://localhost:8000
* Before you start developing, enable the handlebars task, if any change is done on a file by running once:
  bbb watch or run bbb handlebars every time manually


###<a name="general">General</a>


###<a name="basicclasses">Abstract classes for simple usage</a>

#### <a name="basicclasses_basicview">[`BasicView`](app/modules/BasicView.js)</a>

##### Use case:
For any jQuery mobile page view.
(To e.g. support transparent dialogs, the page will be removed from the DOM when the same page is requested again)

##### How to use:

* getSpecificTemplateValues() is an abstract method. The json values are used for the handlebars.js context variables.
* id is the pageID, which will automatically load the name of the template
* if you want to use your own page template ( instead of "baisc_page_simple"),
  override getTemplateID in your class


##### Examples:

An example usage for a simple JQM page:
```javascript
ConcreteExampleView = BasicView.extend({
    id: "content",
      getSpecificTemplateValues : function(){
  		return "something"
  	}
  });
```

#### <a name="basicclasses_basicdialog">[`BasicDialog`](app/modules/BasicDialog.js)</a>

##### Use case:
It will render the page as dialog with or without validation.
The previous page will be displayed transparent.

##### How to use

- Define transparentBackgroundPageElID, which is the previous pageID
- Define this.model.settings.validation.rules as validation rules. (see [jQuery.Validation](http://bassistance.de/jquery-plugins/jquery-plugin-validation/) for more information)
- Override  onSuccessfulValidation : function(){}, which will be called,
per default, if the form was submitted and validation is successful.

```css
.transparent {
    background-color: orange;
    zoom: 1;
    filter: alpha(opacity = 50);
    opacity: 0.5;
}

.ui-dialog-background {
    opacity: 0.5;
    display: block !important;
    -webkit-transition: opacity 0.5s ease-in;
}
```

##### Examples
```javascript
define([
    "backbone", "modules/view/abstract/BasicDialog"],
    function (Backbone, BasicDialog) {
        return BasicDialog.extend({
            id : "editTodoDialog",
            model : new Backbone.Model.extend({
               settings : {
                  validation : {
                    rules : {
                      title : {
                        "required" : true,
                        "min" : 5
                    }
                  }
                }
              },
            }),
            headerTitle : "Edit Todo",
            transparentBackgroundPageElID : "Todos",
            getSpecificTemplateValues : function () {
                return this.model.toJSON();
            },
            onSuccessfulValidation : function () {
                this.model.save({title : $("#edit_title", this.el).val()});
                window.location = ""; // route to another page(close dialog)
            }
        });
    });
```


#### <a name="basicclasses_validateable">[`Validateable`](app/modules/Validateable.js)</a>
Extending Validateable will use the jquery.validate plugin with jquery mobile like described here:
http://www.elijahmanor.com/2011/02/jquery-mobile-form-validation.html

##### Use case
Pages, where validation has to be applied.
The validate rules are part of the model under this property
this.model.settings.validation.rules

##### How to use
- Define this.model.settings.validation.rules as validation rules. (see [jQuery.Validation](http://bassistance.de/jquery-plugins/jquery-plugin-validation/) for more information)
- Override  onSuccessfulValidation : function(){}, which will be called,
per default, if the form was submitted and validation is successful.

```css
label.error {
    color: red;
    font-size: 16px;
    font-weight: normal;
    line-height: 1.4;
    margin-top: 0.5em;
    width: 100%;
    float: none;
}
```

##### Examples
```javascript
define([
    "backbone", "modules/view/abstract/Validateable"],
    function (Backbone, Validateable) {
        return Validateable.extend({
            id : "editTodoDialog",
            model : new Backbone.Model.extend({
               settings : {
                  validation : {
                    rules : {
                      title : {
                        "required" : true,
                        "min" : 5
                    }
                  }
                }
              },
            }),
            headerTitle : "Edit Todo",
            getSpecificTemplateValues : function () {
                return this.model.toJSON();
            },
            onSuccessfulValidation : function () {
                this.model.save({title : $("#edit_title", this.el).val()});
            }
        });
    });
```

#### <a name="handlebars">Registering handlebars.js templates</a>
run to compile all templates found in app/templates to dist/debug/
Partials will be registered to Handlebars.partials,
Templates to window.JST['templateName'] by default.

bbb handlebars
or
bbb watch

#### dynamic scripting and jQuery Mobile
see [jQuery Mobile documentation](http://jquerymobile.com/test/docs/pages/page-scripting.html)
There are two ways to render HTML code (loaded by template) to jQuery Mobile HTML code:
- $.mobile.changePage({])
- $("domNodeAccessor").create("trigger")

### <a name="grunt">Usage of grunt</a>

GRUNT:
* concat - Concatenate files.
* init - Generate project scaffolding from a predefined template.
* lint - Validate files with JSHint.
* min - Minify files with UglifyJS.
* qunit - Run QUnit unit tests in a headless PhantomJS instance.
* server - Start a static web server.
* test - Run unit tests with nodeunit.
* watch - Run predefined tasks whenever watched files change.

### <a name="grunt_contrib">Additional Grunt-Contrib tasks</a>

[`clean`](/gruntjs/grunt-contrib/blob/master/docs/clean.md) - Clear files and folders.

[`coffee`](/gruntjs/grunt-contrib/blob/master/docs/coffee.md) - Compile CoffeeScript files into JavaScript.

[`compress`](/gruntjs/grunt-contrib/blob/master/docs/compress.md) - Compress files and folders using gzip or zip.

[`handlebars`](/gruntjs/grunt-contrib/blob/master/docs/handlebars.md) - Compile handlebars templates to JST file.

[`jade`](/gruntjs/grunt-contrib/blob/master/docs/jade.md) - Compile Jade templates to HTML.

[`jst`](/gruntjs/grunt-contrib/blob/master/docs/jst.md) - Compile underscore templates to JST file.

[`less`](/gruntjs/grunt-contrib/blob/master/docs/less.md) - Compile LESS files to CSS.

[`mincss`](/gruntjs/grunt-contrib/blob/master/docs/mincss.md) - Minify CSS files.

[`requirejs`](/gruntjs/grunt-contrib/blob/master/docs/requirejs.md) - Optimize RequireJS projects using r.js.

[`stylus`](/gruntjs/grunt-contrib/blob/master/docs/stylus.md) - Compile Stylus files into CSS.



### <a name="settings">jQuery Mobile Settings</a>

To support right behavior in e.g. navigation and use default backbone.js routing, use the following properties:

```javascript
   $(document).bind("mobileinit", function(){
        $.mobile.ajaxEnabled = false;
        $.mobile.hashListeningEnabled = false;
        $.mobile.pushStateEnabled = false;
        $.mobile.linkBindingEnabled = false;
        $.mobile.defaultPageTransition = "none";
        $.mobile.page.prototype.options.degradeInputs.date = true; // optional
        $.mobile.page.prototype.options.domCache = false; // optional
        $.mobile.defaultDialogTransition = "none"; // optional depends on performance
      });
  })
```

If you want to enable transitions per device ( where you expect good performance) you can use logic per device like described here http://backbonefu.com/2012/01/jquery-mobile-and-backbone-js-the-ugly/:

```javascript
var iosDevice = ((navigator.userAgent.match(/iPhone/i)) || (navigator.userAgent.match(/iPod/i))) ? true : false;
 
  $.extend(  $.mobile , {
    slideText :  (iosDevice) ? "slide" : "none",
    slideUpText :  (iosDevice) ? "slideup" : "none",
    defaultPageTransition:(iosDevice) ? "slide" : "none",
    defaultDialogTransition:(iosDevice) ? "slide" : "none"
  });
```


Backbone.history.start({ pushState: false });
is used to work properly with forward/back buttons
pushState: true will only work with modern browsers( fails for some mobile browsers as well)


### <a name="todo">TODO</a>

* Make an Addy Osmani TodoMVC application out of it. -> ongoing
* Implement good back button functionality e.g. for dialog
* add subview support with jQuery Mobile and backbone -> partly
* Extend documentation
* Write a chapter in Addy Osmani Backbone fundamentals about this project
* Add tests ( jasmine + sinon.js + + phantom.js ? + continous integration  with jstestdriver and jenkins)
* Make a mobile + desktop app out of it, using common coed


##### <a name="references">References and similar projects</a>

- https://github.com/Filirom1/jquery-mobile-backbone-requirejs

uses the jQuery Mobile Router plugin ( extends/manipulates  the existing backbone.js routing)

- https://github.com/ccoenraets/backbone-jquerymobile uses no AMD


The knownledge about how to run JQM with backbone is partial derived by results of several resources:

http://stackoverflow.com/questions/10904433/jquery-mobile-require-js-and-backbone

http://addyosmani.github.com/backbone-fundamentals/

http://coenraets.org/blog/2012/03/using-backbone-js-with-jquery-mobile/

https://github.com/azicchetti/jquerymobile-router

https://github.com/buildmobile/backbone.js/tree/master/js

Thanks for them !


