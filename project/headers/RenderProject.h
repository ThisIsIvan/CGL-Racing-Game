#ifndef PROJECT_MAIN_H
#define PROJECT_MAIN_H

#include "bRenderer.h"
#include "GameObject.h"
#include "Car.h"

class RenderProject : public IRenderProject
{
public:
	/* Constructor and Destructor */
	RenderProject() : IRenderProject(){}
    virtual ~RenderProject(){bRenderer::log("RenderProject deleted");}

    /* Initialize the Project */
    void init();
    
	//// RenderProject functions ////

	/* This function is executed when initializing the renderer */
	void initFunction();

	/* Draw your scene here */
	void loopFunction(const double &deltaTime, const double &elapsedTime);

	/* This function is executed when terminating the renderer */
	void terminateFunction();

	//// iOS specific ////

    /* For iOS only: Handle device rotation */
    void deviceRotated();
    
    /* For iOS only: Handle app going into background */
    void appWillResignActive();
    
    /* For iOS only: Handle app coming back from background */
    void appDidBecomeActive();
    
    /* For iOS only: Handle app being terminated */
    void appWillTerminate();
    
	/* Make renderer accessible to public so we can get the UIView on iOS */
    Renderer& getProjectRenderer()
    {
        return bRenderer();
    }
    
    void showCPPassedText();
    
private:
    
    // draw sphere at origin of current reference frame
    void drawSphere();
    
	/* Update render queue */
	void updateRenderQueue(const std::string &camera, const double &deltaTime);

	/* Camera movement */
	void updateCamera(const std::string &camera, const double &deltaTime);

	/* Helper Functions */
	GLfloat randomNumber(GLfloat min, GLfloat max);
    
    void updateTimeText(std::string);
    void updateSpeedText(std::string);
//    void drawStartText();
    void drawText(std::string, bool);
    void drawStandingsText();
    void drawCountdown();
    
    // Draw functions
    void drawTerrain(GameObject);
    void drawCheckpoint(GameObject);
    void drawRoad(GameObject);
    void drawSkybox(vmml::Matrix4f);
    void drawCar(Car, bool);
    void drawShadow();
    
    // PostProcessing
    void renderBloomEffect(Car, bool, GLint);
    void renderCar(Car, bool, GLint);
    ShaderPtr setShaderUniforms(std::string, vmml::Matrix4f, bool);
    
    void resetGame();
    
	/* Variables */
	vmml::Matrix4f _viewMatrixHUD;
    
    double _time;
    double countDown;
    float _pitchSum;
    vmml::AABBf aabb2;
    vmml::AABBf aabb3;
    vmml::AABBf aabb4;
    vmml::AABBf aabb5;
    vmml::Vector4f eyePos;
    vmml::Matrix4f viewMatrix;
    FontPtr font;
    FontPtr font2;
    bool isRunning;
    bool isActivated;
    bool gameStart;
    float pitch;
    double roundTimes[3];
    int roundCounter;
    vmml::Vector3f camPosition;
    CameraPtr cameraPtr;
    vmml::Matrix4f checkpointMatrix;
    vmml::Matrix4f roadMatrix;
    vmml::Matrix4f terrainMM;
    vmml::Matrix4f particlesMM;
    vmml::Matrix4f skyMM;

};

#endif /* defined(PROJECT_MAIN_H) */
