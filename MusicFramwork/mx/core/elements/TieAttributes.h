// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#pragma once

#include "../../core/ForwardDeclare.h"
#include "../../core/AttributesInterface.h"
#include "../../core/Enums.h"
#include "../../core/Strings.h"

#include <iosfwd>
#include <memory>
#include <vector>

namespace mx
{
    namespace core
    {

        MX_FORWARD_DECLARE_ATTRIBUTES( TieAttributes )

        struct TieAttributes : public AttributesInterface
        {
        public:
            TieAttributes();
            virtual bool hasValues() const;
            virtual std::ostream& toStream( std::ostream& os ) const;
            StartStop type;
            TimeOnly timeOnly;
            const bool hasType;
            bool hasTimeOnly;

            bool fromXElement( std::ostream& message, xml::XElement& xelement );
        };
    }
}
