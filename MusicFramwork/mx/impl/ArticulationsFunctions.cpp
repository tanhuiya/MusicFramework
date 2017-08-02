// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#include "../impl/ArticulationsFunctions.h"
#include "../core/elements/Accent.h"
#include "../core/elements/ArticulationsChoice.h"
#include "../core/elements/BreathMark.h"
#include "../core/elements/Caesura.h"
#include "../core/elements/DetachedLegato.h"
#include "../core/elements/Doit.h"
#include "../core/elements/Falloff.h"
#include "../core/elements/OtherArticulation.h"
#include "../core/elements/Plop.h"
#include "../core/elements/Scoop.h"
#include "../core/elements/Spiccato.h"
#include "../core/elements/Staccatissimo.h"
#include "../core/elements/Staccato.h"
#include "../core/elements/Stress.h"
#include "../core/elements/StrongAccent.h"
#include "../core/elements/Tenuto.h"
#include "../core/elements/Unstress.h"
#include "../impl/Converter.h"
#include "../impl/MarkDataFunctions.h"
#include "../impl/PositionFunctions.h"
#include "../impl/PrintFunctions.h"


#include <mutex>

namespace mx
{
    namespace impl
    {
        ArticulationsFunctions::ArticulationsFunctions( const core::ArticulationsChoiceSet& inArticulations, impl::Cursor inCursor )
        : myArticulations{ inArticulations }
        , myCursor{ inCursor }
        {
            
        }
        
        
        void ArticulationsFunctions::parseArticulations( std::vector<api::MarkData>& outMarks ) const
        {
            for( const auto& articulation : myArticulations )
            {
                const auto articulationType = articulation->getChoice();
                Converter converter;
                const auto markType = converter.convertArticulation( articulationType );
                auto markData = api::MarkData{};
                markData.markType = markType;
                markData.tickTimePosition = myCursor.tickTimePosition;
                
                parseArticulation( *articulation, markData );
                
                markData.smuflName = api::MarkSmufl::getName( markType, markData.positionData.placement );
                markData.smuflCodepoint = api::MarkSmufl::getCodepoint( markType, markData.positionData.placement );
                outMarks.emplace_back( std::move( markData ) );
            }
        }
        
        
        void ArticulationsFunctions::parseArticulation( const core::ArticulationsChoice& inArticulation, api::MarkData& outMark ) const
        {
            switch ( inArticulation.getChoice() )
            {
                case core::ArticulationsChoice::Choice::accent:
                {
                    parseMarkDataAttributes( *inArticulation.getAccent()->getAttributes(), outMark );
                    outMark.name = "accent";
                    break;
                }
                case core::ArticulationsChoice::Choice::strongAccent:
                {
                    parseMarkDataAttributes( *inArticulation.getStrongAccent()->getAttributes(), outMark );
                    outMark.name = "strong-accent";
                    break;
                }
                case core::ArticulationsChoice::Choice::staccato:
                {
                    parseMarkDataAttributes( *inArticulation.getStaccato()->getAttributes(), outMark );
                    outMark.name = "staccato";
                    break;
                }
                case core::ArticulationsChoice::Choice::tenuto:
                {
                    parseMarkDataAttributes( *inArticulation.getTenuto()->getAttributes(), outMark );
                    outMark.name = "tenuto";
                    break;
                }
                case core::ArticulationsChoice::Choice::detachedLegato:
                {
                    parseMarkDataAttributes( *inArticulation.getDetachedLegato()->getAttributes(), outMark );
                    outMark.name = "detached-legato";
                    break;
                }
                case core::ArticulationsChoice::Choice::staccatissimo:
                {
                    parseMarkDataAttributes( *inArticulation.getStaccatissimo()->getAttributes(), outMark );
                    outMark.name = "staccatissimo";
                    break;
                }
                case core::ArticulationsChoice::Choice::spiccato:
                {
                    parseMarkDataAttributes( *inArticulation.getSpiccato()->getAttributes(), outMark );
                    outMark.name = "spiccato";
                    break;
                }
                case core::ArticulationsChoice::Choice::scoop:
                {
                    parseMarkDataAttributes( *inArticulation.getScoop()->getAttributes(), outMark );
                    outMark.name = "scoop";
                    break;
                }
                case core::ArticulationsChoice::Choice::plop:
                {
                    parseMarkDataAttributes( *inArticulation.getPlop()->getAttributes(), outMark );
                    outMark.name = "plop";
                    break;
                }
                case core::ArticulationsChoice::Choice::doit:
                {
                    parseMarkDataAttributes( *inArticulation.getDoit()->getAttributes(), outMark );
                    outMark.name = "doit";
                    break;
                }
                case core::ArticulationsChoice::Choice::falloff:
                {
                    parseMarkDataAttributes( *inArticulation.getFalloff()->getAttributes(), outMark );
                    outMark.name = "falloff";
                    break;
                }
                case core::ArticulationsChoice::Choice::breathMark:
                {
                    parseMarkDataAttributes( *inArticulation.getBreathMark()->getAttributes(), outMark );
                    outMark.name = "breath-mark";
                    break;
                }
                case core::ArticulationsChoice::Choice::caesura:
                {
                    parseMarkDataAttributes( *inArticulation.getCaesura()->getAttributes(), outMark );
                    outMark.name = "caesura";
                    break;
                }
                case core::ArticulationsChoice::Choice::stress:
                {
                    parseMarkDataAttributes( *inArticulation.getStress()->getAttributes(), outMark );
                    outMark.name = "stress";
                    break;
                }
                case core::ArticulationsChoice::Choice::unstress:
                {
                    parseMarkDataAttributes( *inArticulation.getUnstress()->getAttributes(), outMark );
                    outMark.name = "unstress";
                    break;
                }
                case core::ArticulationsChoice::Choice::otherArticulation:
                {
                    parseMarkDataAttributes( *inArticulation.getAccent()->getAttributes(), outMark );
                    outMark.smuflName = outMark.name;
                    outMark.smuflCodepoint = api::Smufl::findCodepoint( outMark.smuflName );
                    break;
                }
                default:
                    break;
            }
            

        }
    }
}
