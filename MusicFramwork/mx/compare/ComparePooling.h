//
//  ComparePooling.hpp
//  musicXML
//
//  Created by tanhui on 2017/8/24.
//  Copyright © 2017年 tanhui. All rights reserved.
//

#ifndef ComparePooling_hpp
#define ComparePooling_hpp

#include <stdio.h>
#include <iostream>    
#include <vector>

enum ECNoteType{
    CNoteTypeNone = 0,
    CNoteTypeOn,
    CNoteTypeOff,
    CNoteTypeWhole,
    CNoteTypeShort,
    CNoteTypeTimeout,
    CNoteTypeError,
    CNoteTypeLost,
};

class Signal {
public:
    int mNoteNumber;
    int mChannel;
    ECNoteType mType;
};

class MidiSignal :public Signal{
public:
    double mTimeStamp;
    /**
     * channel 通道
     * noteNumber 音符对应数字
     * timeStamp 信号的时间(以tick 为单位)
     * type 信号的类型
     */
    MidiSignal(int channel, double timeStamp, int noteNumber, ECNoteType type);
};


class CNote :public Signal{
public:
    int mDuration;
    double mStartTime;
    int mStaff;
    /**
     * channel 通道
     * noteNumber 音符对应数字
     * startTime 音符的开始时间(以tick 为单位)
     * duration 音符持续时长(以tick为单位)
     * staff 第几个staff
     * xPos 对应乐谱的位置
     */
    CNote(int channel, int noteNumber,  double startTime, int duration,int staff,int xPos = 0);
    bool isMapped;
    double mXpos;
};


class CResult {
public:
    CResult();
    double rate;
    std::vector<CNote*> mErrorNotes;
    std::vector<CNote*> mTimeOutNotes;
    std::vector<CNote*> mShortNotes;
    std::vector<CNote*> mLostNotes;
    std::vector<CNote*> mCorrectNotes;
};


class ComparePooling {
    std::vector<CNote*> mNotes;
    std::vector<MidiSignal*> mTempSignals; // 存放传过来的midi信号，处理完相应node后移除
    double mTheta;
public:
    ComparePooling(std::vector<CNote*> notes, double theta);
    // 定时调用此方法
    std::vector<CNote*> receiveCNotes(std::vector<MidiSignal*> notes, double currentTime);
    CResult* getFinalResult();
    
    void resetPool();// for development
private:
    CNote* getNearestCNote(MidiSignal* signal) ;
    CNote* getErrorCNote(MidiSignal* signal);
    CNote* getCNoteBy(int channel,int notenumber, int time );
};


#endif /* ComparePooling_hpp */
