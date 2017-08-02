// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License


#include "../impl/DirectionWriter.h"
#include "../api/BarlineData.h"
#include "../core/elements/AccordionRegistration.h"
#include "../core/elements/BeatUnit.h"
#include "../core/elements/BeatUnitDot.h"
#include "../core/elements/BeatUnitGroup.h"
#include "../core/elements/BeatUnitPer.h"
#include "../core/elements/BeatUnitPerOrNoteRelationNoteChoice.h"
#include "../core/elements/Bracket.h"
#include "../core/elements/Bracket.h"
#include "../core/elements/Coda.h"
#include "../core/elements/Damp.h"
#include "../core/elements/DampAll.h"
#include "../core/elements/Dashes.h"
#include "../core/elements/Direction.h"
#include "../core/elements/DirectionType.h"
#include "../core/elements/Dynamics.h"
#include "../core/elements/EditorialVoiceDirectionGroup.h"
#include "../core/elements/Eyeglasses.h"
#include "../core/elements/Footnote.h"
#include "../core/elements/HarpPedals.h"
#include "../core/elements/Image.h"
#include "../core/elements/Level.h"
#include "../core/elements/Metronome.h"
#include "../core/elements/OctaveShift.h"
#include "../core/elements/Offset.h"
#include "../core/elements/OtherDirection.h"
#include "../core/elements/Pedal.h"
#include "../core/elements/Percussion.h"
#include "../core/elements/PerMinute.h"
#include "../core/elements/PerMinuteOrBeatUnitChoice.h"
#include "../core/elements/PrincipalVoice.h"
#include "../core/elements/Rehearsal.h"
#include "../core/elements/Scordatura.h"
#include "../core/elements/Segno.h"
#include "../core/elements/Sound.h"
#include "../core/elements/Staff.h"
#include "../core/elements/StringMute.h"
#include "../core/elements/Voice.h"
#include "../core/elements/Wedge.h"
#include "../core/elements/Words.h"
#include "../impl/DynamicsWriter.h"
#include "../impl/LineFunctions.h"
#include "../impl/MarkDataFunctions.h"
#include "../impl/SpannerFunctions.h"

namespace mx
{
    namespace impl
    {
        DirectionWriter::DirectionWriter( const api::DirectionData& inDirectionData, const Cursor& inCursor )
        : myDirectionData{ inDirectionData }
        , myCursor{ inCursor }
        , myOutDirectionPtr{ nullptr }
        , myConverter{}
        , myPlacements{}
        {
            
        }
        
        core::DirectionPtr DirectionWriter::getDirection()
        {
            myOutDirectionPtr = core::makeDirection();
            myPlacements.clear();
            myIsFirstDirectionTypeAdded = false;
            auto& directionAttributes = *myOutDirectionPtr->getAttributes();
            
            if( myDirectionData.placement != api::Placement::unspecified )
            {
                directionAttributes.hasPlacement = true;
                directionAttributes.placement = myConverter.convert( myDirectionData.placement );
            }
            
            if( myDirectionData.isStaffValueSpecified || myCursor.staffIndex != 0 )
            {
                myOutDirectionPtr->setHasStaff( true );
                myOutDirectionPtr->getStaff()->setValue( core::StaffNumber{ myCursor.staffIndex + 1 } );
            }
            
            if( myDirectionData.isOffsetSpecified )
            {
                myOutDirectionPtr->setHasOffset( true );
                myOutDirectionPtr->getOffset()->setValue( core::DivisionsValue{ static_cast<core::DecimalType>( myDirectionData.offset ) } );
                if( myDirectionData.offsetSound != api::Bool::unspecified )
                {
                    myOutDirectionPtr->getOffset()->getAttributes()->hasSound = true;
                    myOutDirectionPtr->getOffset()->getAttributes()->sound = myConverter.convert( myDirectionData.offsetSound );
                }
            }
            
            for( const auto& mark : myDirectionData.marks )
            {
                // TODO - skip marks that aren't of the correct type (i.e. direction marks)
                // if !isDirection( mark ) continue;
                if( isMarkDynamic( mark.markType ) )
                {
                    auto directionTypePtr = core::makeDirectionType();
                    this->addDirectionType( directionTypePtr );
                    directionTypePtr->setChoice( core::DirectionType::Choice::dynamics );
                    DynamicsWriter dynamicsWriter{ mark, myCursor };
                    MX_ASSERT( directionTypePtr->getDynamicsSet().size() == 1 );
                    directionTypePtr->addDynamics( dynamicsWriter.getDynamics() );
                    directionTypePtr->removeDynamics( directionTypePtr->getDynamicsSet().cbegin() );
                }
            }
            
            for( const auto& wedgeStart : myDirectionData.wedgeStarts )
            {
                auto wedgeStartDirectionTypePtr = core::makeDirectionType();
                wedgeStartDirectionTypePtr->setChoice( core::DirectionType::Choice::wedge );
                this->addDirectionType( wedgeStartDirectionTypePtr );
                auto wedgePtr = wedgeStartDirectionTypePtr->getWedge();
                auto& attr = *wedgePtr->getAttributes();

                if( wedgeStart.wedgeType != api::WedgeType::unspecified )
                {
                    attr.type = myConverter.convert( wedgeStart.wedgeType );
                }
                
                if( wedgeStart.isSpreadSpecified )
                {
                    attr.hasSpread = true;
                    attr.spread = core::DivisionsValue{ static_cast<core::DecimalType>( wedgeStart.spread ) };
                }
                
                auto& attributes = *wedgePtr->getAttributes();
                
                setAttributesFromPositionData( wedgeStart.positionData, attributes );
                setAttributesFromLineData(wedgeStart.lineData, attributes);
                setAttributesFromColorData(wedgeStart.colorData, attributes);
            }
            
            for( const auto& wedgeStop : myDirectionData.wedgeStops )
            {
                auto wedgeStartDirectionTypePtr = core::makeDirectionType();
                wedgeStartDirectionTypePtr->setChoice( core::DirectionType::Choice::wedge );
                this->addDirectionType( wedgeStartDirectionTypePtr );
                auto wedgePtr = wedgeStartDirectionTypePtr->getWedge();
                auto& attr = *wedgePtr->getAttributes();
                attr.type = core::WedgeType::stop;
                
                if( wedgeStop.isSpreadSpecified )
                {
                    attr.hasSpread = true;
                    attr.spread = core::DivisionsValue{ static_cast<core::DecimalType>( wedgeStop.spread ) };
                }
                auto& attributes = *wedgePtr->getAttributes();
                setAttributesFromPositionData( wedgeStop.positionData, attributes );
            }
            
            for( const auto& ottavaStart : myDirectionData.ottavaStarts )
            {
                auto oStartDir = core::makeDirectionType();
                this->addDirectionType( oStartDir );
                oStartDir->setChoice( core::DirectionType::Choice::octaveShift );
                auto oStart = oStartDir->getOctaveShift();
                auto& attr = *oStart->getAttributes();
                impl::setAttributesFromLineData( ottavaStart.spannerStart.lineData, attr );
                
                attr.hasSize = true;
                
                switch( ottavaStart.ottavaType )
                {
                    case api::OttavaType::o15ma:
                    {
                        attr.type = core::UpDownStopContinue::up;
                        attr.size = core::PositiveInteger{ 15 };
                        break;
                    }
                    case api::OttavaType::o15mb:
                    {
                        attr.type = core::UpDownStopContinue::down;
                        attr.size = core::PositiveInteger{ 15 };
                        break;
                    }
                    case api::OttavaType::o8va:
                    {
                        attr.type = core::UpDownStopContinue::up;
                        attr.size = core::PositiveInteger{ 8 };
                        break;
                    }
                    case api::OttavaType::o8vb:
                    {
                        attr.type = core::UpDownStopContinue::down;
                        attr.size = core::PositiveInteger{ 8 };
                        break;
                    }
                    default: break;
                }
            }
            
            for( const auto& ottavaStop : myDirectionData.ottavaStops )
            {
                auto oStopDir = core::makeDirectionType();
                this->addDirectionType( oStopDir );
                oStopDir->setChoice( core::DirectionType::Choice::octaveShift );
                auto oStart = oStopDir->getOctaveShift();
                auto& attr = *oStart->getAttributes();
                attr.type = core::UpDownStopContinue::stop;
                MX_UNUSED( ottavaStop );
            }
            
            for( const auto& item : myDirectionData.bracketStarts )
            {
                auto outDirType = core::makeDirectionType();
                this->addDirectionType( outDirType );
                outDirType->setChoice( core::DirectionType::Choice::bracket );
                auto outElement = outDirType->getBracket();
                auto& attr = *outElement->getAttributes();
                setAttributesFromSpannerStart( item, attr );
            }

            for( const auto& tempo : myDirectionData.tempos )
            {
                if( tempo.tempoType != api::TempoType::beatsPerMinute )
                {
                    MX_THROW( "Only api::TempoType::beatsPerMinute is supported, others are not implemented" );
                }
                
                auto outDirType = core::makeDirectionType();
                this->addDirectionType( outDirType );
                outDirType->setChoice( core::DirectionType::Choice::metronome );
                auto outElement = outDirType->getMetronome();
                auto choice = outElement->getBeatUnitPerOrNoteRelationNoteChoice();
                choice->setChoice( core::BeatUnitPerOrNoteRelationNoteChoice::Choice::beatUnitPer );
                auto bpm = choice->getBeatUnitPer();
                auto beatUnitGroup = bpm->getBeatUnitGroup();
                auto beatUnit = beatUnitGroup->getBeatUnit();
                Converter converter;
                beatUnit->setValue(converter.convert(tempo.beatsPerMinute.durationName));
                
                for( int d = 0; d < tempo.beatsPerMinute.dots; ++d )
                {
                    beatUnitGroup->addBeatUnitDot(core::makeBeatUnitDot());
                }
                
                auto pmobuc = bpm->getPerMinuteOrBeatUnitChoice();
                pmobuc->setChoice( core::PerMinuteOrBeatUnitChoice::Choice::perMinute );
                auto pm = pmobuc->getPerMinute();
                auto str = std::to_string( tempo.beatsPerMinute.beatsPerMinute );
                pm->setValue( core::XsString{ str } );
                //auto& attr = *outElement->getAttributes();
                //setAttributesFromSpannerStart( item, attr );
            }
            
            myPlacements.clear();
            myIsFirstDirectionTypeAdded = false;
            return myOutDirectionPtr;
        }
        
        
        void DirectionWriter::addDirectionType( const core::DirectionTypePtr& directionType )
        {
            myOutDirectionPtr->addDirectionType( directionType );

            if( !myIsFirstDirectionTypeAdded && myOutDirectionPtr->getDirectionTypeSet().size() == 2 )
            {
                myOutDirectionPtr->removeDirectionType( myOutDirectionPtr->getDirectionTypeSet().cbegin() );
            }
            
            myIsFirstDirectionTypeAdded = true;
        }
    }
}
