#pragma once

#include "BinaryArchiveRefl.h"
#include "Maths/Vector.h"
#include "Serialization/Types/String.h"

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

ZE_REFL_SERL_REGISTER_TYPE(maths::Vector3d, Vector3d);
ZE_REFL_SERL_REGISTER_TYPE(maths::Vector3f, Vector3f);