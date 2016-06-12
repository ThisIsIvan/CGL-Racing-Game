#include "RenderProject.h"
#include "GameObject.h"
#include "Car.h"
#include <string>
#include <math.h>

#ifdef __OBJC__
#import <CoreMotion/CoreMotion.h>
#endif

/* Initialize the Project */
Car car = Car(vmml::Vector3f(1.0f, 1.0f, 1.f), vmml::Vector3f(0.f, 0.f, 0.f),  vmml::Vector3f::UNIT_Z, 0.f);

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
    _viewMatrixHUD = Camera::lookAt(vmml::Vector3f(0.0f, 0.0f, 0.25f), vmml::Vector3f::ZERO, vmml::Vector3f::UP);
    _time = 0;
    countDown = 3.0;
    _pitchSum = 0.0f;
    isRunning = false;
    isActivated = false;
    gameStart = true;
    font = bRenderer().getObjects()->loadFont("dig.ttf", 100);
    font2 = bRenderer().getObjects()->loadFont("CloseRace.ttf", 100);
    
    srand((unsigned)time(0));
    for(int i = 0; i < 40; i++){
        cloud_x[i] = randomNumber(-400., 400.);
        cloud_z[i] = randomNumber(-400., 400.);
        cloud_size[i] = randomNumber(3., 15.);
        cloud_speed[i] = randomNumber(0.001, 0.02);
    }
    
    checkpointMatrix = vmml::create_translation(vmml::Vector3f(-59.f, -0.1f, 10.f)) * vmml::create_rotation((float)(90*M_PI_F/180), vmml::Vector3f::UNIT_Y);
    roadMatrix = vmml::create_translation(vmml::Vector3f(0.f, 0.0f, 10.f)) * vmml::create_scaling(vmml::Vector3f(2.0f, 2.0f, 2.0f)) * vmml::create_rotation((float)(M_PI_F), vmml::Vector3f::UNIT_X) * vmml::create_rotation((float)(90*M_PI_F/180), vmml::Vector3f::UNIT_Y);
    terrainMM = vmml::create_translation(vmml::Vector3f(0.0f, 0.f, 0.f)) * vmml::create_rotation((float)(M_PI_F), vmml::Vector3f::UNIT_X);
    particlesMM = vmml::create_translation(vmml::Vector3f(0.0f, 10.f, 10.f));
    skyMM = vmml::create_translation(vmml::Vector3f(0.0f, 20.f, 0.f)) * vmml::create_scaling(vmml::Vector3f(2000.f));
    
    // set shader versions (optional)
    bRenderer().getObjects()->setShaderVersionDesktop("#version 120");
    bRenderer().getObjects()->setShaderVersionES("#version 100");
    
    // load materials and shaders before loading the model
    ShaderPtr planeShader = bRenderer().getObjects()->loadShaderFile("plane", 0, false, false, false, false, false);
    ShaderPtr skyShader = bRenderer().getObjects()->loadShaderFile("skybox", 0, false, false, false, false, false);
    ShaderPtr planeShadowShader = bRenderer().getObjects()->loadShaderFile("planeShadow", 0, false, false, false, false, false);
    ShaderPtr cpShadowShader = bRenderer().getObjects()->loadShaderFile("cpShadow", 0, false, false, false, false, false);
    ShaderPtr guyShader = bRenderer().getObjects()->loadShaderFile("guy", 0, false, false, false, false, false);
    
    // load models
    car.aabb = bRenderer().getObjects()->loadObjModel("plane.obj", false, true, planeShader, nullptr)->getBoundingBoxObjectSpace();
    aabb2 = bRenderer().getObjects()->loadObjModel("cp.obj", false, false, true, 0, false, false, nullptr)->getBoundingBoxObjectSpace();
    aabb3 = bRenderer().getObjects()->loadObjModel("terrain.obj", false, false, true, 0, false, false, nullptr)->getBoundingBoxObjectSpace();
    aabb4 = bRenderer().getObjects()->loadObjModel("road.obj", false, true, true, 0, false, true, nullptr)->getBoundingBoxObjectSpace();
    bRenderer().getObjects()->loadObjModel("skybox.obj", false, true, skyShader, nullptr);
    bRenderer().getObjects()->loadObjModel("planeShadow.obj", false, true, planeShadowShader, nullptr);
    bRenderer().getObjects()->loadObjModel("cpshadow.obj", false, true, cpShadowShader, nullptr);
    bRenderer().getObjects()->loadObjModel("clouds.obj", false, true, guyShader, nullptr);
    
    // create camera
    cameraPtr = bRenderer().getObjects()->createCamera("camera");
    
    // postprocessing
    // create framebuffer objects
    bRenderer().getObjects()->createFramebuffer("fbo");
    bRenderer().getObjects()->createFramebuffer("car");
    bRenderer().getObjects()->createFramebuffer("glow");
    bRenderer().getObjects()->createFramebuffer("glow2");
    // create textures
    bRenderer().getObjects()->createTexture("particle_texture", 0.f, 0.f);
    bRenderer().getObjects()->createTexture("fbo_texture1", 0.f, 0.f);
    bRenderer().getObjects()->createTexture("fbo_texture2", 0.f, 0.f);
    bRenderer().getObjects()->createTexture("glow_texture", 0.f, 0.f);
    bRenderer().getObjects()->createTexture("glow2_texture", 0.f, 0.f);
    // BLUR material, shader, texture
    ShaderPtr blurShader = bRenderer().getObjects()->loadShaderFile("blurShader", 0, false, false, false, false, false);
    MaterialPtr blurMaterial = bRenderer().getObjects()->createMaterial("blurMaterial", blurShader);
    bRenderer().getObjects()->createSprite("blurSprite", blurMaterial);
    // CAR material, shader, texture
    ShaderPtr carOnlyShader = bRenderer().getObjects()->loadShaderFile("carShader", 0, false, false, false, false, false);
    MaterialPtr carOnlyMaterial = bRenderer().getObjects()->createMaterial("carMaterial", carOnlyShader);
    bRenderer().getObjects()->createSprite("carSprite", carOnlyMaterial);
    // GLOW material, shader, texture
    ShaderPtr glowShader = bRenderer().getObjects()->loadShaderFile("glowShader", 0, false, false, false, false, false);
    MaterialPtr glowMaterial = bRenderer().getObjects()->createMaterial("glowMaterial", glowShader);
    bRenderer().getObjects()->createSprite("glowSprite", glowMaterial);
    // GLOWBLOOM material, shader, texture
    ShaderPtr glow2Shader = bRenderer().getObjects()->loadShaderFile("carBlurShader", 0, false, false, false, false, false);
    MaterialPtr glow2Material = bRenderer().getObjects()->createMaterial("glow2Material", glow2Shader);
    bRenderer().getObjects()->createSprite("glow2Sprite", glow2Material);
    
    text = "Touch To Start";
    
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
    GameObject checkpoint = GameObject(checkpointMatrix, aabb2, ObjectType::CHECKPOINT);
    GameObject road = GameObject(roadMatrix, aabb4, ObjectType::FLOOR);
    GameObject terr = GameObject(terrainMM, aabb3, ObjectType::FLOOR);
    
    car.clearCollidables();
    car.addCollidable(checkpoint);
    //    car.addCollidable(road);
    car.addCollidable(terr);
    
    GLint m_viewport[4];
    glGetIntegerv( GL_VIEWPORT, m_viewport);
    
    TouchMap touchMap = bRenderer().getInput()->getTouches();
    
    bool braking = false;
    bool cpPassed = false;
    
    // User Inputs
    if(!isRunning){
        if(!isActivated && !touchMap.empty()){
            isActivated = true;
        }
            if(isActivated){
                countDown -= deltaTime;
                isRunning = countDown <= 0.0;
            }
        }
    else{
        if(touchMap.empty()){
            car.decelerate();
        } else {
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
                braking = true;
                car.brake();
            }
        }
    }
    
    GLint defaultFBO = 0;
    
    if(isRunning){
        _time += deltaTime;
        bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());
        defaultFBO = Framebuffer::getCurrentFramebuffer();	// get current fbo to bind it again after drawing the scene
        bRenderer().getObjects()->getFramebuffer("fbo")->bindTexture(bRenderer().getObjects()->getTexture("fbo_texture1"), false);
    }
    
    if (car.speed < 1){
        car.boost = 0.;
        pitch = 0.0f;
    } else {
        pitch = (float)(bRenderer().getInput()->getGyroscopePitch()/10);
        _pitchSum += pitch;
    }
    
    vmml::Matrix4f rotationY = vmml::create_rotation(-pitch, vmml::Vector3f::UNIT_Y);
    cpPassed = car.move(rotationY);
    camPosition = vmml::Vector3f(-car.modelMatrix.x() - 4. * sinf(_pitchSum),
                                 -2.0,
                                 -car.modelMatrix.z() + 4. * cosf(_pitchSum));
    cameraPtr->rotateCamera(0.0f, -pitch, 0.0f);
    cameraPtr->setPosition(camPosition);
    viewMatrix = cameraPtr->getViewMatrix();
    
    drawRoad(road);
    drawTerrain(terr);
    drawCheckpoint(checkpoint);
    drawShadow();
    drawSkybox(skyMM);
    drawClouds(deltaTime);
    
    if(!isRunning && !isActivated){
        drawStandingsText();
    }
    if(!isRunning && isActivated){
        drawCountdown();
        drawStandingsText();
    }
    if(isRunning){
        vmml::Matrix4f modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
        
        bRenderer().getObjects()->getFramebuffer("fbo")->unbind(defaultFBO); //unbind (original fbo will be bound)
        bRenderer().getObjects()->getMaterial("blurMaterial")->setTexture("fbo_texture", bRenderer().getObjects()->getTexture("fbo_texture1"));
        
        ShaderPtr shader = bRenderer().getObjects()->getShader("blurShader");
        float speed = car.speed;
        if(boosting){
            speed = 220.0;
        }
        
        shader->setUniform("speed", speed);
        bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("blurSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
        std::ostringstream timeString;
        timeString << _time;
        updateTimeText(timeString.str());
        
        std::ostringstream speedString;
        speedString << (int)car.speed << " km/h";
        updateSpeedText(speedString.str());
        drawStandingsText();
        renderCar(car, braking, defaultFBO);
        if(cpPassed){
            showCPPassedText();
            if(_time > 5.0){
                roundTimes[roundCounter] = _time;
                roundCounter++;
                _time = 0.0;
                if(roundCounter == 3){
                    resetGame();
                    text = "Touch To Restart";
                    gameStart = true;
                }
            }
        }
        glBlendFunc(GL_ONE, GL_ONE);
        renderBloomEffect(car, braking, defaultFBO);
        glBlendFunc(GL_ONE, GL_ZERO);
    }
    else{
        if(!isActivated){
            drawText(text, gameStart);
        }
        drawCar(car, false);
    }
    
    if(car.modelMatrix.y() < -1){
        resetGame();
        text = "You drove off the map";
        gameStart = false;
    }
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
float RenderProject::randomNumber(float min, float max){
    return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
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

void RenderProject::drawText(std::string text, bool start){
    float base_x;
    
    if(start) base_x = -4.f;
    else base_x = -5.5f;
    
    vmml::Matrix4f textMatrix = vmml::create_scaling(vmml::Vector3f(0.1f)) * vmml::create_translation(vmml::Vector3f(base_x, 1.f, 0.0f));
    
    bRenderer().getObjects()->removeTextSprite("startText", true);
    bRenderer().getObjects()->createTextSprite("startText", vmml::Vector3f(1.f, 1.f, 1.f), text, font2);
    vmml::Matrix4f invViewMatrix = bRenderer().getObjects()->getCamera("camera")->getInverseViewMatrix();
    bRenderer().getModelRenderer()->drawText("startText", "camera", invViewMatrix * textMatrix, std::vector<std::string>({ }), false);
}

void RenderProject::drawStandingsText(){
    vmml::Matrix4f textMatrix = vmml::create_scaling(vmml::Vector3f(0.05f)) * vmml::create_translation(vmml::Vector3f(-15.f, 10.f, 0.0f));
    std::string results[3] = {"--.--", "--.--", "--.--"};
    for(int i = 0; i < 3; i++){
        if(roundTimes[i] > 0.0){
            std::ostringstream timeOStr;
            timeOStr << roundTimes[i];
            std::string timeString = timeOStr.str();
            
            if(timeString.find(".") == std::string::npos){
                timeString += ".00";
            }
            if(timeString.length() > timeString.find(".") + 3){
                timeString = timeString.substr(0, timeString.find(".") + 3);
            }
            results[i] = timeString;
        }
    }
    
    
    bRenderer().getObjects()->removeTextSprite("startText", true);
    bRenderer().getObjects()->createTextSprite("startText", vmml::Vector3f(1.f, 1.f, 1.f), "1: " + results[0] + "\n2: " + results[1] + "\n3: " + results[2], font);
    vmml::Matrix4f invViewMatrix = bRenderer().getObjects()->getCamera("camera")->getInverseViewMatrix();
    bRenderer().getModelRenderer()->drawText("startText", "camera", invViewMatrix * textMatrix, std::vector<std::string>({ }), false);
}


void RenderProject::drawCountdown(){
    int number = (int)countDown + 1;
    float scaleFactor = (float)countDown - (float)(number-1);
    vmml::Matrix4f textMatrix = vmml::create_scaling(vmml::Vector3f(0.5f*scaleFactor)) * vmml::create_translation(vmml::Vector3f(0.f));
    
    std::string numberString = std::to_string(number);
    
    bRenderer().getObjects()->removeTextSprite("startText", true);
    bRenderer().getObjects()->createTextSprite("startText", vmml::Vector3f(0.f, 0.f, 0.f), numberString, font2);
    vmml::Matrix4f invViewMatrix = bRenderer().getObjects()->getCamera("camera")->getInverseViewMatrix();
    bRenderer().getModelRenderer()->drawText("startText", "camera", invViewMatrix * textMatrix, std::vector<std::string>({ }), false);
    
    roundTimes[0] = 0.0;
    roundTimes[1] = 0.0;
    roundTimes[2] = 0.0;
}
// Draw Functions
void RenderProject::drawShadow(){
    vmml::Matrix4f shadowMatrix = car.modelMatrix;
    shadowMatrix *= vmml::create_scaling(vmml::Vector3f(1.f, 0.1f, 1.f));
    
    vmml::Matrix4f tempMatrix;
    ShaderPtr shader;
    float angle_x = sinf(_pitchSum);
    float angle_z = cosf(_pitchSum);
    float base_x = 0.04;
    float base_z = 0.08;
    float offset_z = 0.35;
    float inc_val = 0.05;
    float base_y = car.modelMatrix.get_translation().y()+0.2;
    
    for(int i = 0; i < 9; i++){
        tempMatrix = shadowMatrix * vmml::create_translation(vmml::Vector3f(base_x * angle_x, base_y, offset_z + base_z * angle_z));
        shader = setShaderUniforms("planeShadow", tempMatrix, false);
        bRenderer().getModelRenderer()->drawModel("planeShadow", "camera", tempMatrix, std::vector<std::string>({ }));
        base_x += inc_val;
        base_z += inc_val;
    }
    
    tempMatrix = checkpointMatrix * vmml::create_translation(vmml::Vector3f(0.f, 0.2f, 0.f));
    shader = setShaderUniforms("cpShadow", tempMatrix, false);
    bRenderer().getModelRenderer()->drawModel("cpshadow", "camera", tempMatrix, std::vector<std::string>({ }));
    
}

void RenderProject::drawTerrain(GameObject terr){
    ShaderPtr shader = setShaderUniforms("terrain", terr.modelMatrix, true);
    bRenderer().getModelRenderer()->drawModel("terrain", "camera", terr.modelMatrix, std::vector<std::string>({ }));
}

void RenderProject::drawCheckpoint(GameObject checkpoint){
    ShaderPtr shader = setShaderUniforms("cp", checkpoint.modelMatrix, false);
    bRenderer().getModelRenderer()->drawModel("cp", "camera", checkpoint.modelMatrix, std::vector<std::string>({ }));
}

void RenderProject::drawRoad(GameObject road){
    ShaderPtr shader = setShaderUniforms("road", road.modelMatrix, true);
    bRenderer().getModelRenderer()->drawModel("road", "camera", road.modelMatrix, std::vector<std::string>({ }));
}

void RenderProject::drawSkybox(vmml::Matrix4f skyMM){
    ShaderPtr shader = setShaderUniforms("skybox", skyMM, false);
    bRenderer().getModelRenderer()->drawModel("skybox", "camera", skyMM, std::vector<std::string>({ }));
}

void RenderProject::drawClouds(double deltaTime){
    vmml::Matrix4f mm;
    for(int i = 0; i < 40; i++){
        cloud_x[i] = cloud_x[i] + cloud_speed[i];
        cloud_z[i] = cloud_z[i] + cloud_speed[i];
        mm = vmml::create_translation(vmml::Vector3f(cloud_x[i], 60.f, cloud_z[i])) * vmml::create_scaling(vmml::Vector3f(cloud_size[i]));
        ShaderPtr shader = setShaderUniforms("guy", mm, true);
        bRenderer().getModelRenderer()->drawModel("clouds", "camera", mm, std::vector<std::string>({ }));
    }
}

void RenderProject::drawCar(Car car, bool braking){
    ShaderPtr shader = setShaderUniforms("plane", car.modelMatrix, true);
    shader->setUniform("braking", braking);
    bRenderer().getModelRenderer()->drawModel("plane", "camera", car.modelMatrix, std::vector<std::string>({ }));
}

void RenderProject::renderBloomEffect(Car car, bool braking, GLint defaultFBO){
    if(braking){
        bRenderer().getObjects()->getFramebuffer("glow2")->bindTexture(bRenderer().getObjects()->getTexture("glow2_texture"), false);
        GLint glow2FBO = Framebuffer::getCurrentFramebuffer();
        bRenderer().getObjects()->getFramebuffer("glow")->bindTexture(bRenderer().getObjects()->getTexture("glow_texture"), false);	// bind the fbo
        glClearColor(0.0, 0.0, 0.0, 0.0);
        // draw vehicle
        drawCar(car, braking);
        
        vmml::Matrix4f glowMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5f));
        bRenderer().getObjects()->getFramebuffer("glow")->unbind(glow2FBO); //unbind (original fbo will be bound)
        bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());								// reset vieport size
        bRenderer().getObjects()->getMaterial("glowMaterial")->setTexture("fbo_texture", bRenderer().getObjects()->getTexture("glow_texture"));
        
        ShaderPtr shader = bRenderer().getObjects()->getShader("glowShader");
        bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("glowSprite"), glowMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
        
        bRenderer().getObjects()->getFramebuffer("glow2")->unbind(defaultFBO); //unbind (original fbo will be bound)
        bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());								// reset vieport size
        bRenderer().getObjects()->getMaterial("glow2Material")->setTexture("fbo_texture", bRenderer().getObjects()->getTexture("glow2_texture"));
        
        shader = bRenderer().getObjects()->getShader("carBlurShader");
        shader->setUniform("speed", car.speed);
        bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("glow2Sprite"), glowMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
        
    }
    
}

void RenderProject::renderCar(Car car, bool braking, GLint defaultFBO){
    bRenderer().getObjects()->getFramebuffer("car")->bindTexture(bRenderer().getObjects()->getTexture("fbo_texture2"), false);	// bind the fbo
    glClearColor(0.0, 0.0, 0.0, 0.0);
    
    drawCar(car, braking);
    
    vmml::Matrix4f modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5f));
    bRenderer().getObjects()->getFramebuffer("car")->unbind(defaultFBO); //unbind (original fbo will be bound)
    bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());								// reset vieport size
    bRenderer().getObjects()->getMaterial("carMaterial")->setTexture("fbo_texture", bRenderer().getObjects()->getTexture("fbo_texture2"));
    
    ShaderPtr shader = bRenderer().getObjects()->getShader("carShader");
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("carSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
}

ShaderPtr RenderProject::setShaderUniforms(std::string shaderName, vmml::Matrix4f modelMatrix, bool illumination){
    ShaderPtr shader = bRenderer().getObjects()->getShader(shaderName);
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", vmml::Matrix4f::IDENTITY);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("ModelMatrix", modelMatrix);
        
        if (illumination){
            vmml::Matrix3f normalMatrix;
            vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrix)), normalMatrix);
            shader->setUniform("NormalMatrix", normalMatrix);
            shader->setUniform("EyePos", cameraPtr->getPosition());
            shader->setUniform("LightPos", vmml::Vector4f(0.0f, 300.f, 100.f, 1.));
            shader->setUniform("Ia", vmml::Vector3f(1.f));
            shader->setUniform("Id", vmml::Vector3f(1.f));
            shader->setUniform("Is", vmml::Vector3f(1.f));
        }
    }
    else
    {
        bRenderer::log("No shader available.");
    }
    return shader;
}

void RenderProject::resetGame(){
    _time = 0;
    countDown = 3.0;
    cameraPtr->resetCamera();
    _pitchSum = 0.0f;
    isRunning = false;
    isActivated = false;
    pitch = 0.0;
    roundCounter = 0;
    car.reset();
}
