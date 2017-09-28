//
//  ComparePooling.cpp
//  musicXML
//
//  Created by tanhui on 2017/8/24.
//  Copyright © 2017年 tanhui. All rights reserved.
//

#include "ComparePooling.h"

#pragma mark MidiSignal Class

MidiSignal::MidiSignal(int channel, double timeStamp, int noteNumber, EMidiSignalType type) {
    this->mTimeStamp = timeStamp;
    this->mNoteNumber = noteNumber;
    this->mType = type;
    this->mChannel = channel;
}

#pragma mark CNote Class

CNote::CNote(int staffIndex, int noteNumber,  double startTime, double duration ){
    this->mNoteNumber = noteNumber;
    this->mStaffIndex = staffIndex;
    this->mDuration = duration;
    this->mStartTime = startTime;
    this->mType = ECNoteResultType::ECNoteResultTypeNone;
    this->mErrorNumber = 0;
}

#pragma mark CResult Class
CResult::CResult(){
    this->mTotalNotes = 0;
    this->mResultNotes = std::vector<CNote*>();
}
CResult::~CResult(){
    //    for(std::vector<CNote *>::iterator it = this->mResultNotes.begin(); it != this->mResultNotes.end(); it ++){
    //        if (NULL != *it) {
    //            delete *it;
    //            *it = NULL;
    //        }
    //    }
}
#pragma mark ComparePooling Class
ComparePooling::ComparePooling(std::vector<CNote *> notes, double theta) {
    this->mNotes = notes;
    this->mTheta = theta;
    this->mTempSignals = std::vector<MidiSignal*>();
    this->mMode = InstrumentMode_DoubleKey;// 默认双排键
}

void ComparePooling::resetPool(){
    for (auto noteIndex = this->mNotes.cbegin(); noteIndex != this->mNotes.cend(); noteIndex++) {
        CNote* note = (*noteIndex);
        note->mType = ECNoteResultType::ECNoteResultTypeNone;
        note->mErrorNumber = 0;
    }
}

void ComparePooling::setMode(EInstrumentMode mode){
    this->mMode = mode;
}

std::vector<CNote*> ComparePooling::receiveMidiSignals(std::vector<MidiSignal*> signals, double currentTime) {
    std::vector<CNote*> retVec = std::vector<CNote*>();
    
    for (auto it = signals.cbegin(); it != signals.cend(); it++ ) {
        CNote* note = this->getNearestCNote( *it);
        if (!note && (*it)->mType == EMidiSignalType::EMidiSignalTypeOn) {
            // 该信号是错误信号 找出弹错的信号
            note = this->getErrorCNote( *it);
        }
        if (note) {
            auto pos= std::find(retVec.begin(), retVec.end(), note);
            if (pos == retVec.end()) {
                retVec.push_back(note);
            }
        }
    }
    // 筛选漏掉的note
    for (auto noteIndex = this->mNotes.cbegin(); noteIndex != this->mNotes.cend(); noteIndex++) {
        CNote* note = (*noteIndex);
        // 定义超过2倍的误差 theta 为漏键
        if (note->mType == ECNoteResultType::ECNoteResultTypeNone&& note->mStartTime + 2 * this->mTheta < currentTime) {
            note->mType = ECNoteResultType::ECNoteResultTypeLost;
            auto pos= std::find(retVec.begin(), retVec.end(), note);
            if (pos == retVec.end()) {
                retVec.push_back(note);
            }
        }
    }
    
    return retVec;
}
// 获取结果
CResult* ComparePooling::getFinalResult(){
    CResult* result = new CResult();
    result->mTotalNotes = (int)this->mNotes.size();
    for (auto noteIndex = this->mNotes.cbegin(); noteIndex != this->mNotes.cend(); noteIndex++) {
        CNote* note = (*noteIndex);
        if(note->mType == ECNoteResultType::ECNoteResultTypeError){
            result->mResultNotes.push_back(note);
        }else if (note->mType == ECNoteResultType::ECNoteResultTypeShort){
            result->mResultNotes.push_back(note);
        }else if(note->mType == ECNoteResultType::ECNoteResultTypeTimeout){
            result->mResultNotes.push_back(note);
        }else if(note->mType == ECNoteResultType::ECNoteResultTypeLost){
            result->mResultNotes.push_back(note);
        }
    }
    return result;
}

// 检查是否错误信号 时间差在theta 范围内，但是notenumber 不对
CNote* ComparePooling::getErrorCNote(MidiSignal* signal) {
    CNote* ret = NULL;
    for (auto noteIndex = this->mNotes.cbegin(); noteIndex != this->mNotes.cend(); noteIndex++) {
        CNote* note = (*noteIndex);
        if (note->mType == ECNoteResultType::ECNoteResultTypeNone && abs(note->mStartTime - signal->mTimeStamp)  <= this->mTheta) {
            note->mType = ECNoteResultType::ECNoteResultTypeError;
            note->mErrorNumber = signal->mNoteNumber;
            ret = note;
            break;// 避免将多个同时间的note置为error
        }
    }
    return ret;
}


// 通过midisignal 找到该时间对应的标准note
CNote* ComparePooling::getNearestCNote(MidiSignal* signal) {
    CNote* ret = NULL;
    
    if (signal->mType == EMidiSignalType::EMidiSignalTypeOn) { // 处理noteOn 信号
        CNote* note = this->getCNoteBy(signal->mChannel, signal->mNoteNumber, signal->mTimeStamp);
        if (note) {
            ret = note;
            ret->mType = ECNoteResultType::ECNoteResultTypeOn;
            this->mTempSignals.push_back(signal);// 将 noteOn 信号储存起来，等待noteOff信号
        }
    }else if (signal->mType == EMidiSignalType::EMidiSignalTypeOff) {// 处理Off 信号
        for (std::vector<MidiSignal*>::const_iterator tempSignalIndex = this->mTempSignals.cbegin(); tempSignalIndex != this->mTempSignals.cend(); tempSignalIndex++) {
            if ((*tempSignalIndex)->mNoteNumber == signal->mNoteNumber &&
                (*tempSignalIndex)->mChannel == signal->mChannel) {
                ret = this->getCNoteBy(signal->mChannel, signal->mNoteNumber, (*tempSignalIndex)->mTimeStamp);
                if (ret) {
                    if (signal->mTimeStamp-(*tempSignalIndex)->mTimeStamp < ret->mDuration - this->mTheta) {
                        // todo:暂时不处理时间长短
                        ret->mType = ECNoteResultType::ECNoteResultTypeOff; // 按键时间太短
                    }else if (signal->mTimeStamp-(*tempSignalIndex)->mTimeStamp > ret->mDuration + this->mTheta) {
                        ret->mType = ECNoteResultType::ECNoteResultTypeOff; // 按键时间太长
                    }else{
                        ret->mType = ECNoteResultType::ECNoteResultTypeOff;
                    }
                    this->mTempSignals.erase(tempSignalIndex);
                }
                break;
            }
        }
    }
    return ret;
}

// 通过 midi信号的channel-> staff ,notenumber ,time 获取note
CNote* ComparePooling::getCNoteBy(int channel,int notenumber, int time ) {
    int staffIndex = this->getStaffIndexByChannel(channel);
    for (auto noteIndex = this->mNotes.cbegin(); noteIndex != this->mNotes.cend(); noteIndex++ ) {
        CNote* note = * noteIndex;
        if ((note->mType == ECNoteResultTypeOn||note->mType == ECNoteResultTypeNone)&& // 信号未结束或未处理
            note->mStaffIndex == staffIndex &&  // partType 相等
            note->mNoteNumber == notenumber &&
            note->mStartTime - mTheta <= time &&  // 在 theta 范围之内
            note->mStartTime + mTheta >= time) {
            return note;
        }
    }
    return NULL;
}
/**
 * 根据 channel 获取staff index
 * 1 对应 左手
 * 2 对应 右手
 * 3 对应 脚
 */
int ComparePooling::getStaffIndexByChannel(int channel) {
    int staff = 0;
    switch (this->mMode) {
        case InstrumentMode_Piano:
            staff = channel;
            break;
        case InstrumentMode_DoubleKey:
            if (0 == channel || 1 == channel|| 2 == channel|| 7 == channel) {
                staff = 1;
            }else if (3 == channel || 4 == channel){
                staff = 2;
            }else if (5 == channel || 6 == channel){
                staff = 3;
            }
            break;
        default:
            break;
    }
    return staff;
}
ComparePooling::~ComparePooling(){
    std::cout<< "*** ComparePooling Release";
    for(std::vector<MidiSignal *>::iterator it = this->mTempSignals.begin(); it != this->mTempSignals.end(); it ++){
        if (NULL != *it) {
            delete *it;
            *it = NULL;
        }
    }
    
    for(std::vector<CNote *>::iterator it = this->mNotes.begin(); it != this->mNotes.end(); it ++){
        if (NULL != *it) {
            delete *it;
            *it = NULL;
        }
    }
}

