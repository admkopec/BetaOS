//
//  puts.c
//  BetaOS
//
//  Created by Adam Kopeć on 12/8/15.
//  Copyright © 2015 Adam Kopeć. All rights reserved.
//

#include <stdio.h>

int puts(const char* string)
{
	return printf("%s\n", string);
}
