//
// Created by luckynote on 18-11-10.
//

#include <iostream>
#include <string>
#include <vector>
#include "helper.h"

int main()
{
    std::vector<ImageLabel> Imagelabels;
    std::string parentPath = "/media/luckynote/Data/face-recognition/face-dataset/landmarks/68/ibug_300W_large_face_landmark_dataset/";
    std::string xmlDocNameToRead = "testing_with_face_landmarks.xml";
    std::string xmlDocNameToWrite = "testing_with_face_landmarks_1.xml";
    helper helper;


    // newLandmarksIndex: input your necessary keypoint index
    std::vector<int> newLandmarksIndex =
            {
                    0, 3, 8, 13, 16,            // chin
                    17, 19, 21,                 // left brow
                    22, 24, 26,                 // right brow
                    30, 31, 35,                 // nose
                    36, 37, 38, 39, 40, 41,     // left eye
                    42, 43, 44, 45, 46, 47,     // right eye
                    48, 51, 54, 57, 62, 66      // mouth
            };

    bool isonlyReadXML = true;

    if(isonlyReadXML)
    {
        helper.ReadLabelsFromXMLFileByCustom(Imagelabels, parentPath, xmlDocNameToRead, "_68", true, false);
    }else
    {
        // make a new landmarks xml
        helper.ReadLabelsFromXMLFileByTiny(Imagelabels, parentPath, xmlDocNameToRead, true);
        helper.WriteLabelsToXMLFileByTiny(Imagelabels, parentPath, xmlDocNameToWrite, newLandmarksIndex);
    }

    return 0;
}