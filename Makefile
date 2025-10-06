CC       := gcc
CFLAGS   := $(shell pkg-config --cflags gtk+-3.0 gstreamer-1.0)
LDFLAGS  := $(shell pkg-config --libs   gtk+-3.0 gstreamer-1.0)
MACROS   := -D H264_ENCRYPTION=1
packages := \
	build-essential libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
	libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base \
	gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly \
	gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa \
	gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio \
	gstreamer1.0-libcamera libgtk-3-dev

hairline: hairline.c
	$(CC) $^ -o $@ $(CFLAGS) $(MACROS) $(LDFLAGS)

install:
	sudo apt-get install -y $(packages)

again: clean hairline

clean:
	rm -f hairline
