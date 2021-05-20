#pragma once

#include "BinaryArchiveRefl.h"
#include "maths/Vector.h"
#include "serialization/types/String.h"

namespace ze::serialization
{

template<typename ArchiveType>
void serialize(ArchiveType& in_archive, maths::Vector3d& in_vec)
{
	in_archive <=> in_vec.x;
	in_archive <=> in_vec.y;
	in_archive <=> in_vec.z;
}

template<typename ArchiveType>
void serialize(ArchiveType& in_archive, maths::Vector3f& in_vec)
{
	in_archive <=> in_vec.x;
	in_archive <=> in_vec.y;
	in_archive <=> in_vec.z;
}

}

ZE_REFL_DECLARE_TYPE(ze::maths::Vector3d)
ZE_REFL_DECLARE_TYPE(ze::maths::Vector3f)
ZE_REFL_SERL_REGISTER_TYPE(maths::Vector3d, Vector3d);
ZE_REFL_SERL_REGISTER_TYPE(maths::Vector3f, Vector3f);