
CFLAGS=`pkg-config --cflags gstreamer-1.0 glib-2.0`  
LIBS=`pkg-config --libs gstreamer-1.0 glib-2.0`  
TARGET=MediaPlayer

all: gst_player  

gst_player:
	 gcc -o $(TARGET) mediaPlayer.c mediaPlayer.h $(LIBS) $(CFLAGS)
clean:
	rm -rf $(TARGET)