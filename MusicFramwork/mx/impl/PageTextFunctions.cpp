// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#include "../impl/PageTextFunctions.h"
#include "../core/elements/Credit.h"
#include "../core/elements/CreditChoice.h"
#include "../core/elements/CreditType.h"
#include "../core/elements/CreditWords.h"
#include "../core/elements/ScoreHeaderGroup.h"
#include "../impl/PositionFunctions.h"

namespace mx
{
    namespace impl
    {
        void createPageTextItems( const std::vector<api::PageTextData>& inPageTextItems, core::ScoreHeaderGroup& outHeader )
        {
            for ( const auto& p : inPageTextItems )
            {
                auto credit = core::makeCredit();
                auto choice = credit->getCreditChoice();
                choice->setChoice( core::CreditChoice::Choice::creditWords );
                auto words = choice->getCreditWords();
                auto attr = credit->getAttributes();
                
                words->setValue( core::XsString{ p.text } );
                if( !p.description.empty() )
                {
                    auto descrip = core::makeCreditType();
                    descrip->setValue( core::XsString{ p.description } );
                    credit->addCreditType( descrip );
                }
                
                if( p.pageNumber > 0 )
                {
                    credit->getAttributes()->hasPage = true;
                    credit->getAttributes()->page = core::PositiveInteger{ p.pageNumber };
                }
                const auto wattr = words->getAttributes();
                
                //getPositionData( p.positionData, *wattr );
                
                outHeader.addCredit( credit );
            }
        }
        
        void createPageTextItems( const core::ScoreHeaderGroup& inHeader, std::vector<api::PageTextData>& outPageTextItems )
        {
            
            for( const auto& c : inHeader.getCreditSet() )
            {
                const auto choice = c->getCreditChoice();
                
                if( choice->getChoice() != core::CreditChoice::Choice::creditWords )
                {
                    // ignore images for now
                    continue;
                }
                
                auto pageText = api::PageTextData{};
                
                if( c->getAttributes()->hasPage )
                {
                    pageText.pageNumber = c->getAttributes()->page.getValue();
                }
                
                auto words = choice->getCreditWords();
                pageText.text = words->getValue().getValue();
                auto attr = words->getAttributes();
                
                const auto& creditTypeSet = c->getCreditTypeSet();
                if( creditTypeSet.size() > 0 )
                {
                    const auto& t = *creditTypeSet.cbegin();
                    pageText.description = t->getValue().getValue();
                }
                
                pageText.positionData = getPositionData( *attr );
                
                outPageTextItems.push_back( pageText );
            }
        }
    }
}
