Kindlegen requires title and author metadata. To generate a mobi file from
a pandoc ebook, first generate an epub file using the metadata.xml file 
included:

```
pandoc -f markdown -t epub --epub-metadata=metadata.xml -o backbone-fundamentals.epub index.md
```

then use the kindlegen tool to make a mobi from that epub:

```
kindlegen backbone-fundamentals.epub
```
