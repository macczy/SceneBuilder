#include "CommonHeader.h"
#include "shader.h"
#include "ComplexObject.h"
#include "BasicObject.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <signal.h>
#include <fcntl.h>
#include <thread>
#include <vector>

#include "Scene.h"
#include "MyScene.h"


using namespace std;
int main()
{
    MyScene scene;
    scene.run();
}