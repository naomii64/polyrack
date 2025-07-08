class Save{
    static async saveProject(obj){
        const zip = new JSZip();

        const module={};
        //saves as module.json
        module.name=obj.name;
        module.description=obj.description;
        module.tags=obj.tags;
        //create folder where the textures will be stored
        const textureFolder=zip.folder('textures');
        const modelFolder=zip.folder('models');
        //save the models
        module.models=[];
        for(let i in obj.models){
            const v=obj.models[i];
            const modelOBJ=convertVerticesToOBJ(v.data);
            const path=`models/${i}.obj`
            zip.file(path,modelOBJ);

            const currentModelData={};

            currentModelData.name=v.name;
            currentModelData.path=path;
            currentModelData.textureID=v.textureID;

            module.models.push(currentModelData);
        }
        
        //save the textures
        module.textures=[];
        for(let i in obj.textures){
            const v=obj.textures[i];
            const canvas=document.createElement('canvas');
            //temportary
            //document.body.prepend(canvas);
            canvas.width=v.width;
            canvas.height=v.height;
            const ctx=canvas.getContext('2d');
            const imgdata=new ImageData(v.pixels,v.width,v.height);
            ctx.putImageData(imgdata,0,0);
            const blob=canvasToBlobAsync(canvas);
            const path=`textures/${i}.png`
            zip.file(path,blob);

            const tex={};
            tex.path=path;
            tex.name=v.name;

            module.textures.push(tex);
        }
        //save the animations
        module.animations=[];
        for(let i in obj.animations){
            const v=obj.animations[i];
            const anim={};
            anim.name=v.name;
            anim.models=v.models;

            module.animations.push(anim);
        }
        //save the layout
        module.layout={};
        module.layout.width=project.layout.width;
        module.layout.height=project.layout.height;
        module.layout.components=[];
        for(let v of project.layout.components){
            const savableKeys=v.parent.savableKeys;
            const comp={};
            comp.type=v.parent.type;
            for(let k of savableKeys){
                comp[k.key]=v[k.key];
            }
            console.log(comp)
            module.layout.components.push(comp)
        }

        //save the main module.json
        zip.file('module.json',JSON.stringify(module,null,4));        
        // Generate the zip and trigger download
        zip.generateAsync({ type: "blob" }).then(function (blob) {
            const url = URL.createObjectURL(blob);
            const link = document.createElement("a");
            link.href = url;
            link.download = "project.plk";
            document.body.appendChild(link);
            link.click();
            document.body.removeChild(link);
            URL.revokeObjectURL(url);
        });
    }
    static async loadProject(file){
        const arrayBuffer = await file.arrayBuffer();
        const zip = await JSZip.loadAsync(arrayBuffer);
            
        const mainJson=zip.files['module.json'];
        const moduleContent=await mainJson.async("string");
        const moduleObject=JSON.parse(moduleContent);
        //reset stuff
        MV.reset();
        TE.reset();
        AE.reset();
        //load stuff to project
        project.name=moduleObject.name;
        project.description=moduleObject.description;
        transferArrayValues(project.tags,moduleObject.tags);
        const module=moduleObject;
        //load models
        for(let v of module.models){
            const file=zip.files[v.path]
            const obj=await file.async("string");

            const contents=parseOBJ(obj);

            const newModel={};
            newModel.data=contents;
            newModel.textureID=v.textureID;
            newModel.name=v.name;
            
            MV.addModel(newModel);
        }
        //load textures
        for(let v of module.textures){
            const file=zip.files[v.path];
            const blob = await file.async("blob");

            const imageBitmap = await createImageBitmap(blob);
            const canvas=document.createElement('canvas');
            canvas.width=imageBitmap.width;
            canvas.height=imageBitmap.height;
            const ctx = canvas.getContext("2d");
            ctx.drawImage(imageBitmap, 0, 0);
            const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
            const newTex={};
            newTex.pixels=imageData.data;
            newTex.width=imageData.width;
            newTex.height=imageData.height;
            newTex.name=v.name;
            TE.addTexture(newTex);
        }
        for(let v of module.animations){
            const anim=AE.addAnimation();
            anim.models=v.models;
            anim.name=v.name;
        }
        //load the layout
        project.layout.width=module.layout.width;
        project.layout.height=module.layout.height;
        LE.updateGrid();
        for(let v of module.layout.components){
            const comp=LE.addByType(v.type);
            for(let k of comp.parent.savableKeys){
                if(v[k.key]==undefined){
                    comp[k.key]=k.defaultValue;
                    continue;
                }
                comp[k.key]=v[k.key];
            }
            comp.onUpdate();
        }
        //update all ui
        UIHelper.updateAll();
    }
}