docpkg = $(PACKAGE_TARNAME)-doxy-$(PACKAGE_VERSION).tar.gz
docdir = /data/brown22/local/share/doc

EXTRA_DIST = header.html footer.html
doc_DATA = $(docpkg)

$(docpkg): doxygen.stamp
	tar czf $(docpkg) --no-same-owner html
	cp -r html $(docdir)/$(PACKAGE_TARNAME)

doxygen.stamp: Doxyfile
	$(DOXYGEN) $(DOXYFLAGS) $<
	echo Timestamp > $@

CLEANFILES = doxywarn.txt doxygen.stamp $(docpkg)

clean-local:
	rm -rf html
