/*
 * StackMapTableAttrParser.hpp
 *
 *  Created on: Jun 23, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_STACKMAPTABLEATTRPARSER_HPP
#define JNIF_PARSER_STACKMAPTABLEATTRPARSER_HPP

#include <jnif.hpp>

namespace jnif {

    namespace parser {

        class StackMapTableAttrParser {
        public:

            static constexpr const char* AttrName = "StackMapTable";

            Type parseType(BufferReader* br, const ConstPool* cp, LabelManager* labelManager) {
                u1 tag = br->readu1();

                switch (tag) {
                    case TYPE_TOP:
                        return TypeFactory::topType();
                    case TYPE_INTEGER:
                        return TypeFactory::intType();
                    case TYPE_FLOAT:
                        return TypeFactory::floatType();
                    case TYPE_LONG:
                        return TypeFactory::longType();
                    case TYPE_DOUBLE:
                        return TypeFactory::doubleType();
                    case TYPE_NULL:
                        return TypeFactory::nullType();
                    case TYPE_UNINITTHIS:
                        return TypeFactory::uninitThisType();
                    case TYPE_OBJECT: {
                        u2 cpIndex = br->readu2();
                        JnifError::check(cp->isClass(cpIndex), "Bad cpindex: ", cpIndex);
                        string className = cp->getClassName(cpIndex);
                        return TypeFactory::objectType(className, cpIndex);
                    }
                    case TYPE_UNINIT: {
                        u2 offset = br->readu2();
                        LabelInst* label = labelManager->createLabel(offset);
                        return TypeFactory::uninitType(offset, label);
                    }
                }

                throw Exception("Error on parse smt");
            }

            void parseTs(BufferReader* br, int count, std::vector<Type>& locs,
                         const ConstPool* cp, LabelManager* labelManager) {
                for (u1 i = 0; i < count; i++) {
                    Type t = parseType(br, cp, labelManager);
                    locs.push_back(t);
                }
            }

            Attr* parse(BufferReader* br, ClassFile* cp, u2 nameIndex, LabelManager* labelManager) {
                SmtAttr* smt = cp->_arena.create<SmtAttr>(nameIndex, cp);

                u2 numberOfEntries = br->readu2();

                int toff = -1;

                for (u2 i = 0; i < numberOfEntries; i++) {
                    u1 frameType = br->readu1();

                    SmtAttr::Entry e;
                    e.frameType = frameType;

                    if (0 <= frameType && frameType <= 63) {
                        toff += frameType;
                    } else if (64 <= frameType && frameType <= 127) {
                        parseTs(br, 1, e.sameLocals_1_stack_item_frame.stack, cp, labelManager);

                        toff += frameType - 64;
                    } else if (frameType == 247) {
                        u2 offsetDelta = br->readu2();
                        e.same_locals_1_stack_item_frame_extended.offset_delta = offsetDelta;

                        toff += e.same_locals_1_stack_item_frame_extended.offset_delta;
                        parseTs(br, 1, e.same_locals_1_stack_item_frame_extended.stack, cp, labelManager);
                    } else if (248 <= frameType && frameType <= 250) {
                        u2 offsetDelta = br->readu2();
                        e.chop_frame.offset_delta = offsetDelta;

                        toff += e.chop_frame.offset_delta;
                    } else if (frameType == 251) {
                        u2 offsetDelta = br->readu2();
                        e.same_frame_extended.offset_delta = offsetDelta;

                        toff += e.same_frame_extended.offset_delta;
                    } else if (252 <= frameType && frameType <= 254) {
                        u2 offsetDelta = br->readu2();
                        e.append_frame.offset_delta = offsetDelta;
                        parseTs(br, frameType - 251, e.append_frame.locals, cp, labelManager);

                        toff += e.append_frame.offset_delta;
                    } else if (frameType == 255) {
                        u2 offsetDelta = br->readu2();
                        e.full_frame.offset_delta = offsetDelta;

                        u2 numberOfLocals = br->readu2();
                        parseTs(br, numberOfLocals, e.full_frame.locals, cp, labelManager);

                        u2 numberOfStackItems = br->readu2();
                        parseTs(br, numberOfStackItems, e.full_frame.stack, cp, labelManager);

                        toff += e.full_frame.offset_delta;
                    }

                    toff += 1;

                    LabelInst* label = labelManager->createLabel(toff);

                    e.label = label;

                    smt->entries.push_back(e);
                }

                return smt;
            }

        };

    }
}

#endif
