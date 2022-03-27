JAVA  := java
JAVAC := javac
JAVAH := javah
CC    := gcc

CWD = $(shell pwd)

# jni.h / jni_md.h
CFLAGS := -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
# class和动态库的查找路径
JAVAFLAGS := -classpath "$(CLASSPATH):." -Djava.library.path=$(CWD)

all: TestJNI.class libtestJniLib.so
	@$(JAVA) $(JAVAFLAGS) TestJNI

TestJNI.class: TestJNI.java
	@$(JAVAC) $^

TestJNI.h: TestJNI.class
	@$(JAVAH) $(subst .class,,$^)

libtestJniLib.so: testJniLib.c TestJNI.h
	@$(CC) -shared $(CFLAGS) $^ -o $@

.PHONE: clean
clean:
	rm -rf TestJNI.class TestJNI.h libtestJniLib.so
