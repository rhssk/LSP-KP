TARGETS = common client server

all:
	$(foreach var,$(TARGETS),cd $(var) && make && cd ..;)

release: $(TARGETS)
	$(foreach var,$(TARGETS),cd $(var) && make release && cd ..;)

clean:
	$(foreach var,$(TARGETS),cd $(var) && make clean && cd ..;)
