#pragma once

#include <string>
#include <xstring>


enum {
	PKGT_SYSTEM = 0,
	PKGT_THIRD_PARTY = 1,
};


typedef struct {
	std::wstring package;
	std::wstring path;
	int type;
	std::wstring UID;
} ANDROID_PACKAGE_T, *P_ANDORID_PACKAGE_T;