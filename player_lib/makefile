
CFLAGS = `pkg-config --cflags gstreamer-1.0 glib-2.0`  
LIBS = `pkg-config --libs gstreamer-1.0 glib-2.0`  
TARGET = libmmplayer.so
BUILD_DIR = $(PWD)

CFLAGS += -std=c++11 -shared -fPIC
all: gst_player  release_so

gst_player:
	 gcc -o $(TARGET) mediaPlayer.c $(LIBS) $(CFLAGS)

release_so:
	mkdir -p $(BUILD_DIR)/../qt_gst_player/gst_player/libs
	cp -rf *.h $(BUILD_DIR)/../qt_gst_player/gst_player/libs
	cp -rf *.so $(BUILD_DIR)/../qt_gst_player/gst_player/libs
	export  LD_LIBRARY_PATH=LD_LIBRARY_PATH:$(BUILD_DIR)/../qt_gst_player/gst_player/libs



clean:
	rm -rf $(TARGET)