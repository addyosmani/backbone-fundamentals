# Developing Backbone.js Applications

## About

This is the home of [Developing Backbone.js Applications](http://shop.oreilly.com/product/0636920025344.do), an (in-progress) book about the Backbone.js library for structuring JavaScript applications. It is released under a Creative Commons Attribution-Noncommercial- No Derivative Works 3.0 United States License and is currently available for early purchase via O'Reilly if you would like to support the project.


## Contributing

We appreciate any and all efforts to help improve the book. If you catch a typo or code flaw, please edit the chapter's Markdown file responsible for the error in the `/chapters` directory. You can preview the result of your change by running the build script via `make`, which generates updated versions of the formats we support.

With all of the great contributions from different authors, we try to keep our code style consistent for the benefit of the reader. If your contribution entails an example of JavaScript code, please adhere to our [Code Style Guide](codestyle.md).

If you have an idea for a large-scale improvement, please [create an issue](https://github.com/addyosmani/backbone-fundamentals/issues) where we can discuss your ideas and find the best way to integrate them with the book.


## Reviews

If you are interested in leaving a review to let others know what you thoughts of the book this can be done through [O'Reilly](http://shop.oreilly.com/product/0636920025344/ReviewSubmit.do?sortby=publicationDate?pageId=0636920025344.IP).


## Building

You can build the book using `make` or `make -f Makefile`. This will output the HTML, ePub, PDF and RTF versions of the book.

### Dependencies

* Make
* [Pandoc](https://github.com/jgm/pandoc)
* pdflatex (and recommended latex fonts)

#### Linux Debian based distros

```
apt-get install pandoc
apt-get install texlive-latex-base
apt-get install texlive-fonts-recommended
apt-get install cm-super
```

If you wish to avoid the hassle of installing the dependencies required, please mention so in your Pull Request so we can run the build.
