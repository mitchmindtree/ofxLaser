//
//  ofxLaserUI.hpp
//  ofxLaser
//
//  Created by Seb Lee-Delisle on 23/03/2021.
//

#pragma once
#include "ofxImGui.h"
#include "ofMain.h"
#include "Poco/PriorityDelegate.h"
#include "RobotoMedium.cpp"

namespace ofxLaser {

class UI {
    
    public :
    
    static ofxImGui::Gui imGui;
    static ImFont* font;
    static bool initialised; 
    
    static void setupGui();
    static void updateGui();
    static void startGui();
    
    static bool addIntSlider(ofParameter<int>& param);
    static bool addFloatSlider(ofParameter<float>& param, const char* format="%.2f", float power = 1.0f) ;
    static bool addFloat2Slider(ofParameter<glm::vec2>& param, const char* format="%.2f", float power = 1.0f) ;
    static bool addFloat3Slider(ofParameter<glm::vec3>& parameter, const char* format="%.2f", float power = 1.0f);

   
    static bool addFloatAsIntSlider(ofParameter<float>& param, float multiplier);
    static bool addFloatAsIntPercentage(ofParameter<float>& param);
    
    static bool addResettableFloatSlider(ofParameter<float>& param, float resetParam, string tooltip="", const char* format="%.2f", float power = 1.0f);
    static bool addResettableIntSlider(ofParameter<int>& param, int resetParam, string tooltip="");

    

    static bool addCheckbox(ofParameter<bool>&param);
    
    static bool addColour(ofParameter<ofFloatColor>& parameter, bool alpha = false);
    static bool addColour(ofParameter<ofColor>& parameter, bool alpha = false);

    static bool addParameter(shared_ptr<ofAbstractParameter>& param);
    
    static void addParameterGroup(ofParameterGroup& parameterGroup);
    
    static void startWindow(string name, ImVec2 pos, ImVec2 size = ImVec2(0,0), ImGuiWindowFlags flags = 0, bool resetPosition = false, bool* openstate = nullptr) {
        ImGuiWindowFlags window_flags = flags;
        window_flags |= ImGuiWindowFlags_NoNav;
        //      if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
        //      if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
        //      if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
        //
        //
        //      if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
        //      if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
        //      if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
        //      if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
        //      if (no_docking)         window_flags |= ImGuiWindowFlags_NoDocking;
        //      if (no_close)           p_open = NULL; // Don't pass our bool* to Begin
        
        
        // set the main window size and position
        ImGui::SetNextWindowSize(size, ImGuiCond_Once);
        ImGui::SetNextWindowPos(pos, resetPosition ? ImGuiCond_Always : ImGuiCond_Once);
        
        // start the main window!
        ImGui::Begin(name.c_str(), openstate, window_flags);
        
        
    }
    static void endWindow() {
        ImGui::End();
    }
    
    static void secondaryColourButtonStart() {
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 1.0f, 1.0f));
        
    }
    
    static void secondaryColourButtonEnd() {
        ImGui::PopStyleColor(3);
    }
    
    static void largeItemStart() {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 10.0f)); // 3 Size of elements (padding around contents);
        // increase the side of the slider grabber
        ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 26.0f); // 4 minimum size of slider grab
        
    }
    static void largeItemEnd() {
        
        ImGui::PopStyleVar(2);
    }
    
    static bool updateMouse(ofMouseEventArgs &e) {
		ImGui::GetIO().MousePos = ImVec2((float)e.x, (float)e.y);
		//ofLogNotice("Mouse updated " + ofToString(ImGui::GetIO().MousePos.x) +" " +ofToString(ImGui::GetIO().MousePos.y));
		return false; // propogate events 
    }
    static bool mousePressed(ofMouseEventArgs &e) {
        
        ImGui::GetIO().MouseDown[e.button] = true;
        //cout << (ImGui::GetIO().WantCaptureMouse)<< endl;
        if(ImGui::GetIO().WantCaptureMouse) {
            //ofLogNotice("ImGui captured mouse press");
            return true;
        }
        else {
            //ofLogNotice("ImGui no capture mouse press");
            return false;
        }
    }
    static bool mouseReleased(ofMouseEventArgs &e) {
        ImGui::GetIO().MouseDown[e.button] = false;
        if(ImGui::GetIO().WantCaptureMouse) return true;
        else return false;
    }
    static bool keyPressed(ofKeyEventArgs &e) {
        ImGui::GetIO().KeysDown[e.key] = true;
        if(ImGui::GetIO().WantCaptureKeyboard) {
            
            //ofLogNotice("ImGui captured key press");
            return true;
        }
        else {
            //ofLogNotice("ImGui no capture key press");
            return false;
        }
    }
    static bool keyReleased(ofKeyEventArgs &e) {
        // TODO check but I think this happens twice...
        ImGui::GetIO().KeysDown[e.key] = false;
        if(ImGui::GetIO().WantCaptureKeyboard) {
            return false;
        }
        else return false;
    }
    static void render();
    
    static void toolTip(string& str) {
        toolTip(str.c_str());
    }
  
    static void toolTip(const char* desc)
    {
        ImGui::SameLine(0,3);
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 15.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
    
    
    static ofMesh dashedLineMesh;

    static void drawDashedLine(glm::vec3 p1, glm::vec3 p2);
    
    
    
    static ImU32 getColourForState(int state) {
        const ImVec4 stateCols[] = {{0,1,0,1}, {1,1,0,1}, {1,0,0,1}};
        return ImGui::GetColorU32(stateCols[state]);
    }
    
};




}
