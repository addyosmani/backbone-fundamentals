# Conclusions

I hope that you've found this walkthrough of Backbone.js of value. 

In the words of Jeremy Ashkenas, "The essential premise at the heart of Backbone has always been to try and discover the minimal set of data-structuring (Models and Collections) and user interface (Views and URLs) primitives that are useful when building web applications with JavaScript. "

Building a single-page application using nothing more than a DOM manipulation library (such as jQuery) is certainly possible, however it is difficult to build anything non-trivial without any formal structure in place. Your nested pile of jQuery callbacks and DOM elements are unlikely to scale and they can be very difficult to maintain as your application grows.

The beauty of Backbone.js is it's simplicity. It's very small given the functionality and flexibility it provides, which is evident if you begin to study the Backbone.js source. It's core only contains five components: Events, Model, Collection, View and Router. Backbone just helps you improve the structure of your applications, helping you better separate concerns. There isn't anything more to it than that.

To summarize what we've learned, Backbone offers models with key-value bindings and events, collectiions with an API of rich enumerable methods, declarative views with event handling and a simple way to connect an existing API to your client-side application over a RESTful JSON interface. Use it and you can abstract away data into sane models and your DOM manipulation into views, binding together using nothing more than events.

Almost any developer working on JavaScript applications for a while will ultimately come to creating a similar solution to it on their own if they value architecture and maintainability. That said, rather than re-inventing the wheel there are many advantages to structuring your application using a solution based on the collective knowledge and experience of an entire community. 

In addition to helping provide sane structure to your applications, Backbone is highly extensible supporting more custom architecture should you require more than what is prescribed out of the box. This is evident by the number of extensions and plugins which have been released for it over the past year, including those which we have touched upon such as Backbone.Marionette and Thorax. 

These days Backbone.js powers many complex web applications, ranging from the LinkedIn mobile app to popular RSS readers such as NewsBlur and beyond, to social commentary widgets such as Disqus. This small library of simple, but sane abstractions has helped to create a new generation of rich web applications, and I and my collaborators hope that in time it can help you too. 

Backbone is neither difficult to learn nor use, however the time and effort you spend learning how to structure applications using it will be well worth it. Whilst reading this book will equip you with the fundamentals needed to understand the library, the best way to learn is to try building your own real-world applications. You will hopefully find that the end product is cleaner, better organized and more maintainable code.