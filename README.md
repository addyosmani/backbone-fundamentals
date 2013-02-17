# Developing Backbone.js Applications

## About

This is the home of [Developing Backbone.js Applications](http://shop.oreilly.com/product/0636920025344.do), an (in-progress) book about the 
Backbone.js library for structuring JavaScript applications. It is released under a 
Creative Commons Attribution-Noncommercial- No Derivative Works 3.0 United States License and is currently available for early purchase via O'Reilly if you would like to support the project.

We appreciate any and all efforts to help improve the book. If you would like to submit pull requests, please feel free to apply them against `index.md`. These can be previewed by running the build script via `make`, which will also generate updated versions of the other formats we support.

## Reviews

If you are interested in leaving a review to let others know what you thoughts of the book this can be done through [O'Reilly](http://shop.oreilly.com/product/0636920025344/ReviewSubmit.do?sortby=publicationDate?pageId=0636920025344.IP).

## Building

You can build the book using `make` or `make -f Makefile`. This will output the HTML, ePub, PDF and RTF versions of the book. 

### Dependencies

* Make
* [Pandoc](https://github.com/jgm/pandoc)
* pdflatex (and recommended latex fonts)

#### Linux Debian based distros:

* <code>apt-get install pandoc</code>
* <code>apt-get install texlive-latex-base</code>
* <code>apt-get install texlive-fonts-recommended</code>
* <code>apt-get install cm-super</code>
