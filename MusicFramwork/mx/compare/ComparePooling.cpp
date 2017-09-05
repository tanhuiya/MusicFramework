//
//  ComparePooling.cpp
//  musicXML
//
//  Created by tanhui on 2017/8/24.
//  Copyright © 2017年 tanhui. All rights reserved.
//

#include "ComparePooling.h"

#pragma mark MidiSignal Class

MidiSignal::MidiSignal(int channel, double timeStamp, int noteNumber, ECNoteType type) {
    this->mTimeStamp = timeStamp;
    this->mNoteNumber = noteNumber;
    this->mType = type;
    this->mChannel = channel;
}

#pragma mark CNote Class

CNote::CNote(int channel, int noteNumber,  double startTime, int duration ,int mStaff,int xPos){
    this->mNoteNumber = noteNumber;
    this->mChannel = channel;
    this->mDuration = duration;
    this->mStartTime = startTime;
    this->isMapped = false;
    this->mType = ECNoteType::CNoteTypeNone;
    this->mStaff = mStaff;
    this->mXpos = xPos;
}

#pragma mark CResult Class
CResult::CResult(){
    this->rate = 0.0;
    this->mTimeOutNotes = std::vector<CNote*>();
    this->mErrorNotes = std::vector<CNote*>();
    this->mShortNotes = std::vector<CNote*>();
    this->mLostNotes = std::vector<CNote*>();
    this->mCorrectNotes = std::vector<CNote*>();
}

#pragma mark ComparePooling Class
ComparePooling::ComparePooling(std::vector<CNote *> notes, double theta) {
    this->mNotes = notes;
    this->mTheta = theta;
    this->mTempSignals = std::vector<MidiSignal*>();
}

void ComparePooling::resetPool(){
    for (auto noteIndex = this->mNotes.cbegin(); noteIndex != this->mNotes.cend(); noteIndex++) {
        CNote* note = (*noteIndex);
        note->isMapped = false;
        note->mType = ECNoteType::CNoteTypeNone;
    }
}

std::vector<CNote*> ComparePooling::receiveCNotes(std::vector<MidiSignal*> signals, double currentTime) {
    std::vector<CNote*> retVec = std::vector<CNote*>();
    
    for (auto it = signals.cbegin(); it != signals.cend(); it++ ) {
        CNote* note = this->getNearestCNote( *it);
        if (!note && (*it)->mType == ECNoteType::CNoteTypeOn) {
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
        if (note->mType == ECNoteType::CNoteTypeNone && !note->isMapped && note->mStartTime + 2 * this->mTheta < currentTime) {
            note->isMapped = true;
            note->mType = ECNoteType::CNoteTypeLost;
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
    for (auto noteIndex = this->mNotes.cbegin(); noteIndex != this->mNotes.cend(); noteIndex++) {
        CNote* note = (*noteIndex);
        if(note->mType == ECNoteType::CNoteTypeError){
            result->mErrorNotes.push_back(note);
        }else if (note->mType == ECNoteType::CNoteTypeShort){
            result->mShortNotes.push_back(note);
        }else if(note->mType == ECNoteType::CNoteTypeTimeout){
            result->mTimeOutNotes.push_back(note);
        }else if(note->mType == ECNoteType::CNoteTypeLost){
            result->mLostNotes.push_back(note);
        }else {
            result->mCorrectNotes.push_back(note);
        }
    }
    result->rate = result->mCorrectNotes.size() * 1.0 / this->mNotes.size();
    return result;
}

CNote* ComparePooling::getErrorCNote(MidiSignal* signal) {
    CNote* ret = NULL;
    for (auto noteIndex = this->mNotes.cbegin(); noteIndex != this->mNotes.cend(); noteIndex++) {
        CNote* note = (*noteIndex);
        if (note->mType == ECNoteType::CNoteTypeNone && !note->isMapped && abs(note->mStartTime - signal->mTimeStamp)  <= this->mTheta) {
            note->isMapped = true;
            note->mType = ECNoteType::CNoteTypeError;
            ret = note;
            break;// 避免将多个同时间的note置为error
        }
    }
    return ret;
}


CNote* ComparePooling::getNearestCNote(MidiSignal* signal) {
    CNote* ret = NULL;
    
    if (signal->mType == ECNoteType::CNoteTypeOn) { // 处理noteOn 信号
        CNote* note = this->getCNoteBy(signal->mChannel, signal->mNoteNumber, signal->mTimeStamp);
        if (note) {
            ret = note;
            ret->mType = ECNoteType::CNoteTypeOn;
            this->mTempSignals.push_back(signal);// 将 noteOn 信号储存起来，等待noteOff信号
        }
    }else {
        for (std::vector<MidiSignal*>::const_iterator tempSignalIndex = this->mTempSignals.cbegin(); tempSignalIndex != this->mTempSignals.cend(); tempSignalIndex++) {
            if ((*tempSignalIndex)->mNoteNumber == signal->mNoteNumber &&
                (*tempSignalIndex)->mChannel == signal->mChannel) {
                ret = this->getCNoteBy(signal->mChannel, signal->mNoteNumber, (*tempSignalIndex)->mTimeStamp);
                if (ret) {
                    if (signal->mTimeStamp-(*tempSignalIndex)->mTimeStamp < ret->mDuration - this->mTheta) {
                        // todo:暂时不处理时间长短
                        ret->mType = ECNoteType::CNoteTypeWhole; // 按键时间太短
                    }else if (signal->mTimeStamp-(*tempSignalIndex)->mTimeStamp > ret->mDuration + this->mTheta) {
                        ret->mType = ECNoteType::CNoteTypeWhole; // 按键时间太长
                    }else{
                        ret->mType = ECNoteType::CNoteTypeWhole;
                    }
                    ret->isMapped = true;
                    this->mTempSignals.erase(tempSignalIndex);
                }
                break;
            }
        }
    }
    return ret;
}

CNote* ComparePooling::getCNoteBy(int channel,int notenumber, int time ) {
    for (auto noteIndex = this->mNotes.cbegin(); noteIndex != this->mNotes.cend(); noteIndex++ ) {
        CNote* note = * noteIndex;
        if (!note->isMapped && // 没有被匹配
            note->mChannel == channel &&  // channel 相等
            note->mNoteNumber == notenumber &&
            note->mStartTime - mTheta <= time &&  // 在 theta 范围之内
            note->mStartTime + mTheta >= time) {
            return note;
        }
    }
    return NULL;
}
