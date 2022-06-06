# Simplayer

Simplayer, combined with 'simple' and 'player', is a simple but strong media player. This player uses ffmpeg, OpenGL, and MultiMedia. It was made by Zhuohua Huang and Tianyin Wang.

## This player has the following functions: 

### 1. basic functions
* Can play audio &amp; video files in various mainstream formats;

* Playback control includes: pause, play, fast forward, rewind, stop, previous one, next one, 0.5~ 4x speed, choose to loop or play in sequence, and can use shortcut to control, select video file Open and play automatically;

* The media library is composed of all the files that have been opened in the past.  You can use the previous one and the next one to select the video file to play.  At the same time, consider the situation when the file has been deleted.  If the file that has been opened in the past no longer exists locally, When this file is selected, it will be automatically deleted and continue to search down for the next file that can be played, and the user can freely choose to play in a loop or in sequence;

* Program packaging: use qt's packaging tool to package the program and run it on any platform;

### 2. Advanced Features

* Progress bar preview: When the user hovers the mouse over a valid position on the progress bar, an image at the position will be displayed above the corresponding position of the progress bar;

* Video playback: (non-real-time) The user selects the video file that needs to be reversed, and processes it in the background.  After waiting for a period of time, it can be played from the end of the video to the beginning;

* Double speed playback: achieve 0.5 to 4 times the double speed playback

* View media information: display the metadata information of the current media on the right side of the playlist, and display it in the form of key-value;

* Progress fine-tuning (fast forward, fast rewind): Click the fast forward or fast rewind key to fast forward (rewind) 1s forward or backward, in addition, it can be achieved through shortcuts;

* Audio waveform diagram: If it is an audio file without picture output, the dynamic display of the audio spectrogram can be performed and the playback progress of the audio can be kept consistent;
