
include_dir=build
pandoc=`which pandoc`
source=index.md
title="Developing Backbone.js Applications"
filename="backbone-fundamentals"
pdflatex=`which pdflatex`

all: pdf html epub rtf

pdf: $(sorce)
	$(pandoc) -s $(source) -o $(filename).tex -w latex \
	       	--title-prefix=$(filename)
	$(pdflatex) $(filename).tex

html: $(source)
	$(pandoc) -s $(source) -o index.html -c style.css \
			--include-in-header=$(include_dir)/head.html \
	       	--include-before-body=$(include_dir)/author.html \
	       	--include-before-body=$(include_dir)/share.html \
		--include-after-body=$(include_dir)/stats.html \
	       	--title-prefix=$(title)

epub: $(source)
	$(pandoc) -s $(source) -t epub --epub-metadata=$(include_dir)/metadata.xml \
	       	-o $(filename).epub --epub-stylesheet=epub.css --title-prefix=$(title) \
		--epub-cover-image=img/cover.jpg

clean: 
	rm -f $(filename).aux \
       		$(filename).db \
	       	$(filename).log \
		$(filename).out \
	       	$(filename).tex
