class ModelViewer{
    static viewingModel = null;
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
        //get elements
        ModelViewer.list= document.getElementById('model-list');
        //setup renderer
        MV.renderer=GLOBAL.createView();
        document.getElementById('model-view').appendChild(MV.renderer.canvas);
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
        MV.viewingModel = MV.renderer.newMeshRaw(model.data);
        const tex=project.textures[model.textureID];
        if(tex!=null){
            MV.viewingModel.texture=MV.renderer.createGLTexture(tex);
        }else{
            MV.viewingModel.texture=null;
        }
    }
    static changeModelViewShader(shaderName){
        const shader=MV.alternateShaders[shaderName];
        MV.renderer.setShaderProgram(vsSource,shader);
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
    //every tab with a renderer will have an animate function
    static animate(){
        MV.renderer.drawAxis();
        MV.renderer.stretchToParent();
        if(MV.viewingModel!=null){
            MV.renderer.drawMesh(MV.viewingModel);
        }
    }
}
const MV=ModelViewer;