
AUTOGEN=autogen
AUTOGEN_DEF_DIR=$(top_builddir)/autogen

# .tpl to .h
%.h: %.h.tpl
	$(AUTOGEN) \
		-b $(basename $@) \
		-T $< \
		$(AUTOGEN_DEF_DIR)/$(basename $(notdir $@)).def


# .tpl to .cpp
%.cpp: %.cpp.tpl
	$(AUTOGEN) \
		-b $(basename $@) \
		-T $< \
		$(AUTOGEN_DEF_DIR)/$(basename $(notdir $@)).def
