.PHONY: runtest

RUNTEST_MAKE_REDIRECT ?= 2>&1
 #>/dev/null 2>&1
RUNTEST_MAKE_TARGET ?= test
RUNTEST_NPROC ?= 1
BUILD_CMD="" 

runtest:
	@ \
	RESULT=true ; \
	$(COLOR_ECHO) -n "Testing application $(COLOR_GREEN)$(APPLICATION)$(COLOR_RESET)\n\n" ; \
	for board in $(BOARDS); do \
		if BOARD=$${board} $(MAKE) check-toolchain-supported > /dev/null 2>&1; then \
			$(COLOR_ECHO) $(BUILD_CMD) ; \
			BOARD=$${board} QUITE=0 $(MAKE) -s $(RUNTEST_MAKE_TARGET) $(RUNTEST_MAKE_REDIRECT) || (echo "Try 2..." && \
			BOARD=$${board} QUITE=0 $(MAKE) -s $(RUNTEST_MAKE_TARGET) $(RUNTEST_MAKE_REDIRECT) || (echo "Try 3..." && \
			BOARD=$${board} QUITE=0 $(MAKE) -s $(RUNTEST_MAKE_TARGET) $(RUNTEST_MAKE_REDIRECT) )); \
			RES=$$? ; \
			$(COLOR_ECHO) -n "Testing for $$board ... " ; \
			if [ $$RES -eq 0 ]; then \
				$(COLOR_ECHO) "$(COLOR_GREEN)success.$(COLOR_RESET)" ; \
			else \
				$(COLOR_ECHO) "$(COLOR_RED)failed!$(COLOR_RESET)" ; \
				RESULT=false ; \
			fi ; \
		fi; \
	done ; \
	$${RESULT}
