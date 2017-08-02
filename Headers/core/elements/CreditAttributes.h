// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#pragma once

#include "../../core/ForwardDeclare.h"
#include "../../core/AttributesInterface.h"
#include "../../core/Integers.h"

#include <iosfwd>
#include <memory>
#include <vector>

namespace mx
{
    namespace core
    {

        MX_FORWARD_DECLARE_ATTRIBUTES( CreditAttributes )

        struct CreditAttributes : public AttributesInterface
        {
        public:
            CreditAttributes();
            virtual bool hasValues() const;
            virtual std::ostream& toStream( std::ostream& os ) const;
            PositiveInteger page;
            bool hasPage;

            bool fromXElement( std::ostream& message, xml::XElement& xelement );
        };
    }
}
