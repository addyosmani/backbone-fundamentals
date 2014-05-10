# Developing Backbone.js Applications

## About

This is the home of [Developing Backbone.js Applications](http://shop.oreilly.com/product/0636920025344.do), an open-source book about the Backbone.js library for structuring JavaScript applications. It is released under a 
Creative Commons Attribution-Noncommercial-No Derivative Works 3.0 United States License and is currently available for early purchase via O'Reilly if you would like to support the project.

## Contributing

We appreciate any and all efforts to help improve the book. Please note that the repository is structured as follows:

* chapters - contains the source Markdown for each chapter. This is compiled into the HTML, ePub, Mobi and other formats found in the root of the project.
* build - contains templates and metadata for compiling the book
* img - contains image assets for the project
* practicals - contains the source code for practicals

If you would like to submit pull requests, please feel free to apply them against the relevant markdown file in `chapter`. These can be previewed by running the build script via `make`, which will also generate updated versions of the other formats we support. Please ensure that you are only submitting the modified chapter file for your changes and not the compiled HTML/other format binaries. 

## Reviews

If you are interested in leaving a review to let others know what you thoughts of the book this can be done through [O'Reilly](http://shop.oreilly.com/product/0636920025344/ReviewSubmit.do?sortby=publicationDate?pageId=0636920025344.IP).

## Building

You can build the book using `make` or `make -f Makefile`. This will output the HTML, ePub, Mobi, PDF and RTF versions of the book. 

Note: At this time, generating all of the above formats should be straight-forward with the exception of the PDF. We have an open [pull-request](https://github.com/addyosmani/backbone-fundamentals/pull/369) for `xelatex` which is being considered and better guides for working around cross-platform PDF issues will be supplied as time allows.

### Dependencies

* Make
* [Pandoc](https://github.com/jgm/pandoc)
* pdflatex (and recommended latex fonts)

#### Linux Debian based distros:

<code>apt-get install pandoc texlive-latex-base texlive-fonts-recommended cm-super etoolbox</code>
