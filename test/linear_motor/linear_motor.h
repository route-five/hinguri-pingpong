#pragma once
#include "AXL.h"
#include "AXM.h"
#include "stddef.h"
#include <conio.h>
#include <iostream>

class linear_motor
{
public:
	linear_motor();
	~linear_motor();
	void move_actu(int pos);
};