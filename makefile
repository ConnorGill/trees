OOPTS = -g -std=c99 -Wall -Wextra -c
LOPTS = -g -std=c99 -Wall -Wextra
CDAOBJS = cda.o cda-4-16.o integer.o
QOBJS = queue.o cda.o test-queue.o integer.o
BSTOBJS = tnode.o bst.o queue.o cda.o bst-0-4.o integer.o real.o
GSTOBJS = tnode.o bst.o queue.o cda.o gst.o gst-0-0.o integer.o real.o
RBTOBJS = tnode.o bst.o queue.o cda.o gst.o rbt-4-2.o integer.o string.o real.o rbt.o
TREESOBJS = tnode.o bst.o gst.o rbt.o cda.o queue.o string.o real.o integer.o scanner.o interpreter.o trees.o

TESTDIR = .testing/
TESTS = $(sort $(basename $(notdir $(wildcard $(TESTDIR)*.c))))
RBT-TESTS = rbt-4-0 rbt-4-1 rbt-4-2 rbt-4-3 rbt-4-4 rbt-4-5 rbt-4-6 rbt-4-7 rbt-4-8 rbt-4-9 rbt-4-10

all : cda queue bst gst rbt trees

copy: all
	cp -f *.o $(TESTDIR)
	cp -f *.h $(TESTDIR)
	cp -f rbt $(TESTDIR)

test-ass2: all copy
	for x in $(RBT-TESTS); do \
          echo; echo -------; echo $$x.expected; echo -------; cat $(TESTDIR)$$x.expected; \
					gcc $(OOPTS) $$x.c; \
					gcc $(LOPTS) tnode.o bst.o queue.o cda.o gst.o $$x.o integer.o string.o real.o rbt.o -o rbt; \
					./rbt $(TESTDIR)$$x > $(TESTDIR)$$x.yours; \
					echo -------; echo $$x.yours; echo -------; cat $(TESTDIR)$$x.yours; echo -------; \
          cmp --silent $(TESTDIR)$$x.expected $(TESTDIR)$$x.yours && echo "PASSED" || echo "FAILED"; echo -------; \
					rm -f $$x.o; \
  done

tester : all $(TESTS)
	for x in $(TESTS); do \
          echo; echo -------; echo $$x.expected; echo -------; cat $(TESTDIR)$$x.expected; \
          ./$(TESTDIR)$$x > $(TESTDIR)$$x.yours; \
          echo -------; echo $$x.yours; echo -------; cat $(TESTDIR)$$x.yours; echo -------; \
          cmp --silent $(TESTDIR)$$x.expected $(TESTDIR)$$x.yours && echo "PASSED" || echo "FAILED"; echo -------; \
  done

cda : $(CDAOBJS)
	gcc $(LOPTS) $(CDAOBJS) -o cda
queue : $(QOBJS)
	gcc $(LOPTS) $(QOBJS) -o queue
bst : $(BSTOBJS)
	gcc $(LOPTS) $(BSTOBJS) -o bst
gst : $(GSTOBJS)
	gcc $(LOPTS) $(GSTOBJS) -o gst
rbt : $(RBTOBJS)
	gcc $(LOPTS) $(RBTOBJS) -o rbt
trees : $(TREESOBJS)
	gcc $(LOPTS) $(TREESOBJS) -o trees
integer.o : integer.c integer.h
	gcc $(OOPTS) integer.c
real.o : real.c real.h
	gcc $(OOPTS) real.c
string.o : string.c string.h
	gcc $(OOPTS) string.c
scanner.o : scanner.c scanner.h
	gcc $(OOPTS) scanner.c
interpreter.o : interpreter.c interpreter.h rbt.h gst.h
	gcc $(OOPTS) interpreter.c
cda.o : cda.c cda.h
	gcc $(OOPTS) cda.c
queue.o : queue.c queue.h cda.h
	gcc $(OOPTS) queue.c
tnode.o : tnode.c tnode.h
	gcc $(OOPTS) tnode.c
bst.o : bst.c bst.h tnode.h queue.h
	gcc $(OOPTS) bst.c
gst.o : gst.c gst.h bst.h tnode.h queue.h
	gcc $(OOPTS) gst.c
rbt.o : rbt.c rbt.h tnode.h gst.h
	gcc $(OOPTS) rbt.c
trees.o : trees.c gst.h rbt.h string.h interpreter.h
	gcc $(OOPTS) trees.c
cda-4-16.o : cda-4-16.c cda.h
	gcc $(OOPTS) cda-4-16.c
test-queue.o : test-queue.c queue.h cda.h
	gcc $(OOPTS) test-queue.c
test-bst.o : test-bst.c integer.h string.h queue.h bst.h real.h
	gcc $(OOPTS) test-bst.c
bst-0-4.o : bst-0-4.c integer.h string.h queue.h bst.h real.h
	gcc $(OOPTS) bst-0-4.c
gst-0-0.o : gst-0-0.c integer.h string.h queue.h bst.h real.h
	gcc $(OOPTS) gst-0-0.c
rbt-4-2.o : rbt-4-2.c tnode.h gst.h rbt.h integer.h real.h string.h
	gcc $(OOPTS) rbt-4-2.c
test : trees #rbt #cda #trees #rbt #bst gst rbt #trees cda queue
	echo "Test is -r t-6-10"
	./trees -r t-6-10.corpus t-6-10.commands
valgrind : trees #bst gst rbt #trees cda queue
	#valgrind --leak-check=full ./da
	#valgrind --leak-check=full ./cda
	#valgrind --leak-check=full ./stack
	#valgrind --leak-check=full ./queue
	#valgrind --leak-check=full bst
	#valgrind --leak-check=full gst
	#valgrind --leak-check=full rbt
	valgrind --leak-check=full trees
clean :
	rm -f $(CDAOBJS) $(QOBJS) $(BSTOBJS) $(GSTOBJS) $(RBTOBJS) $(QOBJS) $(CDAOBJS) $(TREESOBJS) \
	bst gst cda queue rbt trees
