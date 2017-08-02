// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#pragma once
#include "../core/AttributesInterface.h"
#include "../core/Decimals.h"
#include "../core/Strings.h"
#include "../core/Enums.h"
#include "../core/FontSize.h"
#include "../core/Color.h"

namespace mx
{
    namespace core
    {
        struct EmptyPrintObjectStyleAlignAttributes;
        using EmptyPrintObjectStyleAlignAttributesPtr = std::shared_ptr<EmptyPrintObjectStyleAlignAttributes>;
        
        struct EmptyPrintObjectStyleAlignAttributes : public AttributesInterface
        {
        public:
            EmptyPrintObjectStyleAlignAttributes();
            virtual bool hasValues() const;
            virtual std::ostream& toStream( std::ostream& os ) const;
            
            YesNo printObject;
            TenthsValue defaultX;
            TenthsValue defaultY;
            TenthsValue relativeX;
            TenthsValue relativeY;
            CommaSeparatedText fontFamily;
            FontStyle fontStyle;
            FontSize fontSize;
            FontWeight fontWeight;
            Color color;
            LeftCenterRight halign;
            Valign valign;
            
            bool hasPrintObject;
            bool hasDefaultX;
            bool hasDefaultY;
            bool hasRelativeX;
            bool hasRelativeY;
            bool hasFontFamily;
            bool hasFontStyle;
            bool hasFontSize;
            bool hasFontWeight;
            bool hasColor;
            bool hasHalign;
            bool hasValign;

            virtual bool fromXElement( std::ostream& message, xml::XElement& xelement );
        };
    }
}
