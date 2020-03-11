#include "MeshCollection.h"
#include "Render/World/RenderableComponentProxy.h"

void CMeshCollection::AddInstance(const SMeshCollectionInstance& InInstance)
{
	/*if(bUseInstancing)
	{
		if(!Instances[0])
			Instances[0] = SMeshCollectionInstance(nullptr, nullptr);

		Instances[0].InstanceCount += InInstance.InstanceCount;

		UpdateInstancingData();
	}
	else
	{
		if(!Instances[0])
			Instances[0] = InInstance;
		else
			Instances.push_back(InInstance);
	}*/
}

void CMeshCollection::RemoveInstance(CRenderableComponentProxy* InProxy)
{
	//Instances[0].InstanceCount--;

	/*if(bUseInstancing)
	{


		UpdateInstancingData();
	}
	else
	{
		SMeshCollectionInstance* InstanceToDelete = nullptr;

		size_t Idx = 0;
		for (const auto& Instance : Instances)
		{
			if (Instance.UniformBuffer == InProxy->GetUniformBuffer())
			{
				InstanceToDelete = const_cast<SMeshCollectionInstance*>(&Instance);
				break;
			}

			Idx++;
		}

		if (InstanceToDelete)
			Instances.erase(Instances.begin() + Idx);
	}*/
}