#pragma once

namespace ze::editor
{

class Tab
{
public:
	virtual ~Tab() = default;

	virtual void draw() = 0;
	virtual std::string get_name() const = 0;
};

}