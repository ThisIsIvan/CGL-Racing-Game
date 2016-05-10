#include "RenderProject.h"
#include "GameObject.h"
#include "Car.h"

#ifdef __OBJC__
#import <CoreMotion/CoreMotion.h>
#endif

/* Initialize the Project */
Car plane1 = Car(vmml::Vector3f(1.0f, 1.0f, 1.f), vmml::Vector3f(1.f),  vmml::Vector3f::UNIT_Z, (float)(180*M_PI_F/180));

vmml::Matrix4f model2Matrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, 1000.f)) * vmml::create_scaling(vmml::Vector3f(1.f))* vmml::create_rotation((float)(180*M_PI_F/180), vmml::Vector3f::UNIT_Z);
double _time = 0;
double _pitchSum;
float angle=0.f;
vmml::AABBf aabb2;
vmml::Vector4f eyePos;
vmml::Matrix4f viewMatrix;
CubeMapPtr cubemap;
vmml::Vector3f cameraOffset = vmml::Vector3f(0., -20., 100.); //(0., -20., 100.)


void RenderProject::init()
{
    bRenderer::loadConfigFile("config.json");	// load custom configurations replacing the default values in Configuration.cpp
   
    // let the renderer create an OpenGL context and the main window
    if(Input::isTouchDevice())
        bRenderer().initRenderer(true);										// full screen on iOS
    else
        bRenderer().initRenderer(1920, 1080, false, "Assignment 6");		// windowed mode on desktop
    //bRenderer().initRenderer(View::getScreenWidth(), View::getScreenHeight(), true);		// full screen using full width and height of the screen
    
    
    
    // start main loop
    bRenderer().runRenderer();
}

/* This function is executed when initializing the renderer */
void RenderProject::initFunction()
{
    // get OpenGL and shading language version
    bRenderer::log("OpenGL Version: ", glGetString(GL_VERSION));
    bRenderer::log("Shading Language Version: ", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    // initialize variables
    
    carIsMoving = false;
    gameRunning = false;
    velocity = vmml::Vector3f(0.0f, 0.0f, -20.0f);
    
    
    _offset = 0.0f;
    _randomOffset = 0.0f;
    _cameraSpeed = 40.0f;
    _running = true; _lastStateSpaceKey = bRenderer::INPUT_UNDEFINED;
    _viewMatrixHUD = Camera::lookAt(vmml::Vector3f(0.0f, 0.0f, 0.25f), vmml::Vector3f::ZERO, vmml::Vector3f::UP);
    
    // set shader versions (optional)
    bRenderer().getObjects()->setShaderVersionDesktop("#version 120");
    bRenderer().getObjects()->setShaderVersionES("#version 100");
    
    // load materials and shaders before loading the model
    //loadShaderFile(const std::string &shaderName, GLuint shaderMaxLights, bool variableNumberOfLights, bool ambientLighting, bool diffuseLighting, bool specularLighting, bool cubicReflectionMap);
    
    ShaderPtr guyShader = bRenderer().getObjects()->loadShaderFile("guy", 0, false, false, false, false, false);
    ShaderPtr planeShader = bRenderer().getObjects()->loadShaderFile("plane", 0, false, false, false, false, false);
    //ShaderPtr terrainShader = bRenderer().getObjects()->loadShaderFile("terrain", 0, false, false, false, false, false);
    
    // load shader from file without lighting, the number of lights won't ever change during rendering (no variable number of lights)
    
    // create additional properties for a model
    PropertiesPtr guyProperties = bRenderer().getObjects()->createProperties("guyProperties");
    PropertiesPtr terrainProperties = bRenderer().getObjects()->createProperties("terrainProperties");
    PropertiesPtr planeProperties = bRenderer().getObjects()->createProperties("planeProperties");
    
    plane1.aabb = bRenderer().getObjects()->loadObjModel("plane.obj", false, true, planeShader, planeProperties)->getBoundingBoxObjectSpace();
    aabb2 = bRenderer().getObjects()->loadObjModel("terrain.obj", false, false, true, 4, false, false, terrainProperties)->getBoundingBoxObjectSpace();
    
    // create camera
    bRenderer().getObjects()->createCamera("camera", vmml::Vector3f(0.0f, -30.0f, 300.0f), vmml::Vector3f(0.0f, 0.0f, 0.0f));
    
    // Update render queue
    updateRenderQueue("camera", 0.0f);
}

/* Draw your scene here */
void RenderProject::loopFunction(const double &deltaTime, const double &elapsedTime)
{
    /// Draw scene ///
    
    bRenderer().getModelRenderer()->drawQueue(/*GL_LINES*/);
    bRenderer().getModelRenderer()->clearQueue();
    
    //// Camera Movement ////
    updateCamera("camera", deltaTime);
    
    /// Update render queue ///
    updateRenderQueue("camera", deltaTime);
    
    // Quit renderer when escape is pressed
    if (bRenderer().getInput()->getKeyState(bRenderer::KEY_ESCAPE) == bRenderer::INPUT_PRESS)
        bRenderer().terminateRenderer();
}

/* This function is executed when terminating the renderer */
void RenderProject::terminateFunction()
{
    bRenderer::log("I totally terminated this Renderer :-)");
}

/* Update render queue */
void RenderProject::updateRenderQueue(const std::string &camera, const double &deltaTime)
{
//    glDisable(GL_CULL_FACE);§
    _pitchSum += bRenderer().getInput()->getGyroscopePitch()* 1.0f;
    
    //bRenderer().getObjects()->createTextSprite("instructions", vmml::Vector3f(1.f, 1.f, 1.f), "Press space to start", font);
    
    float pitch = (float)(bRenderer().getInput()->getGyroscopePitch()/50);
    vmml::Matrix4f rotationY = vmml::create_rotation(pitch, vmml::Vector3f::UNIT_Y);
    
    GameObject plane2 = GameObject(model2Matrix, aabb2);
    plane1.addCollidable(plane2);
    
    GLint m_viewport[4];
    glGetIntegerv( GL_VIEWPORT, m_viewport);
    
    TouchMap touchMap = bRenderer().getInput()->getTouches();
    if(touchMap.empty()){
        plane1.decelerate();
    }
    else{
        auto t = touchMap.begin();
        Touch touch = t->second;
        if(touch.lastPositionX > m_viewport[2]/2){
            if(touch.lastPositionY < m_viewport[3] / 2){
                plane1.activateBoost();
            }
            else{
                plane1.accelerate();
            }
        }
        else{
            plane1.brake();
        }
    }
    
    vmml::Matrix4f modelMatrixTerrain = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, 0.f));
    plane1.move(rotationY);
    vmml::Vector3f cameraPos = bRenderer().getObjects()->getCamera("camera")->getPosition();
    
    modelMatrixTerrain *= plane1.modelMatrix;
    /*** solar system ***/
    ShaderPtr shader = bRenderer().getObjects()->getShader("plane");
    
    plane1.modelMatrix = vmml::create_scaling(vmml::Vector3f(1.0f, 1.0f, 1.0f)) * plane1.modelMatrix;
    
    vmml::Matrix3f normalMatrixPlane;
    vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(plane1.modelMatrix)), normalMatrixPlane);

    
    vmml::Vector3f camPosition = cameraOffset + vmml::Vector3f(-2.*plane1.modelMatrix.x(), -1.*plane1.modelMatrix.y(), -1.*plane1.modelMatrix.z());
    bRenderer().getObjects()->getCamera("camera")->rotateCamera(0., -pitch, 0.);
    bRenderer().getObjects()->getCamera("camera")->setPosition(camPosition);
    viewMatrix = bRenderer().getObjects()->getCamera("camera")->getViewMatrix();
    
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", vmml::Matrix4f::IDENTITY);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("modelMatrixTerrain", modelMatrixTerrain);
        shader->setUniform("ModelMatrix", plane1.modelMatrix);
        //shader->setUniform("model2Matrix", normal2Matrix);
        
        
        vmml::Matrix3f normalMatrix;
        vmml::Matrix3f normalMatrixPlane;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrixTerrain)), normalMatrix);
        //vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(normal2Matrix)), normalMatrixPlane);
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(plane1.modelMatrix)), normalMatrixPlane);
        
        shader->setUniform("NormalMatrix", normalMatrix);
        shader->setUniform("NormalMatrixPlane", normalMatrixPlane);
        
        vmml::Vector4f eyePos = vmml::Vector4f(0.0f, 0.0f, 10.0f, 1.0f);
        //shader[i]->setUniform("EyePos", eyePos);
        shader->setUniform("EyePos", eyePos);
        shader->setUniform("LightPos", vmml::Vector4f(plane1.modelMatrix.x(), plane1.modelMatrix.y()+25., plane1.modelMatrix.z()-20., 1.));
        shader->setUniform("LightPos2", vmml::Vector4f(1.f, 100.f, 15.5f, 1.f));
        shader->setUniform("Ia", vmml::Vector3f(5.f));
        shader->setUniform("Id", vmml::Vector3f(1.f));
        shader->setUniform("Is", vmml::Vector3f(1.f));
    }
    else
    {
        bRenderer::log("No shader available.");
    }
    
    shader->setUniform("NormalMatrix", vmml::Matrix3f(modelMatrixTerrain));
    bRenderer().getModelRenderer()->drawModel("plane", "camera", plane1.modelMatrix, std::vector<std::string>({ }));
    
    shader = bRenderer().getObjects()->getShader("terrain");
    
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", vmml::Matrix4f::IDENTITY);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("modelMatrixTerrain", modelMatrixTerrain);
        shader->setUniform("ModelMatrix", plane2.modelMatrix);
        
        vmml::Matrix3f normalMatrix;
        vmml::Matrix3f normalMatrixPlane;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrixTerrain)), normalMatrix);
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(plane2.modelMatrix)), normalMatrixPlane);
        
        shader->setUniform("NormalMatrix", normalMatrix);
        shader->setUniform("NormalMatrixPlane", normalMatrixPlane);
        
        vmml::Vector4f eyePos = vmml::Vector4f(0.0f, 0.0f, 10.0f, 1.0f);
        shader->setUniform("EyePos", eyePos);
        shader->setUniform("LightPos", vmml::Vector4f(.5f, 100.f, 30.5f, 1.f));
        shader->setUniform("LightPos2", vmml::Vector4f(1.f, 100.f, 15.5f, 1.f));
        shader->setUniform("Ia", vmml::Vector3f(5.f));
        shader->setUniform("Id", vmml::Vector3f(1.f));
        shader->setUniform("Is", vmml::Vector3f(1.f));
    }
    
    bRenderer().getModelRenderer()->drawModel("terrain", "camera", plane2.modelMatrix, std::vector<std::string>({ }));
    //shader->setUniform("NormalMatrix", vmml::Matrix3f(modelMatrix));
    ObjectManagerPtr _objManager = bRenderer().getModelRenderer()->getObjectManager();

    
}

/* Camera movement */
void RenderProject::updateCamera(const std::string &camera, const double &deltaTime)
{
    //// Adjust aspect ratio ////
    bRenderer().getObjects()->getCamera(camera)->setAspectRatio(bRenderer().getView()->getAspectRatio());
}

/* For iOS only: Handle device rotation */
void RenderProject::deviceRotated()
{
    if (bRenderer().isInitialized()){
        // set view to full screen after device rotation
        bRenderer().getView()->setFullscreen(true);
        bRenderer::log("Device rotated");
    }
}

/* For iOS only: Handle app going into background */
void RenderProject::appWillResignActive()
{
    if (bRenderer().isInitialized()){
        // stop the renderer when the app isn't active
        bRenderer().stopRenderer();
    }
}

/* For iOS only: Handle app coming back from background */
void RenderProject::appDidBecomeActive()
{
    if (bRenderer().isInitialized()){
        // run the renderer as soon as the app is active
        bRenderer().runRenderer();
    }
}

/* For iOS only: Handle app being terminated */
void RenderProject::appWillTerminate()
{
    if (bRenderer().isInitialized()){
        // terminate renderer before the app is closed
        bRenderer().terminateRenderer();
    }
}

/* Helper functions */
GLfloat RenderProject::randomNumber(GLfloat min, GLfloat max){
    return min + static_cast <GLfloat> (rand()) / (static_cast <GLfloat> (RAND_MAX / (max - min)));
}