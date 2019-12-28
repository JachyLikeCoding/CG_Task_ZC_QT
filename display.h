#pragma once
#ifndef DISPLAY_H
#define DISPLAY_H

#include "basic.h"
#include "ObjLoader.h"
#include "ScanlineZBuffer.h"

void display(int argc, char ** argv, Object &obj, ScanlineZBufferProcessor &Processor);


#endif // !DISPLAY_H
