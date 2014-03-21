
CC				= clang
SHELL			= /bin/bash

INSTALL			?= install
DESTDIR			?= /usr/local
SOV				= 1
VERSION			= 0.90
PKG				= 1

TARGET			= libejson
DESC			= JSON and EJSON C parser library

O				:= ..
ARCH			:= $(shell $(CC) -dumpmachine)
BUILD			:= $(O)/build-$(ARCH)
OBJ				:= $(BUILD)/obj/$(TARGET)
BIN				:= $(BUILD)/bin
LIB				:= $(BUILD)/lib

EXTRA_CFLAGS	+= -g -O2 -fPIC --std=gnu99


all				:	$(OBJ) $(BIN) $(LIB) $(LIB)/$(TARGET).a \
	$(LIB)/$(TARGET).so.$(SOV) \
	$(BIN)/ejson_test

$(OBJ) $(BIN) $(LIB):
	mkdir -p $(OBJ) $(BIN) $(LIB)
	
$(BIN)/ejson_test		:	$(OBJ)/ejson_test.o $(LIB)/$(TARGET).a
$(LIB)/$(TARGET).a		:	$(OBJ)/ejson.o
$(LIB)/$(TARGET).so.$(SOV):	$(OBJ)/ejson.o

ejson.c	: 	ejson.rl ejson.h
	ragel -G1 -e $<

clean			:
	rm -rf $(OBJ)/ejson* $(LIB)/libejson* $(BIN)/ejson*

install			:
	mkdir -p $(DESTDIR)/lib/ $(DESTDIR)/include/
	$(INSTALL) -m 644 $(LIB)/$(TARGET).a $(DESTDIR)/lib/
	$(INSTALL) -m 644 ejson.h $(DESTDIR)/include/
	rm -rf $(DESTDIR)/lib/$(TARGET).so*
	$(INSTALL) -m 644 $(LIB)/$(TARGET).so.$(VERSION).$(SOV) $(DESTDIR)/lib/
	cp -f -d  $(LIB)/{$(TARGET).so.$(SOV),$(TARGET).so} $(DESTDIR)/lib/
	mkdir -p $(DESTDIR)/lib/pkgconfig 
	sed -e "s|PREFIX|$(PREFIX)|" -e "s|VERSION|$(VERSION)|" \
		ejson.pc >$(DESTDIR)/lib/pkgconfig/ejson.pc

deb 			:
	rm -rf /tmp/deb
	make clean && make all && make install DESTDIR=/tmp/deb/usr
	mkdir -p $(BUILD)/debian; (cd $(BUILD)/debian; \
	fpm -s dir -t deb -C /tmp/deb -n $(TARGET) -v $(VERSION) --iteration $(PKG) \
		--description "$(DESC)" \
		usr/lib/{$(TARGET).so.$(SOV),$(TARGET).so.$(VERSION).$(SOV),$(TARGET).so} && \
	fpm -s dir -t deb -C /tmp/deb -n $(TARGET)-dev -v $(VERSION) --iteration $(PKG) \
		--description "$(DESC) - development files" \
		-d "$(TARGET) (= $(VERSION))" \
		usr/lib/$(TARGET).a \
		usr/lib/pkgconfig \
		usr/include ; \
	)

tarball			:
	make clean
	rm -rf /tmp/$(TARGET)-$(VERSION)
	mkdir /tmp/$(TARGET)-$(VERSION)
	rsync -a ./ /tmp/$(TARGET)-$(VERSION)/ --exclude=.git\* --exclude=\*.deb 
	( cd /tmp ; tar jcf  $(TARGET)-$(VERSION).tar.bz2 $(TARGET)-$(VERSION) ) 
	 
# Include autogenerated dependencies
-include ${OBJ}/*.d 

$(LIB)/%.so.$(SOV) $(LIB)/%.so.$(VERSION).$(SOV) $(LIB)/%.so: 
ifeq ($(V),1)
	{ \
		naked=${shell basename ${basename $@}}; \
		${CC} -shared -Wl,-soname,$$naked \
			-o $(LIB)/$$naked.$(VERSION).$(SOV) \
			$(filter %.o %.a,$^) \
			${EXTRA_LDFLAGS} ${BASE_LDFLAGS} ${LDFLAGS} && \
		ln -sf $$naked.$(VERSION).$(SOV) ${LIB}/$$naked.$(SOV) && \
		ln -sf $$naked.$(SOV) ${LIB}/$$naked ; \
	}
else
	@echo "  LDSO $@"
	@{ \
		naked=${shell basename ${basename $@}}; \
		${CC} -shared -Wl,-soname,$$naked \
			-o $(LIB)/$$naked.$(VERSION).$(SOV) \
			$(filter %.o %.a,$^) \
			${EXTRA_LDFLAGS} ${BASE_LDFLAGS} ${LDFLAGS} && \
		ln -sf $$naked.$(VERSION).$(SOV) ${LIB}/$$naked.$(SOV) && \
		ln -sf $$naked.$(SOV) ${LIB}/$$naked ; \
	} || echo ERROR: $@ -- relaunch with V=1
endif

$(LIB)/%.a :
ifeq ($(V),1)
	$(AR) cru $@ $^
	ranlib $@
else
	@echo "  LIB   ${@}" 
	@$(AR) cru $@ $^ \
		|| echo Error: $(AR) cru $@ $^
endif
	
${OBJ}/%.o: %.c
ifeq ($(V),1)
	${CC} -MMD ${EXTRA_CFLAGS} ${CPPFLAGS} ${CFLAGS} -c -o ${@} ${<}
else
	@echo "  CC    ${<}"
	@${CC} -MMD ${EXTRA_CFLAGS} ${CPPFLAGS} ${CFLAGS} -c -o ${@} ${<} \
		|| echo Error: ${CC} -MD ${EXTRA_CFLAGS} ${CPPFLAGS} ${CFLAGS} -c -o ${@} ${<}
endif

${BIN}/%:
ifeq ($(V),1)
	${CC} -o ${@} ${^} ${EXTRA_LDFLAGS} ${LDFLAGS} 
else
	@echo "  LD    ${*}"
	@${CC} -o ${@} ${^} ${LDFLAGS} ${EXTRA_LDFLAGS} \
		|| echo Error: ${CC} -o ${@} ${^} ${LDFLAGS} ${EXTRA_LDFLAGS}
endif
