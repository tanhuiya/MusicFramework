// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#include "../impl/PartReader.h"
#include "../core/elements/AccidentalText.h"
#include "../core/elements/Direction.h"
#include "../core/elements/DisplayText.h"
#include "../core/elements/DisplayTextOrAccidentalText.h"
#include "../core/elements/Elevation.h"
#include "../core/elements/Ensemble.h"
#include "../core/elements/Forward.h"
#include "../core/elements/Harmony.h"
#include "../core/elements/InstrumentAbbreviation.h"
#include "../core/elements/InstrumentName.h"
#include "../core/elements/InstrumentSound.h"
#include "../core/elements/MidiBank.h"
#include "../core/elements/MidiChannel.h"
#include "../core/elements/MidiDevice.h"
#include "../core/elements/MidiDeviceInstrumentGroup.h"
#include "../core/elements/MidiInstrument.h"
#include "../core/elements/MidiName.h"
#include "../core/elements/MidiProgram.h"
#include "../core/elements/MidiUnpitched.h"
#include "../core/elements/MusicDataChoice.h"
#include "../core/elements/MusicDataGroup.h"
#include "../core/elements/Note.h"
#include "../core/elements/Note.h"
#include "../core/elements/Pan.h"
#include "../core/elements/PartAbbreviation.h"
#include "../core/elements/PartName.h"
#include "../core/elements/PartNameDisplay.h"
#include "../core/elements/PartwisePart.h"
#include "../core/elements/Properties.h"
#include "../core/elements/ScoreInstrument.h"
#include "../core/elements/ScorePart.h"
#include "../core/elements/Solo.h"
#include "../core/elements/SoloOrEnsembleChoice.h"
#include "../core/elements/Staff.h"
#include "../core/elements/Staves.h"
#include "../core/elements/VirtualInstrument.h"
#include "../core/elements/VirtualLibrary.h"
#include "../core/elements/VirtualName.h"
#include "../core/elements/Volume.h"
#include "../impl/MeasureReader.h"
#include "../impl/PrintFunctions.h"

#include <sstream>

namespace mx
{
    namespace impl
    {
        PartReader::PartReader( const core::ScorePart& inScorePart, const core::PartwisePart& inPartwisePartRef, int globalTicksPerMeasure )
        : myPartwisePart{ inPartwisePartRef }
        , myScorePart{ inScorePart }
        , myNumStaves{ -1 }
        , myGlobalTicksPerMeasure{ globalTicksPerMeasure }
        {
            const auto ppId = myPartwisePart.getAttributes()->id.getValue();
            const auto spId = myScorePart.getAttributes()->id.getValue();
            if( ppId != spId )
            {
                MX_THROW( "the partwise-part id must match the score-part id" );
            }
            myNumStaves = calculateNumStaves();
        }
        
        
        api::PartData PartReader::getPartData() const
        {
            std::lock_guard<std::mutex> lock{ myMutex };
            myOutPartData = api::PartData{};
            parseScorePart();
            
            MeasureCursor currentCursor{ myNumStaves, myGlobalTicksPerMeasure };
            MeasureCursor previousCursor{ myNumStaves, myGlobalTicksPerMeasure };
            for( const auto& mxMeasurePtr : myPartwisePart.getPartwiseMeasureSet() )
            {
                const auto& mxMeasure = *mxMeasurePtr;
                MeasureReader reader{ mxMeasure, currentCursor, previousCursor };
                auto measureData = reader.getMeasureData();
                currentCursor.timeSignature = measureData.timeSignature;
                myOutPartData.measures.emplace_back( std::move( measureData ) );
                ++currentCursor.measureIndex;
                previousCursor = currentCursor;
            }
            
            api::PartData tempReturn = std::move( myOutPartData );
            myOutPartData = api::PartData{};
            return tempReturn;
        }
        
        int PartReader::calculateNumStaves() const
        {
            int numStaves = 1;
            
            for( const auto& measure : myPartwisePart.getPartwiseMeasureSet() )
            {
                for( const auto& mdc : measure->getMusicDataGroup()->getMusicDataChoiceSet() )
                {
                    const auto choiceType = mdc->getChoice();
                    
                    switch ( choiceType )
                    {
                        case core::MusicDataChoice::Choice::note:
                        {
                            updateNumStaves( *mdc->getNote(), numStaves );
                            break;
                        }

                        case core::MusicDataChoice::Choice::forward:
                        {
                            updateNumStaves( *mdc->getForward(), numStaves );
                            break;
                        }
                        case core::MusicDataChoice::Choice::direction:
                        {
                            updateNumStaves( *mdc->getDirection(), numStaves );
                            break;
                        }
                        case core::MusicDataChoice::Choice::properties:
                        {
                            const auto& properties = *mdc->getProperties();
                            if( properties.getHasStaves() )
                            {
                                int temp = properties.getStaves()->getValue().getValue();
                                if( temp > numStaves )
                                {
                                    numStaves = temp;
                                }
                            }
                            break;
                        }
                        case core::MusicDataChoice::Choice::harmony:
                        {
                            updateNumStaves( *mdc->getHarmony(), numStaves );
                            break;
                        }
                        // these are not specific to a staff number
                        case core::MusicDataChoice::Choice::backup:
                        case core::MusicDataChoice::Choice::figuredBass:
                        case core::MusicDataChoice::Choice::print:
                        case core::MusicDataChoice::Choice::sound:
                        case core::MusicDataChoice::Choice::barline:
                        case core::MusicDataChoice::Choice::grouping:
                        case core::MusicDataChoice::Choice::link:
                        case core::MusicDataChoice::Choice::bookmark:
                        default: break;
                    }
                }
            }
            
            return numStaves;
        }
        
        
        void PartReader::parseScorePart() const
        {
            myOutPartData.uniqueId = myScorePart.getAttributes()->id.getValue();
            myOutPartData.name = myScorePart.getPartName()->getValue().getValue();
            
            if( myScorePart.getHasPartNameDisplay() )
            {
                myOutPartData.displayName = extractDisplayText( myScorePart.getPartNameDisplay()->getDisplayTextOrAccidentalTextSet() );
            }
            
            if( myScorePart.getHasPartAbbreviation() )
            {
                myOutPartData.abbreviation = myScorePart.getPartAbbreviation()->getValue().getValue();
            }
            
            if( myScorePart.getHasPartAbbreviationDisplay() )
            {
                myOutPartData.displayAbbreviation = extractDisplayText( myScorePart.getPartNameDisplay()->getDisplayTextOrAccidentalTextSet() );
            }
            
            if( myScorePart.getScoreInstrumentSet().size() > 0 )
            {
                parseScoreInstrument( **myScorePart.getScoreInstrumentSet().cbegin() );
            }
            
            if( myScorePart.getMidiDeviceInstrumentGroupSet().size() > 0 )
            {
                parseMidiDeviceInstrumentGroup( **myScorePart.getMidiDeviceInstrumentGroupSet().cbegin() );
            }
        }
        
        
        std::string PartReader::extractDisplayText( const core::DisplayTextOrAccidentalTextSet& items ) const
        {
            std::stringstream ss;
            for( const auto& c : items )
            {
                if( c->getChoice() == core::DisplayTextOrAccidentalText::Choice::displayText )
                {
                    ss << c->getDisplayText()->getValue().getValue();
                }
                else if( c->getChoice() == core::DisplayTextOrAccidentalText::Choice::accidentalText )
                {
                    if( c->getAccidentalText()->getValue() == core::AccidentalValue::flat )
                    {
                        ss << "b"; // TODO - support accidental text correctly
                    }
                    else if( c->getAccidentalText()->getValue() == core::AccidentalValue::flat )
                    {
                        ss << "#";
                    }
                }
            }
            return ss.str();
        }
        
        
        void PartReader::parseScoreInstrument( const core::ScoreInstrument& scoreInstrument ) const
        {
            myOutPartData.instrumentData.uniqueId = scoreInstrument.getAttributes()->id.getValue();
            myOutPartData.instrumentData.name = scoreInstrument.getInstrumentName()->getValue().getValue();
            
            if( scoreInstrument.getHasInstrumentAbbreviation() )
            {
                myOutPartData.instrumentData.abbreviation = scoreInstrument.getInstrumentAbbreviation()->getValue().getValue();
            }
            
            if( scoreInstrument.getHasSoloOrEnsembleChoice() )
            {
                const auto& soec = *scoreInstrument.getSoloOrEnsembleChoice();
                if( soec.getChoice() == core::SoloOrEnsembleChoice::Choice::ensemble )
                {
                    myOutPartData.instrumentData.soloOrEnsemble = api::SoloOrEnsemble::ensemble;
                }
                else if( soec.getChoice() == core::SoloOrEnsembleChoice::Choice::solo )
                {
                    myOutPartData.instrumentData.soloOrEnsemble = api::SoloOrEnsemble::solo;
                }
            }
            
            if( scoreInstrument.getHasInstrumentSound() )
            {
                myOutPartData.instrumentData.sound = scoreInstrument.getInstrumentSound()->getValue().getValue();
            }
            
            if( scoreInstrument.getHasVirtualInstrument() )
            {
                parseVirtualInstrument( *scoreInstrument.getVirtualInstrument() );
            }
        }
        
        
        void PartReader::parseVirtualInstrument( const core::VirtualInstrument& virtualInstrument ) const
        {
            if( virtualInstrument.getHasVirtualName() )
            {
                myOutPartData.instrumentData.midiData.virtualName = virtualInstrument.getVirtualName()->getValue().getValue();
            }
            
            if( virtualInstrument.getHasVirtualLibrary() )
            {
                myOutPartData.instrumentData.midiData.virtualLibrary = virtualInstrument.getVirtualLibrary()->getValue().getValue();
            }
        }
        
        
        void PartReader::parseMidiDeviceInstrumentGroup( const core::MidiDeviceInstrumentGroup& grp ) const
        {
            if( grp.getHasMidiDevice() )
            {
                myOutPartData.instrumentData.midiData.device = grp.getMidiDevice()->getValue().getValue();
            }
            
            if( grp.getHasMidiInstrument() )
            {
                parseMidiInstrument( *grp.getMidiInstrument() );
            }
        }
        
        void PartReader::parseMidiInstrument( const core::MidiInstrument& inst ) const
        {
            if( inst.getHasMidiChannel() )
            {
                myOutPartData.instrumentData.midiData.channel = inst.getMidiChannel()->getValue().getValue();
            }
            
            if( inst.getHasMidiName() )
            {
                myOutPartData.instrumentData.midiData.name = inst.getMidiName()->getValue().getValue();
            }
            
            if( inst.getHasMidiChannel() )
            {
                myOutPartData.instrumentData.midiData.channel = inst.getMidiChannel()->getValue().getValue();
            }
            
            if( inst.getHasMidiBank() )
            {
                myOutPartData.instrumentData.midiData.bank = inst.getMidiBank()->getValue().getValue();
            }
            
            if( inst.getHasMidiProgram() )
            {
                myOutPartData.instrumentData.midiData.program = inst.getMidiProgram()->getValue().getValue();
            }
            
            if( inst.getHasMidiUnpitched() )
            {
                myOutPartData.instrumentData.midiData.unpitched = inst.getMidiUnpitched()->getValue().getValue();
            }
            
            if( inst.getHasVolume() )
            {
                myOutPartData.instrumentData.midiData.isVolumeSpecified = true;
                myOutPartData.instrumentData.midiData.volume = inst.getVolume()->getValue().getValue();
            }
            
            if( inst.getHasPan() )
            {
                myOutPartData.instrumentData.midiData.isPanSpecified = true;
                myOutPartData.instrumentData.midiData.pan = inst.getPan()->getValue().getValue();
            }
            
            if( inst.getHasElevation() )
            {
                myOutPartData.instrumentData.midiData.isElevationSpecified = true;
                myOutPartData.instrumentData.midiData.elevation = inst.getElevation()->getValue().getValue();
            }
        }
    }
}
