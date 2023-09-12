#define OLC_PGE_APPLICATION
#include <iomanip>
using namespace std;

#include "olcPixelGameEngine.h"
#include "track.h"

class HangOn : public olc::PixelGameEngine {
    public:
        HangOn() {
            sAppName = "BikeRacer";
        }
    
    private:
        float fBikePos = 0.0f;
        float fBikeDistance = 0.0f;
        float fCurvature = 0.0f;
        float fSpeed = 0.0f;
        float fTrackCurvature = 0.0f;
        float fPlayerCurvature = 0.0f;
        float fTrackDistance = 0.0f;
        float fCurrentLapTime = 0.0f;
        float fRemainingTime = 60.0f;
        int nBikeDirection = 0;
        bool bIsBraking = false;
        bool bIsTurbo = false;
        bool bIncreaseTime = true;
        bool bGameOver = false;
        vector<pair<float, pair<float,bool>>> vecTrack; //Curvature, Distance
        

    protected:
        virtual bool OnUserCreate(){
            //Create the Track
            makeTrack(vecTrack, 100);
            fTrackDistance = getTrackDistance(vecTrack);
            return true;
        }

        virtual bool OnUserUpdate(float fElapsedTime) {
            if (!bGameOver){

                //----BIKE CONTROL----//
                bIsBraking = false;
                bIsTurbo = false;
                //UserInput accelerate
                if (GetKey(olc::Key::UP).bHeld) {
                    fSpeed += 0.8f * fElapsedTime;
                } else {
                    fSpeed -= 0.5f * fElapsedTime;
                }
                //Prevent the speed to going negative
                if (fSpeed < 0.0f) fSpeed = 0.0f;
                
                //UserInput Turbo
                if (GetKey(olc::Key::SPACE).bHeld) {
                    fSpeed += 1.5f * fElapsedTime;
                    bIsTurbo = true;
                    if (fSpeed > 2.2f) fSpeed = 2.2f;
                } else {
                    if (fSpeed > 1.8f) fSpeed = 1.8f;
                }

                //User Input Brake
                if(GetKey(olc::Key::DOWN).bHeld && fSpeed > 0.0f) {
                    fSpeed -= 1.5f * fElapsedTime;
                    bIsBraking = true;
                } 

                // UserInput turn
                nBikeDirection = 0;
                if (GetKey(olc::Key::LEFT).bHeld && fSpeed > 0.0f) {
                    fPlayerCurvature -= 0.7f * fElapsedTime;
                    nBikeDirection = -1;
                }
                if (GetKey(olc::Key::RIGHT).bHeld && fSpeed > 0.0f) {
                    fPlayerCurvature += 0.7f * fElapsedTime;
                    nBikeDirection = 1;
                }

                //Control if the player stay in the road. 
                //If not the speed is decreased 
                if (fabs(fPlayerCurvature - fTrackCurvature) >= 0.78f) {
                    fSpeed -= 5.0f * fElapsedTime;
                } else if (fabs(fPlayerCurvature - fTrackCurvature) >= 0.60f) {
                    fSpeed -= 1.0f * fElapsedTime;
                }
                if (fSpeed < 0.0f) fSpeed = 0.0f;

                //----TIME AND POSITION CONTROL----//
                //Lap Timing
                fCurrentLapTime += fElapsedTime;

                //Move Bike along the track
                fBikeDistance += (70.0f * fSpeed) * fElapsedTime;

                //Get Point on Track
                float fOffset = 0;
                int nTrackSection = 0;

                // Find position on track
                while (nTrackSection < vecTrack.size() && fOffset <= fBikeDistance) {
                    fOffset += vecTrack[nTrackSection].second.first;
                    nTrackSection++;
                }

                //Set the target curvature and player curvature
                float fTargetCurvature = vecTrack[nTrackSection - 1].first;
                float fTrackCurveDiff = (fTargetCurvature - fCurvature) * fElapsedTime * fSpeed;
                                
                fCurvature += fTrackCurveDiff;

                fTrackCurvature += fCurvature * fElapsedTime * fSpeed;

                //Set RemainingTime
                fRemainingTime -= fElapsedTime;
                //If player go through a CheckPoint the time is incremented in 10 seconds
                if(vecTrack[nTrackSection-1].second.second && bIncreaseTime) {
                    fRemainingTime += 10.0f;
                    bIncreaseTime = false;
                } 
                if(!vecTrack[nTrackSection-1].second.second ) {
                    bIncreaseTime = true;
                }

                //----DISPLAY SECTION----//
                //Clean the Screen
                FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::DARK_BLUE);
                

                //Draw Sky
                for (int y = 0; y < ScreenHeight()/2; y++) {
                    for (int x = 0; x < ScreenWidth(); x++){
                        if( y > ScreenHeight() / 4) {
                            Draw(x,y,olc::BLUE);
                        }
                    }
                }

                //Draw Scenary
                for (int x = 0; x < ScreenWidth(); x++) {
                    int nHillHeight = (int)(fabs(sinf(x*0.01f + fTrackCurvature) * 25.0f));
                    for (int y = ScreenHeight()/2 - nHillHeight; y < ScreenHeight()/2; y++){
                        Draw(x,y,olc::DARK_YELLOW);
                    }
                }

                // Draw the Field
                for (int y = 0; y < ScreenHeight() / 2; y++ ) {
                    for (int x = 0; x < ScreenWidth(); x++) {
                        
                        float fPerspective = (float)y / (ScreenHeight()/2.0f);

                        float fMiddlePoint = 0.5f + fCurvature * powf((1.0f - fPerspective), 3); 
                        float fRoadWidth = 0.1f + fPerspective * 0.8f;
                        float fClipWidth = fRoadWidth * 0.15f;

                        fRoadWidth *= 0.5f;
                        int nLeftGrass = (fMiddlePoint - fRoadWidth - fClipWidth) * ScreenWidth();
                        int nLeftClip = (fMiddlePoint - fRoadWidth) * ScreenWidth();
                        int nRightClip = (fMiddlePoint + fRoadWidth) * ScreenWidth();
                        int nRightGrass = (fMiddlePoint + fRoadWidth + fClipWidth) * ScreenWidth();

                        int nRow = ScreenHeight() / 2 + y;

                        int nSoilColour = sinf(20.0f * powf(1.0f - fPerspective, 3) + fBikeDistance * 0.3f) > 0.0f ? 0 : 1;
                        int nClipColour = sinf(80.0f * powf(1.0f - fPerspective, 2) + fBikeDistance * 0.8f) > 0.0f ? 0 : 1;
                        int nCheckPoint = sinf(20.0f * powf(1.0f - fPerspective, 3) + fBikeDistance * 0.8f) > 0.0f ? 0 : 1;


                        if (x >= 0 && x < nLeftGrass) {
                            if (nSoilColour == 0) {
                                Draw(x, nRow, olc::GREEN);
                            } else {
                                Draw(x, nRow, olc::DARK_GREEN);
                            }
                        } else if (x >= nLeftGrass && x < nLeftClip){
                            if (nClipColour == 0) {
                                if (vecTrack[nTrackSection-1].second.second) {
                                    Draw(x, nRow, olc::YELLOW);
                                } else {
                                    Draw(x, nRow, olc::RED);
                                }
                            } else {
                                Draw(x, nRow, olc::WHITE);
                            }
                        } else if (x >= nLeftClip && x < nRightClip) {
                            if (nSoilColour == 0) {
                                if (nCheckPoint == 0 && vecTrack[nTrackSection-1].second.second) {
                                    Draw(x, nRow, olc::YELLOW);
                                } else {
                                    Draw(x, nRow, olc::GREY);
                                }
                            } else {
                                Draw(x, nRow, olc::DARK_GREY);
                            }
                            if (vecTrack[nTrackSection-1].second.second) {
                                Draw(x, nRow, olc::WHITE);
                            }
                        } else if (x >= nRightClip && x < nRightGrass) {
                            if (nClipColour == 0) {
                                if (vecTrack[nTrackSection-1].second.second) {
                                    Draw(x, nRow, olc::YELLOW);
                                } else {
                                    Draw(x, nRow, olc::RED);
                                }
                            } else {
                                Draw(x, nRow, olc::WHITE);
                            }
                        } else if (x >= nRightGrass && x < ScreenWidth()) {
                            if (nSoilColour == 0) {
                                Draw(x, nRow, olc::GREEN);
                            } else {
                                Draw(x, nRow, olc::DARK_GREEN);
                            }
                        }
                    }
                }


                //Draw the Bike
                fBikePos = fPlayerCurvature - fTrackCurvature;
                int nBikePosX = ScreenWidth()/2 + ((int)(ScreenWidth() * fBikePos) / 2.0f);
                int nBikePosY = (int)(ScreenHeight()*0.75f);

                switch (nBikeDirection) {
                    case 0:
                            FillRect(nBikePosX-4,nBikePosY-16,9,4, olc::DARK_BLUE);
                            FillRect(nBikePosX-4,nBikePosY-12,9,12, olc::BLUE);
                            FillRect(nBikePosX-8,nBikePosY-8,4,8, olc::DARK_BLUE);
                            FillRect(nBikePosX+5,nBikePosY-8,4,8, olc::DARK_BLUE);
                            break;
                    case +1:
                            FillRect(nBikePosX+4,nBikePosY-16,8,4, olc::DARK_BLUE);
                            FillRect(nBikePosX+4,nBikePosY-12,8,4, olc::BLUE);
                            FillRect(nBikePosX-4,nBikePosY-8,4,8, olc::DARK_BLUE);
                            FillRect(nBikePosX,nBikePosY-8,9,8, olc::BLUE);
                            FillRect(nBikePosX+8,nBikePosY-8,4,8, olc::DARK_BLUE);
                            break;
                    case -1:
                            FillRect(nBikePosX-12,nBikePosY-16,8,4, olc::DARK_BLUE);
                            FillRect(nBikePosX-12,nBikePosY-12,8,4, olc::BLUE);
                            FillRect(nBikePosX-12,nBikePosY-8,4,8, olc::DARK_BLUE);
                            FillRect(nBikePosX-8,nBikePosY-8,8,8, olc::BLUE);
                            FillRect(nBikePosX,nBikePosY-8,4,8, olc::DARK_BLUE);
                            break;
                }

                FillRect(nBikePosX-8,nBikePosY,4,8, olc::DARK_BLUE);
                FillRect(nBikePosX+4,nBikePosY,5,8, olc::DARK_BLUE);
                if (fSpeed > 0 && bIsBraking == false) {
                    FillRect(nBikePosX-4,nBikePosY,9,4, olc::DARK_RED);
                    if(bIsBraking) {
                        FillRect(nBikePosX-4,nBikePosY,9,4, olc::RED);
                    }
                } else {
                    FillRect(nBikePosX-4,nBikePosY,9,4, olc::RED);
                }
                FillRect(nBikePosX-4,nBikePosY+4,9,13, olc::BLACK);
                if (bIsTurbo) {
                    FillRect(nBikePosX-12,nBikePosY+4,4,4, olc::VERY_DARK_GREY);
                    FillRect(nBikePosX-8,nBikePosY+8,4,4, olc::YELLOW);
                    FillRect(nBikePosX+5,nBikePosY+8,4,4, olc::YELLOW);
                    FillRect(nBikePosX+9,nBikePosY+4,4,4, olc::VERY_DARK_GREY);

                }

                //----DRAW STATS----//
                //little lambda to display current lap time as mm:ss:ms as a String
                auto display_time = [](float time) {
                    int nMinutes = time / 60.f;
                    int nSeconds = time - (nMinutes * 60.0f);
                    int nMilliSeconds = (time - (float)nSeconds) * 1000.0f;
                    return to_string(nMinutes) + ":" + to_string(nSeconds) + ":" + to_string(nMilliSeconds);
                };

                //Draw Current Lap Time
                DrawString((ScreenWidth()/2)*0.7f, 5, "TIME: " + display_time(fRemainingTime));
                
                //Draw Total Distance
                std::stringstream streamTotalDistance;
                streamTotalDistance << std::fixed << std::setprecision(2) << fTrackDistance/1000;
                std::string sTotalDistance = streamTotalDistance.str();
                DrawString(5, 35, "Total Distance: " + sTotalDistance + "Km");
                
                //Draw Current Distance
                if ((int)fBikeDistance < 1000) {
                    DrawString(5, 45, "Current Distance: " + to_string((int)fBikeDistance) + "M", olc::WHITE);
                } else {
                    std::stringstream streamCurrentDistance;
                    streamCurrentDistance << std::fixed << std::setprecision(2) << fBikeDistance/1000;
                    std::string sCurrentDistance = streamCurrentDistance.str();
                    DrawString(5, 45, "Current Distance: " + sCurrentDistance + " Km", olc::WHITE);
                }

                //Draw Player Speed
                DrawString(5, 55, "Speed: " + to_string((int)(fSpeed*100)) + " Km/h", olc::WHITE);

                //Draw Seconds Added When Checkpoint is reached
                if (vecTrack[nTrackSection-1].second.second){
                    DrawString((ScreenWidth()/2)*0.8f, 15, "+10 SECONDS");
                }    

                //----END GAME----//
                if (fBikeDistance > fTrackDistance || fRemainingTime < 0.0f){
                    //GameOver
                    bGameOver = true;
                }
            } else {
               gameOver();     
            }            
            return true;
        }

        void gameOver() {
            //----END GAME----//
            if (fBikeDistance < fTrackDistance && fRemainingTime < 0.0f){
                //GameOver
                DrawString((ScreenWidth()/2)*0.8f, ScreenHeight()/2, "TIME UP!");
                DrawString((ScreenWidth()/2)*0.8f, (ScreenHeight()/2)+10, "GAME OVER");
            }
            if (fBikeDistance > fTrackDistance && fRemainingTime > 0.0f) {
                //Player Win
                DrawString((ScreenWidth()/2)*0.8f, ScreenHeight()/2, "YOU WIN!!");
            }
        }

};

int main()
{
    HangOn game;
    game.Construct(426,240,1,1);
    game.Start();
    return 0;
}

