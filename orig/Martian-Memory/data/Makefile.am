group = games
perms = 666

scoredir = $(SCOREPATH)/data

score_DATA = \
scores.dat

EXTRA_DIST = $(score_DATA)

install-data-hook:
	for file in $(score_DATA); do\
	    chgrp $(group) $(scoredir)/$$file; \
	    chmod $(perms) $(scoredir)/$$file; \
	done					    
