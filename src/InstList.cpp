/*
 * InstList.cpp
 *
 *  Created on: Jun 6, 2014
 *      Author: luigi
 */
#include "InstList.hpp"
#include "Error.hpp"
#include "ClassFile.hpp"

namespace jnif {

Inst* InstList::Iterator::operator*() {
	JnifError::assert(position != NULL, "Dereferencing * on NULL");
	return position;
}

Inst* InstList::Iterator::operator->() const {
	JnifError::assert(position != NULL, "Dereferencing -> on NULL");

	return position;
}

InstList::Iterator& InstList::Iterator::operator++() {
	JnifError::assert(position != NULL, "Doing ++ at NULL");
	position = position->next;

	return *this;
}

InstList::Iterator& InstList::Iterator::operator--() {
	if (position == NULL) {
		position = last;
	} else {
		position = position->prev;
	}

	JnifError::assert(position != NULL, "Doing -- at NULL after last");

	return *this;
}

InstList::~InstList() {
  JnifError::trace("~InstList");

	for (Inst* inst = first; inst != NULL;) {
		Inst* next = inst->next;
    inst->~Inst();
		inst = next;
	}
}

LabelInst* InstList::createLabel() {
	LabelInst* inst = _create<LabelInst>(constPool, nextLabelId);

	nextLabelId++;
	return inst;
}

void InstList::addLabel(LabelInst* inst, Inst* pos) {
	addInst(inst, pos);
}

LabelInst* InstList::addLabel(Inst* pos) {
	LabelInst* inst = createLabel();
	addInst(inst, pos);

	return inst;
}

ZeroInst* InstList::addZero(Opcode opcode, Inst* pos) {
	ZeroInst* inst = _create<ZeroInst>(opcode, constPool);

	addInst(inst, pos);

	return inst;
}

PushInst* InstList::addBiPush(u1 value, Inst* pos) {
	PushInst* inst = _create<PushInst>(OPCODE_bipush, KIND_BIPUSH, value,
			constPool);
	addInst(inst, pos);

	return inst;
}

PushInst* InstList::addSiPush(u2 value, Inst* pos) {
	PushInst* inst = _create<PushInst>(OPCODE_sipush, KIND_SIPUSH, value,
			constPool);
	addInst(inst, pos);

	return inst;
}

LdcInst* InstList::addLdc(Opcode opcode, ConstPool::Index valueIndex, Inst* pos) {
	LdcInst* inst = _create<LdcInst>(opcode, valueIndex, constPool);
	addInst(inst, pos);

	return inst;
}

VarInst* InstList::addVar(Opcode opcode, u1 lvindex, Inst* pos) {
	VarInst* inst = _create<VarInst>(opcode, lvindex, constPool);
	addInst(inst, pos);

	if (opcode == OPCODE_ret) {
		jsrOrRet = true;
	}

	return inst;
}

IincInst* InstList::addIinc(u1 index, u1 value, Inst* pos) {
	IincInst* inst = _create<IincInst>(index, value, constPool);
	addInst(inst, pos);

	return inst;
}

WideInst* InstList::addWideVar(Opcode varOpcode, u2 lvindex, Inst* pos) {
	WideInst* inst = _create<WideInst>(varOpcode, lvindex, constPool);
	addInst(inst, pos);

	return inst;
}

WideInst* InstList::addWideIinc(u2 index, u2 value, Inst* pos) {
	WideInst* inst = _create<WideInst>(index, value, constPool);
	addInst(inst, pos);

	return inst;
}

JumpInst* InstList::addJump(Opcode opcode, LabelInst* targetLabel, Inst* pos) {
	JumpInst* inst = _create<JumpInst>(opcode, targetLabel, constPool);
	addInst(inst, pos);
	branchesCount++;

	targetLabel->isBranchTarget = true;

	if (opcode == OPCODE_jsr) {
		jsrOrRet = true;
	}

	return inst;
}

    FieldInst* InstList::addField(Opcode opcode, ConstPool::Index fieldRefIndex, Inst* pos) {
	FieldInst* inst = _create<FieldInst>(opcode, fieldRefIndex, constPool);
	addInst(inst, pos);
	return inst;
}

InvokeInst* InstList::addInvoke(Opcode opcode, ConstPool::Index methodRefIndex, Inst* pos) {
	InvokeInst* inst = _create<InvokeInst>(opcode, methodRefIndex, constPool);
	addInst(inst, pos);
	return inst;
}

InvokeInterfaceInst* InstList::addInvokeInterface(ConstPool::Index interMethodRefIndex, u1 count, Inst* pos) {
	InvokeInterfaceInst* inst = _create<InvokeInterfaceInst>(interMethodRefIndex, count, constPool);
	addInst(inst, pos);
	return inst;
}

InvokeDynamicInst* InstList::addInvokeDynamic(ConstPool::Index callSite, Inst* pos) {
	InvokeDynamicInst* inst = _create<InvokeDynamicInst>(callSite, constPool);
	addInst(inst, pos);
	return inst;
}

TypeInst* InstList::addType(Opcode opcode, ConstPool::Index classIndex, Inst* pos) {
	TypeInst* inst = _create<TypeInst>(opcode, classIndex, constPool);
	addInst(inst, pos);
	return inst;
}

NewArrayInst* InstList::addNewArray(u1 atype, Inst* pos) {
	NewArrayInst* inst = _create<NewArrayInst>(OPCODE_newarray, atype, constPool);
	addInst(inst, pos);
	return inst;
}

MultiArrayInst* InstList::addMultiArray(ConstPool::Index classIndex, u1 dims, Inst* pos) {
	MultiArrayInst* inst = _create<MultiArrayInst>(OPCODE_multianewarray, classIndex, dims, constPool);
	addInst(inst, pos);
	return inst;
}

TableSwitchInst* InstList::addTableSwitch(LabelInst* def, int low, int high, Inst* pos) {
	TableSwitchInst* inst = _create<TableSwitchInst>(def, low, high, constPool);
	addInst(inst, pos);
	branchesCount++;

	def->isBranchTarget = true;

	return inst;
}

LookupSwitchInst* InstList::addLookupSwitch(LabelInst* def, u4 npairs, Inst* pos) {
	LookupSwitchInst* inst = _create<LookupSwitchInst>(def, npairs, constPool);
	addInst(inst, pos);
	branchesCount++;

	def->isBranchTarget = true;

	return inst;
}

    Inst* InstList::getInst(int offset) {
        for (Inst* inst : *this) {
            if (inst->_offset == offset && !inst->isLabel()) {
                return inst;
            }
        }

        return nullptr;
    }

template<typename TInst, typename ... TArgs>
TInst* InstList::_create(const TArgs& ... args) {
  return constPool->_arena.create<TInst>(args ...);
}

void InstList::addInst(Inst* inst, Inst* pos) {
	JnifError::assert((first == NULL) == (last == NULL),
			"Invalid head/tail/size: head: ", first, ", tail: ", last,
			", size: ", _size);

	JnifError::assert((first == NULL) == (_size == 0),
			"Invalid head/tail/size: head: ", first, ", tail: ", last,
			", size: ", _size);

	Inst* p;
	Inst* n;
	if (first == NULL) {
		JnifError::assert(pos == NULL, "Invalid pos");

		p = NULL;
		n = NULL;
		//first = inst;
		//last = inst;
	} else {
		if (pos == NULL) {
			p = last;
			n = NULL;
			//last = inst;
		} else {
			p = pos->prev;
			n = pos;
		}
	}

	inst->prev = p;
	inst->next = n;

	if (inst->prev != NULL) {
		inst->prev->next = inst;
	} else {
		first = inst;
	}

	if (inst->next != NULL) {
		inst->next->prev = inst;
	} else {
		last = inst;
	}

	_size++;
}

}
