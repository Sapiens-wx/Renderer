#pragma once
#ifndef DISABLE_IO
#include <iostream>
#define COUT std::cerr<<"[INFO] "<<__FILE__<<':'<<__LINE__<<": "
#define CERR std::cerr<<"[ERROR] "<<__FILE__<<':'<<__LINE__<<": "
#define CWARN std::cerr<<"[WARNING] "<<__FILE__<<':'<<__LINE__<<": "
#endif
#define IM_DRAGFLOAT_SPD 0.1f
