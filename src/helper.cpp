//
// Created by luckynote on 18-11-10.
//

#include "../include/helper.h"


std::string helper::trim(const std::string& str)
{
    std::string::size_type pos = str.find_first_not_of(' ');
    if (pos == std::string::npos)
    {
        return str;
    }
    std::string::size_type pos2 = str.find_last_not_of(' ');
    if (pos2 != std::string::npos)
    {
        return str.substr(pos, pos2 - pos + 1);
    }
    return str.substr(pos);
}

std::string helper::replace(const std::string& str, const std::string& dest, const std::string& src)
{
    bool isstop = false;
    std::string ret = str;
    size_t pos = ret.find(dest);
    if(dest == ".jpg" or dest == ".png")
    {
        isstop = true;
    }
    while(pos != std::string::npos){
        ret = ret.replace(pos, dest.length(), src);
        pos = ret.find(dest);
        if(isstop) break;
    }
    return ret;
}

std::vector<std::string> helper::split(const  std::string& s, const std::string& delim)
{
    std::vector<std::string> elems;
    size_t pos = 0;
    size_t len = s.length();
    size_t delim_len = delim.length();
    if (delim_len == 0) return elems;
    while (pos < len)
    {
        int find_pos = s.find(delim, pos);
        if (find_pos < 0)
        {
            elems.push_back(s.substr(pos, len - pos));
            break;
        }
        elems.push_back(s.substr(pos, find_pos - pos));
        pos = find_pos + delim_len;
    }
    return elems;
}

void helper::ReadLabelsFromXMLFileByCustom(std::vector<ImageLabel> &Imagelabels, const std::string parentPath,
                                   const std::string xmlDocName, const std::string extendName, bool isview, bool issave)
{
    std::ifstream LabelsFile(parentPath+xmlDocName, std::ios::in);
    if(!LabelsFile.is_open())
        return;
    std::string linestr;
    int imagecount = 0;
    cv::Mat Image;
    while(std::getline(LabelsFile, linestr)){
        linestr = trim(linestr);
        linestr = replace(linestr, "</", "");
        linestr = replace(linestr, "/>", "");
        linestr = replace(linestr, "<", "");
        linestr = replace(linestr, ">", "");
        linestr = replace(linestr, "'", "");
        // You will meet a matter by tinyXML
        linestr = replace(linestr, "\"", "");

        std::vector<std::string> strNodes = split(linestr, " ");
        static ImageLabel* mImageLabel = NULL;
        switch (strNodes.size()) {
            case 1:
                if(strNodes[0] == "image"){
                    std::string imagePath = mImageLabel->imageName;
                    imagePath = replace(imagePath, ".jpg", extendName + ".jpg");
                    imagePath = replace(imagePath, ".png", extendName + ".png");
                    std::cout << "imagePath= " << imagePath << std::endl;

                    if(issave)
                    {
                        cv::imwrite(imagePath, Image);
                    }

                    if(isview)
                    {
                        cv::imshow("image", Image);
                        cv::waitKey(30);
                    }

                    Imagelabels.push_back(*mImageLabel);
                    delete mImageLabel;
                }
                break;
            case 2:
                if(strNodes[0] == "image"){
                    imagecount ++;

                    mImageLabel = new ImageLabel();
                    mImageLabel->imageName = parentPath + split(strNodes[1], "=")[1];
                    std::cout << "imagePath= " << mImageLabel->imageName << std::endl;
                    Image = cv::imread(mImageLabel->imageName);
                }
                break;
            case 5:
                if(strNodes[0] == "box"){
                    mImageLabel->faceBox[0] = atoi(split(strNodes[1], "=")[1].data());
                    mImageLabel->faceBox[1] = atoi(split(strNodes[2], "=")[1].data());
                    mImageLabel->faceBox[2] = atoi(split(strNodes[3], "=")[1].data());
                    mImageLabel->faceBox[3] = atoi(split(strNodes[4], "=")[1].data());
                    std::cout <<
                              "top: " << mImageLabel->faceBox[0] << "; " <<
                              "left: " << mImageLabel->faceBox[1] << "; " <<
                              "width: " << mImageLabel->faceBox[2]  << "; " <<
                              "height: " << mImageLabel->faceBox[3] <<
                              std::endl;
                    cv::rectangle(Image, cv::Rect(mImageLabel->faceBox[1], mImageLabel->faceBox[0], mImageLabel->faceBox[2], mImageLabel->faceBox[3]), cv::Scalar(0, 255, 0), 1);
                }
                break;
            case 4:
                if(strNodes[0] == "part"){
                    int index = atoi(split(strNodes[1], "=")[1].data());
                    mImageLabel->landmarkPos[index] = atoi(split(strNodes[2], "=")[1].data());
                    mImageLabel->landmarkPos[index+LandmarkPointsNum] = atoi(split(strNodes[3], "=")[1].data());
                    std::cout <<
                              "index: " << index << "; " <<
                              "x= " << mImageLabel->landmarkPos[index] << "; " <<
                              "y= " << mImageLabel->landmarkPos[index+LandmarkPointsNum] <<
                              std::endl;
                    cv::circle(Image, cv::Point(mImageLabel->landmarkPos[index], mImageLabel->landmarkPos[index+LandmarkPointsNum]), 2, cv::Scalar(0,0,255), -1);
                }
                break;
            default:
                break;
        }
    }
    std::cout << "image count: " << imagecount << std::endl;
    LabelsFile.close();
}

void helper::ReadLabelsFromXMLFileByTiny(std::vector<ImageLabel> &Imagelabels, const std::string parentPath,
                                         const std::string xmlDocName, bool isview)
{
    int imagecount = 0;

    TiXmlDocument xmlDoc((parentPath + xmlDocName).data());

    if ( !xmlDoc.LoadFile() )
    {
        std::cout << xmlDoc.ErrorDesc() << std::endl;
        return;
    }

    TiXmlNode* root = xmlDoc.FirstChildElement("dataset");

    if(root == NULL)
    {
        std::cout << "Failed to load file: No root element." << std::endl;
        xmlDoc.Clear();
        return;
    }

    for(TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
    {
        std::string elemName = elem->Value();
        if(elemName == "name")
        {
            std::string nameinfo = elem->FirstChild()->ToText()->Value();
            std::cout << nameinfo << std::endl;
        }else if(elemName == "comment")
        {
            std::string comment = elem->FirstChild()->ToText()->Value();
            std::cout << comment << std::endl;
        }else if(elemName == "images")
        {
            for(TiXmlElement* imageRoot = elem->FirstChildElement("image"); imageRoot != NULL; imageRoot = imageRoot->NextSiblingElement())
            {
                static ImageLabel* mImageLabel = NULL;
                std::string imageName = imageRoot->Attribute("file");
                mImageLabel = new ImageLabel();
                mImageLabel->imageName = imageName;
                std::cout << mImageLabel->imageName << std::endl;

                imagecount ++;

                if(isview)
                {
                    cv::Mat Image = cv::imread(parentPath + mImageLabel->imageName);
                    cv::imshow("Image", Image);
                    cv::waitKey(0);
                }

                TiXmlElement* boxRoot = imageRoot->FirstChildElement("box");

                mImageLabel->faceBox[0] = atof(boxRoot->Attribute("top"));
                mImageLabel->faceBox[1] = atof(boxRoot->Attribute("left"));
                mImageLabel->faceBox[2] = atof(boxRoot->Attribute("width"));
                mImageLabel->faceBox[3] = atof(boxRoot->Attribute("height"));
                std::cout <<
                          "top: " << mImageLabel->faceBox[0] << "; " <<
                          "left: " << mImageLabel->faceBox[1] << "; " <<
                          "width: " << mImageLabel->faceBox[2]  << "; " <<
                          "height: " << mImageLabel->faceBox[3] <<
                          std::endl;
                for(TiXmlElement* partRoot = boxRoot->FirstChildElement("part"); partRoot != NULL; partRoot = partRoot->NextSiblingElement())
                {
                    int index = atoi(partRoot->Attribute("name"));
                    mImageLabel->landmarkPos[index] = atoi(partRoot->Attribute("x"));
                    mImageLabel->landmarkPos[index+LandmarkPointsNum] = atoi(partRoot->Attribute("y"));
                    std::cout <<
                              "index: " << index << "; " <<
                              "x = " << mImageLabel->landmarkPos[index] << "; " <<
                              "y = " << mImageLabel->landmarkPos[index+LandmarkPointsNum] <<
                              std::endl;
                }

                Imagelabels.push_back(*mImageLabel);
                delete mImageLabel;
            }
        }else
        {
            std::cout << "Failed to load file: No root element." << std::endl;
            xmlDoc.Clear();
            return;
        }
    }
    std::cout << "image count: " << imagecount << std::endl;
    xmlDoc.Clear();
}

void helper::WriteLabelsToXMLFileByTiny(const std::vector<ImageLabel> Imagelabels, const std::string parentPath,
                                        const std::string xmlDocName, const std::vector<int> newLandmarksIndex)
{
    TiXmlDocument *xmlDoc = new TiXmlDocument();

    TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0","ISO-8859-1","");
    xmlDoc->LinkEndChild(pDeclaration);

    TiXmlElement *datasetRoot = new TiXmlElement("dataset");    // dataset
    xmlDoc->LinkEndChild(datasetRoot);

    TiXmlElement *nameRoot = new TiXmlElement("name");
    datasetRoot->LinkEndChild(nameRoot);
    TiXmlText *NameText = new TiXmlText("nameinfo");
    nameRoot->LinkEndChild(NameText);

    TiXmlElement *commentRoot = new TiXmlElement("comment");
    datasetRoot->LinkEndChild(commentRoot);
    NameText = new TiXmlText("commentinfo");
    commentRoot->LinkEndChild(NameText);

    TiXmlElement *imagesRoot = new TiXmlElement("images");
    datasetRoot->LinkEndChild(imagesRoot);

    for(int i=0; i < Imagelabels.size(); i++)
    {
        TiXmlElement *imageRoot = new TiXmlElement("image");
        imagesRoot->LinkEndChild(imageRoot);
        imageRoot->SetAttribute("file", Imagelabels[i].imageName.data());

        TiXmlElement *boxRoot = new TiXmlElement("box");
        imageRoot->LinkEndChild(boxRoot);
        boxRoot->SetAttribute("top", Imagelabels[i].faceBox[0]);
        boxRoot->SetAttribute("left", Imagelabels[i].faceBox[1]);
        boxRoot->SetAttribute("width", Imagelabels[i].faceBox[2]);
        boxRoot->SetAttribute("height", Imagelabels[i].faceBox[3]);

        for (int j = 0; j < newLandmarksIndex.size(); ++j) {
            TiXmlElement *partRoot = new TiXmlElement("part");
            boxRoot->LinkEndChild(partRoot);
            partRoot->SetAttribute("name", j);
            partRoot->SetAttribute("x", Imagelabels[i].landmarkPos[newLandmarksIndex[j]]);
            partRoot->SetAttribute("y", Imagelabels[i].landmarkPos[newLandmarksIndex[j]+LandmarkPointsNum]);
        }
    }
    xmlDoc->SaveFile((parentPath + xmlDocName).data());
}