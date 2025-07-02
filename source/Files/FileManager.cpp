#pragma once
#include "FileManager.h"

#define DEBUG_MODULE_FILES true

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
    std::cout << "Plugin folder: " << appDataFolder.getFullPathName() << std::endl;
    std::cout << "Module folder: " << moduleFolder.getFullPathName() << std::endl;
    std::cout << "Assets folder: " << assetFolder.getFullPathName() << std::endl;
}
void FileManager::loadModules(TextureManager& textureAtlas) {
   
    //loop through all the modules
    for (auto& file : moduleFolder.findChildFiles(juce::File::findFiles, false))
    {
        const auto& fileName = file.getFileName();
        const auto& extension = file.getFileExtension();

        //ignore non module extensions
        if(!(extension==".zip"||extension==".plk")){continue;}
        //start reading the data
        juce::ZipFile zipFile(file);
        //get the main module file
        const auto& mainJSON = zipFile.getEntry("module.json");
        juce::String jsonText = "";
        if (mainJSON != nullptr)
        {
            jsonText = readZipEntryAsString(&zipFile,mainJSON);

            juce::var parsed = juce::JSON::parse(jsonText);
            
            //module data to save
            juce::String moduleFilePath=file.getFullPathName();
            juce::String moduleName=parsed.getProperty("name",  "[MISSING]");
            juce::String moduleDesc=parsed.getProperty("description",  "[MISSING]");
            //arrays
            juce::Array<juce::var>* moduleTags = parsed["tags"].getArray();
            juce::Array<juce::var>* moduleTextures = parsed["textures"].getArray();
            juce::Array<juce::var>* moduleModels = parsed["models"].getArray();
            
            #if DEBUG_MODULE_FILES
            std::cout << "registering module " << fileName << ":" << std::endl;
            std::cout <<"\tPath:\t" <<  moduleFilePath  << std::endl;
            std::cout <<"\tName:\t" <<  moduleName  << std::endl;
            std::cout <<"\tDesc:\t" <<  moduleDesc  << std::endl;
            std::cout <<"\tTags:\t" <<  std::endl;
            for (const juce::var& tag : *moduleTags){
                std::cout <<"\t\t"<< tag.toString() <<std::endl;
            }
            #endif
            
            //actually register the module nowww
            ModuleData& moduleData = ModuleManager::addModule();
            moduleData.name=moduleName.toStdString();
            moduleData.description=moduleDesc.toStdString();
            
            #if DEBUG_MODULE_FILES
            std::cout <<"\tTextures:\t" <<  std::endl;
            #endif
            //loop through the textures and initialize them idfk
            for (const juce::var& texture : *moduleTextures){
                juce::String texturePath = texture.getProperty("path", "[MISSING]");
                const juce::ZipFile::ZipEntry* textureEntry = zipFile.getEntry(texturePath);
                std::unique_ptr<juce::InputStream> stream(zipFile.createStreamForEntry(*textureEntry));
                juce::Image textureImage = FileManager::readTextureStream(std::move(stream));
                textureAtlas.addTexture(textureImage);

                #if DEBUG_MODULE_FILES
                std::cout <<"\t\tTexture:" <<  std::endl;
                std::cout <<"\t\t\tpath: "<< texturePath <<std::endl;
                #endif
            }
            
            #if DEBUG_MODULE_FILES
            std::cout <<"\tModels:\t" <<  std::endl;
            #endif
            //loop through the models tooooooo
            for (const juce::var& model : *moduleModels){
                juce::String modelPath = model.getProperty("path", "[MISSING]");
                
                #if DEBUG_MODULE_FILES
                std::cout <<"\t\tModel:" <<  std::endl;
                std::cout <<"\t\t\tpath: "<< modelPath <<std::endl;
                #endif
            }

        }else{
            #if DEBUG_MODULE_FILES
            std::cout << "NO MODULE.JSON FOUND!!" << std::endl;
            #endif

            continue;
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