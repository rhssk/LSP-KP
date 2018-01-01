TARGETS = common client server

all:
	$(foreach var,$(TARGETS),cd $(var) && make && cd ..;)

debug: $(TARGETS)
	$(foreach var,$(TARGETS),cd $(var) && make debug && cd ..;)

clean:
	$(foreach var,$(TARGETS),cd $(var) && make clean && cd ..;)
