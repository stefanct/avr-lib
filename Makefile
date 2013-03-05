all:

doxygen:
	doxygen docs/Doxyfile

clean:
	rm -rf docs/html/

.PHONY: clean doxygen
