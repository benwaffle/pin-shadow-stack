RECURSE = @$(MAKE) --no-print-directory -f mk/Build.mk

all:
	$(RECURSE) TOOL_OBJ=pintool
debug:
	$(RECURSE) TOOL_OBJ=pintool_debug debug
%:
	$(RECURSE) TOOL_OBJ=dont_build $@
