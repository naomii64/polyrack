class Save{
    static MAGIC='POLYRACK';
    static VERSION=1;
    static EXTENSION='.plk';

    static STRING_END=0;
    static dataSaveFunction=function(key){this.saveValue(Save.object[key])};
    static dataNumberCounters={
        count:function(value){Save.pointer+=this.BYTES},
        countArray:function(value){Save.dataTypes['uint16'].count();Save.pointer+=this.BYTES*value.length},
    };
    static dataTypes={
        ['string']:{
            save:Save.dataSaveFunction,
            //cant load strings
            load:function(key){
                const startPos=Save.pointer;
                //count the string length
                while(Save.buffer[Save.pointer]!=Save.STRING_END){Save.pointer++}
                const arr=Save.buffer.slice(startPos,Save.pointer);
                let str='';
                for(let v of arr){str+=String.fromCharCode(v)}
                Save.object[key]=str;
                //escape out of the end character
                Save.pointer++;
            },
            loadArray:function(key){
                const arraySize=Save.dataTypes['uint16'].read();
                const arr=[];
                Save.object[key]=arr;
                Save.gainDepth(arr);
                for(let i=0;i<arraySize;i++){
                    this.load(i);
                }
                Save.loseDepth();
            },
            saveValue:function(value){Save.pushArray(this.encode(value))},
            saveArray:function(key){
                Save.dataTypes['uint16'].saveValue(Save.object[key].length);
                for(let v of Save.object[key]){Save.pushArray(this.encode(v))}
            },
            encode:function(value){return new Uint8Array(value.length+1).fill(0).map((v,i)=>value.charCodeAt(i))},
            count:function(value){Save.pointer+=value.length+1},
            countArray:function(value){Save.dataTypes['uint16'].count();value.forEach(v=>Save.pointer+=v.length+1)}
        },
        ['float32']:{
            save:Save.dataSaveFunction,
            load:function(key){Save.object[key]=this.read()},
            read: function() {
                console.log(Save.pointer,Save.buffer)
                const value = new DataView(Save.buffer.buffer).getFloat32(Save.pointer, true); // true = little endian
                Save.pointer += this.BYTES;
                return value;
            },
            loadArray:function(key){
                const arraySize=Save.dataTypes['uint16'].read();
                const arr=[];
                for(let i=0;i<arraySize;i++){
                    arr.push(this.read());
                }
                Save.object[key]=arr;
                console.log(arr)
            },
            saveValue:function(value){Save.pushArray(new Uint8Array(new Float32Array([value]).buffer))},
            saveArray: function (key) {
                const arr = Save.object[key];
                Save.dataTypes['uint16'].saveValue(arr.length);
                const flat = new Float32Array(arr);
                Save.pushArray(new Uint8Array(flat.buffer));
            },
            ...Save.dataNumberCounters,
            BYTES:4
        },
        ['uint8']:{
            save:Save.dataSaveFunction,
            load:function(key){Save.object[key]=this.read()},
            read:function(){Save.pointer+=this.BYTES;return Save.buffer[Save.pointer-this.BYTES]},
            saveValue:function(value){Save.pushArray(new Uint8Array([value]))},
            saveArray:function(key){
                Save.dataTypes['uint16'].saveValue(Save.object[key].length);
                Save.pushArray(new Uint8Array(Save.object[key]));
            },
            ...Save.dataNumberCounters,
            BYTES:1
        },
        ['uint16']:{
            save:Save.dataSaveFunction,
            load:function(key){Save.object[key]=this.read()},

            read: function() {
                const value = new DataView(Save.buffer.buffer).getUint16(Save.pointer, true); // true = little endian
                Save.pointer += this.BYTES;
                return value;
            },

            saveValue:function(value){Save.pushArray(new Uint8Array(new Uint16Array([value]).buffer))},
            count:function(value){Save.pointer+=this.BYTES},
            countArray:function(value){Save.pointer+=this.BYTES*value.length},
            ...Save.dataNumberCounters,
            BYTES:2
        },
        ['uint32']:{
            load:function(key){},
            save:Save.dataSaveFunction,
            saveValue:function(value){Save.pushArray(new Uint8Array(new Uint32Array([value]).buffer))},
            ...Save.dataNumberCounters,
            BYTES:4
        },
        ['byte']:{
            load:function(key){Save.object[key]=this.read()},
            read:function(){Save.pointer+=this.BYTES;return Save.buffer[Save.pointer-this.BYTES]},
            save:Save.dataSaveFunction,
            saveValue:function(value){Save.pushArray(new Uint8Array([Math.round(value*255)]))},
            ...Save.dataNumberCounters,
            BYTES:1
        },
    };
    static pushArray(arr){
        if(Save.mode==Save.MODE_SAVE){
            Save.buffer.set(arr,Save.pointer);
        }
        Save.pointer+=arr.length;
    }
    static gainDepth(obj){
        Save.object=obj;
        Save.objects.push(obj)
    }
    static loseDepth(){
        Save.objects.pop();
        Save.object=Save.objects[Save.objects.length-1];
    }
    //for saving stuff
    static mode=0;
    static MODE_SAVE=0;
    static MODE_LOAD=1;
    static MODE_MEASURE=2;
    //parsing vars
    static objects;
    static object;
    static pointer;
    static buffer;
    //error stuff
    static keyTypeCheck(type,key){
        if(Save.mode===Save.MODE_LOAD){return false;}
        if(Object.keys(Save.dataTypes).indexOf(type)==-1){alert(`error: "${type}" is not a valid type`);return true;}
        if(Object.keys(Save.object).indexOf(key)==-1){alert(`error: object has no attribute "${key}"`);return true;}
        return false;
    }
    //parsing functions
    static parsingContext;
    static parsingContextSave={
        single:function(type,key){Save.dataTypes[type].save(key)},
        array:function(type,key){Save.dataTypes[type].saveArray(key)},
        structArray:function(key,func){    
            Save.dataTypes['uint16'].saveValue(Save.object[key].length);
            for(v of Save.object[key]){
                Save.gainDepth(v);
                func(Save);
                Save.loseDepth();
            }
        },
        sequencedArray:function(key,dataTypes){
            //create pallete of different sections
            //set[section,section]
            const types=dataTypes.map(v=>v.split('*'));
            types.forEach(v=>v[1]=parseInt(v[1]));
            const attribCount=types.reduce((prev,cur)=>prev+cur[1],0);
            const arr=Save.object[key];
            const pal=new Map();
            let i=0;
            let currentAttrib=0;
            while(i<arr.length){
                const type=types[currentAttrib];
                const section=arr.slice(i,i+type[1]);
                const key= section.toString();

                let entry = pal.get(key);
                if (!entry) {
                  entry = { key, data: section, count: 0, attribIndex: currentAttrib };
                  pal.set(key, entry);
                }
                entry.count++;

                i+=type[1];
                currentAttrib++;
                if(currentAttrib>=types.length){
                    currentAttrib=0;
                }
            }
            const totalSets=arr.length/attribCount;
            //save the pallete
            const seperatedPalletes=new Array(types.length);
            for(let i=0;i<seperatedPalletes.length;i++){seperatedPalletes[i]=[]}
            for(let v of pal.values()){
                seperatedPalletes[v.attribIndex].push(v);
            }
            //finally store all the palletes
            for(let v of seperatedPalletes){
                //store the length of the palette
                Save.dataTypes['uint16'].saveValue(v.length);
                //store the values
                for(let b of v){
                    for(let c of b.data){
                        Save.dataTypes[types[b.attribIndex][0]].saveValue(c)
                    }
                }
            }
            //store the data now
            //amoung of sets
            Save.dataTypes['uint16'].saveValue(totalSets);
            //for each set
            i=0;
            currentAttrib=0;
            //declare stuff for scope reasons
            const currentPalData={id:null,count:0};
            let dataType='uint8';
            if(seperatedPalletes.some(v=>v.length>255)){dataType='uint16';console.log('pallete over 255 detected');}
            let lastAttrib = 0;
            while(i<arr.length){
                const type=types[currentAttrib];
                const section=arr.slice(i,i+type[1]);
                const key= section.toString();
                const pal=seperatedPalletes[currentAttrib];
                //1 is added because zero means the the the th ehte
                const index=pal.findIndex(v=>v.key==key)+1;
                if(currentPalData.id==null){currentPalData.id=index;currentPalData.count=0;}
                if(currentPalData.id!=index){
                    if(currentPalData.count<=3){
                        for(let i=0;i<currentPalData.count;i++){
                            Save.dataTypes[dataType].saveValue(currentPalData.id);
                        }
                    }else{
                        //0 indicates the following value will be repeated the value after it n+4 times (020=2222)
                        //format: [marker][pallete id][count]
                        Save.dataTypes[dataType].saveValue(Save.RLE_MARKER);
                        Save.dataTypes[dataType].saveValue(currentPalData.id);
                        Save.dataTypes[dataType].saveValue(currentPalData.count-4);
                    }
                    currentPalData.id=index;
                    currentPalData.count=0;
                }
                currentPalData.count++;
                
                i+=type[1];
                currentAttrib++;
                if(currentAttrib>=types.length){
                    currentAttrib=0;
                }
            }
            //flush
            if (currentPalData.count > 0) {
                if (currentPalData.count <= 3) {
                    for (let i = 0; i < currentPalData.count; i++) {
                        Save.dataTypes[dataType].saveValue(currentPalData.id);
                    }
                } else {
                    Save.dataTypes[dataType].saveValue(Save.RLE_MARKER);
                    Save.dataTypes[dataType].saveValue(currentPalData.id);
                    Save.dataTypes[dataType].saveValue(currentPalData.count - 4);
                }
            }
        }
    }
    static parsingContextMeasure={
        single:function(type,key){Save.dataTypes[type].count(Save.object[key])},
        array:function(type,key){Save.dataTypes[type].countArray(Save.object[key])},
        structArray:Save.parsingContextSave.structArray,
        sequencedArray:Save.parsingContextSave.sequencedArray
    }
    static parsingContextLoad={
        single:function(type,key){Save.dataTypes[type].load(key)},
        array:function(type,key){
            Save.dataTypes[type].loadArray(key);
        },
        structArray:function(key,func){
            const arraySize=Save.dataTypes['uint16'].read();
            const arr=[];
            Save.object[key]=arr;
            for(let i=0;i<arraySize;i++){
                const obj={};
                arr.push(obj);
                Save.gainDepth(obj);
                func(Save);
                Save.loseDepth();
            }
        },
        sequencedArray:function(key,dataTypes,saveAs){
            const attribCount=dataTypes.length;
            const types=dataTypes.map(v=>v.split('*'));
            types.forEach(v=>v[1]=parseInt(v[1]));

            //load the palletes
            const pals=[];
            let dataType='uint8'
            for(let i=0;i<attribCount;i++){
                const sectionDataType=types[i][0];
                const dataLength=types[i][1];
                const palleteSize=Save.dataTypes['uint16'].read();
                console.log(palleteSize)
                const pal=[];
                pals.push(pal);
                if(palleteSize>255){dataType='uint16'}
                for(let j=0;j<palleteSize;j++){
                    const sequence=[];
                    for(let k=0;k<dataLength;k++){
                        const value=Save.dataTypes[sectionDataType].read();
                        sequence.push(value);
                    }
                    pal.push(sequence)
                }
            }
            if(pals.some(v=>v.length>255)){dataType='uint16';console.log('pallete over 255 when loading')}
            //start reading the actual data
            const dataLength=Save.dataTypes['uint16'].read();
            let i=0;
            const finalArr=[];
            while(i<dataLength){
                const char=Save.dataTypes[dataType].read();
                if(char==Save.RLE_MARKER){
                    const palID=Save.dataTypes[dataType].read()-1;
                    const count=Save.dataTypes[dataType].read()+4;
                    
                    for(let j=0;j<count;j++){
                        const attrib=i%attribCount;    
                        finalArr.push(...pals[attrib][palID])
                        i++;
                    }
                }else{
                    const palID=char-1;
                    const attrib=i%attribCount;  
                    finalArr.push(...pals[attrib][palID])
                    i++;
                }
            }
            //finally store the array to the object
            Save.object[key]=new saveAs(finalArr);
            console.log(Save.object[key])
        }
    }

    static parsingContexts={[Save.MODE_SAVE]:Save.parsingContextSave,[Save.MODE_MEASURE]:Save.parsingContextMeasure,[Save.MODE_LOAD]:Save.parsingContextLoad};

    static single(type,key){
        if(Save.keyTypeCheck(type,key)){return;}
        Save.parsingContext.single(type,key);
    }
    static array(type,key){
        if(Save.keyTypeCheck(type,key)){return;}
        Save.parsingContext.array(type,key);
    }
    static structArray(key,func){
        Save.parsingContext.structArray(key,func);
    }
    static sequencedArray(key,dataTypes,saveAs=Array){
        Save.parsingContext.sequencedArray(key,dataTypes,saveAs);
    }
    static RLE_MARKER=0;
    static reset(obj=null){
        Save.objects=[];
        Save.gainDepth(obj);
        Save.pointer=0;
    }
    static measure(obj,func){
        Save.reset(obj);
        Save.setMode(Save.MODE_MEASURE);
        func(Save);
        return Save.pointer;
    }
    static setMode(mode){
        Save.mode=mode;
        Save.parsingContext=Save.parsingContexts[mode];
    }
    //other
    static saveToBytes(obj,func){
        obj.magic=Save.MAGIC;
        obj.version=Save.VERSION;

        Save.buffer=new Uint8Array(Save.measure(obj,func));
        Save.reset(obj);
        Save.setMode(Save.MODE_SAVE);
        func(Save);
        return Save.buffer;
    }
    static loadFromBytes(bytes,func){
        const obj={};
        Save.reset(obj);
        Save.pointer=0;
        Save.buffer=bytes;
        Save.setMode(Save.MODE_LOAD);
        func(Save)
        return obj;
    }
    //varies between file types
    static saveFunc(s){
        //header
        s.single('string','magic');
        s.single('uint8','version');
        s.single('string','name');
        s.single('string','description');
        s.array('string','tags');
        //body
        s.structArray('textures',(s)=>{
            s.single('string','name')
            s.single('uint8','dataSettings')
            s.single('uint16','width')
            s.single('uint16','height')
            s.sequencedArray('pixels',['uint8*4'],Uint8ClampedArray);
        });
        s.structArray('models',(s)=>{
            s.single('string','name');
            s.single('uint8','dataSettings');
            console.log(Save.objects[0])
            //x,y,z nx,ny,nz, r,g,b,a, u,v
            //sequencedarray doesnt work withj float32 yet for some reason
            s.sequencedArray('data',['float32*3','float32*3','byte*4','float32*1']);
        });
        s.structArray('animations',(s)=>{
            s.single('string','name');
            s.structArray('models',()=>{
                s.single('string','name');
                s.single('uint16','modelID');
                s.single('uint8','count');
                s.array('string','functions');
            });
        });
    }
}
/*
data types:
uint8   =   0-255           :1 byte
uint16  =   0-65,535        :2 bytes
uint32  =   0-4,294,967,295 :4 bytes
*/