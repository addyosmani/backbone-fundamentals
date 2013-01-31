var app = app || {};

app.Library = Backbone.Collection.extend({
	model: app.Book,
	url: '/api/books'
});
