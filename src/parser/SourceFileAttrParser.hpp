/*
 * SourceFileAttrParser.hpp
 *
 *  Created on: Jun 18, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_SOURCEFILEATTRPARSER_HPP
#define JNIF_PARSER_SOURCEFILEATTRPARSER_HPP

namespace jnif {

class SourceFileAttrParser {
public:

	static constexpr const char* AttrName = "SourceFile";

	Attr* parse(BufferReader& br, ClassFile& cp, ConstIndex nameIndex, void*) {
		u2 sourceFileIndex = br.readu2();
		Attr* attr = cp._arena.create<SourceFileAttr>(nameIndex, sourceFileIndex, &cp);
		return attr;
	}

};

}

#endif
