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

    int Open(std::string filename);
    void Close();
    void Play();
    void Pause();
    void Stop();
    bool IsOpened() const;
    bool IsPlaying() const;
    bool IsStopped() const;
    bool IsPaused() const;
    bool IsFinished() const;
    int GetPosition() const;
    int GetDuration() const;

    bool SetVolume(double volume);
    double GetVolume() const;

    void SetPosition(int position);    
    bool Seek(int64_t timestamp);

    bool SetPlaybackRate(double rate);
    double GetPlaybackRate() const;    
    
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
