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

        MX_FORWARD_DECLARE_ATTRIBUTES( SupportsAttributes )

        struct SupportsAttributes : public AttributesInterface
        {
        public:
            SupportsAttributes();
            virtual bool hasValues() const;
            virtual std::ostream& toStream( std::ostream& os ) const;
            YesNo type;
            XsNMToken element;
            XsNMToken attribute;
            XsToken value;
            const bool hasType;
            const bool hasElement;
            bool hasAttribute;
            bool hasValue;

            bool fromXElement( std::ostream& message, xml::XElement& xelement );
        };
    }
}
