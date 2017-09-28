//
//  MidiHandler.cpp
//  musicxml
//
//  Created by tanhui on 2017/7/26.
//  Copyright © 2017年 ci123. All rights reserved.
//

#include "MidiHandler.h"
#include <stdlib.h>
#include <iostream>
#include "../xml/XDoc.h"
#include "../core/Document.h"
#include "../xml/XFactory.h"

using namespace mx::core;
using namespace std;


#pragma mark MidiPart
MidiPart::MidiPart(int staff,int channel, int program,MidiFile* handler){
    this->mStaffs = staff;
    this->mChannel = channel;
    this->mProgram = program;
    this->mFileHandler = handler;
}

void MidiPart::addPatches(double ticks) {
    for (int index = 0; index < this->mStaffs; index++) {
        this->mFileHandler->addPatchChange(this->mStartTrack+index, ticks, this->mChannel, this->mProgram);
    }
}

MidiPart:: ~MidiPart(){
}
#pragma mark MidiInfo
MidiInfo::MidiInfo(){
    this->mPrefixBeat = 0;
    this->mPrefixBeatType = 0 ;
    this->mTempo = 0;
    this->mHasPrefix = false;
}

#pragma mark MidiHandler
MidiHandler::MidiHandler(string filePath) {
    this->init();
    mx::core::DocumentPtr mxDoc = mx::core::makeDocument();
    mx::xml::XDocPtr xmlDoc = mx::xml::XFactory::makeXDoc();
    // read a MusicXML file into the XML DOM structure
    xmlDoc->loadFile( filePath );
    // create an ostream to receive any parsing messages
    std::stringstream parseMessages;
    // convert the XML DOM into MusicXML Classes
    bool isSuccess = mxDoc->fromXDoc( parseMessages, *xmlDoc );
    if( !isSuccess )
    {
        std::cout << "Parsing of the MusicXML document failed with the following message(s):" << std::endl;
        std::cout << parseMessages.str() << std::endl;
    }
    // maybe the document was timewise document. if so, convert it to partwise
    if( mxDoc->getChoice() == mx::core::DocumentChoice::timewise )
    {
        mxDoc->convertContents();
    }
    // get the root
    auto scorePartwise = mxDoc->getScorePartwise();
    this->mScore = scorePartwise;
}

MidiHandler::MidiHandler(mx::core::ScorePartwisePtr score) {
    this->init();
    this->mScore = score;
}

void MidiHandler::init() {
    this->mMidifile = new MidiFile();
    this->mTicks = 0;
    this->mCurrentTrack = 0 ;
    this->mStepMap = {9,11,0,2,4,5,7};
    this->mTpq = 480;
    this->mMidifile->absoluteTicks();
    this->mMidifile->setTicksPerQuarterNote(this->mTpq);
    this->mTempoPercent = 1.0;
    this->mTempo = 120;
    this->mAdjustTempo = 0;
    this->mLoadingAccom = false;
    this->mBeginTicks = 0;
}

void MidiHandler::setTempoPercent(double percent){
    if (percent > 1.0 || percent <=0 ){
        return;
    }
    this->mTempoPercent = percent;
}

void MidiHandler::setTempoValue(int tempoValue){
    if (tempoValue >= 30 && tempoValue <= 120 ){
        this->mAdjustTempo = this->mTempo = tempoValue;
    }
}

MidiInfo* MidiHandler::save(string path,bool hasPrefix){
    MidiInfo* info = new MidiInfo();
    this->midiInfo = info;
    this->parse(this->mScore,hasPrefix);
    this->mMidifile->sortTracks();
    this->mMidifile->write(path);
    info->mTempo = this->getTempo();
    info->mHasPrefix = hasPrefix;
    return info;
}

void MidiHandler::reset () {
    this->mTicks = this->mBeginTicks;
}

/* add midi track */
void MidiHandler::addTrack() {
    this->mMidifile->addTracks(1);
}


void MidiHandler::parse(mx::core::ScorePartwisePtr scorePartwise,bool hasPrefix){
    this->parseScoreHeader(scorePartwise);
    if(hasPrefix){
        this->parseAttribute(scorePartwise);
    }
    this->parsePartList(scorePartwise);
}

// 先取出attribute信息，计算并加入前奏
void MidiHandler::parseAttribute(mx::core::ScorePartwisePtr scorePartwise){
    
    int i = 0;
    int selectPart = 0;
    for (auto partIndex = this->mParts.cbegin(); partIndex != this->mParts.cend(); partIndex++,i++) {
        if (0 == i) {
            this->mCurrentTrack = 0;
        } else {
            MidiPart* prePart = this->mParts[i-1];
            this->mCurrentTrack += prePart->mStaffs;
        }
        // part 中每个 track 调用 addPatch
        MidiPart* part = this->mParts[i];
        part->mStartTrack = this->mCurrentTrack;
        part->addPatches(0);
        if (0 == selectPart && part->mProgram <= 8) {
            selectPart = i;
        }
    }
    
    
    for (auto part_it = scorePartwise->getPartwisePartSet().cbegin(); part_it != scorePartwise->getPartwisePartSet().cend(); part_it++,i++) {
        auto partwise = (*part_it);
        
        for (auto measure_it = partwise->getPartwiseMeasureSet().cbegin();measure_it != partwise->getPartwiseMeasureSet().cend(); measure_it++) {
            MusicDataChoiceSet groupSetPtr = (*measure_it)->getMusicDataGroup()->getMusicDataChoiceSet();
            for (auto choice_it = groupSetPtr.cbegin() ; choice_it != groupSetPtr.cend() ; choice_it ++) {
                auto dataChoice = (*choice_it);
                auto choiceType = dataChoice->getChoice();
                if (choiceType == MusicDataChoice::Choice::properties) {
                    auto property = dataChoice->getProperties();
                    
                    if (property->getHasDivisions()){
                        auto divisions = property->getDivisions()->getValue().getValue();
                        this->mCurrentDivision = (int)divisions;
                    }
                    if (property->getTimeSet().size()){
                        TimePtr time  = *(property->getTimeSet().cbegin());
                        TimeChoice::Choice choice = time->getTimeChoice()->getChoice();
                        if (!(int)choice){
                            TimeSignatureGroupPtr timeSignaturePtr = *(time->getTimeChoice()->getTimeSignatureGroupSet().cbegin());
                            std::string beats = timeSignaturePtr->getBeats()->getValue().getValue();
                            std::string beat_types = timeSignaturePtr->getBeatType()->getValue().getValue();
                            
                            if(!this->mLoadingAccom){
                                // 添加前奏
                                this->mLoadingAccom = true;
                                this->addFrontAccompany(atoi(beats.c_str()),atoi(beat_types.c_str()),selectPart);
                                if (this->mCurrentDivision) break;
                            }
                            
                        }
                    }
                }
            }
        }
    }
    
    
}
void MidiHandler::parseScoreHeader(mx::core::ScorePartwisePtr scorePartwise){
    // Add another one track to the MIDI file
    auto workTitle = scorePartwise->getScoreHeaderGroup()->getWork()->getWorkTitle()->getValue().getValue();
    this->mMidifile->setFilename(workTitle);
    
    // 添加第一个part
    auto partlist = scorePartwise->getScoreHeaderGroup()->getPartList();
    auto name = partlist->getScorePart()->getPartName()->getValue().getValue();
    auto partid = partlist->getScorePart()->getAttributes()->id.getValue();
    int channel = 1;
    if (partlist->getScorePart()->getMidiDeviceInstrumentGroupSet().size()) {
        auto instrument = *(partlist->getScorePart()->getMidiDeviceInstrumentGroupSet().rbegin());
        if (instrument->getHasMidiInstrument()) {
            channel = instrument->getMidiInstrument()->getMidiChannel()->getValue().getValue();
            auto program = instrument->getMidiInstrument()->getMidiProgram()->getValue().getValue();
            this->mParts.push_back(new MidiPart(1,channel,program,this->mMidifile));
        }
    }else if(partlist->getScorePart()->getScoreInstrumentSet().size()){
        this->mParts.push_back(new MidiPart(1,1,1,this->mMidifile));
    }
    
    
    // 添加剩余part
    for (auto partname_it = partlist->getPartGroupOrScorePartSet().rbegin(); partname_it != partlist->getPartGroupOrScorePartSet().rend(); partname_it++) {
        
        this->addTrack();
        
        auto choice = (*partname_it)->getChoice();
        if (choice == PartGroupOrScorePart::Choice::partGroup) {
        }else if(choice == PartGroupOrScorePart::Choice::scorePart){
            int program = 1;
            if ((*partname_it)->getScorePart()->getMidiDeviceInstrumentGroupSet().size()) {
                auto instrument2 = *((*partname_it)->getScorePart()->getMidiDeviceInstrumentGroupSet().rbegin());
                channel = instrument2->getMidiInstrument()->getMidiChannel()->getValue().getValue();
                program = instrument2->getMidiInstrument()->getMidiProgram()->getValue().getValue();
            }else{
                channel++;
            }
            this->mParts.push_back(new MidiPart(1,channel,program,this->mMidifile));
        }
    }
}

void MidiHandler::addFrontAccompany(int beat,int beatType,int trackIndex){
    this->midiInfo->mPrefixBeat = beat;
    this->midiInfo->mPrefixBeatType = beatType;
    double tempo = this->mCurrentDivision * beatType / 4;
    for (int i = 0;i < beat ;i++){
        this->addNote(4 ,4,0,tempo,1,trackIndex);
        this->mTicks -= this->getRelativeDuration(tempo);
        this->addNote(3 ,4,0,tempo,1,trackIndex);
        this->mTicks -= this->getRelativeDuration(tempo);
        this->addNote(2 ,4,0,tempo,1,trackIndex);
    }
    //保存前奏的ticks 时长
    this->mBeginTicks = this->mTicks;
    
}

/* parse score parts */
void MidiHandler::parsePartList(mx::core::ScorePartwisePtr scorePartwise){
    int i = 0 ;
    for (auto part_it = scorePartwise->getPartwisePartSet().cbegin(); part_it != scorePartwise->getPartwisePartSet().cend(); i++, part_it++) {
        if (this->mParts.size()<=i) {
            continue;
        }
        if (0 == i) {
            this->mCurrentTrack = 0;
        } else {
            MidiPart* prePart = this->mParts[i-1];
            this->mCurrentTrack += prePart->mStaffs;
        }
        
        this->reset();
        auto partwise = (*part_it);
        for (auto measure_it = partwise->getPartwiseMeasureSet().cbegin();measure_it != partwise->getPartwiseMeasureSet().cend(); measure_it++) {
            MusicDataChoiceSet groupSetPtr = (*measure_it)->getMusicDataGroup()->getMusicDataChoiceSet();
            for (auto choice_it = groupSetPtr.cbegin() ; choice_it != groupSetPtr.cend() ; choice_it ++) {
                this->parseDataChoice(*choice_it,i);
            }
        }
        //        // part 中每个 track 调用 addPatch
        //        MidiPart* part = this->mParts[i];
        //        part->mStartTrack = this->mCurrentTrack;
        //        part->addPatches(this->mBeginTicks);
    }
    
}

/* parse measure data */
void MidiHandler::parseDataChoice(MusicDataChoicePtr dataChoice, int partIndex){
    auto choiceType = dataChoice->getChoice();
    if (choiceType == MusicDataChoice::Choice::properties) {
        auto choiceMeasureAttr = dataChoice->getProperties();
        this->parseProperties(choiceMeasureAttr,partIndex);
    }else if (choiceType == MusicDataChoice::Choice::note) {
        auto choiceNote = dataChoice->getNote();
        auto normalChoice =  choiceNote->getNoteChoice()->getChoice();
        // 过滤 grace cur
        if (normalChoice != NoteChoice::Choice::normal){
            return;
        }
        this->parseNote(choiceNote,partIndex);
    }else if (choiceType == MusicDataChoice::Choice::forward) {
        auto duration = dataChoice->getForward()->getDuration()->getValue().getValue();
        this->addForward(duration);
    }else if (choiceType == MusicDataChoice::Choice::backup) {
        int duration = dataChoice->getBackup()->getDuration()->getValue().getValue();
        this->addBackup(duration);
    }else if (choiceType == MusicDataChoice::Choice::sound) {
        if (dataChoice->getSound()->getAttributes()->hasTempo) {
            auto tempo = dataChoice->getSound()->getAttributes()->tempo.getValue();
            this->addTempo(this->mCurrentTrack,0,tempo);
        }
    }else if (choiceType == MusicDataChoice::Choice::direction) {
        if (dataChoice->getDirection()->getSound()->getAttributes()->hasTempo) {
            auto tempo = dataChoice->getDirection()->getSound()->getAttributes()->tempo.getValue();
            this->addTempo(this->mCurrentTrack,0,tempo);
        }
    }
}

/* parse note or rest */
void MidiHandler::parseNote(mx::core::NotePtr note, int partIndex) {
    auto noteTypeChoice =  note->getNoteChoice()->getNormalNoteGroup()->getFullNoteGroup()->getFullNoteTypeChoice()->getChoice();
    
    auto duration = note->getNoteChoice()->getNormalNoteGroup()->getDuration()->getValue().getValue();
    if (noteTypeChoice == FullNoteTypeChoice::Choice::pitch){
        auto pitch = note->getNoteChoice()->getNormalNoteGroup()->getFullNoteGroup()->getFullNoteTypeChoice()->getPitch();
        auto step = pitch->getStep()->getValue();
        auto actave = pitch->getOctave()->getValue().getValue();
        auto alter = pitch->getHasAlter()? pitch->getAlter()->getValue().getValue() : 0;
        auto staffth = note->getHasStaff()? note->getStaff()->getValue().getValue() : 1;
        if (note->getNoteChoice()->getNormalNoteGroup()->getFullNoteGroup()->getHasChord()) {
            // 如果是chord ，将偏移量减回来
            this->mTicks -= this->getRelativeDuration(duration);
        }
        this->addNote((int)step ,actave,alter,(int)duration,staffth,partIndex);
        
    }else if(noteTypeChoice == FullNoteTypeChoice::Choice::rest){
        this->addRest(duration);
    }
}

/* parse attributes */
void MidiHandler::parseProperties(mx::core::PropertiesPtr property, int partIndex) {
    if (property->getHasDivisions()){
        // 暂不处理 使用 默认的 120
        auto divisions = property->getDivisions()->getValue().getValue();
        this->mCurrentDivision = (int)divisions;
        
    }
    if (property->getHasStaves()) {
        auto staff = property->getStaves()->getValue().getValue();
        // 判断staff
        if (staff > 1) {
            MidiPart* part = this->mParts[partIndex];
            part->mStaffs = staff;
            this->addTrack();
        }
    }
    if (property->getTimeSet().size()){
        TimePtr time  = *(property->getTimeSet().cbegin());
        TimeChoice::Choice choice = time->getTimeChoice()->getChoice();
        if (!(int)choice){
            TimeSignatureGroupPtr timeSignaturePtr = *(time->getTimeChoice()->getTimeSignatureGroupSet().cbegin());
            std::string beats = timeSignaturePtr->getBeats()->getValue().getValue();
            std::string beat_types = timeSignaturePtr->getBeatType()->getValue().getValue();
            this->mMidifile->addTimeSignature(this->mCurrentTrack, this->mTicks, atoi(beats.c_str()),atoi(beat_types.c_str()));
        }
    }
    //    if (property->getKeySet().size()) {
    //        KeyPtr keyPtr = *(property->getKeySet().cbegin());
    //        auto fifth = keyPtr->getKeyChoice()->getTraditionalKey()->getFifths()->getValue().getValue();
    //        auto mode = keyPtr->getKeyChoice()->getTraditionalKey()->getMode()->getValue().getValue();
    //
    //    }
}

void MidiHandler::addTempo(int track,int tick ,int tempo) {
    if (!this->mAdjustTempo){
        this->mTempo = tempo;
    }else{
        // 如果用户手动设置速度，速度则不改变
        this->mTempo = this->mAdjustTempo;
    }
    this->mMidifile->addTempo(track, tick, this->getTempo());
}

/* add real note */
void MidiHandler::addNote(int step ,int octave,int alter, int duration, int staffth ,int partIndex) {
    MidiPart* part = this->mParts[partIndex];
    int value = this->mStepMap[(int)step] + (octave+1)* 12 + alter;
    int velocity = 90;
    int relativeDivision = this->getRelativeDuration(duration);
    this->mMidifile->addNoteOn(this->mCurrentTrack + (staffth - 1), this->mTicks, part->mChannel, value, velocity);
    this->mTicks += relativeDivision;
    this->mMidifile->addNoteOff(this->mCurrentTrack + (staffth - 1), this->mTicks-1, part->mChannel, value, velocity);
}
/* add real rest */
void MidiHandler::addRest(int duration){
    int relativeDuration = this->getRelativeDuration(duration);
    this->mTicks += relativeDuration;
}
/* decrease duration */
void MidiHandler::addBackup(int duration) {
    int relativeDuration = this->getRelativeDuration(duration);
    this->mTicks -= relativeDuration;
}

/* forward duration */
void MidiHandler::addForward(int duration) {
    int relativeDuration = this->getRelativeDuration(duration);
    this->mTicks += relativeDuration;
}
/* 根据比例放大duration */
int MidiHandler::getRelativeDuration(int duration) {
    return  double(this->mTpq)/double(this->mCurrentDivision) * duration;
}

int MidiHandler::getTempo(){
    return this->mTempo * this->mTempoPercent;
}

MidiHandler::~MidiHandler () {
    delete this->midiInfo;
    delete this->mMidifile;
    for(vector<MidiPart *>::iterator it = this->mParts.begin(); it != this->mParts.end(); it ++){
        if (NULL != *it) {
            delete *it;
            *it = NULL;
        }
    }
    this->mParts.clear();
    this->mStepMap.clear();
}

