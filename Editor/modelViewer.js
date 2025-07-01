class ModelViewer{
    static dropZone = document.getElementById("import-zone");
    static fileInput = document.getElementById("import-file-input");
    static list = document.getElementById('model-list');
    //shaders
    static alternateShaders={
        normals:`
        precision mediump float;

        varying lowp vec4 vColor;
        varying lowp vec3 vNormal;
        varying lowp vec2 vUV;
        uniform sampler2D uTexture;
        
        void main() {
          vec3 betterNormal = (vNormal+vec3(1.0))/vec3(2.0);
          gl_FragColor = vec4(betterNormal,1.0);
        }
        `,
        uvs:`
        precision mediump float;

        varying lowp vec4 vColor;
        varying lowp vec3 vNormal;
        varying lowp vec2 vUV;
        uniform sampler2D uTexture;

        void main() {
          gl_FragColor = vec4(vUV,0.0,1.0);
        }
        `,
        texture:`
        precision mediump float;

        varying lowp vec4 vColor;
        varying lowp vec3 vNormal;
        varying lowp vec2 vUV;
        uniform sampler2D uTexture;

        void main() {
          gl_FragColor = texture2D(uTexture, vec2(vUV.x,1.0-vUV.y));
        }
        `,
        lit:Renderer.fsSource
    }
    static init(){
        MV.dropZone.addEventListener("dragover", function(e) {e.preventDefault();});
        MV.dropZone.addEventListener("drop", function(e) {
            e.preventDefault();
            for(v of e.dataTransfer.files){
                const n=v.name;
                if(n.endsWith('.obj')){
                    MV.handleFile(v);
                }
                if(n.endsWith('.png')){
                    TextureEditor.handleFile(v);
                }
                if(n.endsWith('.plk')){
                    loadProject(v);
                }
            }
            
            closeImport();
        });
    }
    static handleFile(file) {
        const reader = new FileReader();
        reader.onload = function(e) {
            const contents=parseOBJ(e.target.result);
            const newModel={};
            newModel.data=contents;
            newModel.textureID=0;
            const fileName=file.name.substring(0,file.name.length-4);
            newModel.name=fileName;
            MV.addModel(newModel);
        };
        reader.readAsText(file);
    }
    static addModel(model){
        //add the model to the project
        project.models.push(model);
        const modelButton=document.createElement('button');
        modelButton.onclick=function(){
            MV.viewModel(model);
            highlightButtonInList(this);
        }
        modelButton.classList.add('list-button');
        modelButton.textContent=model.name;
        MV.list.appendChild(modelButton);
    }
    static getTexture(id,renderer){
        if(project.textures.length>id){
            return renderer.createGLTexture(project.textures[id])
        }
        return null;
    }
    static viewModel(model){
        MV.currentModel=model;
        viewingModel = modelViewRenderer.newMeshRaw(model.data);
        const tex=project.textures[model.textureID];
        if(tex!=null){
            viewingModel.texture=modelViewRenderer.createGLTexture(tex);
        }else{
            viewingModel.texture=null;
        }
    }
    static changeModelViewShader(shaderName){
        const shader=MV.alternateShaders[shaderName];
        modelViewRenderer.setShaderProgram(vsSource,shader);
    }
    static round(roundth){
        const invRoundth=1/roundth;
        const m=MV.currentModel;
        for(let i=0;i<m.length;i++){
            m.data[i]=Math.round(m.data[i]*roundth)*invRoundth;
        }
        //reload the model in view
        MV.viewModel(MV.currentModel);
    }
    static scaleModel(){
        if(MV.currentModel==null){
            alert('no model selected');
            return;
        }
        let scale=prompt('enter a scale factor');
        if(scale==null){return;}
        
        for(let i=0;i<MV.currentModel.data.length;i+=Renderer.VERTEX_SIZE){
            MV.currentModel.data[i]*=scale;
            MV.currentModel.data[i+1]*=scale;
            MV.currentModel.data[i+2]*=scale;
        }
        //reload the model in view
        MV.viewModel(MV.currentModel)
    }
    static originToCenter(){
        let center = [0,0,0];
        let total=0;
        for(let i=0;i<MV.currentModel.data.length;i+=Renderer.VERTEX_SIZE){
            center[0]+=MV.currentModel.data[i];
            center[1]+=MV.currentModel.data[i+1];
            center[2]+=MV.currentModel.data[i+2];
            total++;
        }
        center=center.map(v=>v/total);
        for(let i=0;i<MV.currentModel.data.length;i+=Renderer.VERTEX_SIZE){
            MV.currentModel.data[i]-=center[0];
            MV.currentModel.data[i+1]-=center[1];
            MV.currentModel.data[i+2]-=center[2];
        }
        MV.viewModel(MV.currentModel);
        
    }
    static forEachVert(fnc){
        for(let i=0;i<MV.currentModel.data.length;i+=Renderer.VERTEX_SIZE){
            const v=fnc(
                //xyz
                MV.currentModel.data[i],
                MV.currentModel.data[i+1],
                MV.currentModel.data[i+2],
                //nxyz
                MV.currentModel.data[i+3],
                MV.currentModel.data[i+4],
                MV.currentModel.data[i+5],
                //rgba
                MV.currentModel.data[i+6],
                MV.currentModel.data[i+7],
                MV.currentModel.data[i+8],
                MV.currentModel.data[i+9],
                //uv
                MV.currentModel.data[i+10],
                MV.currentModel.data[i+11]
            );
            for(let j=0; j<v.length;j++){
                MV.currentModel.data[i+j]=v[j];
            }
        }
        /* MV.forEachVert((x,y,z,nx,ny,nz,r,g,b,a,u,v)=>{
            return [x,y,z,nx,ny,nz,r,g,b,a,u,v];
        })*/
    }
    static rotX(){
        MV.forEachVert((x,y,z,nx,ny,nz,r,g,b,a,u,v)=>{return [x,-z,y,nx,-nz,ny];})
        MV.viewModel(MV.currentModel);
    }
    static rotY(){
        MV.forEachVert((x,y,z,nx,ny,nz,r,g,b,a,u,v)=>{return [z,y,-x,nz,ny,-nx];})
        MV.viewModel(MV.currentModel);
    }
    static rotZ(){
        MV.forEachVert((x,y,z,nx,ny,nz,r,g,b,a,u,v)=>{return [-y,x,z,-ny,nx,nz];})
        MV.viewModel(MV.currentModel);
    }
    static mirX(){
        MV.forEachVert((x,y,z,nx,ny,nz,r,g,b,a,u,v)=>{return [-x,y,z,-nx,ny,nz];})
        MV.flipTris();
    }
    static mirY(){
        MV.forEachVert((x,y,z,nx,ny,nz,r,g,b,a,u,v)=>{return [x,-y,z,nx,-ny,nz];})
        MV.flipTris();
    }
    static mirZ(){
        MV.forEachVert((x,y,z,nx,ny,nz,r,g,b,a,u,v)=>{return [x,y,-z,nx,ny,-nz];})
        MV.flipTris();
    }
    static flipTris(){
        for(let i=0;i<MV.currentModel.data.length;i+=Renderer.VERTEX_SIZE*3){
            const i1=i;
            const i2=i+Renderer.VERTEX_SIZE;
            
            const vert1=MV.currentModel.data.slice(i1,i1+Renderer.VERTEX_SIZE);
            const vert2=MV.currentModel.data.slice(i2,i2+Renderer.VERTEX_SIZE);
            
            MV.currentModel.data.set(vert2,i);
            MV.currentModel.data.set(vert1,i+Renderer.VERTEX_SIZE);
        }
        MV.viewModel(MV.currentModel);
    }
    static scaleModelUnit(){
        //calculate the scale to scale it by
        let scale=0;
        //find largest position
        for(let i=0;i<MV.currentModel.data.length;i+=Renderer.VERTEX_SIZE){
            scale=Math.max(Math.abs(MV.currentModel.data[i]),scale);
            scale=Math.max(Math.abs(MV.currentModel.data[i+1]),scale);
            scale=Math.max(Math.abs(MV.currentModel.data[i+2]),scale);
        }
        //invert
        scale=1/scale;
        for(let i=0;i<MV.currentModel.data.length;i+=Renderer.VERTEX_SIZE){
            MV.currentModel.data[i]*=scale;
            MV.currentModel.data[i+1]*=scale;
            MV.currentModel.data[i+2]*=scale;
        }
        
        MV.viewModel(MV.currentModel);
    }

    static reset(){
        project.models=[];
        MV.list.innerHTML='';
    }
}
const MV=ModelViewer;
MV.init();

let viewingModel=null;
const spinSpeed=0.01;

function animateModelView(){
    modelViewRenderer.drawAxis();
    if(viewingModel!=null){
        //viewingModel.rotation[Y]+=spinSpeed;
        modelViewRenderer.drawMesh(viewingModel);
    }
    requestAnimationFrame(animateModelView);
}

const modelViewRenderer=new Renderer(Main.SCREEN_HEIGHT,Main.SCREEN_HEIGHT);
modelViewRenderer.createPerspective(70);
document.getElementById('model-view').appendChild(modelViewRenderer.canvas);
modelViewRenderer.makeDraggable();


animateModelView();

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
function openImport(){
    MV.dropZone.style.display=null;
}
function closeImport(){
    MV.dropZone.style.display='none';
}
closeImport();