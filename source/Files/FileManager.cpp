#pragma once
#include "FileManager.h"
#include "../Engine/Includes/ParseOBJ.h"

//to import readvec3fromobj
#include "../Engine/Objects/Object.h"


//fileManager Variables
juce::File FileManager::appDataFolder;
juce::File FileManager::moduleFolder;
juce::File FileManager::assetFolder;

//for checking the folder
inline bool verifyFolder(const juce::File& folder)
{
    if (folder.exists()){
        return folder.isDirectory(); // true only if it's a directory
    }
    return folder.createDirectory(); // true if creation succeeded
}
juce::String readZipEntryAsString(juce::ZipFile* zipFile,const juce::ZipFile::ZipEntry* entry)
{
    if (entry == nullptr)
        return {};

    std::unique_ptr<juce::InputStream> stream(zipFile->createStreamForEntry(*entry));

    if (stream == nullptr || !stream->isExhausted())
    {
        juce::MemoryBlock mb;
        stream->readIntoMemoryBlock(mb);

        // Convert raw bytes to a juce::String (assuming UTF-8 or ASCII text)
        return juce::String::fromUTF8(static_cast<const char*>(mb.getData()), (int)mb.getSize());
    }

    return {};
}

void FileManager::init() {
    // initialize files
    appDataFolder = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("polyrack");
    verifyFolder(appDataFolder);
    //get and check for subfolders
    moduleFolder = appDataFolder.getChildFile("modules");
    verifyFolder(moduleFolder);

    assetFolder = appDataFolder.getChildFile("assets");
    verifyFolder(assetFolder);

    //log the file paths
    //std::cout << "Plugin folder: " << appDataFolder.getFullPathName() << std::endl;
    //std::cout << "Module folder: " << moduleFolder.getFullPathName() << std::endl;
    //std::cout << "Assets folder: " << assetFolder.getFullPathName() << std::endl;
}
void FileManager::loadModules(Renderer& renderer,TextureManager& textureAtlas) {
   
    //loop through all the modules
    for (auto& file : moduleFolder.findChildFiles(juce::File::findFiles, false))
    {
        const auto& fileName = file.getFileName();
        const auto& extension = file.getFileExtension();
        if(!(extension==".zip"||extension==".plk")) continue;
        //start reading the data
        juce::ZipFile zipFile(file);
        const auto& mainJSON = zipFile.getEntry("module.json");
        juce::String jsonText = "";

        if (mainJSON == nullptr) continue;
        
        jsonText = readZipEntryAsString(&zipFile,mainJSON);

        juce::var parsed = juce::JSON::parse(jsonText);
            
        //module data to save
        juce::String moduleFilePath=file.getFullPathName();
        juce::String moduleName=parsed.getProperty("name",  "[MISSING]");
        juce::String moduleDesc=parsed.getProperty("description",  "[MISSING]");
        juce::var moduleLayout = parsed["layout"].getObject();
        //arrays
        juce::Array<juce::var>* moduleTags = parsed["tags"].getArray();
        juce::Array<juce::var>* moduleTextures = parsed["textures"].getArray();
        juce::Array<juce::var>* moduleModels = parsed["models"].getArray();
        juce::Array<juce::var>* moduleAnimations = parsed["animations"].getArray();
        juce::Array<juce::var>* layoutComponents = moduleLayout["components"].getArray();
            
        //actually register the module nowww
        ModuleData& moduleData = ModuleManager::addModule();
        moduleData.name=moduleName.toStdString();
        moduleData.description=moduleDesc.toStdString();
        moduleData.layout = *layoutComponents;
        moduleData.width=moduleLayout.getProperty("width",1);
        moduleData.height=moduleLayout.getProperty("height",1);
        
        std::cout<<"loading module: \""<< moduleData.name <<"\"\n";

        std::vector<int> textureIDs;
        //loop through the textures and initialize them
        for (const juce::var& texture : *moduleTextures){
            juce::String texturePath = texture.getProperty("path", "[MISSING]");
            const juce::ZipFile::ZipEntry* textureEntry = zipFile.getEntry(texturePath);
            std::unique_ptr<juce::InputStream> stream(zipFile.createStreamForEntry(*textureEntry));
            juce::Image textureImage = FileManager::readTextureStream(std::move(stream));
            int textureID = textureAtlas.addTexture(textureImage);
            textureIDs.emplace_back(textureID);
        }
        //loop through the models tooooooo
        for (const juce::var& model : *moduleModels){
            juce::String modelPath = model.getProperty("path", "[MISSING]");
            int textureIndex = static_cast<int>(model.getProperty("textureID", 0));
            int modelTextureID = textureIDs[textureIndex];

            const juce::ZipFile::ZipEntry* modelEntry = zipFile.getEntry(modelPath);
            if (modelEntry != nullptr)
            {
                std::unique_ptr<juce::InputStream> inputStream(zipFile.createStreamForEntry(*modelEntry));
                    
                if (inputStream != nullptr)
                {
                    juce::String fileContents = inputStream->readEntireStreamAsString();
                    Model& model = moduleData.models.emplace_back();
                    model.createGeometry(renderer.openGLContext,parseOBJString(fileContents.toStdString()));
                    model.textureID=modelTextureID;
                }
            }
        }
        //now load the animations
        for(const juce::var& animation : *moduleAnimations){
            Animation& animationData = moduleData.animations.emplace_back();
            animationData.hitboxSize=readVec3FromObj(animation["hitboxSize"],Vec3(0.5f));
            juce::Array<juce::var>* animationModels = animation["models"].getArray();
            //for each model in the animation
            for(const juce::var& model : *animationModels){
                int modelID = model.getProperty("modelID",0);
                animationData.models.push_back(&moduleData.models[modelID]);
                juce::var functionsVar = model.getProperty("functions", juce::var());
                std::unique_ptr<AnimFunctionCollection>& funcs = animationData.animFunctions.emplace_back(std::make_unique<AnimFunctionCollection>());
                //import all the functions here
                funcs->x = functionsVar.getProperty("x","0").toString().toStdString();
                funcs->y = functionsVar.getProperty("y","0").toString().toStdString();
                funcs->z = functionsVar.getProperty("z","0").toString().toStdString();
                funcs->rx = functionsVar.getProperty("rx","0").toString().toStdString();
                funcs->ry = functionsVar.getProperty("ry","0").toString().toStdString();
                funcs->rz = functionsVar.getProperty("rz","0").toString().toStdString();
                funcs->sx = functionsVar.getProperty("sx","0").toString().toStdString();
                funcs->sy = functionsVar.getProperty("sy","0").toString().toStdString();
                funcs->sz = functionsVar.getProperty("sz","0").toString().toStdString();

                funcs->compileExpressions();
            }
        }
    }
}

juce::Image FileManager::readTextureFile(juce::File file)
{
    if (file.existsAsFile())
    {
        juce::Image image = juce::ImageFileFormat::loadFrom(file);
        if (image.isValid())
        {
            return image;
        }
        else
        {
            std::cout << "ERROR: Failed to load image from file: " << file.getFullPathName() << std::endl;
        }
    }
    else
    {
        std::cout << "ERROR: File not found: " << file.getFullPathName() << std::endl;
    }

    // Return an empty (invalid) image on failure
    return juce::Image();
}
juce::Image FileManager::readTextureStream(std::unique_ptr<juce::InputStream> stream)
{
    if (stream != nullptr)
    {
        juce::Image image = juce::ImageFileFormat::loadFrom(*stream);
        if (image.isValid())
            return image;
        else
            std::cout << "ERROR: Failed to load image from stream." << std::endl;
    }
    else
    {
        std::cout << "ERROR: Stream is null." << std::endl;
    }

    return juce::Image();
}