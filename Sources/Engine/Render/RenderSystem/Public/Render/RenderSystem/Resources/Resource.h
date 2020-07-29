#pragma once

#include "EngineCore.h"

namespace ZE
{

/**
 * Base interface for a render system resource
 */
class RENDERSYSTEM_API CRSResource :
    public boost::intrusive_ref_counter<CRSResource, boost::thread_unsafe_counter>
{
public:
    CRSResource() : Name("Unknown") {}
    virtual ~CRSResource() = default;

	virtual void SetName(const char* InName) { Name = InName; }

	const char* GetName() const { return Name; }
protected:
    const char* Name;
};

/**
 * Shared structures for every resources
 */

/**
 * Memory type of the specified resources
 *
 * Host = CPU
 * Device = GPU
 */
enum class ERSMemoryUsage
{
	/** Memory that will be only accessed by the GPU */
	DeviceLocal,

	/** Memory that can be mapped by the CPU */
	HostVisible,

	/** Memory stored in host memory */
	HostOnly,
};

/**
 * Hint about the resource memory
 */
enum class ERSMemoryHintFlagBits
{
	None = 0,

	/**
	 * Force the resource to be mapped by default
	 * Used for persistent mapping
	 */
	Mapped = 1 << 0,
};
ENABLE_FLAG_ENUMS(ERSMemoryHintFlagBits, ERSMemoryHintFlags);

/**
 * Compare operation
 */
enum class ERSComparisonOp
{
	Never,
	Always,
	Less,
	Equal,
	LessOrEqual,
	Greater,
	NotEqual,
	GreaterOrEqual
};

}