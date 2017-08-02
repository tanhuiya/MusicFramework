// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#include "../../core/elements/EndParagraph.h"
#include "../../core/FromXElement.h"
#include <iostream>

namespace mx
{
    namespace core
    {
        EndParagraph::EndParagraph() : ElementInterface() {}


        bool EndParagraph::hasAttributes() const { return false; }


        bool EndParagraph::hasContents() const  { return false; }
        std::ostream& EndParagraph::streamAttributes( std::ostream& os ) const { return os; }
        std::ostream& EndParagraph::streamName( std::ostream& os ) const  { os << "end-paragraph"; return os; }
        std::ostream& EndParagraph::streamContents( std::ostream& os, const int indentLevel, bool& isOneLineOnly ) const
        {
            MX_UNUSED( indentLevel );
            isOneLineOnly = true;
            return os;
        }


        bool EndParagraph::fromXElement( std::ostream& message, xml::XElement& xelement )
        {
            MX_UNUSED( message );
            MX_UNUSED( xelement );
            return true;
        }

    }
}
