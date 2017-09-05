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

void MidiPart::addPatches() {
    for (int index = 0; index < this->mStaffs; index++) {
        this->mFileHandler->addPatchChange(this->mStartTrack+index, 0, this->mChannel, this->mProgram);
    }
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
}

void MidiHandler::setTempoPercent(double percent){
    if (percent > 1.0 || percent <=0 ){
        return;
    }
    this->mTempoPercent = percent;
}

MidiInfo* MidiHandler::save (string path) {
    this->parse(this->mScore);
    this->mMidifile->sortTracks();
    this->mMidifile->write(path);
    MidiInfo* info = new MidiInfo();
    info->mTempo = this->getTempo();
    return info;
}

void MidiHandler::reset () {
    this->mTicks = 0;
}

/* add midi track */
void MidiHandler::addTrack() {
    this->mMidifile->addTracks(1);
}


void MidiHandler::parse(mx::core::ScorePartwisePtr scorePartwise){
    this->parseScoreHeader(scorePartwise);
    this->parsePartList(scorePartwise);
}

void MidiHandler::parseScoreHeader(mx::core::ScorePartwisePtr scorePartwise){
    // Add another one track to the MIDI file
    auto workTitle = scorePartwise->getScoreHeaderGroup()->getWork()->getWorkTitle()->getValue().getValue();
    this->mMidifile->setFilename(workTitle);
    
    // 添加第一个part
    auto partlist = scorePartwise->getScoreHeaderGroup()->getPartList();
    auto name = partlist->getScorePart()->getPartName()->getValue().getValue();
    auto partid = partlist->getScorePart()->getAttributes()->id.getValue();
    
    if (partlist->getScorePart()->getMidiDeviceInstrumentGroupSet().size()) {
        auto instrument = *(partlist->getScorePart()->getMidiDeviceInstrumentGroupSet().rbegin());
        if (instrument->getHasMidiInstrument()) {
            auto channel = instrument->getMidiInstrument()->getMidiChannel()->getValue().getValue();
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
            auto instrument2 = *((*partname_it)->getScorePart()->getMidiDeviceInstrumentGroupSet().rbegin());
            auto channel = instrument2->getMidiInstrument()->getMidiChannel()->getValue().getValue();
            auto program = instrument2->getMidiInstrument()->getMidiProgram()->getValue().getValue();
            this->mParts.push_back(new MidiPart(1,channel,program,this->mMidifile));
        }
    }
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
        // part 中每个 track 调用 addPatch
        MidiPart* part = this->mParts[i];
        part->mStartTrack = this->mCurrentTrack;
        part->addPatches();
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
    this->mTempo = tempo;
    this->mMidifile->addTempo(track, tick, tempo * this->mTempoPercent);
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

MidiHandler::~MidiHandler () {}

