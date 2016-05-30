#include "RenderProject.h"
#include "GameObject.h"
#include "Car.h"

#ifdef __OBJC__
#import <CoreMotion/CoreMotion.h>
#endif

/* Initialize the Project */
Car car = Car(vmml::Vector3f(1.0f, 1.0f, 1.f), vmml::Vector3f(0.f, 0.f, 0.f),  vmml::Vector3f::UNIT_Z, 0.f);

vmml::Matrix4f checkpointMatrix = vmml::create_translation(vmml::Vector3f(-59.f, 0.f, 10.f)) * vmml::create_rotation((float)(90*M_PI_F/180), vmml::Vector3f::UNIT_Y);

vmml::Matrix4f roadMatrix = vmml::create_translation(vmml::Vector3f(0.f, 0.3f, 10.f)) * vmml::create_scaling(vmml::Vector3f(10.0f, 10.0f, 10.0f)) * vmml::create_rotation((float)(M_PI_F), vmml::Vector3f::UNIT_X) * vmml::create_rotation((float)(90*M_PI_F/180), vmml::Vector3f::UNIT_Y);

vmml::Matrix4f terrainMM = vmml::create_translation(vmml::Vector3f(0.0f, 0.f, 0.f)) * vmml::create_rotation((float)(M_PI_F), vmml::Vector3f::UNIT_X);

vmml::Matrix4f particlesMM = vmml::create_translation(vmml::Vector3f(0.0f, 10.f, 10.f));
vmml::Matrix4f skyMM = vmml::create_translation(vmml::Vector3f(0.0f, 20.f, 0.f)) * vmml::create_scaling(vmml::Vector3f(2000.f));

double _time = 0;
float _pitchSum = 0.0f;
float angle=0.f;
vmml::AABBf aabb2;
vmml::AABBf aabb3;
vmml::AABBf aabb4;
vmml::AABBf aabb5;
vmml::Vector4f eyePos;
vmml::Matrix4f viewMatrix;
FontPtr font;
FontPtr font2;
bool isRunning = false;
float pitch;

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
    _offset = 0.0f;
    _randomOffset = 0.0f;
    _cameraSpeed = 40.0f;
    _running = true; _lastStateSpaceKey = bRenderer::INPUT_UNDEFINED;
    _viewMatrixHUD = Camera::lookAt(vmml::Vector3f(0.0f, 0.0f, 0.25f), vmml::Vector3f::ZERO, vmml::Vector3f::UP);
    
    font = bRenderer().getObjects()->loadFont("dig.ttf", 100);
    font2 = bRenderer().getObjects()->loadFont("Capture_it.ttf", 110);
    
    bRenderer().getObjects()->createTexture("particle_texture", 0.f, 0.f);
    
    // set shader versions (optional)
    bRenderer().getObjects()->setShaderVersionDesktop("#version 120");
    bRenderer().getObjects()->setShaderVersionES("#version 100");
    
    // load materials and shaders before loading the model
    // loadShaderFile(const std::string &shaderName, GLuint shaderMaxLights, bool variableNumberOfLights, bool ambientLighting, bool diffuseLighting, bool specularLighting, bool cubicReflectionMap);
    
    ShaderPtr planeShader = bRenderer().getObjects()->loadShaderFile("plane", 0, false, false, false, false, false);
    ShaderPtr skyShader = bRenderer().getObjects()->loadShaderFile("skybox", 0, false, false, false, false, false);
    
    // load shader from file without lighting, the number of lights won't ever change during rendering (no variable number of lights)
    
    // create additional properties for a model
    PropertiesPtr guyProperties = bRenderer().getObjects()->createProperties("guyProperties");
    PropertiesPtr terrainProperties = bRenderer().getObjects()->createProperties("terrainProperties");
    PropertiesPtr planeProperties = bRenderer().getObjects()->createProperties("planeProperties");
    PropertiesPtr skyProperties = bRenderer().getObjects()->createProperties("skyProperties");
    
    // loadObjModel(const std::string &fileName, bool flipT, bool flipZ, ShaderPtr shader, PropertiesPtr properties)
    // loadObjModel(const std::string &fileName, bool flipT, bool flipZ, bool shaderFromFile, GLuint shaderMaxLights, bool variableNumberOfLights, bool ambientLighting, PropertiesPtr properties)

    car.aabb = bRenderer().getObjects()->loadObjModel("plane.obj", false, true, planeShader, planeProperties)->getBoundingBoxObjectSpace();
    aabb2 = bRenderer().getObjects()->loadObjModel("cp.obj", false, false, true, 0, false, false, terrainProperties)->getBoundingBoxObjectSpace();
    aabb3 = bRenderer().getObjects()->loadObjModel("terrain.obj", false, false, true, 0, false, false, terrainProperties)->getBoundingBoxObjectSpace();
    aabb4 = bRenderer().getObjects()->loadObjModel("road.obj", false, false, true, 0, false, false, terrainProperties)->getBoundingBoxObjectSpace();
    bRenderer().getObjects()->loadObjModel("skybox2.obj", false, true, skyShader, skyProperties);
    
    // create camera
    bRenderer().getObjects()->createCamera("camera");
    
    
    
    /////////    //      //     //    //
    //           //      //     //   //
    //           //      //     //  //
    //           //      //     // //
    ///////      //      //     ////
    //           //      //     // //
    //           //      //     //  //
    //           //////////     //   //
    // postprocessing
    
    bRenderer().getObjects()->createFramebuffer("fbo");
    bRenderer().getObjects()->createFramebuffer("car");					// create framebuffer object
    bRenderer().getObjects()->createTexture("fbo_texture1", 0.f, 0.f);	// create texture to bind to the fbo
    bRenderer().getObjects()->createTexture("fbo_texture2", 0.f, 0.f);	// create texture to bind to the fbo
    ShaderPtr blurShader = bRenderer().getObjects()->loadShaderFile("blurShader", 0, false, false, false, false, false);			// load shader that blurs the texture
    MaterialPtr blurMaterial = bRenderer().getObjects()->createMaterial("blurMaterial", blurShader);								// create an empty material to assign either texture1 or texture2 to
    bRenderer().getObjects()->createSprite("blurSprite", blurMaterial);																// create a sprite using the material created above
    
    ShaderPtr carOnlyShader = bRenderer().getObjects()->loadShaderFile("carShader", 0, false, false, false, false, false);			// load shader that blurs the texture
    MaterialPtr carOnlyMaterial = bRenderer().getObjects()->createMaterial("carMaterial", carOnlyShader);								// create an empty material to assign either texture1 or texture2 to
    bRenderer().getObjects()->createSprite("carSprite", carOnlyMaterial);																// create a sprite using the material created above
    // Update render queue
    updateRenderQueue("camera", 0.0f);
    
    Framebuffer fbo = *new Framebuffer();
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
//    glDisable(GL_CULL_FACE);
    bool boosting = false;
    float pitch = (float)(bRenderer().getInput()->getGyroscopePitch()/10);
    CameraPtr cameraPtr = bRenderer().getObjects()->getCamera("camera");

    GameObject checkpoint = GameObject(checkpointMatrix, aabb2, ObjectType::CHECKPOINT);
    GameObject road = GameObject(roadMatrix, aabb4, ObjectType::FLOOR);
    GameObject terr = GameObject(terrainMM, aabb3, ObjectType::FLOOR);
    
    
    car.clearCollidables();
    car.addCollidable(checkpoint);
    car.addCollidable(road);
    car.addCollidable(terr);
    
    GLint m_viewport[4];
    glGetIntegerv( GL_VIEWPORT, m_viewport);
    
    
    TouchMap touchMap = bRenderer().getInput()->getTouches();
    if(touchMap.empty()){
        car.decelerate();
    }
    else{
        if(!isRunning){
            isRunning = true;
        }
        auto t = touchMap.begin();
        Touch touch = t->second;
        if(touch.lastPositionX > m_viewport[2]/2){
            if(touch.lastPositionY < m_viewport[3] / 2){
                boosting = car.activateBoost();
            }
            else{
                car.accelerate();
            }
        }
        else{
            car.brake();
            std::cout << _time << std::endl;
        }
    }
    
    vmml::Matrix4f rotationY = vmml::create_rotation(-pitch, vmml::Vector3f::UNIT_Y);
    if(car.move(rotationY)){
        showCPPassedText();
    };
    
    GLint defaultFBO = 0;
    
    if(isRunning){
        _time += deltaTime;

        bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());		// reduce viewport size
        defaultFBO = Framebuffer::getCurrentFramebuffer();	// get current fbo to bind it again after drawing the scene
        bRenderer().getObjects()->getFramebuffer("fbo")->bindTexture(bRenderer().getObjects()->getTexture("fbo_texture1"), false);	// bind the fbo
        
    }
    else{
        drawStartText();
    }
    
    if (car.speed < 1){
        pitch = 0.0f;
    } else {
        pitch = (float)(bRenderer().getInput()->getGyroscopePitch()/10);
        _pitchSum += pitch;
    }
    vmml::Vector3f camPosition = vmml::Vector3f(-car.modelMatrix.x() - 4. * sinf(_pitchSum),
                                                -2.0,
                                                -car.modelMatrix.z() + 4. * cosf(_pitchSum));
    cameraPtr->setPosition(camPosition);
    cameraPtr->rotateCamera(0.0f, -pitch, 0.0f);
    viewMatrix = cameraPtr->getViewMatrix();
    
        //draw terrain
    ShaderPtr shader = bRenderer().getObjects()->getShader("terrain");
    
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", vmml::Matrix4f::IDENTITY);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("ModelMatrix", terr.modelMatrix);
        
        vmml::Matrix3f normalMatrix;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(terr.modelMatrix)), normalMatrix);
        shader->setUniform("NormalMatrix", normalMatrix);
        shader->setUniform("EyePos", eyePos);
        shader->setUniform("LightPos", vmml::Vector4f(10.0f, 10.f, 10.f,1.));        shader->setUniform("Ia", vmml::Vector3f(5.f));
        shader->setUniform("Id", vmml::Vector3f(1.f));
        shader->setUniform("Is", vmml::Vector3f(1.f));
    }
    else
    {
        bRenderer::log("No shader available.");
    }
    
    bRenderer().getModelRenderer()->drawModel("terrain", "camera", terr.modelMatrix, std::vector<std::string>({ }));
    
    //draw Checkpoint
    shader = bRenderer().getObjects()->getShader("cp");
    
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", vmml::Matrix4f::IDENTITY);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("ModelMatrix", checkpoint.modelMatrix);
        
        vmml::Matrix3f normalMatrix;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(checkpoint.modelMatrix)), normalMatrix);
        shader->setUniform("NormalMatrix", normalMatrix);
        shader->setUniform("EyePos", eyePos);
        shader->setUniform("LightPos", vmml::Vector4f(10.0f, 10.f, 10.f,1.));
        shader->setUniform("Ia", vmml::Vector3f(5.f));
        shader->setUniform("Id", vmml::Vector3f(1.f));
        shader->setUniform("Is", vmml::Vector3f(1.f));
    }
    else
    {
        bRenderer::log("No shader available.");
    }
    
    bRenderer().getModelRenderer()->drawModel("cp", "camera", checkpoint.modelMatrix, std::vector<std::string>({ }));
    
    //draw road
    shader = bRenderer().getObjects()->getShader("road");
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", vmml::Matrix4f::IDENTITY);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("ModelMatrix", road.modelMatrix);
        
        vmml::Matrix3f normalMatrix;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(road.modelMatrix)), normalMatrix);
        shader->setUniform("NormalMatrix", normalMatrix);
        shader->setUniform("EyePos", eyePos);
        shader->setUniform("LightPos", vmml::Vector4f(10.0f, 10.f, 10.f,1.));
        shader->setUniform("Ia", vmml::Vector3f(5.f));
        shader->setUniform("Id", vmml::Vector3f(1.f));
        shader->setUniform("Is", vmml::Vector3f(1.f));
    }
    else
    {
        bRenderer::log("No shader available.");
    }
    
    bRenderer().getModelRenderer()->drawModel("road", "camera", road.modelMatrix, std::vector<std::string>({ }));

    //draw skybox
    shader = bRenderer().getObjects()->getShader("skybox");
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", vmml::Matrix4f::IDENTITY);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("ModelMatrix", skyMM);
        
        vmml::Matrix3f normalMatrix;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(skyMM)), normalMatrix);
        shader->setUniform("NormalMatrix", normalMatrix);
        shader->setUniform("EyePos", eyePos);
        //        shader->setUniform("LightPos", vmml::Vector4f(.5f, 1.f, 300.5f, 1.f));
        shader->setUniform("Ia", vmml::Vector3f(1.f));
        shader->setUniform("Id", vmml::Vector3f(1.f));
        shader->setUniform("Is", vmml::Vector3f(1.f));
    }
    else
    {
        bRenderer::log("No shader available.");
    }
    
    bRenderer().getModelRenderer()->drawModel("skybox2", "camera", skyMM, std::vector<std::string>({ }));
    if(isRunning){
        if(car.speed > -1){
            // translate
            vmml::Matrix4f modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));

            bRenderer().getObjects()->getFramebuffer("fbo")->unbind(defaultFBO); //unbind (original fbo will be bound)
            bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());								// reset vieport size
            bRenderer().getObjects()->getMaterial("blurMaterial")->setTexture("fbo_texture", bRenderer().getObjects()->getTexture("fbo_texture1"));

            shader = bRenderer().getObjects()->getShader("blurShader");
            float speed = car.speed;
            if(boosting){
                speed = 220.0;
            }
            shader->setUniform("speed", speed);
            bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("blurSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
        }
        std::ostringstream timeString;
        timeString << _time;
        updateTimeText(timeString.str());
        
        std::ostringstream speedString;
        speedString << (int)car.speed << " km/h";
        updateSpeedText(speedString.str());
    }
    
    GLint currentBuffer = Framebuffer::getCurrentFramebuffer();
    bRenderer().getObjects()->getFramebuffer("car")->bindTexture(bRenderer().getObjects()->getTexture("fbo_texture2"), false);	// bind the fbo
    glClearColor(0.0, 0.0, 0.0, 0.0);
    // draw vehicle
    shader = bRenderer().getObjects()->getShader("plane");
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", vmml::Matrix4f::IDENTITY);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("ModelMatrix", car.modelMatrix);
        
        vmml::Matrix3f normalMatrix;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(car.modelMatrix)), normalMatrix);
        shader->setUniform("NormalMatrix", normalMatrix);
        shader->setUniform("EyePos", eyePos);
        shader->setUniform("LightPos", vmml::Vector4f(10.0f, 10.f, 10.f,1.));
        //car.modelMatrix.x(), car.modelMatrix.y()+25., car.modelMatrix.z()-20., 1.));
        shader->setUniform("Ia", vmml::Vector3f(5.f));
        shader->setUniform("Id", vmml::Vector3f(1.f));
        shader->setUniform("Is", vmml::Vector3f(1.f));
    }
    else
    {
        bRenderer::log("No shader available.");
    }
    
    bRenderer().getModelRenderer()->drawModel("plane", "camera", car.modelMatrix, std::vector<std::string>({ }));
    
    vmml::Matrix4f modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5f));
    bRenderer().getObjects()->getFramebuffer("car")->unbind(currentBuffer); //unbind (original fbo will be bound)
    bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());								// reset vieport size
    bRenderer().getObjects()->getMaterial("carMaterial")->setTexture("fbo_texture", bRenderer().getObjects()->getTexture("fbo_texture2"));
    
    shader = bRenderer().getObjects()->getShader("carShader");
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("carSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
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

void RenderProject::showCPPassedText(){
    vmml::Matrix4f textMatrix = vmml::create_scaling(vmml::Vector3f(0.1f)) * vmml::create_translation(vmml::Vector3f(-5.f, 1.f, 0.0f));
    
    bRenderer().getObjects()->removeTextSprite("cpText", true);
    bRenderer().getObjects()->createTextSprite("cpText", vmml::Vector3f(1.f, 1.f, 1.f), "Checkpoint passed", font2);
    vmml::Matrix4f invViewMatrix = bRenderer().getObjects()->getCamera("camera")->getInverseViewMatrix();
    bRenderer().getModelRenderer()->drawText("cpText", "camera", invViewMatrix * textMatrix, std::vector<std::string>({ }), false);
}

void RenderProject::updateTimeText(std::string timeString){
    if(timeString.find(".") == std::string::npos){
        timeString += ".00";
    }
    if(timeString.length() > timeString.find(".") + 3){
        timeString = timeString.substr(0, timeString.find(".") + 3);
    }
    vmml::Matrix4f textMatrix = vmml::create_scaling(vmml::Vector3f(0.1f, 0.1f, 0.1f)) * vmml::create_translation(vmml::Vector3f(4.f, 5.f, 0.0f));
    bRenderer().getObjects()->removeTextSprite("time", true);
    bRenderer().getObjects()->createTextSprite("time", vmml::Vector3f(1.f, 1.f, 1.f), timeString, font);
    vmml::Matrix4f invViewMatrix = bRenderer().getObjects()->getCamera("camera")->getInverseViewMatrix();
    bRenderer().getModelRenderer()->drawText("time", "camera", invViewMatrix * textMatrix, std::vector<std::string>({ }), false);
}


void RenderProject::updateSpeedText(std::string speedString){
    vmml::Matrix4f textMatrix = vmml::create_scaling(vmml::Vector3f(0.1f, 0.1f, 0.1f)) * vmml::create_translation(vmml::Vector3f(4.f, -5.f, 0.0f));
    
    bRenderer().getObjects()->removeTextSprite("speed", true);
    bRenderer().getObjects()->createTextSprite("speed", vmml::Vector3f(1.f, 1.f, 1.f), speedString, font);
    vmml::Matrix4f invViewMatrix = bRenderer().getObjects()->getCamera("camera")->getInverseViewMatrix();
    bRenderer().getModelRenderer()->drawText("speed", "camera", invViewMatrix * textMatrix, std::vector<std::string>({ }), false);
}

void RenderProject::drawStartText(){
    vmml::Matrix4f textMatrix = vmml::create_scaling(vmml::Vector3f(0.1f)) * vmml::create_translation(vmml::Vector3f(-4.f, 1.f, 0.0f));
    
    bRenderer().getObjects()->removeTextSprite("startText", true);
    bRenderer().getObjects()->createTextSprite("startText", vmml::Vector3f(1.f, 1.f, 1.f), "Touch to start", font2);
    vmml::Matrix4f invViewMatrix = bRenderer().getObjects()->getCamera("camera")->getInverseViewMatrix();
    bRenderer().getModelRenderer()->drawText("startText", "camera", invViewMatrix * textMatrix, std::vector<std::string>({ }), false);
}