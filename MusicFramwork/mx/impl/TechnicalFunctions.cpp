// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#include "../impl/TechnicalFunctions.h"
#include "../core/elements/Arrow.h"
#include "../core/elements/Bend.h"
#include "../core/elements/DoubleTongue.h"
#include "../core/elements/DownBow.h"
#include "../core/elements/Fingering.h"
#include "../core/elements/Fingernails.h"
#include "../core/elements/Fret.h"
#include "../core/elements/HammerOn.h"
#include "../core/elements/Handbell.h"
#include "../core/elements/Harmonic.h"
#include "../core/elements/Heel.h"
#include "../core/elements/Hole.h"
#include "../core/elements/OpenString.h"
#include "../core/elements/OtherTechnical.h"
#include "../core/elements/Pluck.h"
#include "../core/elements/PullOff.h"
#include "../core/elements/SnapPizzicato.h"
#include "../core/elements/Stopped.h"
#include "../core/elements/String.h"
#include "../core/elements/Tap.h"
#include "../core/elements/Technical.h"
#include "../core/elements/TechnicalChoice.h"
#include "../core/elements/ThumbPosition.h"
#include "../core/elements/Toe.h"
#include "../core/elements/TripleTongue.h"
#include "../core/elements/UpBow.h"
#include "../impl/Converter.h"
#include "../impl/MarkDataFunctions.h"

namespace mx
{
    namespace impl
    {
        TechnicalFunctions::TechnicalFunctions( const core::TechnicalChoiceSet& inTechnicalChoiceSet, Cursor inCursor )
        : myTechincalChoiceSet{ inTechnicalChoiceSet }
        , myCursor{ inCursor }
        {
            
        }
        
        
        void TechnicalFunctions::parseTechnicalMarks( std::vector<api::MarkData>& outMarks ) const
        {
            MX_UNUSED( outMarks );
            
            for( const auto& techChoice : myTechincalChoiceSet )
            {
                const auto choiceValue = techChoice->getChoice();
                Converter converter;
                const auto markType = converter.convertTechnicalMark( choiceValue );
                
                if( markType == api::MarkType::unspecified && choiceValue != core::TechnicalChoice::Choice::otherTechnical )
                {
                    continue;
                }
                api::MarkData markData;
                markData.markType = markType;
                markData.tickTimePosition = myCursor.tickTimePosition;
                bool isSuccess = parseTechicalMark( *techChoice, markData );
                
                if( isSuccess )
                {
                    if( markData.smuflName.empty() && markData.smuflCodepoint == 0 )
                    {
                        markData.smuflName = api::MarkSmufl::getName( markType, markData.positionData.placement );
                        markData.smuflCodepoint = api::MarkSmufl::getCodepoint( markType, markData.positionData.placement );
                    }
                    outMarks.emplace_back( std::move( markData ) );
                }
            }
        }
        
        
        bool TechnicalFunctions::parseTechicalMark( const core::TechnicalChoice& techicalChoice, api::MarkData& outMarkData ) const
        {
            switch ( techicalChoice.getChoice() )
            {
                case core::TechnicalChoice::Choice::upBow:
                {
                    parseMarkDataAttributes( *techicalChoice.getUpBow()->getAttributes(), outMarkData );
                    outMarkData.name = "up-bow";
                    return true;
                }
                case core::TechnicalChoice::Choice::downBow:
                {
                    parseMarkDataAttributes( *techicalChoice.getDownBow()->getAttributes(), outMarkData );
                    outMarkData.name = "down-bow";
                    return true;
                }
                case core::TechnicalChoice::Choice::harmonic:
                {
                    parseMarkDataAttributes( *techicalChoice.getHarmonic()->getAttributes(), outMarkData );
                    outMarkData.name = "harmonic";
                    return true;
                }
                case core::TechnicalChoice::Choice::openString:
                {
                    parseMarkDataAttributes( *techicalChoice.getOpenString()->getAttributes(), outMarkData );
                    outMarkData.name = "open-string";
                    return true;
                }
                case core::TechnicalChoice::Choice::thumbPosition:
                {
                    parseMarkDataAttributes( *techicalChoice.getThumbPosition()->getAttributes(), outMarkData );
                    outMarkData.name = "thumb-position";
                    return true;
                }
                case core::TechnicalChoice::Choice::fingering: return false;
                case core::TechnicalChoice::Choice::pluck:
                {
                    parseMarkDataAttributes( *techicalChoice.getPluck()->getAttributes(), outMarkData );
                    outMarkData.name = "pluck";
                    return true;
                }
                case core::TechnicalChoice::Choice::doubleTongue:
                {
                    parseMarkDataAttributes( *techicalChoice.getDoubleTongue()->getAttributes(), outMarkData );
                    outMarkData.name = "double-tongue";
                    return true;
                }
                case core::TechnicalChoice::Choice::tripleTongue:
                {
                    parseMarkDataAttributes( *techicalChoice.getTripleTongue()->getAttributes(), outMarkData );
                    outMarkData.name = "triple-tongue";
                    return true;
                }
                case core::TechnicalChoice::Choice::stopped:
                {
                    parseMarkDataAttributes( *techicalChoice.getStopped()->getAttributes(), outMarkData );
                    outMarkData.name = "stopped";
                    return true;
                }
                case core::TechnicalChoice::Choice::snapPizzicato:
                {
                    parseMarkDataAttributes( *techicalChoice.getSnapPizzicato()->getAttributes(), outMarkData );
                    outMarkData.name = "snap-pizzicato";
                    return true;
                }
                case core::TechnicalChoice::Choice::fret: return false;
                case core::TechnicalChoice::Choice::string_: return false;
                case core::TechnicalChoice::Choice::hammerOn: return false;
                case core::TechnicalChoice::Choice::pullOff: return false;
                case core::TechnicalChoice::Choice::bend: return false;
                case core::TechnicalChoice::Choice::tap: return false;
                case core::TechnicalChoice::Choice::heel:
                {
                    parseMarkDataAttributes( *techicalChoice.getHeel()->getAttributes(), outMarkData );
                    outMarkData.name = "heel";
                    return true;
                }
                case core::TechnicalChoice::Choice::toe:
                {
                    parseMarkDataAttributes( *techicalChoice.getToe()->getAttributes(), outMarkData );
                    outMarkData.name = "toe";
                    return true;
                }
                case core::TechnicalChoice::Choice::fingernails:
                {
                    parseMarkDataAttributes( *techicalChoice.getFingernails()->getAttributes(), outMarkData );
                    outMarkData.name = "fingernails";
                    return true;
                }
                case core::TechnicalChoice::Choice::hole: return false;
                case core::TechnicalChoice::Choice::arrow: return false;
                case core::TechnicalChoice::Choice::handbell: return false;
                case core::TechnicalChoice::Choice::otherTechnical:
                {
                    const auto& other = *techicalChoice.getOtherTechnical();
                    parseMarkDataAttributes( other.getAttributes(), outMarkData );
                    
                    const auto value = other.getValue().getValue();
                    const auto charVal = api::Smufl::findCodepoint( value );
                    outMarkData.name = value;
                    
                    if( charVal != 0 )
                    {
                        outMarkData.smuflName = value;
                        outMarkData.smuflCodepoint = charVal;
                    }

                    return true;
                }
                default:
                    return false;
            }
        }
    }
}
