#include "RenderProject.h"
#include "GameObject.h"

#ifdef __OBJC__
#import <CoreMotion/CoreMotion.h>
#endif

/* Initialize the Project */
vmml::Matrix4f modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, 0.f)) * vmml::create_scaling(vmml::Vector3f(1.f))* vmml::create_rotation((float)(180*M_PI_F/180), vmml::Vector3f::UNIT_Z);
vmml::Matrix4f model2Matrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, 100.f)) * vmml::create_scaling(vmml::Vector3f(1.f))* vmml::create_rotation((float)(180*M_PI_F/180), vmml::Vector3f::UNIT_Z);
double _time = 0;
double _pitchSum;
float angle=0.f;
vmml::AABBf aabb;

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
    ShaderPtr terrainShader = bRenderer().getObjects()->loadShaderFile("terrain", 0, false, false, false, false, false);
    
// load shader from file without lighting, the number of lights won't ever change during rendering (no variable number of lights)
    
    // create additional properties for a model
    PropertiesPtr guyProperties = bRenderer().getObjects()->createProperties("guyProperties");
    PropertiesPtr terrainProperties = bRenderer().getObjects()->createProperties("terrainProperties");
    PropertiesPtr planeProperties = bRenderer().getObjects()->createProperties("planeProperties");
    
    // load model
    
    
    //loadObjModel(const std::string &fileName, bool flipT, bool flipZ, ShaderPtr shader, PropertiesPtr properties);
    //loadObjModel(const std::string &fileName, bool flipT, bool flipZ, bool shaderFromFile, GLuint shaderMaxLights, bool variableNumberOfLights, bool ambientLighting, PropertiesPtr properties);

    
    aabb = bRenderer().getObjects()->loadObjModel("plane.obj", false, true, guyShader, planeProperties)->getBoundingBoxObjectSpace();
    bRenderer().getObjects()->loadObjModel("terrain.obj", false, true, guyShader, terrainProperties);
    //bRenderer().getObjects()->loadObjModel("guy.obj", false, true, guyShader, guyProperties);
    // automatically generates a shader with a maximum of 4 lights (number of lights may vary between 0 and 4 during rendering without performance loss)
    
    // create camera
    bRenderer().getObjects()->createCamera("camera", vmml::Vector3f(0.0f, -30.0f, 300.0f), vmml::Vector3f(0.0f, 0.0f, 0.0f));
    
    // Update render queue
    updateRenderQueue("camera", 0.0f);
}

/* Draw your scene here */
void RenderProject::loopFunction(const double &deltaTime, const double &elapsedTime)
{
    //	bRenderer::log("FPS: " + std::to_string(1 / deltaTime));	// write number of frames per second to the console every frame
    
    //// Draw Scene and do post processing ////
    
    /// Begin post processing ///
    //	GLint defaultFBO;
    //	if (!_running){
    //		bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth() / 5, bRenderer().getView()->getHeight() / 5);		// reduce viewport size
    //		defaultFBO = Framebuffer::getCurrentFramebuffer();	// get current fbo to bind it again after drawing the scene
    //		bRenderer().getObjects()->getFramebuffer("fbo")->bindTexture(bRenderer().getObjects()->getTexture("fbo_texture1"), false);	// bind the fbo
    //	}
    
    
    

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
    _time += deltaTime;
    float angle = _time * 0.9;
//    
    _pitchSum += bRenderer().getInput()->getGyroscopePitch()* 1.0f;
    
    vmml::Matrix4f rotationX = vmml::create_rotation((float)(bRenderer().getInput()->getGyroscopeRoll()/300), vmml::Vector3f::UNIT_X);
    vmml::Matrix4f rotationZ = vmml::create_rotation((float)(bRenderer().getInput()->getGyroscopePitch()/64), vmml::Vector3f::UNIT_Z);
    
    TouchMap touchMap = bRenderer().getInput()->getTouches();
    int i = 0;
    float rotation = 0.0f;
    for (auto t = touchMap.begin(); t != touchMap.end(); ++t)
    {
        Touch touch = t->second;
        rotation = (touch.currentPositionX - touch.startPositionX) / 100;
        if (++i > 1)
            break;
    }
    // get input rotation
    TouchMap touchMap2 = bRenderer().getInput()->getTouches();
    int i2 = 0;
    float rotation2 = 0.0f;
    for (auto t = touchMap2.begin(); t != touchMap2.end(); ++t)
    {
        Touch touch = t->second;
        rotation2 = (touch.currentPositionY - touch.startPositionY) / 100;
        if (++i2 > 1)
            break;
    }
    GameObject plane2 = GameObject(model2Matrix, 50.0f, aabb);
    vmml::Matrix4f viewMatrix = bRenderer().getObjects()->getCamera("camera")->getViewMatrix();
    
    vmml::Matrix4f modelMatrixTerrain = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, 5.5f));
    vmml::Matrix4f rotationMatrixPlane = vmml::create_rotation(rotation, vmml::Vector3f::UNIT_Y);
    rotationMatrixPlane = vmml::create_rotation(rotation2, vmml::Vector3f::UNIT_X);
    
    angle++;
    
    vmml::Vector3f planeChange=vmml::Vector3f(0.f,0.f,angle/50*10.f);
    
    vmml::Matrix4f planeMotion=vmml::create_translation(planeChange);
    modelMatrix *= planeMotion;

    vmml::Matrix4f rotationMatrix = rotationX*rotationZ;
    modelMatrix *= rotationMatrix;
    
    vmml::Vector3f cameraPos=bRenderer().getObjects()->getCamera("camera")->getPosition();

    bRenderer().getObjects()->getCamera("camera")->setRotation(vmml::Vector3f((float)(0*M_PI_F/180),0.f,0.f));
    
    cameraPos=vmml::Vector3f(cameraPos.x()-planeChange.x()+0.0f,-planeChange.y()+cameraPos.y()-0.f,-planeChange.z()+cameraPos.z()+0.f);
    bRenderer().getObjects()->getCamera("camera")->setPosition(cameraPos);
    
    modelMatrixTerrain *= modelMatrix;
    /*** solar system ***/
    ShaderPtr shader = bRenderer().getObjects()->getShader("guy");
    
    //vmml::Matrix4f
    modelMatrix = vmml::create_scaling(vmml::Vector3f(1.0f, 1.0f, 1.0f)) * modelMatrix;;
    //bRenderer().getModelRenderer()->drawModel("plane", "camera", normal2Matrix, std::vector<std::string>({}));ü
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", vmml::Matrix4f::IDENTITY);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("modelMatrixTerrain", modelMatrixTerrain);
        shader->setUniform("ModelMatrix", modelMatrix);
        //shader->setUniform("model2Matrix", normal2Matrix);

        
        vmml::Matrix3f normalMatrix;
        vmml::Matrix3f normalMatrixPlane;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrixTerrain)), normalMatrix);
        //vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(normal2Matrix)), normalMatrixPlane);
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrix)), normalMatrixPlane);
        
        shader->setUniform("NormalMatrix", normalMatrix);
        shader->setUniform("NormalMatrixPlane", normalMatrixPlane);
        
        vmml::Vector4f eyePos = vmml::Vector4f(0.0f, 0.0f, 10.0f, 1.0f);
        //shader[i]->setUniform("EyePos", eyePos);
        shader->setUniform("EyePos", eyePos);
        shader->setUniform("LightPos", vmml::Vector4f(.5f, 100.f, 30.5f, 1.f));
        shader->setUniform("LightPos2", vmml::Vector4f(1.f, 100.f, 15.5f, 1.f));
        shader->setUniform("Ia", vmml::Vector3f(5.f));
        shader->setUniform("Id", vmml::Vector3f(1.f));
        shader->setUniform("Is", vmml::Vector3f(1.f));
        }
        else
        {
        bRenderer::log("No shader available.");
        }
    
        GameObject plane1 = GameObject(modelMatrix, 50.0f, aabb);

    shader->setUniform("NormalMatrix", vmml::Matrix3f(modelMatrixTerrain));
    bRenderer().getModelRenderer()->drawModel("plane", "camera", modelMatrix, std::vector<std::string>({ }));
    
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
        //shader[i]->setUniform("EyePos", eyePos);
        shader->setUniform("EyePos", eyePos);
        shader->setUniform("LightPos", vmml::Vector4f(.5f, 100.f, 30.5f, 1.f));
        shader->setUniform("LightPos2", vmml::Vector4f(1.f, 100.f, 15.5f, 1.f));
        shader->setUniform("Ia", vmml::Vector3f(5.f));
        shader->setUniform("Id", vmml::Vector3f(1.f));
        shader->setUniform("Is", vmml::Vector3f(1.f));
    }
 
    bRenderer().getModelRenderer()->drawModel("plane", "camera", plane2.modelMatrix, std::vector<std::string>({ }));
    //shader->setUniform("NormalMatrix", vmml::Matrix3f(modelMatrix));
    ObjectManagerPtr _objManager = bRenderer().getModelRenderer()->getObjectManager();
    //if(car1aabb.isIn(vmml::Vector3f(0.0f, 0.0f, 20.0f))){
    
    if(plane1.collidesWith(plane2)){
        modelMatrix = vmml::create_translation(vmml::Vector3f(-1.0f, 0.0f, 0.0f)) * modelMatrix;
        std::cerr << "COLLISION DETECTED" <<  std::endl;
    }
    
}

/* Camera movement */
void RenderProject::updateCamera(const std::string &camera, const double &deltaTime)
{
    //// Adjust aspect ratio ////
    bRenderer().getObjects()->getCamera(camera)->setAspectRatio(bRenderer().getView()->getAspectRatio());
    
//    
//    if(carIsMoving){
//        vmml::Vector3f currentPos = bRenderer().getObjects()->getCamera(camera)->getPosition();
//        vmml::Vector3f newPos;
//        if(currentPos.z() < -50)
//            newPos = vmml::Vector3f(0.0f, -20.0f, 100.0f);
//        else
//            newPos = bRenderer().getObjects()->getCamera(camera)->getPosition() + (float)deltaTime * velocity;
//        bRenderer().getObjects()->getCamera(camera)->setPosition(newPos);
//    }
    
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