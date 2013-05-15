# Code Style
To provide a consistent reading experience, please follow these conventions while writing code.

## General Rules
- Place your code in JavaScript Markdown blocks (see Example below)
- 2 Space Indentation
- Single-quotes
- Semicolon
- Multiple variable statements
- Space after keywords and between arguments and operators
- `// Comment.` not `/* Comment. */`
- Return early
- Consistency

Example:

    ```js
    function foo(bar, fum) {
      var i, l, ret;
      var hello = 'Hello';

      if (!bar) {
          return;
      }

      for (i = 0, l = bar.length; i < l; i++) {
        if (bar[i] === hello) {
          ret += fum(bar[i]);
        }
      }

      return ret;
    }
    ```

Read [idiomatic.js](https://github.com/rwldrn/idiomatic.js) for general JavaScript code style best practices.

## Anonymous Functions
When using anonymous functions, leave a space between the function name and opening parenthesis.

Example:

```js
var Thank = function (who) {
  return 'Thanks, ' + who;
};
```

## Comments
Inline comments are a great way of giving new users a better understanding of what you're doing and why.

It's also helpful to let your functions breathe, by leaving additional lines between statements.

Example:

```js
// Ok.
var removeTodo = function (todoItem) {
  var todoModel = todoItem.getModel(); // Grab the model from the todoItem.
  todoItem.find('.destroy').click(); // Trigger a click to remove the element from the <ul>.
  todoModel.remove(); // Removes the todo model from localStorage.
};

// Better.
var removeTodo = function (todoItem) {
  // Grab the model from the todoItem.
  var todoModel = todoItem.getModel();

  // Trigger a click to remove the element from the <ul>.
  todoItem.find('.destroy').click();

  // Removes the todo model from localStorage.
  todoModel.remove();
};
```

## RequireJS
If using RequireJS in your code block, please format your code to these specifications:

```js
define('Block', [
  'jQuery',
  'Handlebars'
], function ($, Handlebars) {
  // Code here.
});
```
