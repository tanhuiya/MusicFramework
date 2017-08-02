// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#include "../xml/XFactory.h"
#include "../xml/PugiDoc.h"
#include "../xml/PugiElement.h"

namespace mx
{
    namespace xml
    {
        XDocPtr XFactory::makeXDoc()
        {
            return XDocPtr{ new PugiDoc{} };
        }
        
        XElementPtr XFactory::makeXElement()
        {
            return XElementPtr{ new PugiElement{} };
        }
    }
}
