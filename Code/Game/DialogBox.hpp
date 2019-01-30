#pragma once
#include "Engine\Core\Widget.hpp"

class DialogBox : public Widget 
{
public:
	DialogBox();
	~DialogBox();

	virtual void Initialize();
	virtual void Update(float deltaSeconds);
	virtual void PreRender();

public:


};
