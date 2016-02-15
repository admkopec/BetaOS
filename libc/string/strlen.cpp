//
//  strlen.cpp
//  BetaOS
//
//  Created by Adam Kopeć on 12/8/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <string.h>

size_t strlen(const char* string)
{
	size_t result = 0;
	while ( string[result] )
		result++;
	return result;
}
