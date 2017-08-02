// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#include "../impl/OrnamentsFunctions.h"
#include "../core/elements/DelayedInvertedTurn.h"
#include "../core/elements/DelayedTurn.h"
#include "../core/elements/InvertedMordent.h"
#include "../core/elements/InvertedTurn.h"
#include "../core/elements/Mordent.h"
#include "../core/elements/Ornaments.h"
#include "../core/elements/OrnamentsChoice.h"
#include "../core/elements/OtherOrnament.h"
#include "../core/elements/Schleifer.h"
#include "../core/elements/Shake.h"
#include "../core/elements/Tremolo.h"
#include "../core/elements/TrillMark.h"
#include "../core/elements/Turn.h"
#include "../core/elements/VerticalTurn.h"
#include "../core/elements/WavyLine.h"
#include "../impl/AccidentalMarkFunctions.h"
#include "../impl/MarkDataFunctions.h"

namespace mx
{
    namespace impl
    {
        OrnamentsFunctions::OrnamentsFunctions( const core::Ornaments& inOrnaments, impl::Cursor inCursor )
        : myOrnaments{ inOrnaments }
        , myCursor{ inCursor }
        {
            
        }
        
        void OrnamentsFunctions::parseOrnaments( std::vector<api::MarkData>& outMarks ) const
        {
            parseOrnamentsSet( outMarks );
            parseAccidentalMarkSet( outMarks );
        }
        
        
        void OrnamentsFunctions::parseOrnamentsSet( std::vector<api::MarkData>& outMarks ) const
        {
            for( const auto& ornament : myOrnaments.getOrnamentsChoiceSet() )
            {
                const auto ornamentType = ornament->getChoice();
                Converter converter;
                const auto markType = converter.convertOrnament( ornamentType );
                auto markData = api::MarkData{};
                markData.markType = markType;
                markData.tickTimePosition = myCursor.tickTimePosition;
                
                parseOrnament( *ornament, markData );
                
                markData.smuflName = api::MarkSmufl::getName( markType, markData.positionData.placement );
                markData.smuflCodepoint = api::MarkSmufl::getCodepoint( markType, markData.positionData.placement );
                outMarks.emplace_back( std::move( markData ) );
            }
        }
        
        
        void OrnamentsFunctions::parseAccidentalMarkSet( std::vector<api::MarkData>& outMarks ) const
        {
            for( const auto& am : myOrnaments.getAccidentalMarkSet() )
            {
                AccidentalMarkFunctions funcs{ *am, myCursor };
                outMarks.emplace_back( funcs.parseAccidentalMark() );
            }
        }

        
        void OrnamentsFunctions::parseOrnament( const core::OrnamentsChoice& choiceObj, api::MarkData& outMark ) const
        {
            switch ( choiceObj.getChoice() )
            {
                case core::OrnamentsChoice::Choice::trillMark:
                {
                    outMark.name = "trill-mark";
                    parseMarkDataAttributes( *choiceObj.getTrillMark()->getAttributes(), outMark );
                    break;
                }
                case core::OrnamentsChoice::Choice::turn:
                {
                    outMark.name = "turn";
                    parseMarkDataAttributes( *choiceObj.getTurn()->getAttributes(), outMark );
                    break;
                }
                case core::OrnamentsChoice::Choice::delayedTurn:
                {
                    outMark.name = "delayed-turn";
                    parseMarkDataAttributes( *choiceObj.getDelayedTurn()->getAttributes(), outMark );
                    break;
                }
                case core::OrnamentsChoice::Choice::invertedTurn:
                {
                    outMark.name = "inverted-turn";
                    parseMarkDataAttributes( *choiceObj.getInvertedTurn()->getAttributes(), outMark );
                    break;
                }
                case core::OrnamentsChoice::Choice::delayedInvertedTurn:
                {
                    outMark.name = "delayed-inverted-turn";
                    parseMarkDataAttributes( *choiceObj.getDelayedInvertedTurn()->getAttributes(), outMark );
                    break;
                }
                case core::OrnamentsChoice::Choice::verticalTurn:
                {
                    outMark.name = "vertical-turn";
                    parseMarkDataAttributes( *choiceObj.getVerticalTurn()->getAttributes(), outMark );
                    break;
                }
                case core::OrnamentsChoice::Choice::shake:
                {
                    outMark.name = "shake";
                    parseMarkDataAttributes( *choiceObj.getShake()->getAttributes(), outMark );
                    break;
                }
                case core::OrnamentsChoice::Choice::wavyLine:
                {
                    outMark.name = "wavy-line";
                    parseMarkDataAttributes( *choiceObj.getWavyLine()->getAttributes(), outMark );
                    break;
                }
                case core::OrnamentsChoice::Choice::mordent:
                {
                    outMark.name = "mordent";
                    parseMarkDataAttributes( *choiceObj.getMordent()->getAttributes(), outMark );
                    break;
                }
                case core::OrnamentsChoice::Choice::invertedMordent:
                {
                    outMark.name = "inverted-mordent";
                    parseMarkDataAttributes( *choiceObj.getInvertedMordent()->getAttributes(), outMark );
                    break;
                }
                case core::OrnamentsChoice::Choice::schleifer:
                {
                    outMark.name = "schleifer";
                    parseMarkDataAttributes( *choiceObj.getSchleifer()->getAttributes(), outMark );
                    break;
                }
                case core::OrnamentsChoice::Choice::tremolo:
                {
                    outMark.name = "tremolo";
                    parseMarkDataAttributes( *choiceObj.getTremolo()->getAttributes(), outMark );
                    break;
                }
                case core::OrnamentsChoice::Choice::otherOrnament:
                {
                    const auto value = choiceObj.getOtherOrnament()->getValue().getValue();
                    parseMarkDataAttributes( *choiceObj.getOtherOrnament()->getAttributes(), outMark );
                    
                    if( value.empty() )
                    {
                        outMark.name = "other-ornament";
                    }
                    else
                    {
                        outMark.name = value;
                    }
                    
                    const auto tempChar = api::Smufl::findCodepoint( value );
                    
                    if( tempChar != 0 )
                    {
                        outMark.smuflName = value;
                        outMark.smuflCodepoint = tempChar;
                    }

                    break;
                }
                default:
                    break;
            }
        }
    }
}
