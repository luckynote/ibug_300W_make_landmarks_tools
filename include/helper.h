//
// Created by luckynote on 18-11-10.
//

#ifndef PRODUCTDATASET_PRJ_HELPER_H
#define PRODUCTDATASET_PRJ_HELPER_H

#include <iostream>
#include <vector>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "tinyXML/tinyxml.h"

#define LandmarkPointsNum  68

struct ImageLabel {
    std::string imageName;
    int faceBox[4];
    int landmarkPos[2 * LandmarkPointsNum];
};


class helper {
private:
    std::string trim(const std::string& str);
    std::string replace(const std::string& str, const std::string& dest, const std::string& src);
    std::vector<std::string> split(const  std::string& s, const std::string& delim);

public:
    void ReadLabelsFromXMLFileByTiny(std::vector<ImageLabel> &Imagelabels, const std::string parentPath,
                                     const std::string xmlDocName, bool isview = false);
    void WriteLabelsToXMLFileByTiny(const std::vector<ImageLabel> Imagelabels, const std::string parentPath,
                                    const std::string xmlDocName, const std::vector<int> newLandmarksIndex);
    void ReadLabelsFromXMLFileByCustom(std::vector<ImageLabel> &Imagelabels, const std::string parentPath,
                                       const std::string xmlDocName, const std::string extendName, bool isview = false, bool issave = false);
};


#endif //PRODUCTDATASET_PRJ_HELPER_H
