all: doc.html design.png

%.html: %.rst
	rst2html $< >$@

design.png: design.dot
	dot -T png design.dot >design.png
