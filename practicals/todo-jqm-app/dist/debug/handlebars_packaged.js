this["JST"] = this["JST"] || {};

Handlebars.registerPartial("editTodoView", Handlebars.template(function (Handlebars,depth0,helpers,partials,data) {
  this.compilerInfo = [2,'>= 1.0.0-rc.3'];
helpers = helpers || Handlebars.helpers; data = data || {};
  var buffer = "", stack1, functionType="function", escapeExpression=this.escapeExpression;


  buffer += "<div data-role=\"fieldcontain\">\r\n    <label for=\"todoDescription\">Todo Description</label>\r\n    <input type=\"text\" name=\"todoDescription\" id=\"todoDescription\" value=\"";
  if (stack1 = helpers.title) { stack1 = stack1.call(depth0, {hash:{},data:data}); }
  else { stack1 = depth0.title; stack1 = typeof stack1 === functionType ? stack1.apply(depth0) : stack1; }
  buffer += escapeExpression(stack1)
    + "\" />\r\n</div>\r\n    <a id=\"saveDescription\" href=\"#\" data-role=\"button\" data-mini=\"true\">Save</a>\r\n";
  return buffer;
  }));

Handlebars.registerPartial("todosPage", Handlebars.template(function (Handlebars,depth0,helpers,partials,data) {
  this.compilerInfo = [2,'>= 1.0.0-rc.3'];
helpers = helpers || Handlebars.helpers; partials = partials || Handlebars.partials; data = data || {};
  var buffer = "", stack1, self=this;


  buffer += "<section id=\"todoapp\">\r\n\r\n    <header id=\"header\">\r\n        <h1>todos</h1>\r\n    </header>\r\n    <div class=\"ui-grid-a\" >\r\n        <div class=\"ui-block-a toggleAllCheckbox\" >\r\n            <label><input type=\"checkbox\" data-mini=\"true\" id=\"toggle-all\"  />All</label>\r\n        </div>\r\n        <div class=\"ui-block-b newTodoText\" >\r\n            <input id=\"new-todo\" type=\"text\" placeholder=\"What needs to be done?\" autofocus=\"true\" />\r\n        </div>\r\n    </div>\r\n\r\n    <section id=\"main\">\r\n\r\n\r\n        <ul id=\"todo-list\">\r\n            <!-- singleTodo.template is included here by view-->\r\n        </ul>\r\n    </section>\r\n\r\n    <footer id=\"footer\">";
  stack1 = self.invokePartial(partials.todosStatisticsAndBulkFooter, 'todosStatisticsAndBulkFooter', depth0, helpers, partials, data);
  if(stack1 || stack1 === 0) { buffer += stack1; }
  buffer += "</footer>\r\n</section>\r\n<footer id=\"info\">\r\n<p>Click to edit a todo</p>\r\n\r\n<p>Written by <a href=\"http://addyosmani.github.com/todomvc/\">David Amend</a></p>\r\n\r\n<p>Part of <a href=\"http://todomvc.com\">TodoMVC</a></p>\r\n</footer>\r\n";
  return buffer;
  }));

Handlebars.registerPartial("todosStatisticsAndBulkFooter", Handlebars.template(function (Handlebars,depth0,helpers,partials,data) {
  this.compilerInfo = [2,'>= 1.0.0-rc.3'];
helpers = helpers || Handlebars.helpers; data = data || {};
  var buffer = "", stack1, functionType="function", escapeExpression=this.escapeExpression, self=this;

function program1(depth0,data) {
  
  
  return "items";
  }

function program3(depth0,data) {
  
  
  return "item";
  }

function program5(depth0,data) {
  
  var buffer = "", stack1;
  buffer += "\r\n    <li>\r\n        <a id=\"clear-completed\" href=\"#\" data-role=\"button\" data-icon=\"delete\" data-mini=\"true\" data-inline=\"true\">Clear completed (";
  if (stack1 = helpers.completed) { stack1 = stack1.call(depth0, {hash:{},data:data}); }
  else { stack1 = depth0.completed; stack1 = typeof stack1 === functionType ? stack1.apply(depth0) : stack1; }
  buffer += escapeExpression(stack1)
    + ")</a>\r\n    </li>\r\n    ";
  return buffer;
  }

  buffer += "<span id=\"todo-count\"><strong>";
  if (stack1 = helpers.remaining) { stack1 = stack1.call(depth0, {hash:{},data:data}); }
  else { stack1 = depth0.remaining; stack1 = typeof stack1 === functionType ? stack1.apply(depth0) : stack1; }
  buffer += escapeExpression(stack1)
    + "</strong> ";
  stack1 = helpers['if'].call(depth0, depth0.isAnyOrOneRemaining, {hash:{},inverse:self.program(3, program3, data),fn:self.program(1, program1, data),data:data});
  if(stack1 || stack1 === 0) { buffer += stack1; }
  buffer += "</span>\r\n<ul id=\"filters\" data-role=\"controlgroup\" data-type=\"horizontal\">\r\n    <li>\r\n        <a class=\"selected\" href=\"todo/all\" data-role=\"button\">All</a>\r\n    </li>\r\n    <li>\r\n        <a href=\"todo/active\" data-role=\"button\">Active</a>\r\n    </li>\r\n    <li>\r\n        <a href=\"todo/completed\" data-role=\"button\">Completed</a>\r\n    </li>\r\n    ";
  stack1 = helpers['if'].call(depth0, depth0.isAnyCompleted, {hash:{},inverse:self.noop,fn:self.program(5, program5, data),data:data});
  if(stack1 || stack1 === 0) { buffer += stack1; }
  buffer += "\r\n</ul>\r\n";
  return buffer;
  }));

Handlebars.registerPartial("welcome", Handlebars.template(function (Handlebars,depth0,helpers,partials,data) {
  this.compilerInfo = [2,'>= 1.0.0-rc.3'];
helpers = helpers || Handlebars.helpers; data = data || {};
  


  return "<h2>Welcome</h2>\r\n";
  }));

this["JST"]["basic_page_simple"] = Handlebars.template(function (Handlebars,depth0,helpers,partials,data) {
  this.compilerInfo = [2,'>= 1.0.0-rc.3'];
helpers = helpers || Handlebars.helpers; partials = partials || Handlebars.partials; data = data || {};
  var buffer = "", stack1, stack2, options, helperMissing=helpers.helperMissing, escapeExpression=this.escapeExpression, functionType="function", self=this;


  buffer += "    <div data-role=\"header\">\r\n        ";
  options = {hash:{},data:data};
  buffer += escapeExpression(((stack1 = helpers.whatis),stack1 ? stack1.call(depth0, "Specific loaded Handlebars parameters:", options) : helperMissing.call(depth0, "whatis", "Specific loaded Handlebars parameters:", options)))
    + "\r\n        ";
  options = {hash:{},data:data};
  buffer += escapeExpression(((stack1 = helpers.whatis),stack1 ? stack1.call(depth0, depth0, options) : helperMissing.call(depth0, "whatis", depth0, options)))
    + "\r\n        <h2>";
  if (stack2 = helpers.headerTitle) { stack2 = stack2.call(depth0, {hash:{},data:data}); }
  else { stack2 = depth0.headerTitle; stack2 = typeof stack2 === functionType ? stack2.apply(depth0) : stack2; }
  buffer += escapeExpression(stack2)
    + "</h2>\r\n        <a id=\"backButton\" href=\"href=\"javascript:history.go(-1);\" data-icon=\"star\" data-rel=\"back\" >back</a>\r\n    </div>\r\n    <div data-role=\"content\">\r\n        ";
  options = {hash:{},data:data};
  buffer += escapeExpression(((stack1 = helpers.whatis),stack1 ? stack1.call(depth0, "Template page trying to load:", options) : helperMissing.call(depth0, "whatis", "Template page trying to load:", options)))
    + "\r\n        ";
  options = {hash:{},data:data};
  buffer += escapeExpression(((stack1 = helpers.whatis),stack1 ? stack1.call(depth0, depth0.templatePartialPageID, options) : helperMissing.call(depth0, "whatis", depth0.templatePartialPageID, options)))
    + "\r\n        ";
  stack2 = self.invokePartial(partials.templatePartialPageID, 'templatePartialPageID', depth0, helpers, partials, data);
  if(stack2 || stack2 === 0) { buffer += stack2; }
  buffer += "\r\n    </div>\r\n    <div data-role=\"footer\">\r\n        Footer\r\n    </div>\r\n";
  return buffer;
  });

this["JST"]["basic_popup"] = Handlebars.template(function (Handlebars,depth0,helpers,partials,data) {
  this.compilerInfo = [2,'>= 1.0.0-rc.3'];
helpers = helpers || Handlebars.helpers; data = data || {};
  


  return "<h3>Popup</h3>\r\n";
  });

this["JST"]["singleTodo"] = Handlebars.template(function (Handlebars,depth0,helpers,partials,data) {
  this.compilerInfo = [2,'>= 1.0.0-rc.3'];
helpers = helpers || Handlebars.helpers; data = data || {};
  var buffer = "", stack1, self=this, functionType="function", escapeExpression=this.escapeExpression;

function program1(depth0,data) {
  
  
  return "selected=\"selected\"";
  }

  buffer += "<div class=\"ui-grid-a\" >\r\n    <div class=\"ui-block-a singleTodoCompletedToggle\">\r\n        <select name=\"completed\" class=\"toggle\" data-role=\"slider\" style=\"\">\r\n            <option value=\"Todo\" ";
  stack1 = helpers['if'].call(depth0, depth0.completed, {hash:{},inverse:self.noop,fn:self.program(1, program1, data),data:data});
  if(stack1 || stack1 === 0) { buffer += stack1; }
  buffer += ">Todo</option>\r\n            <option value=\"Done\" ";
  stack1 = helpers['if'].call(depth0, depth0.completed, {hash:{},inverse:self.noop,fn:self.program(1, program1, data),data:data});
  if(stack1 || stack1 === 0) { buffer += stack1; }
  buffer += ">Done</option>\r\n        </select>\r\n    </div>\r\n    <div class=\"ui-block-b singleTodoDescription\" >\r\n        <label name=\"todo_description\">\r\n            <a href=\"#editTodo/";
  if (stack1 = helpers.id) { stack1 = stack1.call(depth0, {hash:{},data:data}); }
  else { stack1 = depth0.id; stack1 = typeof stack1 === functionType ? stack1.apply(depth0) : stack1; }
  buffer += escapeExpression(stack1)
    + "\" data-rel=\"popup\">";
  if (stack1 = helpers.title) { stack1 = stack1.call(depth0, {hash:{},data:data}); }
  else { stack1 = depth0.title; stack1 = typeof stack1 === functionType ? stack1.apply(depth0) : stack1; }
  buffer += escapeExpression(stack1)
    + "</a>\r\n        </label>\r\n    </div>\r\n</div>";
  return buffer;
  });