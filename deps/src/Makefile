
# Library           : Requires
# ------------------------------------------------------------------------
# deps              : -
#
PROJECTS = cigicl
PROJECTS += loguru
PROJECTS += osg
PROJECTS += freetype2

.PHONY: all clean $(PROJECTS)

#
# Rules
#
all: $(PROJECTS)

$(PROJECTS):
	$(MAKE) -C $@

clean:
	-for d in $(PROJECTS); do (cd $$d; $(MAKE) clean ); done

