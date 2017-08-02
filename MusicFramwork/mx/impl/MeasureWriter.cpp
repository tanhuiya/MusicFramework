// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#include "../impl/MeasureWriter.h"
#include "../core/elements/PartwiseMeasure.h"
#include "../impl/Converter.h"
#include "../core/elements/Backup.h"
#include "../core/elements/Forward.h"
#include "../core/elements/MusicDataGroup.h"
#include "../core/elements/MusicDataChoice.h"
#include "../core/elements/Properties.h"
#include "../core/elements/Clef.h"
#include "../core/elements/Directive.h"
#include "../core/elements/Divisions.h"
#include "../core/elements/EditorialGroup.h"
#include "../core/elements/Footnote.h"
#include "../core/elements/Instruments.h"
#include "../core/elements/Key.h"
#include "../core/elements/KeyChoice.h"
#include "../core/elements/TraditionalKey.h"
#include "../core/elements/Cancel.h"
#include "../core/elements/Fifths.h"
#include "../core/elements/Mode.h"
#include "../core/elements/Level.h"
#include "../core/elements/MeasureStyle.h"
#include "../core/elements/PartSymbol.h"
#include "../core/elements/StaffDetails.h"
#include "../core/elements/Staves.h"
#include "../core/elements/Time.h"
#include "../core/elements/Transpose.h"
#include "../core/elements/TimeChoice.h"
#include "../core/elements/TimeSignatureGroup.h"
#include "../core/elements/BeatType.h"
#include "../core/elements/Beats.h"
#include "../core/elements/Clef.h"
#include "../core/elements/ClefOctaveChange.h"
#include "../core/elements/Line.h"
#include "../core/elements/Sign.h"
#include "../impl/ScoreWriter.h"
#include "../core/elements/Print.h"
#include "../core/elements/LayoutGroup.h"
#include "../core/elements/MeasureLayout.h"
#include "../core/elements/MeasureNumbering.h"
#include "../core/elements/PartAbbreviationDisplay.h"
#include "../core/elements/PartNameDisplay.h"
#include "../core/elements/SystemLayout.h"
#include "../core/elements/SystemDistance.h"
#include "../core/elements/SystemDividers.h"
#include "../core/elements/SystemMargins.h"
#include "../core/elements/TopSystemDistance.h"
#include "../core/elements/LeftMargin.h"
#include "../core/elements/RightMargin.h"
#include "../core/elements/Note.h"
#include "../core/elements/Accidental.h"
#include "../core/elements/Beam.h"
#include "../core/elements/CueNoteGroup.h"
#include "../core/elements/Dot.h"
#include "../core/elements/Duration.h"
#include "../core/elements/EditorialVoiceGroup.h"
#include "../core/elements/Footnote.h"
#include "../core/elements/FullNoteGroup.h"
#include "../core/elements/FullNoteTypeChoice.h"
#include "../core/elements/GraceNoteGroup.h"
#include "../core/elements/Grace.h"
#include "../core/elements/Cue.h"
#include "../core/elements/Instrument.h"
#include "../core/elements/Level.h"
#include "../core/elements/Lyric.h"
#include "../core/elements/Notations.h"
#include "../core/elements/NoteChoice.h"
#include "../core/elements/Notehead.h"
#include "../core/elements/NoteheadText.h"
#include "../core/elements/NormalNoteGroup.h"
#include "../core/elements/Pitch.h"
#include "../core/elements/Play.h"
#include "../core/elements/Staff.h"
#include "../core/elements/Stem.h"
#include "../core/elements/Rest.h"
#include "../core/elements/Tie.h"
#include "../core/elements/TimeModification.h"
#include "../core/elements/Type.h"
#include "../core/elements/Unpitched.h"
#include "../core/elements/Voice.h"
#include "../impl/NoteWriter.h"
#include "../core/elements/Direction.h"
#include "../core/elements/DirectionType.h"
#include "../core/elements/AccordionRegistration.h"
#include "../core/elements/Bracket.h"
#include "../core/elements/Coda.h"
#include "../core/elements/Damp.h"
#include "../core/elements/DampAll.h"
#include "../core/elements/Dashes.h"
#include "../core/elements/Dynamics.h"
#include "../core/elements/Eyeglasses.h"
#include "../core/elements/HarpPedals.h"
#include "../core/elements/Image.h"
#include "../core/elements/Metronome.h"
#include "../core/elements/OctaveShift.h"
#include "../core/elements/OtherDirection.h"
#include "../core/elements/Pedal.h"
#include "../core/elements/Percussion.h"
#include "../core/elements/PrincipalVoice.h"
#include "../core/elements/Rehearsal.h"
#include "../core/elements/Scordatura.h"
#include "../core/elements/Segno.h"
#include "../core/elements/StringMute.h"
#include "../core/elements/Wedge.h"
#include "../core/elements/Words.h"
#include "../impl/DirectionWriter.h"
#include "../core/elements/Barline.h"
#include "../core/elements/BarStyle.h"
#include "../core/elements/Coda.h"
#include "../core/elements/EditorialGroup.h"
#include "../core/elements/Ending.h"
#include "../core/elements/Fermata.h"
#include "../core/elements/Repeat.h"
#include "../core/elements/Segno.h"
#include "../core/elements/WavyLine.h"

#include <limits>

namespace mx
{
    
    
    namespace core
    {
        class PartwiseMeasure;
        using PartwiseMeasurePtr = std::shared_ptr<PartwiseMeasure>;
    }

    namespace impl
    {
        MeasureWriter::MeasureWriter( const api::MeasureData& inMeasureData, const MeasureCursor& inCursor, const ScoreWriter& inScoreWriter )
        : myMeasureData{ inMeasureData }
        , myOutMeasure{ nullptr }
        , myCursor{ inCursor }
        , myPreviousCursor{ inCursor }
        , myScoreWriter{ inScoreWriter }
        , myPropertiesWriter{ nullptr }
        , myConverter{}
        , myBarlinesIter{ inMeasureData.barlines.cbegin() }
        , myBarlinesEnd{ inMeasureData.barlines.cend() }
        , myMeasureKeysIter{ inMeasureData.keys.cbegin() }
        , myMeasureKeysEnd{ inMeasureData.keys.cend() }
        {

        }


        core::PartwiseMeasurePtr MeasureWriter::getPartwiseMeasure()
        {
            myOutMeasure = core::makePartwiseMeasure();
            myPropertiesWriter = std::unique_ptr<PropertiesWriter>{ new PropertiesWriter{ myOutMeasure } };
            myCursor.reset();
            myBarlinesIter = myMeasureData.barlines.cbegin();
            myBarlinesEnd = myMeasureData.barlines.cend();
            myMeasureKeysIter = myMeasureData.keys.cbegin();
            
            writeMeasureGlobals();
            writeStaves();
            myPropertiesWriter->flushBuffer();
            writeBarlines( api::TICK_TIME_INFINITY );
            myPropertiesWriter = nullptr;
            return myOutMeasure;
        }
        
        void MeasureWriter::writeMeasureGlobals()
        {
            writeBarlines( 0 );
            auto& measureAttr = *myOutMeasure->getAttributes();
            
            if( myMeasureData.number.size() > 0 )
            {
                measureAttr.number = core::XsToken{ myMeasureData.number };
            }
            else
            {
                measureAttr.number = core::XsToken{ std::to_string( myCursor.measureIndex + 1 ) };
            }
            
            if( myMeasureData.width >= 0.0 )
            {
                measureAttr.hasWidth = true;
                measureAttr.width = core::PositiveDivisionsValue{ myMeasureData.width };
            }
            
            Converter converter;
            
            if( myMeasureData.implicit != api::Bool::unspecified )
            {
                measureAttr.hasImplicit = true;
                measureAttr.implicit = converter.convert( myMeasureData.implicit );
            }
            
            if( myScoreWriter.isStartOfSystem( myCursor.measureIndex ) )
            {
                writeSystemInfo();
            }
            
            if( myCursor.isFirstMeasureInPart )
            {
                myPropertiesWriter->writeDivisions( myCursor.getGlobalTicksPerQuarter() );
                
                if( myMeasureData.staves.size() > 1 )
                {
                    myPropertiesWriter->writeNumStaves( static_cast<int>( myMeasureData.staves.size() ) );
                }
            }
            
            if( !myMeasureData.timeSignature.isImplicit )
            {
                myPropertiesWriter->writeTime( myMeasureData.timeSignature );
            }
            
            int localStaffCounter = 0;
            for( const auto staff : myMeasureData.staves )
            {
                auto clefIter = staff.clefs.cbegin();
                auto clefEnd = staff.clefs.cend();
                while( clefIter != clefEnd && clefIter->tickTimePosition == 0 )
                {
                    int desiredStaffIndex = -1;
                    if( myCursor.getNumStaves() > 1 )
                    {
                        desiredStaffIndex = localStaffCounter;
                    }
                    myPropertiesWriter->writeClef( desiredStaffIndex, *clefIter );
                    ++clefIter;
                }
                ++localStaffCounter;
            }
            
            // TODO - these might end up in the wrong place, figure out how
            // to preserve location if the barline is not at the beginning or
            // the end
            
            while( myMeasureKeysIter != myMeasureKeysEnd && myMeasureKeysIter->tickTimePosition == 0 )
            {
                myPropertiesWriter->writeKey( -1, *myMeasureKeysIter );
                ++myMeasureKeysIter;
            }
        }


        void MeasureWriter::writeSystemInfo()
        {
            auto systemData = myScoreWriter.getSystemData( myCursor.measureIndex );
            
            const bool isSystemDataSpecified =
                   systemData.isMarginSpecified
                || systemData.isSystemDistanceSpecified
                || systemData.isTopSystemDistanceSpecified;
            
            if( !isSystemDataSpecified && !myCursor.isFirstMeasureInPart )
            {
                return;
            }
            
            auto printMdc = core::makeMusicDataChoice();
            printMdc->setChoice( core::MusicDataChoice::Choice::print );
            auto& print = *printMdc->getPrint();
            print.getAttributes()->hasNewSystem = true;
            print.getAttributes()->newSystem = core::YesNo::yes;
            auto& layoutGroup = *print.getLayoutGroup();
            myOutMeasure->getMusicDataGroup()->addMusicDataChoice(printMdc );
            if( isSystemDataSpecified )
            {
                layoutGroup.setHasSystemLayout( true );
                auto& systemLayout = *layoutGroup.getSystemLayout();

                if( systemData.isMarginSpecified )
                {
                    systemLayout.setHasSystemMargins( true );
                    auto& margins = *systemLayout.getSystemMargins();
                    margins.getLeftMargin()->setValue( core::TenthsValue{static_cast<core::DecimalType>( systemData.leftMargin ) } );
                    margins.getRightMargin()->setValue( core::TenthsValue{static_cast<core::DecimalType>( systemData.rightMargin ) } );
                }

                if( systemData.isTopSystemDistanceSpecified )
                {
                    systemLayout.setHasTopSystemDistance( true );
                    systemLayout.getTopSystemDistance()->setValue( core::TenthsValue{static_cast<core::DecimalType>( systemData.topSystemDistance ) } );
                }

                if( systemData.isSystemDistanceSpecified )
                {
                    systemLayout.setHasSystemDistance( true );
                    systemLayout.getSystemDistance()->setValue( core::TenthsValue{static_cast<core::DecimalType>( systemData.systemDistance ) } );
                }
           }
        }
        
        
        void MeasureWriter::writeStaves()
        {
            myCursor.tickTimePosition = 0;
            myCursor.staffIndex = 0;
            myCursor.voiceIndex = 0;
            myPreviousCursor = myCursor;
            for( const auto& staff : myMeasureData.staves )
            {
                // auto clefIter = staff.clefs.cbegin();
                // auto clefEnd = staff.clefs.cend();
                
                writeVoices( staff );
                ++myCursor.staffIndex;
            }
        }
        
        
        void MeasureWriter::writeVoices( const api::StaffData& inStaff )
        {
            auto clefIter = inStaff.clefs.cbegin();
            const auto clefEnd = inStaff.clefs.cend();
            while( clefIter != clefEnd && clefIter->tickTimePosition == 0 ) { ++clefIter; }
//            auto myMeasureKeysIter = myMeasureKeys.cbegin();
//            auto myMeasureKeysEnd = myMeasureKeys.cend();
            auto staffKeyIter = inStaff.keys.cbegin();
            auto staffKeyEnd = inStaff.keys.cend();
            auto directionIter = inStaff.directions.cbegin();
            auto directionEnd = inStaff.directions.cend();
            
            for( const auto& voice : inStaff.voices )
            {
                int currentChordTickPosition = -1;
                int previousChordTickPosition = -2;
                myCursor.voiceIndex = voice.first;
                auto noteIter = voice.second.notes.cbegin();
                auto noteEnd = voice.second.notes.cend();
                for( ; noteIter != noteEnd; ++noteIter )
                {
                    // TODO - this is too simplistic. If we have two chords in a row then this
                    // will be incorrect
                    bool isStartOfChord = false;
                    myCursor.isChordActive = noteIter->isChord;
                    
                    if ( noteIter->isChord )
                    {
                        currentChordTickPosition = noteIter->tickTimePosition;
                        if( currentChordTickPosition != previousChordTickPosition )
                        {
                            isStartOfChord = true;
                        }
                        previousChordTickPosition = currentChordTickPosition;
                    }
                    
                    const auto& apiNote = *noteIter;
                    writeForwardOrBackupIfNeeded( apiNote );
                    
                    if( myMeasureKeysIter != myMeasureKeysEnd )
                    {
                        if( myMeasureKeysIter->tickTimePosition <= myCursor.tickTimePosition )
                        {
                            myPropertiesWriter->writeKey( -1, *myMeasureKeysIter );
                            ++myMeasureKeysIter;
                        }
                    }
                    
                    while( clefIter != clefEnd && clefIter->tickTimePosition <= myCursor.tickTimePosition )
                    {
                        myPropertiesWriter->writeClef( myCursor.staffIndex, *clefIter );
                        ++clefIter;
                    }
                    for( ; staffKeyIter != staffKeyEnd && staffKeyIter->tickTimePosition <= myCursor.tickTimePosition; ++staffKeyIter )
                    {
                        myPropertiesWriter->writeKey( myCursor.staffIndex, *staffKeyIter );
                    }
                    myPropertiesWriter->flushBuffer();
                    
                    for( ; directionIter != directionEnd && directionIter->tickTimePosition <= myCursor.tickTimePosition; ++directionIter )
                    {
                        if( isDirectionDataEmpty( *directionIter ) )
                        {
                            continue;
                        }
                        auto directionMdc = core::makeMusicDataChoice();
                        directionMdc->setChoice( core::MusicDataChoice::Choice::direction );
                        DirectionWriter directionWriter{ *directionIter, myCursor };
                        directionMdc->setDirection( directionWriter.getDirection() );
                        myOutMeasure->getMusicDataGroup()->addMusicDataChoice( directionMdc );
                    }
                    
                    auto mdc = core::makeMusicDataChoice();
                    mdc->setChoice( core::MusicDataChoice::Choice::note );
                    NoteWriter writer{ apiNote, myCursor, myScoreWriter, myPreviousCursor.isChordActive };
                    mdc->setNote( writer.getNote(isStartOfChord) );
                    myOutMeasure->getMusicDataGroup()->addMusicDataChoice( mdc );
                    advanceCursorIfNeeded( apiNote, noteIter, noteEnd, isStartOfChord );
                } // foreach note
            } // foreach voice
            
            bool areClefsRemaining = clefIter != clefEnd;
            bool areMeasureKeysRemaining = myMeasureKeysIter != myMeasureKeysEnd;
            bool areStaffKeysRemaining = staffKeyIter != staffKeyEnd;
            
            if( areClefsRemaining || areMeasureKeysRemaining || areStaffKeysRemaining )
            {
                for( ; clefIter != inStaff.clefs.cend(); ++clefIter )
                {
                    MX_ASSERT( clefIter != inStaff.clefs.cend() );
                    api::ClefData clef = *clefIter;
                    myPropertiesWriter->writeClef( myCursor.staffIndex, clef );
                }
                
                if( myMeasureKeysIter != myMeasureKeysEnd )
                {
                    if( myMeasureKeysIter->tickTimePosition )
                    {
                        myPropertiesWriter->writeKey( -1, *myMeasureKeysIter );
                        ++myMeasureKeysIter;
                    }
                }
                
                for( ; staffKeyIter != staffKeyEnd; ++staffKeyIter )
                {
                    myPropertiesWriter->writeKey( myCursor.staffIndex, *staffKeyIter );
                }
                myPropertiesWriter->flushBuffer();
            }
            
            for( ; directionIter != directionEnd; ++directionIter )
            {
                if( isDirectionDataEmpty( *directionIter ) )
                {
                    continue;
                }
                auto directionMdc = core::makeMusicDataChoice();
                directionMdc->setChoice( core::MusicDataChoice::Choice::direction );
                DirectionWriter directionWriter{ *directionIter, myCursor };
                directionMdc->setDirection( directionWriter.getDirection() );
                myOutMeasure->getMusicDataGroup()->addMusicDataChoice( directionMdc );
            }
            
        } // func writeVoices
        
        
        void MeasureWriter::writeForwardOrBackupIfNeeded( const api::NoteData& currentNote )
        {
            myPropertiesWriter->flushBuffer();
            const int timeDifference = currentNote.tickTimePosition - myCursor.tickTimePosition;
            if( timeDifference < 0 )
            {
                backup( -1 * timeDifference );
            }
            else if ( timeDifference > 0 )
            {
                forward( timeDifference );
            }
        }
        
        
        void MeasureWriter::backup( const int ticks )
        {
            auto backupMdc = core::makeMusicDataChoice();
            backupMdc->setChoice( core::MusicDataChoice::Choice::backup );
            auto backup = backupMdc->getBackup();
            backup->getDuration()->setValue( core::PositiveDivisionsValue{ static_cast<core::DecimalType>( ticks ) } );
            myOutMeasure->getMusicDataGroup()->addMusicDataChoice( backupMdc );
            myCursor.tickTimePosition -= ticks;
        }
        
        
        void MeasureWriter::forward( const int ticks )
        {
            auto forwardMdc = core::makeMusicDataChoice();
            forwardMdc->setChoice( core::MusicDataChoice::Choice::forward );
            auto forward = forwardMdc->getForward();
            forward->getDuration()->setValue( core::PositiveDivisionsValue{ static_cast<core::DecimalType>( ticks ) } );
            myOutMeasure->getMusicDataGroup()->addMusicDataChoice( forwardMdc );
            myCursor.tickTimePosition += ticks;
        }
        
        
        void MeasureWriter::advanceCursorIfNeeded( const api::NoteData& currentNote, NoteIter inNoteIter, const NoteIter inEndIter, bool isStartOfChord )
        {
            MX_UNUSED(currentNote);
            MX_UNUSED(isStartOfChord);
            
            ++inNoteIter;
            if( inNoteIter != inEndIter )
            {
                myCursor.tickTimePosition = inNoteIter->tickTimePosition;
            }
            else
            {
                myCursor.tickTimePosition += currentNote.durationData.durationTimeTicks;
                myCursor.isChordActive = true;
            }

            myPreviousCursor = myCursor;
        }
        
        
        void MeasureWriter::writeBarlines( int tickTimePosition )
        {
            for( ; myBarlinesIter != myBarlinesEnd && myBarlinesIter->tickTimePosition <= tickTimePosition; ++myBarlinesIter )
            {
                auto mdc = core::makeMusicDataChoice();
                mdc->setChoice( core::MusicDataChoice::Choice::barline );
                auto barlineElement = mdc->getBarline();
                if( myBarlinesIter->barlineType != api::BarlineType::unspecified && myBarlinesIter->barlineType != api::BarlineType::unsupported )
                {
                    barlineElement->setHasBarStyle( true );
                    barlineElement->getBarStyle()->setValue( myConverter.convert( myBarlinesIter->barlineType ) );
                }
                if( myBarlinesIter->location != api::HorizontalAlignment::unspecified )
                {
                    barlineElement->getAttributes()->hasLocation = true;
                    barlineElement->getAttributes()->location = myConverter.convertBarlinePlacement( myBarlinesIter->location );
                }
                myOutMeasure->getMusicDataGroup()->addMusicDataChoice( mdc );
            }
        }
    }
}
