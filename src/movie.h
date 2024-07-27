#pragma once

#include "audio.h"
#include "video.h"

#include "chronons.h"
#include <atomic>
#include <string>
#include <thread>

class Movie {
public:
    Movie();
    ~Movie();

    int open(std::string filename);
    void close();
    void play();
    void pause();
    void stop();
    bool isOpen() const;
    bool isPlaying() const;
    bool isStopped() const;
    bool isPaused() const;
    bool isFinished() const;
    int getPosition() const;
    int getDuration() const;

    bool setVolume(double volume);
    double getVolume() const;

    void setPosition(int position);    
    bool seek(int64_t timestamp);

    bool setPlaybackRate(double rate);
    double getPlaybackRate() const;    
    
    std::pair<AVFrame *, int64_t> currentFrame();

    nanoseconds getMasterClock();
    nanoseconds getClock();

private:
    int start();
    int streamComponentOpen(unsigned int stream_index);

    friend class Audio;
    friend class Video;

    Audio audio_;
    Video video_;
    std::string filename_;
    AVFormatContextPtr fmtctx_;

    microseconds clockbase_{microseconds::min()};
    SyncMaster sync_{SyncMaster::Default};

    std::atomic<bool> quit_{false};

    std::thread parseThread_;
    std::thread audioThread_;
    std::thread videoThread_;
private:
    bool isPlaying_;
};
