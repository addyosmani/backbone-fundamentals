
#pandoc is the path to the pandoc executable. It can either simply be 'pandoc', 
#or if you require a custom path just replace it with that path. 
#e.g /Users/apple/Desktop/usr/local/bin/pandoc

#html
pandoc -s ../index.md -o ../index.html -c style.css --include-before-body=author.html --include-before-body=share.html --title-prefix='Developing Backbone.js Applications'

#epub
pandoc -s ../index.md -t epub --epub-metadata=metadata.xml -o ../backbone-fundamentals.epub -c style.css --title-prefix='Developing Backbone.js Applications' --epub-cover-image=../img/cover.jpg

#rtf
pandoc -s title.txt -S ../index.md -o ../backbone-fundamentals.rtf

