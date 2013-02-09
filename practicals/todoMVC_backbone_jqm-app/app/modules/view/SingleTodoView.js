define([
    "backbone",
    "lodash",
    "jquery",
    "modules/view/abstract/BasicView",
    "modules/TodoModel",
    "common",
    "jquerymobile"
],
    function (Backbone, _, $, BasicView, TodoModel, Common, jquerymobile) {
        var singleTodoView = Backbone.View.extend({
            tagName : 'li',
            template : window.JST['singleTodo'],
            events : {
                'change .toggle' : 'toggleCompleted',
                'click label' : 'edit',
                'click .destroy' : 'clear',
                'keypress .edit' : 'updateOnEnter',
                'blur .edit' : 'close'
            },
            initialize : function () {
                this.model.on('change', this.updateValue, this);
                this.model.on('change:completed', this.refreshCompeltedStateInUI, this);
                this.model.on('destroy', this.remove, this);
                this.model.on('visible', this.toggleVisible, this);
            },

            // Re-render the titles of the todo item.
            render : function () {
                this.$el.html(this.template(this.model.toJSON()));

                this.$el.toggleClass('completed', this.model.get('completed'));

                this.toggleVisible();
                this.input = this.$('.edit input');
                // this.trigger("create");
                return this;
            },
            refreshCompeltedStateInUI : function (model){
                var isCompleted = model.get("completed");
                $("select", this.$el)[0].selectedIndex = (isCompleted) ? 1 : 0;
                $("select", this.$el).slider('refresh');
                this.$el.toggleClass('completed', isCompleted);
            },
            updateValue : function (model) {
                $("label", this.el).html(model.get("title"));

            },
            toggleVisible : function () {
                this.$el.toggleClass('hidden', this.isHidden());
            },

            isHidden : function () {
                var isCompleted = this.model.get('completed');
                return ( // hidden cases only
                    (!isCompleted && Common.TodoFilter === 'completed')
                        || (isCompleted && Common.TodoFilter === 'active')
                    );
            },
            toggleCompleted : function () {
                this.model.toggle();
                this.$el.toggleClass('completed', 'completed');
            },
            edit : function () {
                window.location = "#editTodoTitle/"+this.model.cid;
                this.$el.addClass('editing');
                this.input.focus();
            },
            close : function () {
                var value = this.input.val().trim();

                if (value) {
                    this.model.save({ title : value });
                } else {
                    this.clear();
                }

                this.$el.removeClass('editing');
            },
            updateOnEnter : function (e) {
                if (e.keyCode === Common.ENTER_KEY) {
                    this.close();
                }
            },
            clear : function () {
                this.model.destroy();
            }
        });

        return singleTodoView;
    });
