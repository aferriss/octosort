#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
        int h, w, inc, thresh, currColor;
        ofImage img, newImg;
    
        ofVideoPlayer movie;
    
        
        int mod;
        struct Pixel {
            ofVec3f pixelColor;
            float pixelBri;
            //int pixelX;
            //int pixelY;
            int pixelLoc;
            
            bool operator<(const Pixel& a) const{
                return pixelBri < a.pixelBri;
            }

        };
        
        struct QuadPixel {
            Pixel nw;
            Pixel ne;
            Pixel sw;
            Pixel se;
            
            float avgCol;
            
            void setAvg(){
                avgCol = (nw.pixelBri + ne.pixelBri + sw.pixelBri + se.pixelBri)/4;
            }
        };
    
        struct OctPixel {
            Pixel a1;
            Pixel a2;
            Pixel a3;
            Pixel a4;
            
            Pixel b1;
            Pixel b2;
            Pixel b3;
            Pixel b4;
            
            Pixel c1;
            Pixel c2;
            Pixel c3;
            Pixel c4;
            
            Pixel d1;
            Pixel d2;
            Pixel d3;
            Pixel d4;
            
            float avgCol;
            
            void setAvg(){
                float aAvg = (a1.pixelBri + a2.pixelBri + a3.pixelBri + a4.pixelBri)/4;
                float bAvg = (b1.pixelBri + b2.pixelBri + b3.pixelBri + b4.pixelBri)/4;
                float cAvg = (c1.pixelBri + c2.pixelBri + c3.pixelBri + c4.pixelBri)/4;
                float dAvg = (d1.pixelBri + d2.pixelBri + d3.pixelBri + d4.pixelBri)/4;
                
                avgCol = (aAvg + bAvg + cAvg + dAvg)/4;
            }
        };
    
        void exit();
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        int rLoc, gLoc, bLoc;
        int loc, loc2;
        bool locSwap;
        void quicksort(unsigned char x[], int l1, int r1);
        void swap(unsigned char x[], int rl, int rr, int gl, int gr, int bl, int br);
        void singleSwap(unsigned char x[], int l, int r);
        void swapPixel(vector<Pixel> &PA, int l, int r);
        void swapQuadPixel(vector<QuadPixel> &QPA, int l, int r);
        void swapOctPixel(vector<OctPixel> &QPA, int l, int r);
    
        ofImage screen;
        ofPixels newPix;
        ofFbo drawingFbo;
        vector<Pixel> pixelArray;
        vector<QuadPixel> quadPixelArray;
        vector<OctPixel> octPixelArray;
    
        vector <vector<Pixel> > rows;
        vector <vector<Pixel> > cols;
        int boxW;
        int boxH;
    
        void quicksort2(vector<Pixel>&, int l, int r);
        int partition2(vector<Pixel>&, int l, int r);
    
        int mouseX;
    
        int hue;
        ofImage maskImg;
        ofPixels maskPixels, pixels;
        bool save, verbose, savePic;
        int pxColors[921600];
        bool sortNow;
        bool start;
        bool compare( const Pixel &a, const Pixel &b);
    
};
