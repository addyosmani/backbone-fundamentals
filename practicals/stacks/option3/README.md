# Weapon of Choice

This app implements Backbone.js's TODO example app with MongoDB and either Sinatra or Node.js as a backend.

1. HTML5 Boilerplate - http://html5boilerplate.com/
2. Backbone.js - http://documentcloud.github.com/backbone/
3. Sinatra - http://www.sinatrarb.com/
4. HAML - http://haml-lang.com/
4. Node.js - http://nodejs.org/
5. Express - http://expressjs.com/
6. Mongoose - https://github.com/LearnBoost/mongoose
7. Jade - http://jade-lang.com/
7. MongoDB - http://www.mongodb.org/

Obviously, you don't need both the Node.js and Sinatra backends. You can pick one and delete the other.

## To Run

Make sure that you have installed (and are running) MongoDB.

By default, the app connects to MongoDB running on localhost with its default port.

### Ruby

Make sure that you have bundler and Ruby installed.

    git clone git://github.com/criccomini/weapon-of-choice.git
    cd weapon-of-choice
    bundle install
    ruby app.rb

Visit http://localhost:4567

### Node.js / Express

Make sure that you have Node.js installed.

    git clone git://github.com/criccomini/weapon-of-choice.git
    cd weapon-of-choice
    ./install.sh
    node app.js

Visit http://localhost:3000

## Customizing

### HTML

HAML/Jade files should be created in the views folder.

All of the current layout.haml/layout.jade is straight out of HTML5 Boilerplate.

### CSS

All of the CSS is from HTML5 Boilerplate, except for the TODO css, which is marked with:

    /*
     * Delete below. This is the Todo app's css.
     */
    
    ...
    
    /*
     * Delete above. This is the Todo app's css.
     */

### JS

I've dumped the Backbone.js and Underscore.js libs inside the public/js/mylibs folder. This is, again, just HTML5 Boilerplate layout.

## Notes

There is still a little CSS weirdness. The (x) button in the "real" Backbone.js TODO demo doesn't appear. I'm pretty sure this has to do with a fight between HTML5 Boilerplate, and the copy/pased CSS for the Backbone.js TODO app.

It would be pretty cool if the Backbone.js and Mongoose models were actually the same class. I need to figure out how to do this.