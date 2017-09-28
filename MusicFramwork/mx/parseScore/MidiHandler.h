//
//  MidiHandler.h
//  musicxml
//
//  Created by tanhui on 2017/7/26.
//  Copyright © 2017年 ci123. All rights reserved.
//

#ifndef MidiHandler_h
#define MidiHandler_h

#include <stdio.h>
#include <queue>
#include <vector>
#include "../core/Elements.h"
#include "../midifile/MidiFile.h"


class MidiPart {
    public :
    
    MidiPart(int staff,int channel, int program ,MidiFile* fileHandler);
    ~MidiPart();
    void addPatches(double ticks);
    
    MidiFile* mFileHandler;
    int mStaffs;// default 1
    int mChannel;
    int mProgram;
    int mStartTrack;
};

class MidiInfo {
public:
    MidiInfo();
    int mTempo;
    bool mHasPrefix;
    int mPrefixBeat;
    int mPrefixBeatType;
};

class MidiHandler {
public:
    MidiHandler(string filePath);
    MidiHandler(mx::core::ScorePartwisePtr score);
    void setTempoPercent(double percent);
    void setTempoValue(int tempoValue);
    MidiInfo* save(string path,bool hasPrefix = false);
    ~MidiHandler();
    int  getTempo();
private:
    void init ();
    void reset () ;
    void parse(mx::core::ScorePartwisePtr score,bool hasPrefix);
    void parseScoreHeader(mx::core::ScorePartwisePtr score);
    void parseAttribute(mx::core::ScorePartwisePtr score);
    void parsePartList(mx::core::ScorePartwisePtr score);
    void parseDataChoice(mx::core::MusicDataChoicePtr dataChoice, int partIndex);
    void parseProperties(mx::core::PropertiesPtr property, int partIndex);
    void parseNote(mx::core::NotePtr note, int partIndex);
    void addNote(int step ,int octave,int alter, int duration,int staffth, int partIndex);
    void addRest(int duration);
    void addBackup(int duration);
    void addForward(int duration);
    void addTrack();
    int  getRelativeDuration(int duration);
    void addTempo(int track,int tick ,int tempo);
    void addFrontAccompany(int beat,int beatType,int trackIndex);
protected:
    MidiFile* mMidifile;
    int mTpq;
    int mTicks ;
    int mCurrentTrack;
    int mCurrentDivision;
    std::vector<int> mStepMap;
    std::vector<MidiPart*> mParts;
    double mTempoPercent;
    // 用户手动设置的
    int mAdjustTempo;
    mx::core::ScorePartwisePtr mScore;
    string mOutputPath;
    int mTempo;
    bool mLoadingAccom;
    int mBeginTicks;// 除去前奏的tempo
    MidiInfo* midiInfo;
};

#endif /* MidiHandler_h */

