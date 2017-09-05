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
    
    void addPatches();
    
    MidiFile* mFileHandler;
    int mStaffs;// default 1
    int mChannel;
    int mProgram;
    int mStartTrack;
};

class MidiInfo {
public:
    int mTempo;
};

class MidiHandler {
public:
    MidiHandler(string filePath);
    MidiHandler(mx::core::ScorePartwisePtr score);
    void setTempoPercent(double percent);
    MidiInfo* save(string path);
    ~MidiHandler();
    int  getTempo();
private:
    void init ();
    void reset () ;
    void parse(mx::core::ScorePartwisePtr score);
    void parseScoreHeader(mx::core::ScorePartwisePtr score);
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
protected:
    MidiFile* mMidifile;
    int mTpq;
    int mTicks ;
    int mCurrentTrack;
    int mCurrentDivision;
    std::vector<int> mStepMap;
    std::vector<MidiPart*> mParts;
    double mTempoPercent;
    mx::core::ScorePartwisePtr mScore;
    string mOutputPath;
    int mTempo;
};

#endif /* MidiHandler_h */
