class Save{
    static MAGIC="PRMD";
    static EXTENSION=".plk";
    static VERSION=1;

    static INCLUDE_0X=false;
    static bufferToByteList(array,start,end){
        try{
        let ret='';
        for(let i=start;i<=end;i++){
            ret+=`${(Save.INCLUDE_0X?'0x':'')}${array[i].toString(16).padStart(2,'0')} `
        }
        return ret;
            }catch(err){return 'failed :('}
    }
    static hexCodeFromArraySection(array,start,end){
        try{
        let ret=`#`;
        for(let i=start;i<=end;i++){
            ret+=`${array[i].toString(16).padStart(2,'0')}`
        }
        return ret;
            }catch(err){return 'failed :('}
    }
    static stringToBytes(str){
        const arr=new Uint8Array(str.length);
        for(let i=0;i<str.length;i++){
            arr[i]=str.charCodeAt(i);
        }
        return arr;
    }
    constructor(){
        this.models=[];
        this.textures=[];
    }
    addData(models,textures){
        this.models=this.models.concat(models);
        this.textures=this.textures.concat(textures);
    }
    compressData(){
        for(v of this.textures){
            const palMap = new Map();
            let colors=0;
            v.palette=[];
            const indexedPixels=new Uint8Array(v.pixels.length/4);
            for(let i=0;i<v.pixels.length;i+=4){
                const key=Save.hexCodeFromArraySection(v.pixels,i,i+3);
                if(!palMap.has(key)){
                    v.palette.push(v.pixels[i]);
                    v.palette.push(v.pixels[i+1]);
                    v.palette.push(v.pixels[i+2]);
                    v.palette.push(v.pixels[i+3]);
                    palMap.set(key,colors);
                    colors++;
                }
                indexedPixels[i/4]=palMap.get(key);
            }
            //add repeat marks
            const newIndexedPixels=[];
            for(b of indexedPixels){
                if(
                    (newIndexedPixels.length==0)||
                    (newIndexedPixels[newIndexedPixels.length-1][0]!=b)
                ){
                    newIndexedPixels.push([b,0]);
                }else{
                    newIndexedPixels[newIndexedPixels.length-1][1]++;
                }
            }
            const finalIndexedPixels=[];
            for(b of newIndexedPixels){
                finalIndexedPixels.push(b[0]+1)
                if(b[1]>1){
                    finalIndexedPixels.push(0,b[1]-2)
                }else if(b[1]==1){
                    finalIndexedPixels.push(b[0]+1)
                }
            }
            v.indexedPixels=new Uint8Array(finalIndexedPixels);
        }
        for(v of this.models){
            const f32s=4;
            v.vertCount=v.data.length/Renderer.VERTEX_SIZE;
            v.compressedData=new Uint8Array(v.data.length*f32s*(3+3+(4/f32s/*rgba/f32s*/)+2)/Renderer.VERTEX_SIZE);
            for(let i=0;i<v.data.length/Renderer.VERTEX_SIZE;i++){
                const i2=i*Renderer.VERTEX_SIZE;
                const xyznxyz=convert16BitTo8Bit(new Float32Array([v.data[i2],v.data[i2+1],v.data[i2+2],
                                                                  v.data[i2+3],v.data[i2+4],v.data[i2+5]]));
                const uv=convert16BitTo8Bit(new Float32Array([v.data[i2+10],v.data[i2+11]]));
                 
                //mul by 1000 and round to store as 16 bit int
                const i3=i*((4*6)+4+(4*2));
                
                v.compressedData.set(xyznxyz,i3);
                v.compressedData[i3+(6*f32s)]=Math.round(v.data[i2+6]*255);
                v.compressedData[i3+(6*f32s)+1]=Math.round(v.data[i2+7]*255);
                v.compressedData[i3+(6*f32s)+2]=Math.round(v.data[i2+8]*255);
                v.compressedData[i3+(6*f32s)+3]=Math.round(v.data[i2+9]*255);
                v.compressedData.set(uv,i3+(6*f32s)+4);
                
                
            }
        }
    }
    static byteString(num){
        const k=1024;
        const decimals=0;
        const datas=['bytes','KB','MB','GB']
        let n=num;
        const d=10**decimals;
        for(let v of datas){
            if(n<k){return `${Math.round(n*d)/d} ${v}`}
            n/=k;
        }

        return `[num too high]`;
    }
    getHeader(){
        let arr=new Uint8Array(9);
        arr.set(Save.stringToBytes(Save.MAGIC),0);
        arr[4]=Save.VERSION;
        const texCount=convert16BitTo8Bit(new Uint16Array([this.textures.length]));
        arr.set(texCount,5);
        const modelCount=convert16BitTo8Bit(new Uint16Array([this.models.length]));
        arr.set(modelCount,7);
        return arr;
    }
    getTextureSaveData(tex){
        let arr=new Uint8Array(5+tex.palette.length+tex.indexedPixels.length);
        const w=convert16BitTo8Bit(new Uint16Array([tex.width]));
        const h=convert16BitTo8Bit(new Uint16Array([tex.height]));
        arr.set(w,0);
        arr.set(h,2);
        arr[4]=tex.palette.length/4;
        arr.set(tex.palette,5);
        arr.set(tex.indexedPixels,5+tex.palette.length);
        
        return new Uint8Array(arr);
    }
    getModelSaveData(model){
        let arr=new Uint8Array(4+(model.compressedData.length));
        const verts=convert16BitTo8Bit(model.data);
        const vertCount=convert16BitTo8Bit(new Uint16Array([model.vertCount]));
        const texID=convert16BitTo8Bit(new Uint16Array([model.textureID]));
        arr.set(vertCount,0);
        arr.set(texID,2);
        arr.set(model.compressedData,4);
        
        return arr;
    }
    static combineUint8Arrays(arrs){
        let size=0;
        for(v of arrs){size+=v.length};
        const ret=new Uint8Array(size);
        let i=0;
        for(v of arrs){ret.set(v,i);i+=v.length};
        return ret;
    }
    getAllTextureSaveData(){
        let size=0;
        const datas=[];
        for(v of this.textures){
            const data=this.getTextureSaveData(v);
            datas.push(data);
            size+=data.length;
        }
        const arr=new Uint8Array(size);
        let i=0;
        for(v of datas){
            arr.set(v,i);
            i+=v.length;
        }        
        return arr;
    }
    getAllModelSaveData(){
        let size=0;
        const datas=[];
        for(v of this.models){
            const data=this.getModelSaveData(v);
            datas.push(data);
            size+=data.length;
        }
        const arr=new Uint8Array(size);
        let i=0;
        for(v of datas){
            arr.set(v,i);
            i+=v.length;
        }        
        return arr;
    }
    getSaveBytes(){
        const header=this.getHeader();
        const textures=this.getAllTextureSaveData();
        const models=this.getAllModelSaveData();
        
        return Save.combineUint8Arrays([header,textures,models]);
    }
    static saveByteArray = function () {
    var a = document.createElement("a");
    document.body.appendChild(a);
    a.style = "display: none";
    return function (data, name) {
        var blob = new Blob(data, {type: "octet/stream"}),
            url = window.URL.createObjectURL(blob);
        a.href = url;
        a.download = name;
        a.click();
        window.URL.revokeObjectURL(url);
    };
    }();
    static HEADER_SIZE=9;
    static readArray(arr){
        let i=0;
        const int8=function(){
            i++;
            return arr[i-1];
        }        
        const int16=function(){
            i+=2;
            return Save.arr8to16(arr.slice(i-2,i))[0];
        }
        const float32=function(){
            i+=4;
            return Save.arr8tof32(arr.slice(i-4,i))[0];
        }
        
        const obj={};
        const header=arr.slice(0,Save.HEADER_SIZE);
        const magic=Save.arrToStr(header.slice(0,Save.MAGIC.length));
        i+=magic.length;
        if(magic!=Save.MAGIC){alert(`failed to load file, incorrect header. ${magic} should be ${Save.MAGIC}`)}
        //version is one bit
        obj.version=int8();
        obj.texCount=int16();
        obj.modelCount=int16();
        //on to load textures
        obj.textures=[];
        obj.models=[];
        //load a texture
        for(let q=0;q<obj.texCount;q++){
            const tex={};
            tex.width=int16();
            tex.height=int16();
            tex.palSize=int8();
            tex.pal=[];
            const totalPixels=tex.width*tex.height;
            for(let j=0;j<tex.palSize;j++){
                tex.pal[j]=[];
                //rgba 8bits
                for(let i=0;i<4;i++){tex.pal[j].push(int8())}
            }
            //start loading actual image data
            let pix=0;
            tex.pixels=new Uint8ClampedArray(totalPixels*4);
            let col=0;
            while(pix<totalPixels){
            const curPixel=int8();
                if(curPixel==0){
                    const repeatCount=int8()+2;
                    for(let n=0;n<repeatCount;n++){
                        tex.pixels.set(tex.pal[col],pix*4);
                        pix++;
                    }
                    continue;
                }else{
                    const palID=curPixel-1;
                    col=palID;
                    tex.pixels.set(tex.pal[col],pix*4);
                    pix++;
                    continue;
                }
            }
            obj.textures.push(tex);
        }
        //load the models now
        for(let q=0;q<obj.modelCount;q++){
            const mod={};
            mod.vertCount=int16();
            mod.textureID=int16();
            mod.data=new Float32Array(mod.vertCount*Renderer.VERTEX_SIZE);
            const rep=function(n,f){for(let i=0;i<n;i++){f(i)}}
            //start parsing verts
            let dataPos=0;
            for(let vert=0;vert<mod.vertCount;vert++){
                //xyz nxyz
                rep(6,n=>{
                    mod.data[dataPos]=float32();
                    dataPos++;
                });
                //rgb
                rep(4,n=>{
                    mod.data[dataPos]=int8()/255;
                    dataPos++;
                });
                //uv
                rep(2,n=>{
                    mod.data[dataPos]=float32();
                    dataPos++;
                });
            }
            obj.models.push(mod);
        }
        return obj;
    }
    static arr8to16(arr){
        return new Uint16Array(arr.buffer);
    }
    static arr8tof32(arr){
        return new Float32Array(arr.buffer);
    }
    
    static arrToStr(arr){
        return String.fromCharCode(...arr);
    }
    static loadFile(file){
        const reader = new FileReader();
        reader.onload = function(e) {
            const arrayBuffer = reader.result;
            // Process the arrayBuffer here
            const arr=new Uint8Array(arrayBuffer);
            const obj=Save.readArray(arr);
            resetAll();
            //load textures from save object
            for(v of obj.textures){TE.addTexture(v);}
            for(v of obj.models){MV.addModel(v);}
        }
        reader.readAsArrayBuffer(file);
    }
}

function convert16BitTo8Bit(uint16Array) {
    const uint8Array = new Uint8Array(uint16Array.buffer);
    return uint8Array;
}