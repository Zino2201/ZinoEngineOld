#pragma once

#include "Logger/Sink.h"
#include "Serialization/FileArchive.h"

namespace ZE::Logger::Sinks
{

/**
 * A sink that supports writing to a file
 */
class ENGINECORE_API CFileSink : public CSink
{
public:
	CFileSink(const std::string& InName,
		const std::string& InFileName);

	void Log(const SMessage& InMessage) override;
private:
	std::string Filename;
	Serialization::CFileArchive Archive;
};

}