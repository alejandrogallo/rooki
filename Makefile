test:
	for script in examples/*; do \
		test -f $$script && \
	 	test -x $$script && \
		./rooki $$script; \
 	done
