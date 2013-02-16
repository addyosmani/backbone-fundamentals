var app = app || {};

app.LibraryView = Backbone.View.extend({
	el: $( '#books' ),

	initialize: function() {
		this.collection = new app.Library();
		this.collection.fetch();
		this.render();

		this.collection.on( 'add', this.renderBook, this );
		this.collection.on( 'reset', this.render, this );
	},

	events: {
		'click #add': 'addBook'
	},

	addBook: function( e ) {
		e.preventDefault();

		var formData = {};

		$( '#addBook div' ).children( 'input' ).each( function( i, el ) {
			if( $( el ).val() != "" )
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
		});

		this.collection.create( formData );
	},

	render: function() {
		_.each( this.collection.models, function( item ) {
			this.renderBook( item );
		}, this );
	},

	renderBook: function( item ) {
		var bookView = new app.BookView({
			model: item
		});
		this.$el.append( bookView.render().el );
	}
});
