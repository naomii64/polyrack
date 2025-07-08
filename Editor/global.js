function lerp(a,b,i){
    return a+((b-a)*i)
}
function clamp(n,min,max){
    return Math.max(Math.min(n,max),min)
}
function mod(n){
    return n-Math.floor(n);
}
function convertVerticesToOBJ(vertices) {
    const VERTEX_SIZE=12;
    const optimizeNumberString = function(n){
        const mul=10**6;
        return (Math.round(n*mul)/mul).toString();
    }
    let faceStr=``;
    const findLoc=function(arr,key){
        let index=arr.indexOf(key);
        if(index!=-1){return index}
        index=arr.length;
        arr[index]=key;
        return index;
    }
    const positions=[];
    const uvs=[];
    const normals=[];

    let currentFace=[];
    for(let fakeI=0;fakeI<vertices.length/VERTEX_SIZE;fakeI++){
        const i=fakeI*Renderer.VERTEX_SIZE;
        //invert v coordinate to account for the way objs are stored
        vertices[i+11]=1-vertices[i+11];
        const vert=Array.from(vertices.slice(i,i+VERTEX_SIZE)).map(optimizeNumberString);
        //undo that last thing to not modify the original array
        vertices[i+11]=1-vertices[i+11];

        const xyz = vert.slice(0,3).toString().replaceAll(',',' ');
        const nxyz = vert.slice(3,6).toString().replaceAll(',',' ');
        const rgba = vert.slice(6,10).toString().replaceAll(',',' ');
        const uv = vert.slice(10,12).toString().replaceAll(',',' ');
        //will ignore vertex color for now
        const v=findLoc(positions,xyz);
        const vt=findLoc(uvs,uv);
        const vn=findLoc(normals,nxyz);
            
        const currentVertex=`${v+1}/${vt+1}/${vn+1}`;
        currentFace.push(currentVertex);
        if(currentFace.length>=3){
            const currentFaceString=`f ${currentFace.toString().replaceAll(',',' ')}\n`;
            currentFace.length=0;
            faceStr+=currentFaceString;
        }
    }
    function buildString(arr,start){
        let ret=``;
        for(let v of arr){
            ret+=`${start} ${v}\n`
        }
        return ret;
    }
    let vertStr=buildString(positions,'v');
    let uvStr=buildString(uvs,'vt');
    let normalStr=buildString(normals,'vn');
    const full=vertStr+uvStr+normalStr+faceStr;
    
    return full;
    
}
function base64ToTextureData(base64, callback) {
    const img = new Image();
    img.onload = () => {
        const canvas = document.createElement('canvas');
        canvas.width = img.width;
        canvas.height = img.height;

        const ctx = canvas.getContext('2d');
        ctx.drawImage(img, 0, 0);

        const imageData = ctx.getImageData(0, 0, img.width, img.height);
        const texture = {
            width: img.width,
            height: img.height,
            pixels: imageData.data // Uint8ClampedArray, 4 values per pixel (RGBA)
        };

        callback(texture);
    };
    img.src = base64;
}
function canvasToBlobAsync(canvas, type = 'image/png', quality) {
    return new Promise((resolve) => {
        canvas.toBlob((blob) => resolve(blob), type, quality);
    });
}
function parseOBJ(objText) {
    const lines = objText.split('\n');

    const vertices = [];
    const uvs = [];
    const normals = [];
    const faces = [];

    for (let line of lines) {
        line = line.trim();
        if (line.startsWith('#') || line === '') continue;

        const [keyword, ...parts] = line.split(/\s+/);

        switch (keyword) {
            case 'v':
                vertices.push(parts.map(Number));
                break;
            case 'vt':
                uvs.push(parts.map(Number));
                break;
            case 'vn':
                normals.push(parts.map(Number));
                break;
            case 'f':
                const face = parts.map(part => {
                    const [vIdx, vtIdx, vnIdx] = part.split('/').map(i => i ? parseInt(i) - 1 : null);
                    return { v: vIdx, vt: vtIdx, vn: vnIdx };
                });
                faces.push(face);
                break;
        }
    }

    const result = [];

    // For each face vertex, push: position(3), normal(3), color(3), uv(2)
    // Default color = white (1,1,1)
    for (const face of faces) {
        for (const vert of face) {
            const pos = vertices[vert.v] || [0, 0, 0];
            const norm = (vert.vn !== null && normals[vert.vn]) ? normals[vert.vn] : [0, 0, 0];
            const uv = (vert.vt !== null && uvs[vert.vt]) ? uvs[vert.vt] : [0, 0];

            // Position
            result.push(pos[0], pos[1], pos[2]);
            // Normal
            result.push(norm[0], norm[1], norm[2]);
            // Color (white)
            result.push(1, 1, 1, 1);
            // UV
            //needs to be flipped to top left origin
            result.push(uv[0],1-uv[1]);
        }
    }

    return new Float32Array(result);
}
function saveProject() {
    Save.saveProject(project);
}
function loadProject(file){
    Save.loadProject(file);
}
function resetAll(){
    MV.reset();
    TE.reset();
}
function download(bytes, fname)  {
  let blob = new Blob([bytes], {type:"application/octet-stream"});
  let link = document.createElement('a');
  link.href = window.URL.createObjectURL(blob);
  link.download = fname;
  link.click();
}
function transferArrayValues(target,values){
    //clear the target
    //THIS COULD BE MADE FASTER LATER
    target.length=0;
    for(const v of values){target.push(v);}
}
function highlightButtonInList(btn){
    for(v of btn.parentElement.children){v.style=null}            
    btn.style.background='black';
    btn.style.color='white';
}
//for storing global constants
class GLOBAL{
    static SCREEN_HEIGHT=2**9;
    static FOV=70;
    static createView(){
        const renderer=new Renderer(GLOBAL.SCREEN_HEIGHT,GLOBAL.SCREEN_HEIGHT);
        renderer.createPerspective(GLOBAL.FOV);
        renderer.makeDraggable();
        return renderer;
    }
    static init(){
        //for shortcuts
        document.addEventListener('keydown', function(event) {
            if (event.ctrlKey && event.key === 's') {
                event.preventDefault();
                saveProject();
            }
            if (event.ctrlKey && event.key === 'd') {
                event.preventDefault();
                Importer.open();
            }
        });
    }
}