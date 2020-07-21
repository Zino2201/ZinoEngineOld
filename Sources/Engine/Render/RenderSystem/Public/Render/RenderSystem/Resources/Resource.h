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
 * Memory usage of the specified resources
 */
enum class ERSMemoryUsage
{
    None = 1 << 0,

	/** MEMORY TYPES */

	/** Memory that will be only accessed by the GPU */
	DeviceLocal = 1 << 1,

	/** Memory that can be mapped by the CPU */
	HostVisible = 1 << 2,

	/** Memory stored in host memory */
	HostOnly = 1 << 3,

	/** FLAGS */
    
    /** Use persistent mapping (BUFFER ONLY) */
	UsePersistentMapping = 1 << 4,
};
DECLARE_FLAG_ENUM(ERSMemoryUsage);

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