class AnimationEditor{
    static list=document.getElementById('animation-list');
    static modelList=document.getElementById('animation-model-list');
    static modelInputs=document.getElementById('animation-model-settings');
    static canvDiv=document.getElementById('animation-canvas-div');
    static currentAnim=null;

    static addAnimation(name='animation'){
        const animation={}
        project.animations.push(animation);
        animation.models=[];
        animation.name=name;
        const button=document.createElement('button');
        AE.list.appendChild(button);
        button.textContent=animation.name
        button.classList.add('list-button')
        button.onclick=function(){
            highlightButtonInList(this)
            AE.loadAnimation(animation);
        }
        button.onclick();
        return animation;
    }
    static moveModels(){
        const num=AE.previewSlider.value;
        for(let i in AE.scene){
            const secondTime=performance.now()/1000;
            const mesh=AE.scene[i]
            const model=mesh.model;
            const useParam=function(paramName){
                const expr=model.functions[paramName];
                const allowed = {
                    //functions
                    min:Math.min,
                    max:Math.max,
                    
                    sin:Math.sin,
                    cos:Math.cos,
                    tan:Math.tan,
                    
                    //constants
                    pi:Math.PI,
                    e:Math.E,
                    //my functions
                    lerp:lerp,
                    clamp:clamp,
                    //variables
                    x:num,
                    n:mesh.instanceID,
                    c:model.count,
                    nc:mesh.instanceID/model.count,
                    nm:mesh.instanceID/(model.count-1),
                    t:secondTime,
                };
                const keys=Object.keys(allowed);
                const values=Object.values(allowed);
                let fn;
                try{
                fn=Function(...keys,`return(${expr})`)
                }catch(err){
                    return 0;
                }
                try{
                return fn(...values);
                }catch(err){return 0;}
            }

            mesh.position[X]=useParam('x')
            mesh.position[Y]=useParam('y')
            mesh.position[Z]=useParam('z')

            mesh.rotation[X]=useParam('rx')*Math.PI/180
            mesh.rotation[Y]=useParam('ry')*Math.PI/180
            mesh.rotation[Z]=useParam('rz')*Math.PI/180

            mesh.scale[X]=useParam('sx')
            mesh.scale[Y]=useParam('sy')
            mesh.scale[Z]=useParam('sz')
        
        }
    }
    static init(){
        AE.previewSlider=document.getElementById('animation-preview-slider')
        AE.renderer=new Renderer(700,700);
        AE.renderer.createPerspective(70);
        AE.canvDiv.appendChild(AE.renderer.canvas);
        
        const draw=function(){
            AE.moveModels();
            for(v of AE.scene){
                AE.renderer.drawMesh(v);
            }
            AE.renderer.drawAxis();
            requestAnimationFrame(draw);
        }
        AE.renderer.makeDraggable();
        
        requestAnimationFrame(draw);
    }
    static loadAnimation(anim){
        AE.currentAnim=anim;
        AE.modelList.innerHTML='';
        for(let i in anim.models){
            const button=document.createElement('button');
            button.textContent=`model ${i}`;
            button.classList.add('list-button')
            const currentModel=anim.models[i];
            AE.modelList.appendChild(button);
            button.onclick=function(){
                highlightButtonInList(this);
                AE.selectAnimationModel(currentModel);     
            }
        }
        AE.scene=[];
        for(let i in anim.models){
            const v=anim.models[i];
            const model=project.models[v.modelID];
            for(let n=0;n<v.count;n++){
                const mesh=AE.renderer.newMeshRaw(model.data);
                mesh.model=v;
                AE.scene.push(mesh)
                mesh.texture=AE.renderer.createGLTexture(project.textures[model.textureID]);
                mesh.instanceID=n;
            }
        }
    }
    static selectAnimationModel(model){
        AE.modelInputs.innerHTML='';
        const modelIDinput=document.createElement('input');
        modelIDinput.type='Number';
        modelIDinput.min=0;
        modelIDinput.max=project.models.length-1;
        modelIDinput.step=1;
        modelIDinput.onchange=function(){
            model.modelID=this.value;
            AE.loadAnimation(AE.currentAnim);    
        }
        modelIDinput.value=model.modelID;

        const modelCountInput=document.createElement('input');
        modelCountInput.type='Number';
        modelCountInput.min=1;
        modelCountInput.step=1;
        modelCountInput.onchange=function(){
            model.count=this.value;
            AE.loadAnimation(AE.currentAnim);    
        }
        modelCountInput.value=model.count;

        
        AE.modelInputs.appendChild(modelIDinput)
        AE.modelInputs.appendChild(modelCountInput)

        
        const addLabel=function(text){
            const div=document.createElement('div');
            const ruleDiv=document.createElement('div');
            ruleDiv.innerHTML='<hr style="width:100%;">'
            ruleDiv.style.flexGrow=1;
            div.style.display='flex';
            const label=document.createElement('div');
            div.appendChild(label);            
            label.style.flexGrow=0;
            div.style.gap='4px';
            label.textContent=text;
            div.appendChild(label);
            div.style.width='100%'
            div.appendChild(ruleDiv)
            AE.modelInputs.appendChild(div);
        }
        const addParamInput=function(paramKey,paramLabel){
            const div=document.createElement('div');
            const input=document.createElement('input');
            const inputSubDiv=document.createElement('div');
            const labelDiv=document.createElement('div');
            labelDiv.textContent=paramLabel;
            

            input.style.display='inline';
            
            input.style.flexGrow=1;
            input.style.minWidth=0;
            inputSubDiv.style.display='flex';

            inputSubDiv.appendChild(input);
            div.appendChild(labelDiv);
            div.appendChild(inputSubDiv);
            div.style.display='flex';
            labelDiv.style.flexGrow=1;
            labelDiv.classList.add('label');
            inputSubDiv.style.width='75%';
            div.style.width='100%'
            AE.modelInputs.appendChild(div);

            //wory around the actual input stuff now
            input.value=model.functions[paramKey];
            
            input.onchange=function(){
                model.functions[paramKey]=this.value;
            }
        }
        addLabel('position');
        addParamInput('x','X');
        addParamInput('y','Y');
        addParamInput('z','Z');
        
        addLabel('rotation');
        addParamInput('rx','X');
        addParamInput('ry','Y');
        addParamInput('rz','Z');
        
        addLabel('scale');
        addParamInput('sx','X');
        addParamInput('sy','Y');
        addParamInput('sz','Z');
    }
    static scene=[];
    static addModel(){
        const mdl={};

        mdl.modelID=0;
        mdl.count=1;
        mdl.name='model';

        mdl.functions={
            ['x']:'0',
            ['y']:'0',
            ['z']:'0',

            ['rx']:'0',
            ['ry']:'0',
            ['rz']:'0',

            ['sx']:'1',
            ['sy']:'1',
            ['sz']:'1',
        }

        AE.currentAnim.models.push(mdl);
        AE.loadAnimation(AE.currentAnim);
    }
    static reset(){
        project.animations=[];
        AE.list.innerHTML='';
        AE.modelList.innerHTML='';
        AE.modelInputs.innerHTML='';
        AE.currentAnim=null;
    }
}
const AE=AnimationEditor;
AE.init();

function lerp(a,b,i){
    return a+((b-a)*i)
}
function clamp(n,min,max){
    return Math.max(Math.min(n,max),min)
}
function mod(n,i){
    return n-Math.floor(n);
}

class AnimationModel{
    constructor(){}
    static from(fromAnim,renderer){
        const anim=new AnimationModel();
        anim.models=[];
        anim.position=[0,0,0];
        //create models for drawing
        for(let v of fromAnim.models){
            const model=renderer.newMeshRaw(project.models[v.modelID].data);
            model.texture=MV.getTexture(project.models[v.modelID].textureID,renderer);
            anim.models.push(model)
        }
        return anim;
    }
    draw(renderer){
        for(let v of this.models){
            v.position=this.position;
            
            renderer.drawMesh(v);
        }
    }
}