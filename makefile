CXX           = g++
LINK          = g++
DEFINES       = 
CXXFLAGS      = -pipe -g -W -Wno-unused-variable -Wno-sign-compare $(DEFINES)
LIBS          = -L/usr/lib -lcurl -lssl -lcrypto 
INCPATH       = -I. -I./src -I./example -I/usr/include

SRCS = ./example/ucloud.cpp \
	./src/AccessRequest.cpp \
	./src/BaseRequest.cpp \
	./src/HttpRequest.cpp \
	./src/TokenRequest.cpp \
	./src/UcloudOpenApi.cpp \
	./src/util.cpp

OBJECTS = $(SRCS:%.cpp=%.o)

TARGET = ucloud_test
 
.SUFFIXES: .o .cpp

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"
 
all : $(TARGET)
 
$(TARGET):  $(OBJECTS) 
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)
 
depend:
	gccmakedep $(SRCS) $(INCPATH)
 
clean:
	rm -f $(OBJECTS) $(TARGET)