INCLUDES = -I. -I/usr/local/include/dar -I/usr/include/cc++2
CXX = g++
CXXFLAGS = -g -DDEBUG -O0 $(INCLUDES) `pkg-config gtkmm-2.4 --cflags`
LIBS = `pkg-config gtkmm-2.4 --libs` `pkg-config --libs gthread-2.0` -ldar  -lccgnu2

.SUFFIXES: .cc

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

.cc :
	$(CXX) $(CXXFLAGS) $(LIBS) $< -o $@ -lg++


SRC = multipleoptionview.cpp daraction.cpp thread.cpp backup.cpp log.cpp rootwindow.cpp config.cpp ufbackup.cpp aboutdialog.cpp propertiesdialog.cpp backupdialog.cpp preferencesdialog.cpp

OBJ = $(addsuffix .o, $(basename $(SRC)))

ufbackup: $(OBJ)
	$(CXX) $(CXXFLAGS) $(LIBS) -o $@ $(OBJ)

clean:
	rm -f ufbackup $(OBJ)
