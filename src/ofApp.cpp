#include "ofApp.h"
#include <vector>
#include <algorithm>



//--------------------------------------------------------------
void ofApp::setup(){
    
    img.load("snowdenClose2.jpg");
    
    ofSetFrameRate(30);
    //ofSetVerticalSync(true);
    
    
    inc = 3;
    thresh = 255;
    
    w = img.getWidth();
    h = img.getHeight();
    int winW = w;//min(w, (int)img.getWidth());
    int winH = h;//min(h, (int)img.getHeight());
    ofSetWindowShape(winW,winH);
    //imgSize = w*h;
    newImg.allocate(w, h, OF_IMAGE_COLOR);
    

    save = false;
    locSwap = true;
    verbose = true;
    savePic = false;
    
    unsigned char * pixels = img.getPixels();
    
    newPix.allocate(w, h, OF_IMAGE_COLOR);
    
    
    for(int y = 0; y<h; y++){
        vector<Pixel> row;
        row.clear();
        for(int x = 0; x<w; x++){
            int pixelLoc = x + (y*w);
            
            int colorLoc = pixelLoc*3;
            
            int r = pixels[colorLoc];
            int g = pixels[colorLoc+1];
            int b = pixels[colorLoc+2];
            float avg = (r+g+b)/3;
            
            Pixel pixel;
            pixel.pixelColor = ofVec3f(r,g,b);
            pixel.pixelBri = avg;
            pixel.pixelLoc = pixelLoc;
            row.push_back(pixel);
        }
        rows.push_back(row);
    }
    
    
    for(int y = 0; y<h; y++){
        vector<Pixel> col;
        col.clear();
        for(int x = 0; x<w; x++){
            int pixelLoc = y + (x*h);
            
            int colorLoc = pixelLoc*3;
            
            int r = pixels[colorLoc];
            int g = pixels[colorLoc+1];
            int b = pixels[colorLoc+2];
            float avg = (r+g+b)/3;
            
            Pixel pixel;
            pixel.pixelColor = ofVec3f(r,g,b);
            pixel.pixelBri = avg;
            pixel.pixelLoc = pixelLoc;
            col.push_back(pixel);
        }
        cols.push_back(col);
    }
    
    
    for(int y = 0; y<h; y++){
        for(int x = 0; x<w; x++){
            
            int pixelLoc = x + (y*w);
            
            int colorLoc = pixelLoc*3;
            int r = pixels[colorLoc];
            int g = pixels[colorLoc+1];
            int b = pixels[colorLoc+2];
            float avg = (r+g+b)/3;
            
            Pixel pixel;
            pixel.pixelColor = ofVec3f(r,g,b);
            pixel.pixelBri = avg;
            pixel.pixelLoc = pixelLoc;
            pixelArray.push_back(pixel);
            
            newPix[r] = r;
            newPix[g] = g;
            newPix[b] = b;
        }
    }
    
    
    for(int y = 0; y<h; y+=2){
        for(int x = 0; x<w; x+=2){
            int loc = y*w + x;
            
            QuadPixel qp;
            qp.nw = pixelArray[loc];
            qp.ne = pixelArray[loc+1];
            qp.sw = pixelArray[loc+w];
            qp.se = pixelArray[loc+w+1];
            
            quadPixelArray.push_back(qp);
        }
    }
    
    for(int i = 0; i<quadPixelArray.size(); i++){
        quadPixelArray[i].setAvg();
    }

    for(int y = 0; y<h; y+=4){
        for(int x = 0; x<w; x+=4){
            int loc = y*w + x;
            
            OctPixel op;
            op.a1 = pixelArray[loc + 0];
            op.a2 = pixelArray[loc + 1];
            op.a3 = pixelArray[loc + 2];
            op.a4 = pixelArray[loc + 3];
            
            op.b1 = pixelArray[loc + w + 0];
            op.b2 = pixelArray[loc + w + 1];
            op.b3 = pixelArray[loc + w + 2];
            op.b4 = pixelArray[loc + w + 3];
            
            op.c1 = pixelArray[loc + w*2 + 0];
            op.c2 = pixelArray[loc + w*2 + 1];
            op.c3 = pixelArray[loc + w*2 + 2];
            op.c4 = pixelArray[loc + w*2 + 3];
            
            op.d1 = pixelArray[loc + w*3 + 0];
            op.d2 = pixelArray[loc + w*3 + 1];
            op.d3 = pixelArray[loc + w*3 + 2];
            op.d4 = pixelArray[loc + w*3 + 3];
            
            octPixelArray.push_back(op);
        }
    }
    
    for(int i = 0; i<octPixelArray.size(); i++){
        octPixelArray[i].setAvg();
    }


    newPix.setFromPixels(img.getPixels(), w, h, 3);
    
    boxW = 300;
    boxH = 900;
    
    sortNow = false;
    
    drawingFbo.allocate(winW, winH);
    
    mouseX = 2500;
    start = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    if(save){
        ofPixels savePix;
        savePix = newImg.getPixels();
        ofSaveImage(savePix, "saved/"+ofGetTimestampString()+".jpg");
        save = false;
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
//    if(ofGetFrameNum()<10){
//        pixels = img.getPixels();
//    }
//
//    for(int i =0; i<h-3; i++){
//        for(int j = 0; j<w-3; j++){
//
//            
//            int loc = (j+(w*i))*3;
//
//            int r = loc;
//            int g = loc+1;
//            int b = loc+2;
//            int av = (r+g+b)/3;
//            int thresh = 256;//ofMap(ofGetMouseX(), 0,w, 0,255);
//            int step = inc;
//            int avg = pixels[r] + pixels[g] + pixels[b];
//            avg /=3;// 0.333333;
//            
//            if(pixels[r] < pixels[r+3]  && pixels[g ] < pixels[g+3] && pixels[b ] < pixels[b+3] ){
////                if(pixels[r]>160 || pixels[g]>160 || pixels[b] > 160){
////                if( j > ofGetMouseX() && j< ofGetMouseX()+100 && i>ofGetMouseY() && i<ofGetMouseY()+100 ){
//                    swap(pixels, r, r-3, g, g-3, b, b-3);
////                }
////                }
////                swap(maskPixels, r, r+3*w, g, g+3*w, b, b+3*w);
////                swap(pixels, r, r+3*w, g, g+3*w, b, b+3*w);
////                quicksort(pixels, r,r+3);
//            }
//        }
//    }
    
    
    
//    for(int y = 0; y<h-1; y++){
//        for(int x = 0; x<w-1; x++){
//            int loc = x + (y*w);
//            int newLoc = loc*3;
////            if(pixelArray[loc].pixelBri < pixelArray[loc+1].pixelBri ){
//            if(pixelArray[loc].pixelBri > mouseX){
//                    sort(pixelArray.begin()+loc, pixelArray.begin()+loc+2);
////                    swapPixel(pixelArray, loc, loc-1);
//                
//                    newPix[newLoc] = pixelArray[loc].pixelColor.x;
//                    newPix[newLoc + 1] = pixelArray[loc].pixelColor.y;
//                    newPix[newLoc + 2] = pixelArray[loc].pixelColor.z;
//            }
//        }
//    }
    
    
    
    int threshold = 80;

//    if(start){
//        for(int y = 0; y<h; y+=2){
//            for(int x = 0; x<w; x+=2){
//                int loc = (y*(w/2)+x)/2;
//                int newLoc = (y*w+x)*3;
//                 if(quadPixelArray[loc+1].avgCol > quadPixelArray[loc].avgCol && x > 0  && x < w){
//                     if( quadPixelArray[loc].avgCol > ofMap(mouseX, 0, w, 0, 255) && quadPixelArray[loc].avgCol <threshold && quadPixelArray[loc-1].avgCol <threshold && quadPixelArray[loc+1].avgCol <threshold){
//                        swapQuadPixel(quadPixelArray, loc+1, loc);
//                        
//                        newPix[newLoc] = quadPixelArray[loc].nw.pixelColor.x;
//                        newPix[newLoc + 1] = quadPixelArray[loc].nw.pixelColor.y;
//                        newPix[newLoc + 2] = quadPixelArray[loc].nw.pixelColor.z;
//                        
//                        newPix[newLoc + 3] = quadPixelArray[loc].ne.pixelColor.x;
//                        newPix[newLoc + 4] = quadPixelArray[loc].ne.pixelColor.y;
//                        newPix[newLoc + 5] = quadPixelArray[loc].ne.pixelColor.z;
//
//                        newPix[newLoc + w*3 + 0] = quadPixelArray[loc].sw.pixelColor.x;
//                        newPix[newLoc + w*3 + 1] = quadPixelArray[loc].sw.pixelColor.y;
//                        newPix[newLoc + w*3 + 2] = quadPixelArray[loc].sw.pixelColor.z;
//                        
//                        newPix[newLoc + w*3 + 3] = quadPixelArray[loc].se.pixelColor.x;
//                        newPix[newLoc + w*3 + 4] = quadPixelArray[loc].se.pixelColor.y;
//                        newPix[newLoc + w*3 + 5] = quadPixelArray[loc].se.pixelColor.z;
//                     }
//                }
//            }
//        }
//    }

    
 
if(start){
    for(int y = 0; y<h; y+=4){
        for(int x = 0; x<w; x+=4){
            int loc = (y*(w/4)+x)/4;
            int newLoc = (y*w+x)*3;
            
            
            if(octPixelArray[loc+1].avgCol > octPixelArray[loc].avgCol && x > 0  && x < w){
                 //if( octPixelArray[loc].avgCol > ofMap(mouseX, 0, w, 0, 255) && octPixelArray[loc].avgCol <threshold && octPixelArray[loc-1].avgCol <threshold && octPixelArray[loc+1].avgCol <threshold){
                if( octPixelArray[loc].avgCol > ofMap(mouseX, 0, w, 0, 255) && octPixelArray[loc].avgCol < threshold){
                swapOctPixel(octPixelArray, loc-1, loc);
            //a row
                newPix[newLoc + 0] = octPixelArray[loc].a1.pixelColor.x;
                newPix[newLoc + 1] = octPixelArray[loc].a1.pixelColor.y;
                newPix[newLoc + 2] = octPixelArray[loc].a1.pixelColor.z;

                newPix[newLoc + 3] = octPixelArray[loc].a2.pixelColor.x;
                newPix[newLoc + 4] = octPixelArray[loc].a2.pixelColor.y;
                newPix[newLoc + 5] = octPixelArray[loc].a2.pixelColor.z;
                
                newPix[newLoc + 6] = octPixelArray[loc].a3.pixelColor.x;
                newPix[newLoc + 7] = octPixelArray[loc].a3.pixelColor.y;
                newPix[newLoc + 8] = octPixelArray[loc].a3.pixelColor.z;
                
                newPix[newLoc + 9] = octPixelArray[loc].a4.pixelColor.x;
                newPix[newLoc + 10] = octPixelArray[loc].a4.pixelColor.y;
                newPix[newLoc + 11] = octPixelArray[loc].a4.pixelColor.z;
                
                //b row
                newPix[newLoc + w*3 + 0] = octPixelArray[loc].b1.pixelColor.x;
                newPix[newLoc + w*3 + 1] = octPixelArray[loc].b1.pixelColor.y;
                newPix[newLoc + w*3 + 2] = octPixelArray[loc].b1.pixelColor.z;
                
                newPix[newLoc + w*3 + 3] = octPixelArray[loc].b2.pixelColor.x;
                newPix[newLoc + w*3 + 4] = octPixelArray[loc].b2.pixelColor.y;
                newPix[newLoc + w*3 + 5] = octPixelArray[loc].b2.pixelColor.z;
                
                newPix[newLoc + w*3 + 6] = octPixelArray[loc].b3.pixelColor.x;
                newPix[newLoc + w*3 + 7] = octPixelArray[loc].b3.pixelColor.y;
                newPix[newLoc + w*3 + 8] = octPixelArray[loc].b3.pixelColor.z;
                
                newPix[newLoc + w*3 + 9] = octPixelArray[loc].b4.pixelColor.x;
                newPix[newLoc + w*3 + 10] = octPixelArray[loc].b4.pixelColor.y;
                newPix[newLoc + w*3 + 11] = octPixelArray[loc].b4.pixelColor.z;
                
                //c row
                newPix[newLoc + w*3*2 + 0] = octPixelArray[loc].c1.pixelColor.x;
                newPix[newLoc + w*3*2 + 1] = octPixelArray[loc].c1.pixelColor.y;
                newPix[newLoc + w*3*2 + 2] = octPixelArray[loc].c1.pixelColor.z;
                
                newPix[newLoc + w*3*2 + 3] = octPixelArray[loc].c2.pixelColor.x;
                newPix[newLoc + w*3*2 + 4] = octPixelArray[loc].c2.pixelColor.y;
                newPix[newLoc + w*3*2 + 5] = octPixelArray[loc].c2.pixelColor.z;
                
                newPix[newLoc + w*3*2 + 6] = octPixelArray[loc].c3.pixelColor.x;
                newPix[newLoc + w*3*2 + 7] = octPixelArray[loc].c3.pixelColor.y;
                newPix[newLoc + w*3*2 + 8] = octPixelArray[loc].c3.pixelColor.z;
                
                newPix[newLoc + w*3*2 + 9] = octPixelArray[loc].c4.pixelColor.x;
                newPix[newLoc + w*3*2 + 10] = octPixelArray[loc].c4.pixelColor.y;
                newPix[newLoc + w*3*2 + 11] = octPixelArray[loc].c4.pixelColor.z;
                
                //d row
                newPix[newLoc + w*3*3 + 0] = octPixelArray[loc].d1.pixelColor.x;
                newPix[newLoc + w*3*3 + 1] = octPixelArray[loc].d1.pixelColor.y;
                newPix[newLoc + w*3*3 + 2] = octPixelArray[loc].d1.pixelColor.z;
                
                newPix[newLoc + w*3*3 + 3] = octPixelArray[loc].d2.pixelColor.x;
                newPix[newLoc + w*3*3 + 4] = octPixelArray[loc].d2.pixelColor.y;
                newPix[newLoc + w*3*3 + 5] = octPixelArray[loc].d2.pixelColor.z;
                
                newPix[newLoc + w*3*3 + 6] = octPixelArray[loc].d3.pixelColor.x;
                newPix[newLoc + w*3*3 + 7] = octPixelArray[loc].d3.pixelColor.y;
                newPix[newLoc + w*3*3 + 8] = octPixelArray[loc].d3.pixelColor.z;
                
                newPix[newLoc + w*3*3 + 9] = octPixelArray[loc].d4.pixelColor.x;
                newPix[newLoc + w*3*3 + 10] = octPixelArray[loc].d4.pixelColor.y;
                newPix[newLoc + w*3*3 + 11] = octPixelArray[loc].d4.pixelColor.z;
            
            }
            }
        }
    }
}

    

    
//    for(int y = 0; y<h; y++){
//        for(int x=0; x<w; x++){
//            int loc = x + (y*w);
//            int newLoc = loc*3;
//            if(rows[y][x].pixelBri > rows[y][x+1].pixelBri){
//                swapPixel(rows[y], x, x+1);
//                
//                newPix[newLoc] = rows[y][x].pixelColor.x;
//                newPix[newLoc + 1] = rows[y][x].pixelColor.y;
//                newPix[newLoc + 2] = rows[y][x].pixelColor.z;
//            }
//        }
//    }
    
//    for(int y = 0; y<h; y++){
//        for(int x=0; x<w; x++){
//            int loc = x + (y*w);
//            int newLoc = loc*3;
//            if(cols[y][x].pixelBri <ofMap(ofGetMouseX(), 0, w, 0, 255)){
//                swapPixel(cols[y], x, x+1);
//                
//                newPix[newLoc] = rows[y][x].pixelColor.x;
//                newPix[newLoc + 1] = rows[y][x].pixelColor.y;
//                newPix[newLoc + 2] = rows[y][x].pixelColor.z;
//            }
//        }
//    }
    
    
    
//    img.setFromPixels(pixels);
//    newImg.update();
    newImg.setFromPixels(newPix);
//    newImg.draw(0,0);
    drawingFbo.begin();
        newImg.draw(0,0, drawingFbo.getWidth(), drawingFbo.getHeight());
    drawingFbo.end();
    
    drawingFbo.draw(0,0);

    
}

int partition(unsigned char a [], int l, int r) {
    
    int i = l;
    int j= r;
    int temp;
    float pivot = a[(i+j)/2];
    
    while (i<=j) {
        
        while (a[i] < pivot) {
            i++;
        }
         
        while (a[j] > pivot) {
            j--;
        }
         
        if (i <= j) {
            
            temp = a[i];
            a[i] = a[j];
            a[j] = temp;
            
            i++;
            j--;
        }
    }
    
    return i;
}


void ofApp::quicksort(unsigned char x [], int left, int right) {
    int index = partition(x, left, right);
    
    if (left < index -3) {
        quicksort(x, left, index-3);
    }
     
    if (index > right) {
        quicksort(x, index, right);
    }
    
}






void ofApp::quicksort2(vector<Pixel>& PA , int left, int right) {
//    int index = partition2(PA, left, right);
//    
//    
//    if (left < index -1) {
//        quicksort2(PA, left, index-1);
//    }
//    if (index > right) {
//        quicksort2(PA, index, right);
//    }
//
    
    int r;
    if(left < right){
        r = partition2(PA, left, right);
        quicksort2(PA, left, r);
        quicksort2(PA, r+1, right);
    }
}




int ofApp::partition2(vector<Pixel>& PA, int l, int r) {
    
//    int ii = l;
//    int jj = r;
//    Pixel temp;
    
    //int pivot = a[ (i+j)/2 ].pixelLoc;
//    int pivot = PA[(l+r)/2].pixelLoc;
//
//    while (l<=r) {
//        
//        while (PA[l].pixelLoc < pivot) {
//            l++;
//        }
//         
//        while (PA[r].pixelLoc > pivot) {
//            r--;
//        }
//        
//        if (l <= r) {
//            Pixel temp = PA[l];
//            PA[l] = PA[r];
//            PA[r] = temp;
//            l++;
//            r--;
//        }
//        
//    }
//    
//    return l;
    int pivot = PA[l].pixelLoc;
    int ii = l;
    int jj;
    
    for (jj = l+1; jj<r; jj++){
        
        if (PA[jj].pixelLoc<=pivot) {
            ii = ii+1;
            std::swap(PA[ii], PA[jj]);
        }
    }
    
    std::swap(PA[ii], PA[pivot]);
    
    return ii;
    
}







//int partition2(unsigned char a [], int rl, int rr) {
//    
//    
//    //--------------Red
//    int ir = rl;
//    int jr= rr;
//    
//    int tempR;
//    
//
//    
//    float pivot = a[(ir+jr)/2];
//    
//    while (ir<=jr) {
//        
//        while (a[ir] < pivot) {
//            ir++;
//        }
//        
//        while (a[jr] > pivot) {
//            jr--;
//        }
//        
//        if (ir <= jr) {
//            tempR = a[ir];
//            a[ir] = a[jr];
//            a[jr] = tempR;
//            ir++;
//            jr--;
//        }
//    }
//    
//    return ir;
//    
//}
//
//
//int partitiong(unsigned char a [], int rl, int rr) {
//    
//    
//    //--------------Red
//    int ir = rl;
//    int jr= rr;
//    
//    int tempR;
//    
//    
//    
//    float pivot = a[(ir+jr)/2];
//    
//    while (ir<=jr) {
//        
//        while (a[ir] < pivot) {
//            ir++;
//        }
//        
//        while (a[jr] > pivot) {
//            jr--;
//        }
//        
//        if (ir <= jr) {
//            tempR = a[ir];
//            a[ir] = a[jr];
//            a[jr] = tempR;
//            ir++;
//            jr--;
//        }
//    }
//    
//    return ir;
//    
//}
//
//
//
//int partitionb(unsigned char a [], int rl, int rr) {
//    
//    
//    //--------------Red
//    int ir = rl;
//    int jr= rr;
//    
//    int tempR;
//    
//    
//    
//    float pivot = a[(ir+jr)/2];
//    
//    while (ir<=jr) {
//        
//        while (a[ir] < pivot) {
//            ir++;
//        }
//        
//        while (a[jr] > pivot) {
//            jr--;
//        }
//        
//        if (ir <= jr) {
//            tempR = a[ir];
//            a[ir] = a[jr];
//            a[jr] = tempR;
//            ir++;
//            jr--;
//        }
//    }
//    
//    return ir;
//    
//}
//
//
//
//void ofApp::quicksort2(unsigned char x [], int rl, int rr, int gl , int gr, int bl, int br ) {
//    int index = partition2(x, rl, rr);
///*
//    if (rl < index -1) {
//        quicksort2(x, rl, index-1, 0,0,0,0);
//
//    }
//    
//    if (index > rr) {
//        quicksort2(x, index, rr, 0,0,0,0);
//    }
// */
//    int gindex = partitiong(x, gl, gr);
//    int bindex = partitionb(x, bl, br);
//    
//}























//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == ' '){
        start = true;
    }
    
    if(key == 's'){
        //saver.setCodecQualityLevel(OF_QT_SAVER_CODEC_QUALITY_HIGH);
        //saver.setup(w, h, "nike"+ofToString(ofGetTimestampString())+".mov");
        
        save = !save;
    }
    
    if(key == 'r'){
        start = false;
            newPix.setFromPixels(img.getPixels(), w, h, 3);
        start = true;
    }
    
    if(key == 'c'){
        ofSetWindowTitle(ofToString(ofMap(mouseX, 0, w, 0, 255)));
    }
    
    if(key == '1'){
        boxW ++;
    }
    if(key == '2'){
        boxW --;
    }
    if(key == '3'){
        boxH ++;
    }
    if(key=='4'){
        boxH --;
    }
    if(key == 'q'){
        thresh--;
    }
    if(key == 'w'){
        thresh++;
    }
    
    if(key == 'p'){
        sortNow = true;
    }
    
}

void ofApp::swap(unsigned char x[], int rl , int rr, int gl, int gr, int bl, int br){
    int rTemp = x[rl];
    x[rl] = x[rr];
    x[rr] = rTemp;

    int gTemp = x[gl];
    x[gl] = x[gr];
    x[gr] = gTemp;

    int bTemp = x[bl];
    x[bl] = x[br];
    x[br] = bTemp;
}
//--------------------------------------------------------------

void ofApp::singleSwap(unsigned char x[], int l , int r){
    int temp = x[l];
    x[l] = x[r];
    x[r] = temp;
}

void ofApp::swapPixel(vector<Pixel> &PA, int l, int r){
    Pixel temp = PA[l];
    PA[l] = PA[r];
    PA[r] = temp;
}

void ofApp::swapQuadPixel(vector<QuadPixel> &QPA, int l, int r){
    QuadPixel temp = QPA[l];
    QPA[l] = QPA[r];
    QPA[r] = temp;
}

void ofApp::swapOctPixel(vector<OctPixel> &QPA, int l, int r){
    OctPixel temp = QPA[l];
    QPA[l] = QPA[r];
    QPA[r] = temp;
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    mouseX = x;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    inc+=1;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::exit(){
    //saver.finishMovie();
}




