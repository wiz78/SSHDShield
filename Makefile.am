## Makefile.am -- Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.1.1.1 2005/10/04 10:50:23 wiz Exp $

auxdir			= @ac_aux_dir@
AUX_DIST		= $(auxdir)/install-sh $(auxdir)/missing \
			      $(auxdir)/mkinstalldirs
AUX_DIST_EXTRA	= 

EXTRA_DIST			= bootstrap
AUTOMAKE_OPTIONS 	= foreign

SUBDIRS 			= src

MAINTAINERCLEANFILES 	= Makefile.in aclocal.m4 configure src/include/config-h.in \
						  $(AUX_DIST)

ACLOCAL		= aclocal -I $(auxdir)
LIBTOOL_DEPS	= @LIBTOOL_DEPS@

libtool: $(LIBTOOL_DEPS)
	        $(SHELL) ./config.status --recheck

dist-hook:
	(cd $(distdir) && mkdir -p $(auxdir))
	
	for file in $(AUX_DIST) $(AUX_DIST_EXTRA); do \
	  cp $$file $(distdir)/$$file; \
	done
	
	cp -f README $(distdir)
