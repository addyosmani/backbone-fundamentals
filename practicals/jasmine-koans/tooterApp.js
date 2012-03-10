/*global $, _, Backbone */

(function() {
    var Toot = Backbone.Model.extend({
        defaults: function() {
            return {
                user: 'Mario',
                message: 'Squishing Goombas, collecting coins.',
                created_on: new Date()
            };
        },
        
        initialize: function() {
            // Capitalize the user's name associated with this Toot.
            var userName = this.get('user');
            userName = userName.slice(0,1).toUpperCase() + userName.slice(1).toLowerCase();
            
            this.set({ user: userName }, { silent: true });
        },
        
        validate: function(attrs) {
            if (_.isString(attrs.user) && attrs.user.length === 0) {
                return 'Must have a user.';
            }
            
            if (_.isString(attrs.message) && attrs.message.length === 0) {
                return 'Must contain a message.';
            }
        }
    });
    
    var Toots = Backbone.Collection.extend({
        model: Toot,
        
        url: '/toots/'
    });
    
    var TootView = Backbone.View.extend({
        tagName: 'li',
        
        initialize: function(options) {
            this.options = options || {};
            _.defaults(this.options, {
                parentElt: 'body'
            });
        },
        
        render: function() {
            var model = this.model;
            var content = _.template($('#tootViewTmpl').html())({
                iconUrl: '',
                message: this.model.get('message')
            });
            
            $(this.el).html(content);
            $(this.options.parentElt).append(this.el);
        }
    });
    
    var TootApp = Backbone.View.extend({
        
        events: {
            'keypress .tootInput': 'createToot'
        },
        
        toots: new Toots(),
        
        initialize: function(options) {
            this.options = options || {};
            _.defaults(this.options, {
                parentElt: 'body'
            });
            
            this.markup = _.template($('#tooterAppTmpl').html());
            $(this.options.parentElt).append(this.el);
            $(this.el).append(this.markup());
            
            this.toots.bind('add', this.addToot, this);
            this.toots.bind('reset', function(toots) {
                toots.each(function(toot) {
                    this.addToot(toot);
                }, this);
            }, this);
            
            this.toots.fetch();
        },
        
        addToot: function(toot) {
            var view = new TootView({
                parentElt: this.$('.tootList'),
                model: toot
            }).render();
        },
        
        createToot: function(evt) {
            var text = this.$('.tootInput').val();
            
            if (!text || evt.keyCode !== 13) {
                return;
            }
            
            this.toots.create({
                user: 'Me',
                message: text
            });
        }
    });
    
    window.Tooter = {
        Toot: Toot,
        Toots: Toots,
        TootView: TootView,
        TootApp: TootApp
    };
})();