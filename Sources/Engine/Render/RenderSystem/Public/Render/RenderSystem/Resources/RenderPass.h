#pragma once

#include "Resource.h"
#include <array>

namespace ZE
{

/** Render pass */
enum class ERSRenderPassAttachmentLoadOp
{
    Clear,
    Load,
    DontCare
};

enum class ERSRenderPassAttachmentStoreOp
{
    Store,
    DontCare
};

/**
 * Layout of the attachment
 */
enum class ERSRenderPassAttachmentLayout
{
	/** Don't care */
	Undefined,

	/** Used for color attachment */
	ColorAttachment,

    /** Used for depth attachment */
    DepthStencilAttachment,

    /** Optimal for shader read */
    ShaderReadOnlyOptimal,

    /** Optimal for shader read */
    DepthStencilReadOnlyOptimal,

	/** Used for present */
	Present,
};

/**
 * Describe an attachment
 */
struct SRSRenderPassAttachment
{
	EFormat Format;
	ESampleCount SampleCount;
	ERSRenderPassAttachmentLoadOp Load;
	ERSRenderPassAttachmentStoreOp Store;
	ERSRenderPassAttachmentLayout InitialLayout;
	ERSRenderPassAttachmentLayout FinalLayout;

	bool operator==(const SRSRenderPassAttachment& InOther) const
	{
		return Format == InOther.Format &&
			SampleCount == InOther.SampleCount &&
			Load == InOther.Load &&
			Store == InOther.Store &&
			InitialLayout == InOther.InitialLayout &&
			FinalLayout == InOther.FinalLayout;
	}
};

/*
 * A subpass attachment reference
 */
struct SRSRenderPassSubpassAttachmentRef
{
	/** Attachment idx */
	uint32_t Index;

	/** Layout that the attachment should have when beginning the subpass */
	ERSRenderPassAttachmentLayout Layout;

	bool operator==(const SRSRenderPassSubpassAttachmentRef& InOther) const
	{
		return Index == InOther.Index &&
			Layout == InOther.Layout;
	}
};

/**
 * Describe a subpass
 */
struct SRSRenderPassSubpass
{
    std::vector<SRSRenderPassSubpassAttachmentRef> ColorAttachmentRefs;
    std::vector<SRSRenderPassSubpassAttachmentRef> DepthAttachmentRefs;
    std::vector<SRSRenderPassSubpassAttachmentRef> InputAttachmentRefs;

    bool operator==(const SRSRenderPassSubpass& InOther) const
    {
        return ColorAttachmentRefs == InOther.ColorAttachmentRefs &&
            DepthAttachmentRefs == InOther.DepthAttachmentRefs &&
            InputAttachmentRefs == InOther.InputAttachmentRefs;
    }
};

/**
 * Represents a render pass
 * 
 * A render pass can contains subpasses that depend from specific attachments
 */
struct SRSRenderPass
{
    std::vector<SRSRenderPassAttachment> ColorAttachments;
    std::vector<SRSRenderPassAttachment> DepthAttachments;
    std::vector<SRSRenderPassSubpass> Subpasses;

    bool operator==(const SRSRenderPass& InOther) const
    {
        return ColorAttachments == InOther.ColorAttachments &&
            DepthAttachments == InOther.DepthAttachments &&
            Subpasses == InOther.Subpasses;
    }
};

struct SRSRenderPassAttachmentHash
{
	std::size_t operator()(const SRSRenderPassAttachment& InAttachment) const noexcept
	{
		std::size_t Seed = 0;

		HashCombine(Seed, InAttachment.Format);
		HashCombine(Seed, InAttachment.Load);
		HashCombine(Seed, InAttachment.Store);
		HashCombine(Seed, InAttachment.InitialLayout);
		HashCombine(Seed, InAttachment.FinalLayout);

		return Seed;
	}
};

struct SRSRenderPassSubpassAttachmentRefHash
{
	std::size_t operator()(const SRSRenderPassSubpassAttachmentRef& InRef) const noexcept
	{
		std::size_t Seed = 0;

		HashCombine(Seed, InRef.Index);
		HashCombine(Seed, InRef.Layout);

		return Seed;
	}
};

struct SRSRenderPassSubpassHash
{
	std::size_t operator()(const SRSRenderPassSubpass& InSubpass) const noexcept
	{
		std::size_t Seed = 0;

		for (const auto& Ref : InSubpass.ColorAttachmentRefs)
		{
			HashCombine<SRSRenderPassSubpassAttachmentRef,
				SRSRenderPassSubpassAttachmentRefHash>(Seed, Ref);
		}

		return Seed;
	}
};

struct SRSRenderPassHash
{
	std::size_t operator()(const SRSRenderPass& InPass) const noexcept
	{
		std::size_t Seed = 0;

		for (const auto& Attachment : InPass.ColorAttachments)
		{
			HashCombine<SRSRenderPassAttachment,
				SRSRenderPassAttachmentHash>(Seed, Attachment);
		}

		for (const auto& Subpass : InPass.Subpasses)
		{
			HashCombine<SRSRenderPassSubpass,
				SRSRenderPassSubpassHash>(Seed, Subpass);
		}

		return Seed;
	}
};

}