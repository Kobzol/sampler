#pragma once

#define WRAP_ERROR(fn)\
	if ((fn) == 0)\
	{\
		printf("Error: %s:%d: %d\n", __FILE__, __LINE__, GetLastError());\
	}
#define WRAP_HANDLE(fn)\
	if ((fn) == INVALID_HANDLE_VALUE)\
	{\
		printf("Invalid handle: %s:%d: %d\n", __FILE__, __LINE__, GetLastError());\
	}
