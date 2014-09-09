 /**********************************************************
 *                                                          
 *  BlitzRGBClolor
 *  convenience and color related functions...
 *
 *                                                       
 **********************************************************
 *                              
 * $Author: teynor $                                       
 *
 *********************************************************/	

#include "BlitzRGBColor.hh"

namespace BlitzRGBColors {

//get a get color according to the float val (between 0-1)
blitz::TinyVector<unsigned char,3> jet(float val) {

    BlitzRGBColor jetCol[100];

    jetCol[0] = blitz::TinyVector<unsigned char,3>(  0,   0,  133);
    jetCol[1] = blitz::TinyVector<unsigned char,3>(  0,   0,  142);
    jetCol[2] = blitz::TinyVector<unsigned char,3>(  0,   0,  153);    
    jetCol[3] = blitz::TinyVector<unsigned char,3>(  0,   0,  163);
    jetCol[4] = blitz::TinyVector<unsigned char,3>(  0,   0,  173);
    jetCol[5] = blitz::TinyVector<unsigned char,3>(  0,   0,  183);
    jetCol[6] = blitz::TinyVector<unsigned char,3>(  0,   0,  193);
    jetCol[7] = blitz::TinyVector<unsigned char,3>(  0,   0,  204);
    jetCol[8] = blitz::TinyVector<unsigned char,3>(  0,   0,  214);
    jetCol[9] = blitz::TinyVector<unsigned char,3>(  0,   0,  224);
    jetCol[10] = blitz::TinyVector<unsigned char,3>(  0,   0,  234);
    jetCol[11] = blitz::TinyVector<unsigned char,3>(  0,   0,  244);
    jetCol[12] = blitz::TinyVector<unsigned char,3>(  0,   0,  255);
    jetCol[13] = blitz::TinyVector<unsigned char,3>(  0,  10,  255);
    jetCol[14] = blitz::TinyVector<unsigned char,3>(  0,  20,  255);
    jetCol[15] = blitz::TinyVector<unsigned char,3>(  0,  30,  255);
    jetCol[16] = blitz::TinyVector<unsigned char,3>(  0,  40,  255);
    jetCol[17] = blitz::TinyVector<unsigned char,3>(  0,  51,  255);
    jetCol[18] = blitz::TinyVector<unsigned char,3>(  0,  61,  255);
    jetCol[19] = blitz::TinyVector<unsigned char,3>(  0,  71,  255);
    jetCol[20] = blitz::TinyVector<unsigned char,3>(  0,  81,  255);
    jetCol[21] = blitz::TinyVector<unsigned char,3>(  0,  91,  255);
    jetCol[22] = blitz::TinyVector<unsigned char,3>(  0, 102,  255);
    jetCol[23] = blitz::TinyVector<unsigned char,3>(  0, 112,  255);
    jetCol[24] = blitz::TinyVector<unsigned char,3>(  0, 122,  255);
    jetCol[25] = blitz::TinyVector<unsigned char,3>(  0, 132,  255);
    jetCol[26] = blitz::TinyVector<unsigned char,3>(  0, 142,  255);
    jetCol[27] = blitz::TinyVector<unsigned char,3>(  0, 153,  255);
    jetCol[28] = blitz::TinyVector<unsigned char,3>(  0, 163,  255);
    jetCol[29] = blitz::TinyVector<unsigned char,3>(  0, 173,  255);
    jetCol[30] = blitz::TinyVector<unsigned char,3>(  0, 183,  255);
    jetCol[31] = blitz::TinyVector<unsigned char,3>(  0, 193,  255);
    jetCol[32] = blitz::TinyVector<unsigned char,3>(  0, 204,  255);
    jetCol[33] = blitz::TinyVector<unsigned char,3>(  0, 214,  255);
    jetCol[34] = blitz::TinyVector<unsigned char,3>(  0, 224,  255);
    jetCol[35] = blitz::TinyVector<unsigned char,3>(  0, 234,  255);
    jetCol[36] = blitz::TinyVector<unsigned char,3>(  0, 244,  255);
    jetCol[37] = blitz::TinyVector<unsigned char,3>(  0, 255,  255);
    jetCol[38] = blitz::TinyVector<unsigned char,3>( 10, 255,  244);
    jetCol[39] = blitz::TinyVector<unsigned char,3>( 20, 255,  234);
    jetCol[40] = blitz::TinyVector<unsigned char,3>( 30, 255,  224);
    jetCol[41] = blitz::TinyVector<unsigned char,3>( 40, 255,  214);
    jetCol[42] = blitz::TinyVector<unsigned char,3>( 51, 255,  204);
    jetCol[43] = blitz::TinyVector<unsigned char,3>( 61, 255,  193);
    jetCol[44] = blitz::TinyVector<unsigned char,3>( 71, 255,  183);
    jetCol[45] = blitz::TinyVector<unsigned char,3>( 81, 255,  173);
    jetCol[46] = blitz::TinyVector<unsigned char,3>( 91, 255,  163);
    jetCol[47] = blitz::TinyVector<unsigned char,3>(102, 255,  153);
    jetCol[48] = blitz::TinyVector<unsigned char,3>(112, 255,  142);
    jetCol[49] = blitz::TinyVector<unsigned char,3>(122, 255,  132);
    jetCol[50] = blitz::TinyVector<unsigned char,3>(132, 255,  122);
    jetCol[51] = blitz::TinyVector<unsigned char,3>(142, 255,  112);
    jetCol[52] = blitz::TinyVector<unsigned char,3>(153, 255,  102);
    jetCol[53] = blitz::TinyVector<unsigned char,3>(163, 255,   91);
    jetCol[54] = blitz::TinyVector<unsigned char,3>(173, 255,   81);
    jetCol[55] = blitz::TinyVector<unsigned char,3>(183, 255,   71);
    jetCol[56] = blitz::TinyVector<unsigned char,3>(193, 255,   61);
    jetCol[57] = blitz::TinyVector<unsigned char,3>(204, 255,   51);
    jetCol[58] = blitz::TinyVector<unsigned char,3>(214, 255,   40);
    jetCol[59] = blitz::TinyVector<unsigned char,3>(224, 255,   30);
    jetCol[60] = blitz::TinyVector<unsigned char,3>(234, 255,   20);
    jetCol[61] = blitz::TinyVector<unsigned char,3>(244, 255,   10);
    jetCol[62] = blitz::TinyVector<unsigned char,3>(255, 255,    0);
    jetCol[63] = blitz::TinyVector<unsigned char,3>(255, 244,    0);
    jetCol[64] = blitz::TinyVector<unsigned char,3>(255, 234,    0);
    jetCol[65] = blitz::TinyVector<unsigned char,3>(255, 224,    0);
    jetCol[66] = blitz::TinyVector<unsigned char,3>(255, 214,    0);
    jetCol[67] = blitz::TinyVector<unsigned char,3>(255, 204,    0);
    jetCol[68] = blitz::TinyVector<unsigned char,3>(255, 193,    0);
    jetCol[69] = blitz::TinyVector<unsigned char,3>(255, 183,    0);
    jetCol[70] = blitz::TinyVector<unsigned char,3>(255, 173,    0);
    jetCol[71] = blitz::TinyVector<unsigned char,3>(255, 163,    0);
    jetCol[72] = blitz::TinyVector<unsigned char,3>(255, 153,    0);
    jetCol[73] = blitz::TinyVector<unsigned char,3>(255, 142,    0);
    jetCol[74] = blitz::TinyVector<unsigned char,3>(255, 132,    0);
    jetCol[75] = blitz::TinyVector<unsigned char,3>(255, 122,    0);
    jetCol[76] = blitz::TinyVector<unsigned char,3>(255, 112,    0);
    jetCol[77] = blitz::TinyVector<unsigned char,3>(255, 102,    0);
    jetCol[78] = blitz::TinyVector<unsigned char,3>(255,  91,    0);
    jetCol[79] = blitz::TinyVector<unsigned char,3>(255,  81,    0);
    jetCol[80] = blitz::TinyVector<unsigned char,3>(255,  71,    0);
    jetCol[81] = blitz::TinyVector<unsigned char,3>(255,  61,    0);
    jetCol[82] = blitz::TinyVector<unsigned char,3>(255,  51,    0);
    jetCol[83] = blitz::TinyVector<unsigned char,3>(255,  40,    0);
    jetCol[84] = blitz::TinyVector<unsigned char,3>(255,  30,    0);
    jetCol[85] = blitz::TinyVector<unsigned char,3>(255,  20,    0);
    jetCol[86] = blitz::TinyVector<unsigned char,3>(255,  10,    0);
    jetCol[87] = blitz::TinyVector<unsigned char,3>(255,   0,    0);
    jetCol[88] = blitz::TinyVector<unsigned char,3>(244,   0,    0);
    jetCol[89] = blitz::TinyVector<unsigned char,3>(234,   0,    0);
    jetCol[90] = blitz::TinyVector<unsigned char,3>(224,   0,    0);
    jetCol[91] = blitz::TinyVector<unsigned char,3>(214,   0,    0);
    jetCol[92] = blitz::TinyVector<unsigned char,3>(204,   0,    0);
    jetCol[93] = blitz::TinyVector<unsigned char,3>(193,   0,    0);
    jetCol[94] = blitz::TinyVector<unsigned char,3>(183,   0,    0);
    jetCol[95] = blitz::TinyVector<unsigned char,3>(173,   0,    0);
    jetCol[96] = blitz::TinyVector<unsigned char,3>(163,   0,    0);
    jetCol[97] = blitz::TinyVector<unsigned char,3>(153,   0,    0);
    jetCol[98] = blitz::TinyVector<unsigned char,3>(142,   0,    0);
    jetCol[99] = blitz::TinyVector<unsigned char,3>(132,   0,    0);
                                                               
    int idx = static_cast<int>(round(val*100));
    
    if (idx <= 0) {
        return jetCol[0];               
    } else if (idx > 99) {
        return jetCol[99];          
    } else {
        return jetCol[idx];               
    }    
}

}


