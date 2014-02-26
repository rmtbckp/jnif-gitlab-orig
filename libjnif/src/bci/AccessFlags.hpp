#ifndef __BCI__ACCESS_FLAGS__HPP__
#define __JNIF__ACCESS_FLAGS__HPP__

namespace jnif {

/**
 *
 */
enum AccessFlags {

	/**
	 * Declared public; may be accessed from outside its package.
	 */
	ACC_PUBLIC = 0x0001,

	/**
	 * Declared private; accessible only within the defining class.
	 */
	ACC_PRIVATE = 0x0002,

	/**
	 * Declared protected; may be accessed within subclasses.
	 */
	ACC_PROTECTED = 0x0004,

	/**
	 * Declared static.
	 */
	ACC_STATIC = 0x0008,

	/**
	 * Declared final; must not be overridden (see 5.4.5).
	 */
	ACC_FINAL = 0x0010,

	/**
	 * Declared synchronized; invocation is wrapped by a monitor use.
	 */
	ACC_SYNCHRONIZED = 0x0020,

	/**
	 * A bridge method, generated by the compiler.
	 */
	ACC_BRIDGE = 0x0040,

	/**
	 * Declared with variable number of arguments.
	 */
	ACC_VARARGS = 0x0080,

	/**
	 * Declared native; implemented in a language other than Java.
	 */
	ACC_NATIVE = 0x0100,

	/**
	 * Declared abstract; no implementation is provided.
	 */
	ACC_ABSTRACT = 0x0400,

	/**
	 * Declared strictfp; floating-point mode is FP-strict.
	 */
	ACC_STRICT = 0x0800,

	/**
	 * Declared synthetic; not present in the source code.
	 */
	ACC_SYNTHETIC = 0x1000,
};

}

#endif
